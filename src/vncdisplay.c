/*
 * GTK VNC Widget
 *
 * Copyright (C) 2006  Anthony Liguori <anthony@codemonkey.ws>
 * Copyright (C) 2009-2010 Daniel P. Berrange <dan@berrange.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>
#include <locale.h>

#include "vncdisplay.h"
#include "vncconnection.h"
#include "vncutil.h"
#include "vncmarshal.h"
#include "vncdisplaykeymap.h"
#include "vncdisplayenums.h"
#include "vnccairoframebuffer.h"

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef G_OS_WIN32
#include <windows.h>
#include <gdk/gdkwin32.h>
#ifndef MAPVK_VK_TO_VSC /* may be undefined in older mingw-headers */
#define MAPVK_VK_TO_VSC 0
#endif
#endif

#define VNC_DISPLAY_GET_PRIVATE(obj)                                    \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), VNC_TYPE_DISPLAY, VncDisplayPrivate))

struct _VncDisplayPrivate
{
    GdkCursor *null_cursor;
    GdkCursor *remote_cursor;

    VncConnection *conn;
    VncCairoFramebuffer *fb;
    cairo_surface_t *fbCache; /* Cache on server display */

    VncDisplayDepthColor depth;

    gboolean in_pointer_grab;
    gboolean in_keyboard_grab;

    guint down_keyval[16];
    guint down_scancode[16];

    int button_mask;
    int last_x;
    int last_y;

    gboolean absolute;

    gboolean grab_pointer;
    gboolean grab_keyboard;
    gboolean local_pointer;
    gboolean read_only;
    gboolean allow_lossy;
    gboolean allow_scaling;
    gboolean shared_flag;
    gboolean force_size;

    GSList *preferable_auths;
    GSList *preferable_vencrypt_subauths;
    size_t keycode_maplen;
    const guint16 *keycode_map;

    gboolean vncgrabpending; /* Key sequence detected, waiting for release */
    VncGrabSequence *vncgrabseq; /* the configured key sequence */
    gboolean *vncactiveseq; /* the currently pressed keys */

#ifdef WIN32
    HHOOK keyboard_hook;
#endif
};

G_DEFINE_TYPE(VncDisplay, vnc_display, GTK_TYPE_DRAWING_AREA)

/* Properties */
enum
{
    PROP_0,
    PROP_POINTER_LOCAL,
    PROP_POINTER_GRAB,
    PROP_KEYBOARD_GRAB,
    PROP_READ_ONLY,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_NAME,
    PROP_LOSSY_ENCODING,
    PROP_SCALING,
    PROP_SHARED_FLAG,
    PROP_FORCE_SIZE,
    PROP_DEPTH,
    PROP_GRAB_KEYS,
    PROP_CONNECTION,
};

/* Signals */
typedef enum
    {
        VNC_POINTER_GRAB,
        VNC_POINTER_UNGRAB,
        VNC_KEYBOARD_GRAB,
        VNC_KEYBOARD_UNGRAB,

        VNC_CONNECTED,
        VNC_INITIALIZED,
        VNC_DISCONNECTED,
        VNC_AUTH_CREDENTIAL,

        VNC_DESKTOP_RESIZE,

        VNC_AUTH_FAILURE,
        VNC_AUTH_UNSUPPORTED,

        VNC_SERVER_CUT_TEXT,
        VNC_BELL,
        VNC_ERROR,

        LAST_SIGNAL
    } vnc_display_signals;


/* Some compatibility defines to let us build on both Gtk2 and Gtk3 */
#if GTK_CHECK_VERSION (2, 91, 0)

static inline void gdk_drawable_get_size(GdkWindow *w, gint *ww, gint *wh)
{
    *ww = gdk_window_get_width(w);
    *wh = gdk_window_get_height(w);
}

#define GtkObject GtkWidget
#define GtkObjectClass GtkWidgetClass
#define GTK_OBJECT_CLASS(c) GTK_WIDGET_CLASS(c)
#define gdk_cursor_unref(c) g_object_unref(c)
#endif


static guint signals[LAST_SIGNAL] = { 0, 0, 0, 0,
                                      0, 0, 0, 0,
                                      0, 0, 0, 0, 0,};

static gboolean vnc_debug_option_arg(const gchar *option_name G_GNUC_UNUSED,
                                     const gchar *value G_GNUC_UNUSED,
                                     gpointer data G_GNUC_UNUSED,
                                     GError **error G_GNUC_UNUSED)
{
    vnc_util_set_debug(TRUE);
    return TRUE;
}

static const GOptionEntry gtk_vnc_args[] =
    {
        { "gtk-vnc-debug", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
          vnc_debug_option_arg, N_("Enables debug output"), 0 },
        { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, 0 }
    };


static void
vnc_display_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    VncDisplay *vnc = VNC_DISPLAY (object);

    switch (prop_id)
        {
        case PROP_POINTER_LOCAL:
            g_value_set_boolean (value, vnc->priv->local_pointer);
            break;
        case PROP_POINTER_GRAB:
            g_value_set_boolean (value, vnc->priv->grab_pointer);
            break;
        case PROP_KEYBOARD_GRAB:
            g_value_set_boolean (value, vnc->priv->grab_keyboard);
            break;
        case PROP_READ_ONLY:
            g_value_set_boolean (value, vnc->priv->read_only);
            break;
        case PROP_WIDTH:
            g_value_set_int (value, vnc_display_get_width (vnc));
            break;
        case PROP_HEIGHT:
            g_value_set_int (value, vnc_display_get_height (vnc));
            break;
        case PROP_NAME:
            g_value_set_string (value, vnc_display_get_name (vnc));
            break;
        case PROP_LOSSY_ENCODING:
            g_value_set_boolean (value, vnc->priv->allow_lossy);
            break;
        case PROP_SCALING:
            g_value_set_boolean (value, vnc->priv->allow_scaling);
            break;
        case PROP_SHARED_FLAG:
            g_value_set_boolean (value, vnc->priv->shared_flag);
            break;
        case PROP_FORCE_SIZE:
            g_value_set_boolean (value, vnc->priv->force_size);
            break;
        case PROP_DEPTH:
            g_value_set_enum (value, vnc->priv->depth);
            break;
        case PROP_GRAB_KEYS:
            g_value_set_boxed(value, vnc->priv->vncgrabseq);
            break;
        case PROP_CONNECTION:
            g_value_set_object(value, vnc->priv->conn);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        }
}

static void
vnc_display_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    VncDisplay *vnc = VNC_DISPLAY (object);

    switch (prop_id)
        {
        case PROP_POINTER_LOCAL:
            vnc_display_set_pointer_local (vnc, g_value_get_boolean (value));
            break;
        case PROP_POINTER_GRAB:
            vnc_display_set_pointer_grab (vnc, g_value_get_boolean (value));
            break;
        case PROP_KEYBOARD_GRAB:
            vnc_display_set_keyboard_grab (vnc, g_value_get_boolean (value));
            break;
        case PROP_READ_ONLY:
            vnc_display_set_read_only (vnc, g_value_get_boolean (value));
            break;
        case PROP_LOSSY_ENCODING:
            vnc_display_set_lossy_encoding (vnc, g_value_get_boolean (value));
            break;
        case PROP_SCALING:
            vnc_display_set_scaling (vnc, g_value_get_boolean (value));
            break;
        case PROP_SHARED_FLAG:
            vnc_display_set_shared_flag (vnc, g_value_get_boolean (value));
            break;
        case PROP_FORCE_SIZE:
            vnc_display_set_force_size (vnc, g_value_get_boolean (value));
            break;
        case PROP_DEPTH:
            vnc_display_set_depth (vnc, g_value_get_enum (value));
            break;
        case PROP_GRAB_KEYS:
            vnc_display_set_grab_keys(vnc, g_value_get_boxed(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
        }
}


/**
 * vnc_display_new:
 *
 * Create a new widget capable of connecting to a VNC server
 * and displaying its contents
 *
 * The widget will initially be in a disconnected state
 *
 * Returns: (transfer full): the new VNC display widget
 */
GtkWidget *vnc_display_new(void)
{
    return GTK_WIDGET(g_object_new(VNC_TYPE_DISPLAY, NULL));
}

static GdkCursor *create_null_cursor(void)
{
    GdkCursor *cursor = gdk_cursor_new(GDK_BLANK_CURSOR);

    return cursor;
}

#ifdef G_OS_WIN32
static HWND win32_window = NULL;

static LRESULT CALLBACK keyboard_hook_cb(int code, WPARAM wparam, LPARAM lparam)
{
    if  (win32_window && code == HC_ACTION && wparam != WM_KEYUP) {
        KBDLLHOOKSTRUCT *hooked = (KBDLLHOOKSTRUCT*)lparam;
        DWORD dwmsg = (hooked->flags << 24) | (hooked->scanCode << 16) | 1;

        if (hooked->vkCode == VK_NUMLOCK || hooked->vkCode == VK_RSHIFT) {
            dwmsg &= ~(1 << 24);
            SendMessage(win32_window, wparam, hooked->vkCode, dwmsg);
        }
        switch (hooked->vkCode) {
        case VK_CAPITAL:
        case VK_SCROLL:
        case VK_NUMLOCK:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_RCONTROL:
        case VK_LMENU:
        case VK_RMENU:
            break;
        case VK_LCONTROL:
            /* When pressing AltGr, an extra VK_LCONTROL with a special
             * scancode with bit 9 set is sent. Let's ignore the extra
             * VK_LCONTROL, as that will make AltGr misbehave. */
            if (hooked->scanCode & 0x200)
                return 1;
            break;
        default:
            SendMessage(win32_window, wparam, hooked->vkCode, dwmsg);
            return 1;
        }
    }
    return CallNextHookEx(NULL, code, wparam, lparam);
}
#endif

static void setup_surface_cache(VncDisplay *dpy, cairo_t *crWin, int w, int h)
{
    VncDisplayPrivate *priv = dpy->priv;
    cairo_surface_t *win = cairo_get_target(crWin);
    cairo_t *crCache;

    if (priv->fbCache)
        return;

    /* Creates a Pixmap on the X11 server matching the Window */
    priv->fbCache = cairo_surface_create_similar(win,
                                                 CAIRO_CONTENT_COLOR,
                                                 w, h);
    crCache = cairo_create(priv->fbCache);

    /* Copy our local framebuffer contents to the Pixmap */
    cairo_set_source_surface(crCache,
                             vnc_cairo_framebuffer_get_surface(priv->fb),
                             0,
                             0);
    cairo_paint(crCache);

    cairo_destroy(crCache);
}

static gboolean draw_event(GtkWidget *widget, cairo_t *cr)
{
    VncDisplay *obj = VNC_DISPLAY(widget);
    VncDisplayPrivate *priv = obj->priv;
    int ww, wh;
    int mx = 0, my = 0;
    int fbw = 0, fbh = 0;

    if (priv->fb) {
        fbw = vnc_framebuffer_get_width(VNC_FRAMEBUFFER(priv->fb));
        fbh = vnc_framebuffer_get_height(VNC_FRAMEBUFFER(priv->fb));

        setup_surface_cache(obj, cr, fbw, fbh);
    }

    gdk_drawable_get_size(gtk_widget_get_window(widget), &ww, &wh);

    if (ww > fbw)
        mx = (ww - fbw) / 2;
    if (wh > fbh)
        my = (wh - fbh) / 2;

    /* If we don't have a pixmap, or we're not scaling, then
       we need to fill with background color */
    if (!priv->fb ||
        !priv->allow_scaling) {
        cairo_rectangle(cr, 0, 0, ww, wh);
        /* Optionally cut out the inner area where the pixmap
           will be drawn. This avoids 'flashing' since we're
           not double-buffering. Note we're using the undocumented
           behaviour of drawing the rectangle from right to left
           to cut out the whole */
        if (priv->fb)
            cairo_rectangle(cr, mx + fbw, my,
                            -1 * fbw, fbh);
        cairo_fill(cr);
    }

    /* Draw the VNC display */
    if (priv->fb) {
        if (priv->allow_scaling) {
            double sx, sy;
            /* Scale to fill window */
            sx = (double)ww / (double)fbw;
            sy = (double)wh / (double)fbh;
            cairo_scale(cr, sx, sy);
            cairo_set_source_surface(cr,
                                     priv->fbCache,
                                     0,
                                     0);
        } else {
            cairo_set_source_surface(cr,
                                     priv->fbCache,
                                     mx,
                                     my);
        }
        cairo_paint(cr);
    }

    return TRUE;
}


#if !GTK_CHECK_VERSION (2, 91, 0)
static gboolean expose_event(GtkWidget *widget, GdkEventExpose *expose)
{
    VncDisplay *obj = VNC_DISPLAY(widget);
    cairo_t *cr;
    gboolean ret;

    cr = gdk_cairo_create(gtk_widget_get_window(GTK_WIDGET(obj)));
    cairo_rectangle(cr,
                    expose->area.x,
                    expose->area.y,
                    expose->area.width,
                    expose->area.height);
    cairo_clip(cr);

    ret = draw_event(widget, cr);

    cairo_destroy(cr);

    return ret;
}
#endif

#if !GTK_CHECK_VERSION(3, 0, 0)
static void do_keyboard_grab_all(GdkWindow *window)
{
    if (window == NULL)
        return;

    gdk_keyboard_grab(window,
                      FALSE,
                      GDK_CURRENT_TIME);
}
static void do_keyboard_ungrab_all(GdkWindow *window G_GNUC_UNUSED)
{
    gdk_keyboard_ungrab(GDK_CURRENT_TIME);
}
static void do_pointer_grab_all(GdkWindow *window,
                                GdkCursor *cursor)
{
    if (window == NULL)
        return;

    gdk_pointer_grab(window,
                     FALSE, /* All events to come to our window directly */
                     GDK_POINTER_MOTION_MASK |
                     GDK_BUTTON_PRESS_MASK |
                     GDK_BUTTON_RELEASE_MASK |
                     GDK_BUTTON_MOTION_MASK |
                     GDK_SCROLL_MASK,
                     NULL, /* Allow cursor to move over entire desktop */
                     cursor,
                     GDK_CURRENT_TIME);
}
static void do_pointer_ungrab_all(GdkWindow *window G_GNUC_UNUSED)
{
    gdk_pointer_ungrab(GDK_CURRENT_TIME);
}
#else
static void do_keyboard_grab_all(GdkWindow *window)
{
    GdkDeviceManager *mgr = gdk_display_get_device_manager(gdk_window_get_display(window));
    GList *devices = gdk_device_manager_list_devices(mgr, GDK_DEVICE_TYPE_MASTER);
    GList *tmp = devices;
    while (tmp) {
        GdkDevice *dev = tmp->data;
        if (gdk_device_get_source(dev) == GDK_SOURCE_KEYBOARD)
            gdk_device_grab(dev,
                            window,
                            GDK_OWNERSHIP_NONE,
                            FALSE,
                            GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK,
                            NULL,
                            GDK_CURRENT_TIME);
        tmp = tmp->next;
    }
    g_list_free(devices);
}

static void do_keyboard_ungrab_all(GdkWindow *window)
{
    GdkDeviceManager *mgr = gdk_display_get_device_manager(gdk_window_get_display(window));
    GList *devices = gdk_device_manager_list_devices(mgr, GDK_DEVICE_TYPE_MASTER);
    GList *tmp = devices;
    while (tmp) {
        GdkDevice *dev = tmp->data;
        if (gdk_device_get_source(dev) == GDK_SOURCE_KEYBOARD)
            gdk_device_ungrab(dev,
                              GDK_CURRENT_TIME);
        tmp = tmp->next;
    }
    g_list_free(devices);
}

static void do_pointer_grab_all(GdkWindow *window,
                                GdkCursor *cursor)
{
    GdkDeviceManager *mgr = gdk_display_get_device_manager(gdk_window_get_display(window));
    GList *devices = gdk_device_manager_list_devices(mgr, GDK_DEVICE_TYPE_MASTER);
    GList *tmp = devices;
    while (tmp) {
        GdkDevice *dev = tmp->data;
        if (gdk_device_get_source(dev) == GDK_SOURCE_MOUSE)
            gdk_device_grab(dev,
                            window,
                            GDK_OWNERSHIP_NONE,
                            FALSE, /* All events to come to our window directly */
                            GDK_POINTER_MOTION_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
                            GDK_BUTTON_MOTION_MASK |
                            GDK_SCROLL_MASK,
                            cursor,
                            GDK_CURRENT_TIME);
        tmp = tmp->next;
    }
    g_list_free(devices);
}

static void do_pointer_ungrab_all(GdkWindow *window)
{
    GdkDeviceManager *mgr = gdk_display_get_device_manager(gdk_window_get_display(window));
    GList *devices = gdk_device_manager_list_devices(mgr, GDK_DEVICE_TYPE_MASTER);
    GList *tmp = devices;
    while (tmp) {
        GdkDevice *dev = tmp->data;
        if (gdk_device_get_source(dev) == GDK_SOURCE_MOUSE)
            gdk_device_ungrab(dev,
                              GDK_CURRENT_TIME);
        tmp = tmp->next;
    }
    g_list_free(devices);
}
#endif

static void do_keyboard_grab(VncDisplay *obj, gboolean quiet)
{
    VncDisplayPrivate *priv = obj->priv;

#ifdef G_OS_WIN32
    if (priv->keyboard_hook == NULL)
        priv->keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook_cb,
                                               GetModuleHandle(NULL), 0);
    g_warn_if_fail(priv->keyboard_hook != NULL);
#endif

    do_keyboard_grab_all(gtk_widget_get_window(GTK_WIDGET(obj)));

    priv->in_keyboard_grab = TRUE;
    if (!quiet)
        g_signal_emit(obj, signals[VNC_KEYBOARD_GRAB], 0);
}


static void do_keyboard_ungrab(VncDisplay *obj, gboolean quiet)
{
    VncDisplayPrivate *priv = obj->priv;

    do_keyboard_ungrab_all(gtk_widget_get_window(GTK_WIDGET(obj)));

#ifdef G_OS_WIN32
    if (priv->keyboard_hook != NULL) {
        UnhookWindowsHookEx(priv->keyboard_hook);
        priv->keyboard_hook = NULL;
    }
#endif

    priv->in_keyboard_grab = FALSE;
    if (!quiet)
        g_signal_emit(obj, signals[VNC_KEYBOARD_UNGRAB], 0);
}

static void do_pointer_hide(VncDisplay *obj)
{
    VncDisplayPrivate *priv = obj->priv;
    GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(obj));

    if (window == NULL)
        return;

    gdk_window_set_cursor(window,
                          priv->remote_cursor ? priv->remote_cursor : priv->null_cursor);
}

static void do_pointer_show(VncDisplay *obj)
{
    VncDisplayPrivate *priv = obj->priv;
    GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(obj));

    if (window == NULL)
        return;

    gdk_window_set_cursor(window, priv->remote_cursor);
}

static void do_pointer_grab(VncDisplay *obj, gboolean quiet)
{
    VncDisplayPrivate *priv = obj->priv;

    /* If we're not already grabbing keyboard, grab it now */
    if (!priv->grab_keyboard)
        do_keyboard_grab(obj, quiet);

    /*
     * For relative mouse to work correctly when grabbed we need to
     * allow the pointer to move anywhere on the local desktop, so
     * use NULL for the 'confine_to' argument. Furthermore we need
     * the coords to be reported to our VNC window, regardless of
     * what window the pointer is actally over, so use 'FALSE' for
     * 'owner_events' parameter
     */
    do_pointer_grab_all(gtk_widget_get_window(GTK_WIDGET(obj)),
                        priv->remote_cursor ? priv->remote_cursor : priv->null_cursor);
    priv->in_pointer_grab = TRUE;
    if (!quiet)
        g_signal_emit(obj, signals[VNC_POINTER_GRAB], 0);
}

static void do_pointer_ungrab(VncDisplay *obj, gboolean quiet)
{
    VncDisplayPrivate *priv = obj->priv;

    /* If we grabbed keyboard upon pointer grab, then ungrab it now */
    if (!priv->grab_keyboard)
        do_keyboard_ungrab(obj, quiet);

    do_pointer_ungrab_all(gtk_widget_get_window(GTK_WIDGET(obj)));
    priv->in_pointer_grab = FALSE;

    if (priv->absolute)
        do_pointer_hide(obj);

    if (!quiet)
        g_signal_emit(obj, signals[VNC_POINTER_UNGRAB], 0);
}

/**
 * vnc_display_force_grab:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to force pointer grabbing, FALSE otherwise
 *
 * If @enable is TRUE, immediately grab the pointer.
 * If @enable is FALSE, immediately ungrab the pointer.
 * This overrides any automatic grabs that may have
 * been done.
 */
void vnc_display_force_grab(VncDisplay *obj, gboolean enable)
{
    if (enable)
        do_pointer_grab(obj, FALSE);
    else
        do_pointer_ungrab(obj, FALSE);
}

static gboolean button_event(GtkWidget *widget, GdkEventButton *button)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;
    int n;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (priv->read_only)
        return FALSE;

    gtk_widget_grab_focus (widget);

    if (priv->grab_pointer && !priv->absolute && !priv->in_pointer_grab &&
        button->button == 1 && button->type == GDK_BUTTON_PRESS)
        do_pointer_grab(VNC_DISPLAY(widget), FALSE);

    n = 1 << (button->button - 1);
    if (button->type == GDK_BUTTON_PRESS)
        priv->button_mask |= n;
    else if (button->type == GDK_BUTTON_RELEASE)
        priv->button_mask &= ~n;

    if (priv->absolute) {
        vnc_connection_pointer_event(priv->conn, priv->button_mask,
                                     priv->last_x, priv->last_y);
    } else {
        vnc_connection_pointer_event(priv->conn, priv->button_mask,
                                     0x7FFF, 0x7FFF);
    }

    return TRUE;
}

static gboolean scroll_event(GtkWidget *widget, GdkEventScroll *scroll)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;
    int mask;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (priv->read_only)
        return FALSE;

    if (scroll->direction == GDK_SCROLL_UP)
        mask = (1 << 3);
    else if (scroll->direction == GDK_SCROLL_DOWN)
        mask = (1 << 4);
    else if (scroll->direction == GDK_SCROLL_LEFT)
        mask = (1 << 5);
    else if (scroll->direction == GDK_SCROLL_RIGHT)
        mask = (1 << 6);
    else
        return FALSE;

    if (priv->absolute) {
        vnc_connection_pointer_event(priv->conn, priv->button_mask | mask,
                                     priv->last_x, priv->last_y);
        vnc_connection_pointer_event(priv->conn, priv->button_mask,
                                     priv->last_x, priv->last_y);
    } else {
        vnc_connection_pointer_event(priv->conn, priv->button_mask | mask,
                                     0x7FFF, 0x7FFF);
        vnc_connection_pointer_event(priv->conn, priv->button_mask,
                                     0x7FFF, 0x7FFF);
    }

    return TRUE;
}


/*
 * There are several scenarios to considier when handling client
 * mouse motion events:
 *
 *  - Mouse in relative mode + centered rendering of desktop
 *  - Mouse in relative mode + scaled rendering of desktop
 *  - Mouse in absolute mode + centered rendering of desktop
 *  - Mouse in absolute mode + scaled rendering of desktop
 *
 * Once scaled / offset, absolute mode is easy.
 *
 * Relative mode has a couple of special complications
 *
 *  - Need to turn client absolute events into a delta
 *  - Need to warp local pointer to avoid hitting a wall
 */
static gboolean motion_event(GtkWidget *widget, GdkEventMotion *motion)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;
    int ww, wh;
    int fbw, fbh;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (!priv->fb)
        return FALSE;

    fbw = vnc_framebuffer_get_width(VNC_FRAMEBUFFER(priv->fb));
    fbh = vnc_framebuffer_get_height(VNC_FRAMEBUFFER(priv->fb));

    /* In relative mode, only move the server mouse pointer
     * if the client grab is active */
    if (!priv->absolute && !priv->in_pointer_grab)
        return FALSE;

    if (priv->read_only)
        return FALSE;

    gdk_drawable_get_size(gtk_widget_get_window(widget), &ww, &wh);

    /* First apply adjustments to the coords in the motion event */
    if (priv->allow_scaling) {
        double sx, sy;
        sx = (double)fbw / (double)ww;
        sy = (double)fbh / (double)wh;

        /* Scaling the desktop, so scale the mouse coords
         * by same ratio */
        motion->x *= sx;
        motion->y *= sy;
    } else {
        int mw = 0, mh = 0;

        if (ww > fbw)
            mw = (ww - fbw) / 2;
        if (wh > fbh)
            mh = (wh - fbh) / 2;

        /* Not scaling, drawing the desktop centered
         * in the larger window, so offset the mouse
         * coords to match centering */
        motion->x -= mw;
        motion->y -= mh;
    }

    /* Next adjust the real client pointer */
    if (!priv->absolute) {
        GdkScreen *screen = gtk_widget_get_screen(widget);
        int x = (int)motion->x_root;
        int y = (int)motion->y_root;

        /* In relative mode check to see if client pointer hit
         * one of the screen edges, and if so move it back by
         * 200 pixels. This is important because the pointer
         * in the server doesn't correspond 1-for-1, and so
         * may still be only half way across the screen. Without
         * this warp, the server pointer would thus appear to hit
         * an invisible wall */
        if (x <= 0) x += 200;
        if (y <= 0) y += 200;
        if (x >= (gdk_screen_get_width(screen) - 1)) x -= 200;
        if (y >= (gdk_screen_get_height(screen) - 1)) y -= 200;

        if (x != (int)motion->x_root || y != (int)motion->y_root) {
#if GTK_CHECK_VERSION(3, 0, 0)
            GdkDevice *dev = gdk_event_get_device((GdkEvent*)motion);
            gdk_device_warp(dev, screen, x, y);
#else
            GdkDisplay *display = gtk_widget_get_display(widget);
            gdk_display_warp_pointer(display, screen, x, y);
#endif
            priv->last_x = -1;
            priv->last_y = -1;
            return FALSE;
        }
    }

    /* Finally send the event to server */
    if (priv->last_x != -1) {
        int dx, dy;
        if (priv->absolute) {
            dx = (int)motion->x;
            dy = (int)motion->y;

            /* If the co-ords are out of bounds we want to clamp
             * them to the boundaries. We don't want to actually
             * drop the events though, because even if the X coord
             * is out of bounds we want the server to see Y coord
             * changes, and vica-verca. */
            if (dx < 0)
                dx = 0;
            if (dy < 0)
                dy = 0;
            if (dx >= fbw)
                dx = fbw - 1;
            if (dy >= fbh)
                dy = fbh - 1;
        } else {
            /* Just send the delta since last motion event */
            dx = (int)motion->x + 0x7FFF - priv->last_x;
            dy = (int)motion->y + 0x7FFF - priv->last_y;
        }

        vnc_connection_pointer_event(priv->conn, priv->button_mask, dx, dy);
    }

    priv->last_x = (int)motion->x;
    priv->last_y = (int)motion->y;

    return TRUE;
}


/*
 * Lets say the grab sequence of Ctrl_L + Alt_L
 *
 * We first need to detect when both Ctrl_L and Alt_L are pressed.
 * When this happens we are "primed" to tigger.
 *
 * If any further key is pressed though, we unprime ourselves
 *
 * If any key is released while we are primed, then we
 * trigger.
 */
static gboolean check_for_grab_key(GtkWidget *widget, int type, int keyval)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;
    int i;

    if (!priv->vncgrabseq->nkeysyms)
        return FALSE;

    if (type == GDK_KEY_RELEASE) {
        gboolean active = priv->vncgrabpending;
        /* Any key release resets the whole grab sequence */
        memset(priv->vncactiveseq, 0,
               sizeof(gboolean)*priv->vncgrabseq->nkeysyms);
        priv->vncgrabpending = FALSE;
        return active;
    } else {
        gboolean setone = FALSE;

        /* Record the new key press */
        for (i = 0 ; i < priv->vncgrabseq->nkeysyms ; i++) {
            if (priv->vncgrabseq->keysyms[i] == keyval) {
                priv->vncactiveseq[i] = TRUE;
                setone = TRUE;
            }
        }

        if (setone) {
            /* Return if any key is not pressed */
            for (i = 0 ; i < priv->vncgrabseq->nkeysyms ; i++)
                if (priv->vncactiveseq[i] == FALSE)
                    return FALSE;

            /* All keys in grab seq are pressed, so prime
             * to trigger on release
             */
            priv->vncgrabpending = TRUE;
        } else {
            /* Key not in grab seq, so must reset any pending
             * grab keys we have */
            memset(priv->vncactiveseq, 0,
                   sizeof(gboolean)*priv->vncgrabseq->nkeysyms);
            priv->vncgrabpending = FALSE;
        }

        return FALSE;
    }
}


static gboolean key_event(GtkWidget *widget, GdkEventKey *key)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;
    int i;
    int keyval = key->keyval;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (priv->read_only)
        return FALSE;

    VNC_DEBUG("%s keycode: %d  state: %u  group %d, keyval: %d",
              key->type == GDK_KEY_PRESS ? "press" : "release",
              key->hardware_keycode, key->state, key->group, keyval);

#ifdef G_OS_WIN32
    /* on windows, we ought to ignore the reserved key event? */
    if (key->hardware_keycode == 0xff)
        return FALSE;

    if (!priv->in_keyboard_grab) {
        if (key->hardware_keycode == VK_LWIN ||
            key->hardware_keycode == VK_RWIN ||
            key->hardware_keycode == VK_APPS)
            return FALSE;
    }
#endif

    keyval = vnc_display_keyval_from_keycode(key->hardware_keycode, keyval);

    /*
     * Some VNC suckiness with key state & modifiers in particular
     *
     * Because VNC has no concept of modifiers, we have to track what keys are
     * pressed and when the widget looses focus send fake key up events for all
     * keys current held down. This is because upon gaining focus any keys held
     * down are no longer likely to be down. This would thus result in keys
     * being 'stuck on' in the remote server. eg upon Alt-Tab to switch window
     * focus you'd never see key up for the Alt or Tab keys without this :-(
     *
     * This is mostly a problem with modifier keys, but its best to just track
     * all key presses regardless. There's a limit to how many keys a user can
     * press at once due to a max of 10 fingers (normally :-), so down_key_vals
     * is only storing upto 16 for now. Should be plenty...
     *
     * Arggggh.
     */

    /*
     * First the key release handling. This is *always* run, even for Key press
     * events, because GTK will often merge sequential press+release pairs of
     * the same key into a sequence of press+press+press+press+release. VNC
     * servers don't like this, so we have to see if we're already pressed
     * send release events. So, we run the release handling code all the time.
     */
    for (i = 0 ; i < (int)(sizeof(priv->down_keyval)/sizeof(priv->down_keyval[0])) ; i++) {
        /* We were pressed, and now we're released, so... */
        if (priv->down_scancode[i] == key->hardware_keycode) {
            guint16 scancode = vnc_display_keymap_gdk2rfb(priv->keycode_map,
                                                          priv->keycode_maplen,
                                                          key->hardware_keycode);
#ifdef G_OS_WIN32
            /* MapVirtualKey doesn't return scancode with needed higher byte */
            scancode = MapVirtualKey(key->hardware_keycode, MAPVK_VK_TO_VSC) |
                (scancode & 0xff00);
#endif
            /*
             * ..send the key release event we're dealing with
             *
             * NB, we use priv->down_keyval[i], and not our
             * current 'keyval', because we need to make sure
             * that the release keyval is identical to the
             * press keyval. In some layouts, this isn't always
             * true, with "Tab" generating Tab on press, and
             * ISO_Prev_Group on release.
             */
            vnc_connection_key_event(priv->conn, 0, priv->down_keyval[i], scancode);
            priv->down_keyval[i] = 0;
            priv->down_scancode[i] = 0;
            break;
        }
    }

    if (key->type == GDK_KEY_PRESS) {
        for (i = 0 ; i < (int)(sizeof(priv->down_keyval)/sizeof(priv->down_keyval[0])) ; i++) {
            if (priv->down_scancode[i] == 0) {
                guint16 scancode = vnc_display_keymap_gdk2rfb(priv->keycode_map,
                                                              priv->keycode_maplen,
                                                              key->hardware_keycode);
#ifdef G_OS_WIN32
                /* MapVirtualKey doesn't return scancode with needed higher byte */
                scancode = MapVirtualKey(key->hardware_keycode, MAPVK_VK_TO_VSC) |
                    (scancode & 0xff00);
#endif
                priv->down_keyval[i] = keyval;
                priv->down_scancode[i] = key->hardware_keycode;
                /* Send the actual key event we're dealing with */
                vnc_connection_key_event(priv->conn, 1, keyval, scancode);
                break;
            }
        }
    }

    if (check_for_grab_key(widget, key->type, key->keyval)) {
        if (priv->in_pointer_grab)
            do_pointer_ungrab(VNC_DISPLAY(widget), FALSE);
        else if (!priv->grab_keyboard || !priv->absolute)
            do_pointer_grab(VNC_DISPLAY(widget), FALSE);
    }

    return TRUE;
}

static gboolean enter_event(GtkWidget *widget, GdkEventCrossing *crossing G_GNUC_UNUSED)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (priv->grab_keyboard)
        do_keyboard_grab(VNC_DISPLAY(widget), FALSE);

    if (priv->local_pointer)
        do_pointer_show(VNC_DISPLAY(widget));

#ifdef G_OS_WIN32
    win32_window = gdk_win32_window_get_impl_hwnd(gtk_widget_get_window(widget));
#endif

    return TRUE;
}

static gboolean leave_event(GtkWidget *widget, GdkEventCrossing *crossing G_GNUC_UNUSED)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (priv->grab_keyboard)
        do_keyboard_ungrab(VNC_DISPLAY(widget), FALSE);

    if (priv->local_pointer)
        do_pointer_hide(VNC_DISPLAY(widget));

    if (priv->grab_pointer && !priv->absolute)
        do_pointer_ungrab(VNC_DISPLAY(widget), FALSE);

    return TRUE;
}

static void release_keys(VncDisplay *display)
{
    VncDisplayPrivate *priv = display->priv;
    int i;

    for (i = 0 ; i < (int)(sizeof(priv->down_keyval)/sizeof(priv->down_keyval[0])) ; i++) {
        /* We are currently pressed so... */
        if (priv->down_scancode[i] != 0) {
            guint16 scancode = vnc_display_keymap_gdk2rfb(priv->keycode_map,
                                                          priv->keycode_maplen,
                                                          priv->down_scancode[i]);
            /* ..send the fake key release event to match */
            vnc_connection_key_event(priv->conn, 0,
                                     priv->down_keyval[i], scancode);
            priv->down_keyval[i] = 0;
            priv->down_scancode[i] = 0;
        }
    }
}

static gboolean focus_in_event(GtkWidget *widget, GdkEventFocus *focus G_GNUC_UNUSED)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    if (!gtk_widget_get_realized(widget))
        return TRUE;

#ifdef G_OS_WIN32
    win32_window = gdk_win32_window_get_impl_hwnd(gtk_widget_get_window(widget));
#endif

    return TRUE;
}


static gboolean focus_out_event(GtkWidget *widget, GdkEventFocus *focus G_GNUC_UNUSED)
{
    VncDisplayPrivate *priv = VNC_DISPLAY(widget)->priv;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return FALSE;

    release_keys(VNC_DISPLAY(widget));
#ifdef G_OS_WIN32
    win32_window = NULL;
#endif

    return TRUE;
}


static void grab_notify(GtkWidget *widget, gboolean was_grabbed)
{
    if (was_grabbed == FALSE)
        release_keys(VNC_DISPLAY(widget));
}


static void realize_event(GtkWidget *widget)
{
    VncDisplay *obj = VNC_DISPLAY(widget);
    VncDisplayPrivate *priv = obj->priv;

    GTK_WIDGET_CLASS (vnc_display_parent_class)->realize (widget);

    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(obj)),
                          priv->remote_cursor ? priv->remote_cursor : priv->null_cursor);
}


static void on_framebuffer_update(VncConnection *conn G_GNUC_UNUSED,
                                  int x, int y, int w, int h,
                                  gpointer opaque)
{
    GtkWidget *widget = GTK_WIDGET(opaque);
    VncDisplay *obj = VNC_DISPLAY(widget);
    VncDisplayPrivate *priv = obj->priv;
    int ww, wh;
    int fbw, fbh;

    fbw = vnc_framebuffer_get_width(VNC_FRAMEBUFFER(priv->fb));
    fbh = vnc_framebuffer_get_height(VNC_FRAMEBUFFER(priv->fb));

    gdk_drawable_get_size(gtk_widget_get_window(widget), &ww, &wh);

    /* If we have a pixmap, update the region which changed.
     * If we don't have a pixmap, the entire thing will be
     * created & rendered during the drawing handler
     */
    if (priv->fbCache) {
        cairo_t *cr = cairo_create(priv->fbCache);
        cairo_surface_t *surface = vnc_cairo_framebuffer_get_surface(priv->fb);

        cairo_rectangle(cr, x, y, w, h);
        cairo_clip(cr);
        cairo_set_source_surface(cr, surface, 0, 0);
        cairo_paint(cr);

        cairo_destroy(cr);
    }

    if (priv->allow_scaling) {
        double sx, sy;

        /* Scale the VNC region to produce expose region */

        sx = (double)ww / (double)fbw;
        sy = (double)wh / (double)fbh;

        x *= sx;
        y *= sy;
        w *= sx;
        h *= sy;

        /* Without this, we get horizontal & vertical line artifacts
         * when drawing. This "fix" is somewhat dubious though. The
         * true mistake & fix almost certainly lies elsewhere.
         */
        x -= 2;
        y -= 2;
        w += 4;
        h += 4;
    } else {
        int mw = 0, mh = 0;

        /* Offset the VNC region to produce expose region */

        if (ww > fbw)
            mw = (ww - fbw) / 2;
        if (wh > fbh)
            mh = (wh - fbh) / 2;

        x += mw;
        y += mh;
    }

    gtk_widget_queue_draw_area(widget, x, y, w, h);

    vnc_connection_framebuffer_update_request(priv->conn, 1,
                                              0, 0,
                                              vnc_connection_get_width(priv->conn),
                                              vnc_connection_get_height(priv->conn));
}


static void do_framebuffer_init(VncDisplay *obj,
                                const VncPixelFormat *remoteFormat,
                                int width, int height, gboolean quiet)
{
    VncDisplayPrivate *priv = obj->priv;

    if (priv->conn == NULL || !vnc_connection_is_initialized(priv->conn))
        return;

    if (priv->fb) {
        g_object_unref(priv->fb);
        priv->fb = NULL;
    }
    if (priv->fbCache) {
        cairo_surface_destroy(priv->fbCache);
        priv->fbCache = NULL;
    }

    if (priv->null_cursor == NULL) {
        priv->null_cursor = create_null_cursor();
        if (priv->local_pointer)
            do_pointer_show(obj);
        else if (priv->in_pointer_grab || priv->absolute)
            do_pointer_hide(obj);
    }

    priv->fb = vnc_cairo_framebuffer_new(width, height, remoteFormat);
    vnc_connection_set_framebuffer(priv->conn, VNC_FRAMEBUFFER(priv->fb));

    if (priv->force_size)
        gtk_widget_set_size_request(GTK_WIDGET(obj), width, height);

    if (!quiet) {
        g_signal_emit(G_OBJECT(obj),
                      signals[VNC_DESKTOP_RESIZE],
                      0,
                      width, height);
    }
}

static void on_desktop_resize(VncConnection *conn G_GNUC_UNUSED,
                              int width, int height,
                              gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;
    const VncPixelFormat *remoteFormat;

    remoteFormat = vnc_connection_get_pixel_format(priv->conn);

    do_framebuffer_init(opaque, remoteFormat, width, height, FALSE);

    vnc_connection_framebuffer_update_request(priv->conn, 0, 0, 0, width, height);
}

static void on_pixel_format_changed(VncConnection *conn G_GNUC_UNUSED,
                                    VncPixelFormat *remoteFormat,
                                    gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;
    gint16 width = vnc_connection_get_width(priv->conn);
    gint16 height = vnc_connection_get_height(priv->conn);

    do_framebuffer_init(opaque, remoteFormat, width, height, TRUE);

    vnc_connection_framebuffer_update_request(priv->conn, 0, 0, 0, width, height);
}

static gboolean vnc_display_set_preferred_pixel_format(VncDisplay *display)
{
    VncDisplayPrivate *priv = display->priv;
    GdkVisual *v =  gtk_widget_get_visual(GTK_WIDGET(display));
    VncPixelFormat fmt;
    const VncPixelFormat *currentFormat;

    memset(&fmt, 0, sizeof(fmt));

    /* Get current pixel format for server */
    currentFormat = vnc_connection_get_pixel_format(priv->conn);

    switch (priv->depth) {
    case VNC_DISPLAY_DEPTH_COLOR_DEFAULT:
        VNC_DEBUG ("Using default colour depth %d (%d bpp) (true color? %d)",
                   currentFormat->depth, currentFormat->bits_per_pixel,
                   currentFormat->true_color_flag);
        /* TigerVNC always sends back the encoding even if
           unchanged from what the server suggested. This is
           important with some VNC servers, since they won't
           otherwise send us the colour map entries */
        memcpy(&fmt, currentFormat, sizeof(fmt));
        break;

    case VNC_DISPLAY_DEPTH_COLOR_FULL:
        fmt.depth = 24;
        fmt.bits_per_pixel = 32;
        fmt.red_max = 255;
        fmt.green_max = 255;
        fmt.blue_max = 255;
        fmt.red_shift = 16;
        fmt.green_shift = 8;
        fmt.blue_shift = 0;
        fmt.true_color_flag = 1;
        break;

    case VNC_DISPLAY_DEPTH_COLOR_MEDIUM:
        fmt.depth = 15;
        fmt.bits_per_pixel = 16;
        fmt.red_max = 31;
        fmt.green_max = 31;
        fmt.blue_max = 31;
        fmt.red_shift = 11;
        fmt.green_shift = 6;
        fmt.blue_shift = 1;
        fmt.true_color_flag = 1;
        break;

    case VNC_DISPLAY_DEPTH_COLOR_LOW:
        fmt.depth = 8;
        fmt.bits_per_pixel = 8;
        fmt.red_max = 7;
        fmt.green_max = 7;
        fmt.blue_max = 3;
        fmt.red_shift = 5;
        fmt.green_shift = 2;
        fmt.blue_shift = 0;
        fmt.true_color_flag = 1;
        break;

    case VNC_DISPLAY_DEPTH_COLOR_ULTRA_LOW:
        fmt.depth = 3;
        fmt.bits_per_pixel = 8;
        fmt.red_max = 1;
        fmt.green_max = 1;
        fmt.blue_max = 1;
        fmt.red_shift = 7;
        fmt.green_shift = 6;
        fmt.blue_shift = 5;
        fmt.true_color_flag = 1;
        break;

    default:
        g_assert_not_reached ();
    }

#if GTK_CHECK_VERSION (2, 21, 1)
    fmt.byte_order = gdk_visual_get_byte_order (v) == GDK_LSB_FIRST ? G_LITTLE_ENDIAN : G_BIG_ENDIAN;
#else
    fmt.byte_order = v->byte_order == GDK_LSB_FIRST ? G_LITTLE_ENDIAN : G_BIG_ENDIAN;
#endif

    VNC_DEBUG ("Set depth color to %d (%d bpp)", fmt.depth, fmt.bits_per_pixel);
    if (!vnc_connection_set_pixel_format(priv->conn, &fmt))
        return FALSE;

    return TRUE;
}

static void on_pointer_mode_changed(VncConnection *conn G_GNUC_UNUSED,
                                    gboolean absPointer,
                                    gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;

    if (absPointer && priv->in_pointer_grab && priv->grab_pointer)
        do_pointer_ungrab(obj, FALSE);

    priv->absolute = absPointer;

    if (!priv->in_pointer_grab && !priv->absolute)
        do_pointer_show(obj);
}

static void on_auth_cred(VncConnection *conn G_GNUC_UNUSED,
                         GValueArray *creds,
                         gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    GValueArray *newCreds = g_value_array_new(0);
    gsize i;

    for (i = 0 ; i < creds->n_values ; i++) {
        GValue *cred = g_value_array_get_nth(creds, i);
        GValue newCred;
        memset(&newCred, 0, sizeof(newCred));
        g_value_init(&newCred, VNC_TYPE_DISPLAY_CREDENTIAL);
        /* Take advantage that VncDisplayCredential &
         * VncConnectionCredential share same enum values
         */
        g_value_set_enum(&newCred, g_value_get_enum(cred));
        newCreds = g_value_array_append(newCreds, &newCred);
    }

    g_signal_emit(G_OBJECT(obj), signals[VNC_AUTH_CREDENTIAL], 0, newCreds);

    g_value_array_free(newCreds);
}

static void on_auth_choose_type(VncConnection *conn,
                                GValueArray *types,
                                gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;
    GSList *l;
    guint i;

    if (!types->n_values) {
        VNC_DEBUG("No auth types available to choose from");
        vnc_connection_shutdown(conn);
        return;
    }

    for (l = priv->preferable_auths; l; l=l->next) {
        int pref = GPOINTER_TO_UINT (l->data);

        for (i=0; i< types->n_values; i++) {
            GValue *type = g_value_array_get_nth(types, i);
            if (pref == g_value_get_enum(type)) {
                vnc_connection_set_auth_type(conn, pref);
                return;
            }
        }
    }

    /* No sub-auth matching our supported auth so have to give up */
    VNC_DEBUG("No preferred auth type found");
    vnc_connection_shutdown(conn);
}

static void on_auth_choose_subtype(VncConnection *conn,
                                   unsigned int type,
                                   GValueArray *subtypes,
                                   gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;
    GSList *l;
    guint i;

    if (!subtypes->n_values) {
        VNC_DEBUG("No subtypes available to choose from");
        vnc_connection_shutdown(conn);
        return;
    }

    if (type == VNC_CONNECTION_AUTH_TLS) {
        l = priv->preferable_auths;
    } else if (type == VNC_CONNECTION_AUTH_VENCRYPT) {
        l = priv->preferable_vencrypt_subauths;
    } else {
        VNC_DEBUG("Unexpected stackable auth type %u", type);
        vnc_connection_shutdown(conn);
        return;
    }

    for (; l; l=l->next) {
        int pref = GPOINTER_TO_UINT (l->data);

        /* Don't want to recursively do the same major auth */
        if (pref == type)
            continue;

        for (i=0; i< subtypes->n_values; i++) {
            GValue *subtype = g_value_array_get_nth(subtypes, i);
            if (pref == g_value_get_enum(subtype)) {
                vnc_connection_set_auth_subtype(conn, pref);
                return;
            }
        }
    }

    /* No sub-auth matching our supported auth so have to give up */
    VNC_DEBUG("No preferred auth subtype found");
    vnc_connection_shutdown(conn);
}

static void on_auth_failure(VncConnection *conn G_GNUC_UNUSED,
                            const char *reason,
                            gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);

    g_signal_emit(G_OBJECT(obj), signals[VNC_AUTH_FAILURE], 0, reason);
}

static void on_auth_unsupported(VncConnection *conn G_GNUC_UNUSED,
                                unsigned int authType,
                                gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);

    g_signal_emit(G_OBJECT(obj), signals[VNC_AUTH_UNSUPPORTED], 0, authType);
}

static void on_server_cut_text(VncConnection *conn G_GNUC_UNUSED,
                               const gchar *text,
                               gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);

    if (obj->priv->read_only)
        return;

    g_signal_emit(G_OBJECT(obj), signals[VNC_SERVER_CUT_TEXT], 0, text);
}

static void on_bell(VncConnection *conn G_GNUC_UNUSED,
                    gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);

    g_signal_emit(G_OBJECT(obj), signals[VNC_BELL], 0);
}

static void on_cursor_changed(VncConnection *conn G_GNUC_UNUSED,
                              VncCursor *cursor,
                              gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;

    VNC_DEBUG("Cursor changed %p x=%d y=%d w=%d h=%d",
              cursor,
              cursor ? vnc_cursor_get_hotx(cursor) : -1,
              cursor ? vnc_cursor_get_hoty(cursor) : -1,
              cursor ? vnc_cursor_get_width(cursor) : -1,
              cursor ? vnc_cursor_get_height(cursor) : -1);

    if (priv->remote_cursor) {
        gdk_cursor_unref(priv->remote_cursor);
        priv->remote_cursor = NULL;
    }

    if (cursor) {
        GdkDisplay *display = gtk_widget_get_display(GTK_WIDGET(obj));
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(vnc_cursor_get_data(cursor),
                                                     GDK_COLORSPACE_RGB,
                                                     TRUE, 8,
                                                     vnc_cursor_get_width(cursor),
                                                     vnc_cursor_get_height(cursor),
                                                     vnc_cursor_get_width(cursor) * 4,
                                                     NULL, NULL);
        priv->remote_cursor = gdk_cursor_new_from_pixbuf(display,
                                                         pixbuf,
                                                         vnc_cursor_get_hotx(cursor),
                                                         vnc_cursor_get_hoty(cursor));
        g_object_unref(pixbuf);
    }

    if (priv->in_pointer_grab) {
        do_pointer_ungrab(obj, TRUE);
        do_pointer_grab(obj, TRUE);
    } else if (priv->absolute) {
        do_pointer_hide(obj);
    }
}

static gboolean check_pixbuf_support(const char *name)
{
    GSList *list, *i;

    list = gdk_pixbuf_get_formats();

    for (i = list; i; i = i->next) {
        GdkPixbufFormat *fmt = i->data;
        gchar *fmt_name = gdk_pixbuf_format_get_name(fmt);
        int cmp;

        cmp = strcmp(fmt_name, name);
        g_free (fmt_name);

        if (!cmp)
            break;
    }

    g_slist_free(list);

    return !!(i);
}

static void on_connected(VncConnection *conn G_GNUC_UNUSED,
                         gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);

    g_signal_emit(G_OBJECT(obj), signals[VNC_CONNECTED], 0);
    VNC_DEBUG("Connected to VNC server");
}


static void on_error(VncConnection *conn G_GNUC_UNUSED,
                     const char *message,
                     gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);

    g_signal_emit(G_OBJECT(obj), signals[VNC_ERROR], 0, message);
    VNC_DEBUG("VNC server error");
}


static void on_initialized(VncConnection *conn G_GNUC_UNUSED,
                           gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VncDisplayPrivate *priv = obj->priv;
    int i;

    /* The order determines which encodings the
     * server prefers when it has a choice to use */
    gint32 encodings[] = {  VNC_CONNECTION_ENCODING_TIGHT_JPEG5,
                            VNC_CONNECTION_ENCODING_TIGHT,
                            VNC_CONNECTION_ENCODING_EXT_KEY_EVENT,
                            VNC_CONNECTION_ENCODING_DESKTOP_RESIZE,
                            VNC_CONNECTION_ENCODING_WMVi,
                            VNC_CONNECTION_ENCODING_AUDIO,
                            VNC_CONNECTION_ENCODING_RICH_CURSOR,
                            VNC_CONNECTION_ENCODING_XCURSOR,
                            VNC_CONNECTION_ENCODING_POINTER_CHANGE,
                            VNC_CONNECTION_ENCODING_ZRLE,
                            VNC_CONNECTION_ENCODING_HEXTILE,
                            VNC_CONNECTION_ENCODING_RRE,
                            VNC_CONNECTION_ENCODING_COPY_RECT,
                            VNC_CONNECTION_ENCODING_RAW };
    int n_encodings = G_N_ELEMENTS(encodings);

#define REMOVE_ENCODING(e)                              \
    for (i = 0 ; i < n_encodings ; i++) {               \
        if (encodings[i] == e) {                        \
            if (i < (n_encodings - 1))                  \
                memmove(encodings + i,                  \
                        encodings + (i + 1),            \
                        sizeof(gint32) *                \
                        (n_encodings - (i + 1)));       \
            n_encodings--;                              \
            VNC_DEBUG("Removed encoding %d", e);        \
            break;                                      \
        }                                               \
    }

    if (!vnc_display_set_preferred_pixel_format(obj))
        goto error;

    do_framebuffer_init(obj,
                        vnc_connection_get_pixel_format(priv->conn),
                        vnc_connection_get_width(priv->conn),
                        vnc_connection_get_height(priv->conn),
                        FALSE);

    if (check_pixbuf_support("jpeg")) {
        if (!priv->allow_lossy)
            REMOVE_ENCODING(VNC_CONNECTION_ENCODING_TIGHT_JPEG5);
    } else {
        REMOVE_ENCODING(VNC_CONNECTION_ENCODING_TIGHT_JPEG5);
        REMOVE_ENCODING(VNC_CONNECTION_ENCODING_TIGHT);
    }

    if (priv->keycode_map == NULL)
        REMOVE_ENCODING(VNC_CONNECTION_ENCODING_EXT_KEY_EVENT);

    VNC_DEBUG("Sending %d encodings", n_encodings);
    if (!vnc_connection_set_encodings(priv->conn, n_encodings, encodings))
        goto error;

    VNC_DEBUG("Requesting first framebuffer update");
    if (!vnc_connection_framebuffer_update_request(priv->conn, 0, 0, 0,
                                                   vnc_connection_get_width(priv->conn),
                                                   vnc_connection_get_height(priv->conn)))
        goto error;

    g_signal_emit(G_OBJECT(obj), signals[VNC_INITIALIZED], 0);

    VNC_DEBUG("Initialized VNC server");
    return;

 error:
    vnc_connection_shutdown(priv->conn);
}


static void on_disconnected(VncConnection *conn G_GNUC_UNUSED,
                            gpointer opaque)
{
    VncDisplay *obj = VNC_DISPLAY(opaque);
    VNC_DEBUG("Disconnected from VNC server");

    g_signal_emit(G_OBJECT(obj), signals[VNC_DISCONNECTED], 0);
    g_object_unref(G_OBJECT(obj));
}


/**
 * vnc_display_open_fd:
 * @obj: (transfer none): the VNC display widget
 * @fd: file descriptor to use for the connection
 *
 * Open a connection using @fd as the transport. If @fd
 * refers to a TCP connection, it is recommended to use
 * vnc_display_open_fd_with_hostname instead, to
 * provide the remote hostname. This allows use of
 * x509 based authentication which requires a hostname
 * to be available.
 *
 * Returns: TRUE if a connection was opened, FALSE if already open
 */
gboolean vnc_display_open_fd(VncDisplay *obj, int fd)
{
    VncDisplayPrivate *priv = obj->priv;

    if (vnc_connection_is_open(priv->conn))
        return FALSE;

    if (!vnc_connection_set_shared(priv->conn, priv->shared_flag))
        return FALSE;

    if (!vnc_connection_open_fd(priv->conn, fd))
        return FALSE;

    g_object_ref(G_OBJECT(obj));

    return TRUE;
}


/**
 * vnc_display_open_fd_with_hostname:
 * @obj: (transfer none): the VNC display widget
 * @fd: file descriptor to use for the connection
 * @hostname: (transfer none)(nullable): the host associated with the connection
 *
 * Open a connection using @fd as the transport. The
 * @hostname provided should reflect the name of the
 * host that the @fd provides a connection to. This
 * will be used by some authentication schemes, for
 * example x509 certificate validation against @hostname.
 *
 * Returns: TRUE if a connection was opened, FALSE if already open
 */
gboolean vnc_display_open_fd_with_hostname(VncDisplay *obj, int fd, const char *hostname)
{
    VncDisplayPrivate *priv = obj->priv;

    if (vnc_connection_is_open(priv->conn))
        return FALSE;

    if (!vnc_connection_set_shared(priv->conn, priv->shared_flag))
        return FALSE;

    if (!vnc_connection_open_fd_with_hostname(priv->conn, fd, hostname))
        return FALSE;

    g_object_ref(G_OBJECT(obj));

    return TRUE;
}


/**
 * vnc_display_open_addr:
 * @obj: (transfer none): the VNC display widget
 * @addr: (transfer none): the socket address
 * @hostname: (transfer none)(nullable): the hostname
 *
 * Open a socket connection to server identified by @addr.
 * @addr may refer to either a TCP address (IPv4/6) or
 * a UNIX socket address. The @hostname provided should
 * reflect the name of the host that the @addr provides a
 * connection to, if it is not already available in @addr.
 * For example, if @addr points to a proxy server, then
 * @hostname can be used to provide the name of the final
 * endpoint. This will be used by some authentication
 * schemes, for example x509 certificate validation
 * against @hostname.
 *
 * Returns: TRUE if a connection was opened, FALSE if already open
 */
gboolean vnc_display_open_addr(VncDisplay *obj, GSocketAddress *addr, const char *hostname)
{
    VncDisplayPrivate *priv = obj->priv;

    if (vnc_connection_is_open(priv->conn))
        return FALSE;

    if (!vnc_connection_set_shared(priv->conn, priv->shared_flag))
        return FALSE;

    if (!vnc_connection_open_addr(priv->conn, addr, hostname))
        return FALSE;

    g_object_ref(G_OBJECT(obj));

    return TRUE;
}


/**
 * vnc_display_open_host:
 * @obj: (transfer none): the VNC display widget
 * @host: (transfer none): the host name or IP address
 * @port: (transfer none): the service name or port number
 *
 * Open a TCP connection to the remote desktop at @host
 * listening on @port.
 *
 * Returns: TRUE if a connection was opened, FALSE if already open
 */
gboolean vnc_display_open_host(VncDisplay *obj, const char *host, const char *port)
{
    VncDisplayPrivate *priv = obj->priv;

    if (vnc_connection_is_open(priv->conn))
        return FALSE;

    if (!vnc_connection_set_shared(priv->conn, priv->shared_flag))
        return FALSE;

    if (!vnc_connection_open_host(priv->conn, host, port))
        return FALSE;

    g_object_ref(G_OBJECT(obj));

    return TRUE;
}


/**
 * vnc_display_is_open:
 * @obj: (transfer none): the VNC display widget
 *
 * Check if the connection for the display is currently open
 *
 * Returns: TRUE if open, FALSE if closing/closed
 */
gboolean vnc_display_is_open(VncDisplay *obj)
{
    VncDisplayPrivate *priv = obj->priv;

    return vnc_connection_is_open(priv->conn);
}


/**
 * vnc_display_close:
 * @obj: (transfer none): the VNC display widget
 *
 * Request that the connection to the remote display
 * is closed. The actual close will complete asynchronously
 * and the "vnc-disconnected" signal will be emitted once
 * complete.
 */
void vnc_display_close(VncDisplay *obj)
{
    VncDisplayPrivate *priv = obj->priv;
    GtkWidget *widget = GTK_WIDGET(obj);

    if (vnc_connection_is_open(priv->conn)) {
        VNC_DEBUG("Requesting graceful shutdown of connection");
        vnc_connection_shutdown(priv->conn);
    }

    if (gtk_widget_get_window(widget)) {
        gint width, height;

        gdk_drawable_get_size(gtk_widget_get_window(widget), &width, &height);
        gtk_widget_queue_draw_area(widget, 0, 0, width, height);
    }
}


/**
 * vnc_display_get_connection:
 * @obj: (transfer none): the VNC display widget
 *
 * Get the VNC connection object associated with the
 * display
 *
 * Returns: (transfer none): the connection object
 */
VncConnection * vnc_display_get_connection(VncDisplay *obj)
{
    VncDisplayPrivate *priv = obj->priv;
    return priv->conn;
}


/**
 * vnc_display_send_keys:
 * @obj: (transfer none): the VNC display widget
 * @keyvals: (array length=nkeyvals): Keyval array
 * @nkeyvals: Length of keyvals
 *
 * Send keyval click events to the display. Al the
 * key press events will be sent first and then all
 * the key release events.
 *
 * @keyvals should contain the X11 key value constants
 */
void vnc_display_send_keys(VncDisplay *obj, const guint *keyvals, int nkeyvals)
{
    vnc_display_send_keys_ex(obj, keyvals,
                             nkeyvals, VNC_DISPLAY_KEY_EVENT_CLICK);
}

static guint get_scancode_from_keyval(VncDisplay *obj, guint keyval)
{
    VncDisplayPrivate *priv = obj->priv;
    guint keycode = 0;
    GdkKeymapKey *keys = NULL;
    gint n_keys = 0;

    if (gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(),
                                          keyval, &keys, &n_keys)) {
        /* FIXME what about levels? */
        keycode = keys[0].keycode;
        g_free(keys);
    }

    return vnc_display_keymap_gdk2rfb(priv->keycode_map, priv->keycode_maplen, keycode);
}


/**
 * vnc_display_send_keys_ex:
 * @obj: (transfer none): the VNC display widget
 * @keyvals: (array length=nkeyvals): Keyval array
 * @nkeyvals: Length of keyvals
 * @kind: the type of event to send
 *
 * Sends key events to the remote server. @keyvals
 * should contain X11 key code values. These will
 * be automatically converted to XT scancodes if
 * needed
 *
 * If @kind is VNC_DISPLAY_KEY_EVENT_CLICK then all
 * the key press events will be sent first, followed
 * by all the key release events.
 */
void vnc_display_send_keys_ex(VncDisplay *obj, const guint *keyvals,
                              int nkeyvals, VncDisplayKeyEvent kind)
{
    int i;

    if (obj->priv->conn == NULL || !vnc_connection_is_open(obj->priv->conn) || obj->priv->read_only)
        return;

    if (kind & VNC_DISPLAY_KEY_EVENT_PRESS) {
        for (i = 0 ; i < nkeyvals ; i++)
            vnc_connection_key_event(obj->priv->conn, 1, keyvals[i],
                                     get_scancode_from_keyval(obj, keyvals[i]));
    }

    if (kind & VNC_DISPLAY_KEY_EVENT_RELEASE) {
        for (i = (nkeyvals-1) ; i >= 0 ; i--)
            vnc_connection_key_event(obj->priv->conn, 0, keyvals[i],
                                     get_scancode_from_keyval(obj, keyvals[i]));
    }
}


/**
 * vnc_display_send_pointer:
 * @obj: (transfer none): the VNC display widget
 * @x: the desired horizontal position
 * @y: the desired vertical position
 * @button_mask: the state of the buttons
 *
 * Move the remote pointer to position (@x, @y) and set the
 * button state to @button_mask.  This method will only
 * work if the desktop is using absolute pointer mode. It
 * will be a no-op if in relative pointer mode.
 */
void vnc_display_send_pointer(VncDisplay *obj, gint x, gint y, int button_mask)
{
    VncDisplayPrivate *priv = obj->priv;

    if (priv->conn == NULL || !vnc_connection_is_open(obj->priv->conn))
        return;

    if (priv->absolute) {
        priv->button_mask = button_mask;
        priv->last_x = x;
        priv->last_y = y;
        vnc_connection_pointer_event(priv->conn, priv->button_mask, x, y);
    }
}

static void vnc_display_destroy (GtkObject *obj)
{
    VncDisplay *display = VNC_DISPLAY (obj);
    VNC_DEBUG("Display destroy, requesting that VNC connection close");
    vnc_display_close(display);
    GTK_OBJECT_CLASS (vnc_display_parent_class)->destroy (obj);
}


static void vnc_display_finalize (GObject *obj)
{
    VncDisplay *display = VNC_DISPLAY (obj);
    VncDisplayPrivate *priv = display->priv;

    VNC_DEBUG("Releasing VNC widget");
    if (vnc_connection_is_open(priv->conn)) {
        g_warning("VNC widget finalized before the connection finished shutting down\n");
    }
    g_object_unref(G_OBJECT(priv->conn));
    display->priv->conn = NULL;

    if (priv->fb) {
        g_object_unref(priv->fb);
        priv->fb = NULL;
    }
    if (priv->fbCache) {
        cairo_surface_destroy(priv->fbCache);
        priv->fbCache = NULL;
    }

    if (priv->null_cursor) {
        gdk_cursor_unref (priv->null_cursor);
        priv->null_cursor = NULL;
    }

    if (priv->remote_cursor) {
        gdk_cursor_unref(priv->remote_cursor);
        priv->remote_cursor = NULL;
    }

    if (priv->vncgrabseq) {
        vnc_grab_sequence_free(priv->vncgrabseq);
        priv->vncgrabseq = NULL;
    }

    g_slist_free (priv->preferable_auths);
    g_slist_free (priv->preferable_vencrypt_subauths);

    vnc_display_keyval_free_entries();

    G_OBJECT_CLASS (vnc_display_parent_class)->finalize (obj);
}

static void vnc_display_class_init(VncDisplayClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass *gtkwidget_class = GTK_WIDGET_CLASS (klass);

#if GTK_CHECK_VERSION (2, 91, 0)
    gtkwidget_class->draw = draw_event;
#else
    gtkwidget_class->expose_event = expose_event;
#endif
    gtkwidget_class->motion_notify_event = motion_event;
    gtkwidget_class->button_press_event = button_event;
    gtkwidget_class->button_release_event = button_event;
    gtkwidget_class->scroll_event = scroll_event;
    gtkwidget_class->key_press_event = key_event;
    gtkwidget_class->key_release_event = key_event;
    gtkwidget_class->enter_notify_event = enter_event;
    gtkwidget_class->leave_notify_event = leave_event;
    gtkwidget_class->focus_in_event = focus_in_event;
    gtkwidget_class->focus_out_event = focus_out_event;
    gtkwidget_class->grab_notify = grab_notify;
    gtkwidget_class->realize = realize_event;

    object_class->finalize = vnc_display_finalize;
    object_class->get_property = vnc_display_get_property;
    object_class->set_property = vnc_display_set_property;

    gtkobject_class->destroy = vnc_display_destroy;

    g_object_class_install_property (object_class,
                                     PROP_POINTER_LOCAL,
                                     g_param_spec_boolean ( "local-pointer",
                                                            "Local Pointer",
                                                            "Whether we should use the local pointer",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_POINTER_GRAB,
                                     g_param_spec_boolean ( "grab-pointer",
                                                            "Grab Pointer",
                                                            "Whether we should grab the pointer",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_KEYBOARD_GRAB,
                                     g_param_spec_boolean ( "grab-keyboard",
                                                            "Grab Keyboard",
                                                            "Whether we should grab the keyboard",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_READ_ONLY,
                                     g_param_spec_boolean ( "read-only",
                                                            "Read Only",
                                                            "Whether this connection is read-only mode",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_WIDTH,
                                     g_param_spec_int     ( "width",
                                                            "Width",
                                                            "The width of the remote screen",
                                                            0,
                                                            G_MAXINT,
                                                            0,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_HEIGHT,
                                     g_param_spec_int     ( "height",
                                                            "Height",
                                                            "The height of the remote screen",
                                                            0,
                                                            G_MAXINT,
                                                            0,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_NAME,
                                     g_param_spec_string  ( "name",
                                                            "Name",
                                                            "The screen name of the remote connection",
                                                            NULL,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_LOSSY_ENCODING,
                                     g_param_spec_boolean ( "lossy-encoding",
                                                            "Lossy Encoding",
                                                            "Whether we should use a lossy encoding",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_SCALING,
                                     g_param_spec_boolean ( "scaling",
                                                            "Scaling",
                                                            "Whether we should use scaling",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_SHARED_FLAG,
                                     g_param_spec_boolean ( "shared-flag",
                                                            "Shared Flag",
                                                            "Whether we should leave other clients connected to the server",
                                                            FALSE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_FORCE_SIZE,
                                     g_param_spec_boolean ( "force-size",
                                                            "Force widget size",
                                                            "Whether we should define the widget size",
                                                            TRUE,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));

    g_object_class_install_property (object_class,
                                     PROP_DEPTH,
                                     g_param_spec_enum    ( "depth",
                                                            "Depth",
                                                            "The color depth",
                                                            VNC_TYPE_DISPLAY_DEPTH_COLOR,
                                                            VNC_DISPLAY_DEPTH_COLOR_DEFAULT,
                                                            G_PARAM_READWRITE |
                                                            G_PARAM_CONSTRUCT |
                                                            G_PARAM_STATIC_NAME |
                                                            G_PARAM_STATIC_NICK |
                                                            G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_GRAB_KEYS,
                                     g_param_spec_boxed( "grab-keys",
                                                         "Grab keys",
                                                         "The key grab sequence",
                                                         VNC_TYPE_GRAB_SEQUENCE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_CONSTRUCT |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_NICK |
                                                         G_PARAM_STATIC_BLURB));
    g_object_class_install_property (object_class,
                                     PROP_CONNECTION,
                                     g_param_spec_object("connection",
                                                         "Connection",
                                                         "The VNC connection",
                                                         VNC_TYPE_CONNECTION,
                                                         G_PARAM_READABLE |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_NICK |
                                                         G_PARAM_STATIC_BLURB));

    signals[VNC_CONNECTED] =
        g_signal_new ("vnc-connected",
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (VncDisplayClass, vnc_connected),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);

    signals[VNC_INITIALIZED] =
        g_signal_new ("vnc-initialized",
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (VncDisplayClass, vnc_initialized),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);

    signals[VNC_DISCONNECTED] =
        g_signal_new ("vnc-disconnected",
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (VncDisplayClass, vnc_disconnected),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);

    signals[VNC_ERROR] =
        g_signal_new ("vnc-error",
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL, NULL,
                      g_cclosure_marshal_VOID__STRING,
                      G_TYPE_NONE,
                      1,
                      G_TYPE_STRING);

    signals[VNC_AUTH_CREDENTIAL] =
        g_signal_new ("vnc-auth-credential",
                      G_OBJECT_CLASS_TYPE (object_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (VncDisplayClass, vnc_auth_credential),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__BOXED,
                      G_TYPE_NONE,
                      1,
                      G_TYPE_VALUE_ARRAY);


    signals[VNC_POINTER_GRAB] =
        g_signal_new("vnc-pointer-grab",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE,
                     0);

    signals[VNC_POINTER_UNGRAB] =
        g_signal_new("vnc-pointer-ungrab",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE,
                     0);

    signals[VNC_KEYBOARD_GRAB] =
        g_signal_new("vnc-keyboard-grab",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE,
                     0);

    signals[VNC_KEYBOARD_UNGRAB] =
        g_signal_new("vnc-keyboard-ungrab",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE,
                     0);


    signals[VNC_DESKTOP_RESIZE] =
        g_signal_new("vnc-desktop-resize",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_user_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2,
                     G_TYPE_INT, G_TYPE_INT);

    signals[VNC_AUTH_FAILURE] =
        g_signal_new("vnc-auth-failure",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__STRING,
                     G_TYPE_NONE,
                     1,
                     G_TYPE_STRING);

    signals[VNC_AUTH_UNSUPPORTED] =
        g_signal_new("vnc-auth-unsupported",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__UINT,
                     G_TYPE_NONE,
                     1,
                     G_TYPE_UINT);

    signals[VNC_SERVER_CUT_TEXT] =
        g_signal_new("vnc-server-cut-text",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__STRING,
                     G_TYPE_NONE,
                     1,
                     G_TYPE_STRING);

    signals[VNC_BELL] =
        g_signal_new("vnc-bell",
                     G_TYPE_FROM_CLASS(klass),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                     0,
                     NULL,
                     NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE,
                     0);

    g_type_class_add_private(klass, sizeof(VncDisplayPrivate));
}

static void vnc_display_init(VncDisplay *display)
{
    GtkWidget *widget = GTK_WIDGET(display);
    VncDisplayPrivate *priv;

    gtk_widget_set_can_focus (widget, TRUE);

    vnc_display_keyval_set_entries();

    gtk_widget_add_events(widget,
                          GDK_POINTER_MOTION_MASK |
                          GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_BUTTON_MOTION_MASK |
                          GDK_ENTER_NOTIFY_MASK |
                          GDK_LEAVE_NOTIFY_MASK |
                          GDK_SCROLL_MASK |
                          GDK_KEY_PRESS_MASK);
    /* We already have off-screen buffers we render to
     * but with GTK-3 there are problems with overlaid
     * windows. We end up rendering over the top of the
     * child overlaid windows despite having a clip
     * mask set :-( We've turned on GTK3's built-in
     * double buffering to work around this until we
     * find a better idea.
     */
#if GTK_CHECK_VERSION(3, 0, 0)
    gtk_widget_set_double_buffered(widget, TRUE);
#else
    gtk_widget_set_double_buffered(widget, FALSE);
#endif

    priv = display->priv = VNC_DISPLAY_GET_PRIVATE(display);
    memset(priv, 0, sizeof(VncDisplayPrivate));
    priv->last_x = -1;
    priv->last_y = -1;
    priv->absolute = TRUE;
    priv->read_only = FALSE;
    priv->allow_lossy = FALSE;
    priv->allow_scaling = FALSE;
    priv->grab_pointer = FALSE;
    priv->grab_keyboard = FALSE;
    priv->local_pointer = FALSE;
    priv->shared_flag = FALSE;
    priv->force_size = TRUE;
    priv->vncgrabseq = vnc_grab_sequence_new_from_string("Control_L+Alt_L");
    priv->vncactiveseq = g_new0(gboolean, priv->vncgrabseq->nkeysyms);

    /*
     * Both these two provide TLS based auth, and can layer
     * all the other auth types on top. So these two must
     * be the first listed
     */
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_VENCRYPT));
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_TLS));

    /*
     * Then stackable auth types in order of preference
     */
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_SASL));
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_MSLOGON));
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_ARD));
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_VNC));

    /*
     * Or nothing at all
     */
    priv->preferable_auths = g_slist_append (priv->preferable_auths, GUINT_TO_POINTER (VNC_CONNECTION_AUTH_NONE));


    /* Prefered order for VeNCrypt subtypes */
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_X509SASL));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_X509PLAIN));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_X509VNC));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_X509NONE));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_TLSSASL));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_TLSPLAIN));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_TLSVNC));
    priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
                                                        GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_TLSNONE));
    /*
     * Refuse fully cleartext passwords
     priv->preferable_vencrypt_subauths = g_slist_append(priv->preferable_vencrypt_subauths,
     GUINT_TO_POINTER(VNC_CONNECTION_AUTH_VENCRYPT_PLAIN));
    */

    priv->conn = vnc_connection_new();

    g_signal_connect(G_OBJECT(priv->conn), "vnc-cursor-changed",
                     G_CALLBACK(on_cursor_changed), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-pointer-mode-changed",
                     G_CALLBACK(on_pointer_mode_changed), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-bell",
                     G_CALLBACK(on_bell), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-server-cut-text",
                     G_CALLBACK(on_server_cut_text), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-framebuffer-update",
                     G_CALLBACK(on_framebuffer_update), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-desktop-resize",
                     G_CALLBACK(on_desktop_resize), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-pixel-format-changed",
                     G_CALLBACK(on_pixel_format_changed), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-auth-failure",
                     G_CALLBACK(on_auth_failure), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-auth-unsupported",
                     G_CALLBACK(on_auth_unsupported), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-auth-credential",
                     G_CALLBACK(on_auth_cred), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-auth-choose-type",
                     G_CALLBACK(on_auth_choose_type), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-auth-choose-subtype",
                     G_CALLBACK(on_auth_choose_subtype), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-connected",
                     G_CALLBACK(on_connected), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-initialized",
                     G_CALLBACK(on_initialized), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-disconnected",
                     G_CALLBACK(on_disconnected), display);
    g_signal_connect(G_OBJECT(priv->conn), "vnc-error",
                     G_CALLBACK(on_error), display);

    priv->keycode_map = vnc_display_keymap_gdk2rfb_table(&priv->keycode_maplen);
}


/**
 * vnc_display_set_credential:
 * @obj: (transfer none): the VNC display widget
 * @type: the authentication credential type
 * @data: (transfer none): the value associated with the credential
 *
 * Sets the value of the authentication credential
 * @type to the string @data.
 *
 * @type is one of the VncConnectionCredential enum vlaues
 *
 * Returns: TRUE if an error occurs, FALSE otherwise
 */
gboolean vnc_display_set_credential(VncDisplay *obj, int type, const gchar *data)
{
    return !vnc_connection_set_credential(obj->priv->conn, type, data);
}


/**
 * vnc_display_set_poiter_local:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to show a local cursor, FALSE otherwise
 *
 * If @enable is TRUE, then a local mouse cursor will be
 * made visible. If @enable is FALSE, the local mouse
 * cursor will be hidden.
 */
void vnc_display_set_pointer_local(VncDisplay *obj, gboolean enable)
{
    if (obj->priv->null_cursor) {
        if (enable)
            do_pointer_show(obj);
        else if (obj->priv->in_pointer_grab || obj->priv->absolute)
            do_pointer_hide(obj);
    }
    obj->priv->local_pointer = enable;
}


/**
 * vnc_display_set_pointer_grab:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to enable automatic pointer grab, FALSE otherwise
 *
 * Set whether the widget will automatically grab the mouse
 * pointer upon a button click
 */
void vnc_display_set_pointer_grab(VncDisplay *obj, gboolean enable)
{
    VncDisplayPrivate *priv = obj->priv;

    priv->grab_pointer = enable;
    if (!enable && priv->absolute && priv->in_pointer_grab)
        do_pointer_ungrab(obj, FALSE);
}


/**
 * vnc_display_set_grab_keys:
 * @obj: (transfer none): the VNC display widget
 * @seq: (transfer none): the new grab sequence
 *
 * Set the sequence of keys that must be pressed to
 * activate keyborad and pointer grab
 */
void vnc_display_set_grab_keys(VncDisplay *obj, VncGrabSequence *seq)
{
    obj->priv->vncgrabpending = FALSE;
    if (obj->priv->vncgrabseq) {
        vnc_grab_sequence_free(obj->priv->vncgrabseq);
        g_free(obj->priv->vncactiveseq);
    }
    if (seq)
        obj->priv->vncgrabseq = vnc_grab_sequence_copy(seq);
    else
        obj->priv->vncgrabseq = vnc_grab_sequence_new_from_string("Control_L+Alt_L");
    obj->priv->vncactiveseq = g_new0(gboolean, obj->priv->vncgrabseq->nkeysyms);
    if (G_UNLIKELY(vnc_util_get_debug())) {
        gchar *str = vnc_grab_sequence_as_string(obj->priv->vncgrabseq);
        VNC_DEBUG("Grab sequence is now %s", str);
        g_free(str);
    }
}


/**
 * vnc_display_get_grab_keys:
 * @obj: (transfer none): the VNC display widget
 *
 * Get the current grab key sequence
 *
 * Returns: (transfer none): the current grab keys
 */
VncGrabSequence *vnc_display_get_grab_keys(VncDisplay *obj)
{
    return obj->priv->vncgrabseq;
}


/**
 * vnc_display_set_keyboard_grab:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to enable keyboard grab, FALSE otherwise
 *
 * Set whether the widget will grab the keyboard when it
 * has focus. Grabbing the keyboard allows it to intercept
 * special key sequences, ensuring they get sent to the
 * remote desktop, rather than intepreted locally.
 */
void vnc_display_set_keyboard_grab(VncDisplay *obj, gboolean enable)
{
    VncDisplayPrivate *priv = obj->priv;

    priv->grab_keyboard = enable;
    if (!enable && priv->in_keyboard_grab && !priv->in_pointer_grab)
        do_keyboard_ungrab(obj, FALSE);
}


/**
 * vnc_display_set_read_only:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to enable read-only mode, FALSE otherwise
 *
 * Set whether the widget is running in read-only mode. In
 * read-only mode, keyboard and mouse events will not be
 * sent to the remote desktop server. The widget will merely
 * display activity from the server.
 */
void vnc_display_set_read_only(VncDisplay *obj, gboolean enable)
{
    obj->priv->read_only = enable;
}

static void vnc_display_convert_data(GdkPixbuf *pixbuf,
                                     cairo_surface_t *surface,
                                     int      width,
                                     int      height)
{
    int x, y;
    guchar  *dest_data = gdk_pixbuf_get_pixels(pixbuf);
    int      dest_stride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar  *src_data = cairo_image_surface_get_data(surface);
    int      src_stride = cairo_image_surface_get_stride(surface);

    for (y = 0; y < height; y++) {
        guint32 *src = (guint32 *) src_data;
        for (x = 0; x < width; x++) {
            dest_data[x * 3 + 0] = src[x] >> 16;
            dest_data[x * 3 + 1] = src[x] >>  8;
            dest_data[x * 3 + 2] = src[x];
        }

        src_data += src_stride;
        dest_data += dest_stride;
    }
}

/**
 * vnc_display_get_pixbuf:
 * @obj: (transfer none): the VNC display widget
 *
 * Take a screenshot of the display.
 *
 * Returns: (transfer full): a #GdkPixbuf with the screenshot image buffer
 */
GdkPixbuf *vnc_display_get_pixbuf(VncDisplay *obj)
{
    VncDisplayPrivate *priv = obj->priv;
    VncFramebuffer *fb;
    cairo_content_t content;
    cairo_surface_t *surface;
    GdkPixbuf *pixbuf;

    if (!priv->conn ||
        !vnc_connection_is_initialized(priv->conn))
        return NULL;

    if (!priv->fb)
        return NULL;

    fb = VNC_FRAMEBUFFER(priv->fb);
    surface = vnc_cairo_framebuffer_get_surface(priv->fb);
    content = cairo_surface_get_content(surface) | CAIRO_CONTENT_COLOR;
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                            !!(content & CAIRO_CONTENT_ALPHA),
                            8,
                            vnc_framebuffer_get_width(fb),
                            vnc_framebuffer_get_height(fb));

    vnc_display_convert_data(pixbuf, surface,
                             vnc_framebuffer_get_width(fb),
                             vnc_framebuffer_get_height(fb));

    return pixbuf;
}


/**
 * vnc_display_get_width:
 * @obj: (transfer none): the VNC display widget
 *
 * Get the width of the remote desktop. This is only
 * valid after the "vnc-initialized" signal has been
 * emitted
 *
 * Returns: the remote desktop width
 */
int vnc_display_get_width(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), -1);

    return vnc_connection_get_width (obj->priv->conn);
}


/**
 * vnc_display_get_height:
 * @obj: (transfer none): the VNC display widget
 *
 * Get the height of the remote desktop. This is only
 * valid after the "vnc-initialized" signal has been
 * emitted
 *
 * Returns: the remote desktop height
 */
int vnc_display_get_height(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), -1);

    return vnc_connection_get_height (obj->priv->conn);
}


/**
 * vnc_display_get_name:
 * @obj: (transfer none): the VNC display widget
 *
 * Get the name of the remote desktop. This is only
 * valid after the "vnc-initialized" signal has been
 * emitted
 *
 * Returns: (transfer none): the remote desktop name
 */
const char * vnc_display_get_name(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), NULL);

    return vnc_connection_get_name (obj->priv->conn);
}


/**
 * vnc_display_cut_text:
 * @obj: (transfer none): the VNC display widget
 * @text: (transfer none): the clipboard text
 *
 * Send a text string to the remote desktop clipboard. The
 * encoding for @text is undefined, but it is recommended
 * to use UTF-8.
 */
void vnc_display_client_cut_text(VncDisplay *obj, const gchar *text)
{
    g_return_if_fail (VNC_IS_DISPLAY (obj));

    if (!obj->priv->read_only)
        vnc_connection_client_cut_text(obj->priv->conn, text, strlen (text));
}


/**
 * vnc_display_set_lossy_encoding:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to permit lossy encodings, FALSE otherwise
 *
 * Set whether the client is willing to accept lossy
 * framebuffer update encodings. Lossy encodings can
 * improve performance by lowering network bandwidth
 * requirements, with a cost that the display received
 * by the client will not be pixel perfect
 */
void vnc_display_set_lossy_encoding(VncDisplay *obj, gboolean enable)
{
    g_return_if_fail (VNC_IS_DISPLAY (obj));
    obj->priv->allow_lossy = enable;
}


/**
 * vnc_display_set_shared_flag:
 * @obj: (transfer none): the VNC display widget
 * @shared: the new sharing state
 *
 * Set the shared state for the connection. A TRUE value
 * allow allow this client to co-exist with other existing
 * clients. A FALSE value will cause other clients to be
 * dropped
 */
void vnc_display_set_shared_flag(VncDisplay *obj, gboolean shared)
{
    g_return_if_fail (VNC_IS_DISPLAY (obj));
    obj->priv->shared_flag = shared;
}


/**
 * vnc_display_set_scaling:
 * @obj: (transfer none): the VNC display widget
 * @enable: TRUE to allow scaling the desktop to fit, FALSE otherwise
 *
 * Set whether the remote desktop contents is automatically
 * scaled to fit the available widget size, or whether it
 * will be rendered at 1:1 size
 *
 * Returns: TRUE always
 */
gboolean vnc_display_set_scaling(VncDisplay *obj,
                                 gboolean enable)
{
    int ww, wh;

    obj->priv->allow_scaling = enable;

    if (obj->priv->fb != NULL) {
        GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(obj));

        if (window != NULL) {
            gdk_drawable_get_size(gtk_widget_get_window(GTK_WIDGET(obj)),
                                  &ww, &wh);
            gtk_widget_queue_draw_area(GTK_WIDGET(obj), 0, 0, ww, wh);
        }
    }

    return TRUE;
}


/**
 * vnc_display_force_size:
 * @obj: (transfer none): the VNC display widget
 * @enabled: TRUE to force the widget size, FALSE otherwise
 *
 * Set whether the widget size will be forced to match the
 * remote desktop size. If the widget size does not match
 * the remote desktop size, and scaling is disabled, some
 * of the remote desktop may be hidden, or black borders
 * may be drawn.
 */
void vnc_display_set_force_size(VncDisplay *obj, gboolean enabled)
{
    g_return_if_fail (VNC_IS_DISPLAY (obj));
    obj->priv->force_size = enabled;
}


/**
 * vnc_display_set_depth:
 * @obj: (transfer none): the VNC display widget
 * @depth: the desired colour depth
 *
 * Set the desired colour depth. Higher quality colour
 * depths will require greater network bandwidth. The
 * colour depth must be set prior to connecting to the
 * remote server
 */
void vnc_display_set_depth(VncDisplay *obj, VncDisplayDepthColor depth)
{
    g_return_if_fail (VNC_IS_DISPLAY (obj));

    /* Ignore if we are already connected */
    if (obj->priv->conn && vnc_connection_is_initialized(obj->priv->conn))
        return;

    if (obj->priv->depth == depth)
        return;

    obj->priv->depth = depth;
}


/**
 * vnc_display_get_depth:
 * @obj: (transfer none): the VNC display widget
 *
 * Get the desired colour depth
 *
 * Returns: the color depth
 */
VncDisplayDepthColor vnc_display_get_depth(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), 0);

    return obj->priv->depth;
}


/**
 * vnc_display_get_force_size:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine whether the widget size is being forced
 * to match the desktop size
 *
 * Returns: TRUE if force size is enabled, FALSE otherwise
 */
gboolean vnc_display_get_force_size(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->force_size;
}


/**
 * vnc_display_get_scaling:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine whether the widget is permitted to
 * scale the remote desktop to fit the current
 * widget size.
 *
 * Returns: TRUE if scaling is permitted, FALSE otherwise
 */
gboolean vnc_display_get_scaling(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->allow_scaling;
}


/**
 * vnc_display_get_lossy_encoding:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine whether lossy framebuffer update encodings
 * are permitted
 *
 * Returns: TRUE if lossy encodings are permitted, FALSE otherwie
 */
gboolean vnc_display_get_lossy_encoding(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->allow_lossy;
}


/**
 * vnc_display_get_shared_flag:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine if other clients are permitted to
 * share the VNC connection
 *
 * Returns: TRUE if sharing is permittted, FALSE otherwise
 */
gboolean vnc_display_get_shared_flag(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->shared_flag;
}


/**
 * vnc_display_get_pointer_local:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine if a local pointer will be shown
 *
 * Returns: TRUE if a local pointer is shown, FALSE otherwise
 */
gboolean vnc_display_get_pointer_local(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->local_pointer;
}


/**
 * vnc_display_get_pointer_grab:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine if the mouse pointer will be grabbed
 * on first click
 *
 * Returns: TRUE if the pointer will be grabbed, FALSE otherwise
 */
gboolean vnc_display_get_pointer_grab(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->grab_pointer;
}


/**
 * vnc_display_get_keyboard_grab:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine if the keyboard will be grabbed when the
 * widget has input focus.
 *
 * Returns: TRUE if the keyboard will be grabbed, FALSE otherwise
 */
gboolean vnc_display_get_keyboard_grab(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->grab_keyboard;
}


/**
 * vnc_display_get_read_only:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine if the widget will operate in read-only
 * mode, denying keyboard/mouse inputs
 *
 * Returns: TRUE if in read-only mode, FALSE otherwise
 */
gboolean vnc_display_get_read_only(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    return obj->priv->read_only;
}


/**
 * vnc_display_is_pointer_absolute:
 * @obj: (transfer none): the VNC display widget
 *
 * Determine if the pointer is operating in absolute
 * mode. This is only valid after the "vnc-initialized"
 * signal has been emitted
 *
 * Returns: TRUE if in absolute mode, FALSE for relative mode
 */
gboolean vnc_display_is_pointer_absolute(VncDisplay *obj)
{
    return obj->priv->absolute;
}


/**
 * vnc_display_get_option_group:
 *
 * Get a command line option group containing VNC specific
 * options.
 *
 * Returns: (transfer full): the option group
 */
GOptionGroup *
vnc_display_get_option_group (void)
{
    GOptionGroup *group;

    group = g_option_group_new ("gtk-vnc", N_("GTK-VNC Options:"), N_("Show GTK-VNC Options"), NULL, NULL);
    g_option_group_set_translation_domain (group, GETTEXT_PACKAGE);

    g_option_group_add_entries (group, gtk_vnc_args);

    return group;
}


/**
 * vnc_display_get_option_entries:
 *
 * Get the array of command line option entries containing
 * VNC specific otions
 *
 * Returns: (array zero-terminated=1): the option entries
 */
const GOptionEntry *
vnc_display_get_option_entries (void)
{
    return gtk_vnc_args;
}


/**
 * vnc_display_:
 * @obj: (transfer none): the VNC display widget
 */
gboolean
vnc_display_request_update(VncDisplay *obj)
{
    g_return_val_if_fail (VNC_IS_DISPLAY (obj), FALSE);

    if (!obj->priv->conn || !vnc_connection_is_initialized(obj->priv->conn))
        return FALSE;

    VNC_DEBUG ("Requesting a full update");
    return vnc_connection_framebuffer_update_request(obj->priv->conn,
                                                     0,
                                                     0,
                                                     0,
                                                     vnc_connection_get_width(obj->priv->conn),
                                                     vnc_connection_get_width(obj->priv->conn));
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
