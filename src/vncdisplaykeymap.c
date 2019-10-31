/*
 * GTK VNC Widget
 *
 * Copyright (C) 2009-2010 Daniel P. Berrange <dan@berrange.com>
 * Copyright (C) 2017 Red Hat, Inc.
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

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>

#include "vncdisplaykeymap.h"
#include "vncutil.h"


#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#endif

#ifdef GDK_WINDOWING_BROADWAY
#include <gdk/gdkbroadway.h>
#endif

#if defined(GDK_WINDOWING_X11) || defined(GDK_WINDOWING_WAYLAND)
/* Wayland or Xorg Linux + evdev (offset evdev keycodes) */
#include "vncdisplaykeymap_xorgevdev2qnum.h"
#endif

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#include <X11/XKBlib.h>
#include <stdbool.h>
#include <string.h>

/* Xorg Linux + kbd (offset + mangled XT keycodes) */
#include "vncdisplaykeymap_xorgkbd2qnum.h"
/* Xorg OS-X aka XQuartz (offset OS-X keycodes) */
#include "vncdisplaykeymap_xorgxquartz2qnum.h"
/* Xorg Cygwin aka XWin (offset + mangled XT keycodes) */
#include "vncdisplaykeymap_xorgxwin2qnum.h"

/* Gtk2 compat */
#ifndef GDK_IS_X11_DISPLAY
#define GDK_IS_X11_DISPLAY(dpy) (dpy != NULL)
#endif
#endif

#ifdef GDK_WINDOWING_WIN32
/* Win32 native virtual keycodes */
#include "vncdisplaykeymap_win322qnum.h"

/* Gtk2 compat */
#ifndef GDK_IS_WIN32_DISPLAY
#define GDK_IS_WIN32_DISPLAY(dpy) (dpy != NULL)
#endif
#endif

#ifdef GDK_WINDOWING_BROADWAY
/* X11 keysyms */
#include "vncdisplaykeymap_x112qnum.h"

/* Gtk2 compat */
#ifndef GDK_IS_BROADWAY_DISPLAY
#define GDK_IS_BROADWAY_DISPLAY(dpy) (dpy != NULL)
#endif

#endif

#ifdef GDK_WINDOWING_QUARTZ
/* OS-X native keycodes */
#include "vncdisplaykeymap_osx2qnum.h"

/* Gtk2 compat */
#ifndef GDK_IS_QUARTZ_DISPLAY
#define GDK_IS_QUARTZ_DISPLAY(dpy) (dpy != NULL)
#endif
#endif

#ifdef GDK_WINDOWING_X11

#define STRPREFIX(a,b) (strncmp((a),(b),strlen((b))) == 0)

static gboolean check_for_xwin(GdkDisplay *dpy)
{
    char *vendor = ServerVendor(gdk_x11_display_get_xdisplay(dpy));

    VNC_DEBUG("Server vendor is '%s'", vendor);

    if (strstr(vendor, "Cygwin/X"))
        return TRUE;

    return FALSE;
}

static gboolean check_for_xquartz(GdkDisplay *dpy)
{
    int nextensions;
    int i;
    gboolean match = FALSE;
    char **extensions = XListExtensions(gdk_x11_display_get_xdisplay(dpy),
                                        &nextensions);
    for (i = 0 ; extensions != NULL && i < nextensions ; i++) {
        VNC_DEBUG("Found extension '%s'", extensions[i]);
        if (strcmp(extensions[i], "Apple-WM") == 0 ||
            strcmp(extensions[i], "Apple-DRI") == 0)
            match = TRUE;
    }
    if (extensions)
        XFreeExtensionList(extensions);

    return match;
}
#endif

const guint16 *vnc_display_keymap_gdk2rfb_table(size_t *maplen)
{
    GdkDisplay *dpy = gdk_display_get_default();

#ifdef GDK_WINDOWING_X11
    if (GDK_IS_X11_DISPLAY(dpy)) {
        XkbDescPtr desc;
        const gchar *keycodes = NULL;

        VNC_DEBUG("Using X11 backend");
        /* There is no easy way to determine what X11 server
         * and platform & keyboard driver is in use. Thus we
         * do best guess heuristics.
         *
         * This will need more work for people with other
         * X servers..... patches welcomed.
         */

        Display *xdisplay = gdk_x11_display_get_xdisplay(dpy);
        desc = XkbGetMap(xdisplay,
                         XkbGBN_AllComponentsMask,
                         XkbUseCoreKbd);
        if (desc) {
            if (XkbGetNames(xdisplay, XkbKeycodesNameMask, desc) == Success) {
                keycodes = gdk_x11_get_xatom_name(desc->names->keycodes);
                if (!keycodes)
                    g_warning("could not lookup keycode name");
                else
                    VNC_DEBUG("XKB keyboard map name '%s'", keycodes);
            } else {
                VNC_DEBUG("No XKB keyboard keyboard map name");
            }
            XkbFreeKeyboard(desc, XkbGBN_AllComponentsMask, True);
        } else {
            VNC_DEBUG("No XKB keyboard description available");
        }

        if (check_for_xwin(dpy)) {
            VNC_DEBUG("Using xwin keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgxwin2qnum);
            return keymap_xorgxwin2qnum;
        } else if (check_for_xquartz(dpy)) {
            VNC_DEBUG("Using xquartz keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgxquartz2qnum);
            return keymap_xorgxquartz2qnum;
        } else if ((keycodes && STRPREFIX(keycodes, "evdev")) ||
                   (XKeysymToKeycode(xdisplay, XK_Page_Up) == 0x70)) {
            VNC_DEBUG("Using evdev keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgevdev2qnum);
            return keymap_xorgevdev2qnum;
        } else if ((keycodes && STRPREFIX(keycodes, "xfree86")) ||
                   (XKeysymToKeycode(xdisplay, XK_Page_Up) == 0x63)) {
            VNC_DEBUG("Using xfree86 keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgkbd2qnum);
            return keymap_xorgkbd2qnum;
        } else {
            g_warning("Unknown X11 keycode mapping '%s'.\n"
                      "Please report to gtk-vnc-list@gnome.org\n"
                      "including the following information:\n"
                      "\n"
                      "  - Operating system\n"
                      "  - GDK build\n"
                      "  - X11 Server\n"
                      "  - xprop -root\n"
                      "  - xdpyinfo\n",
                      keycodes ? keycodes : "<null>");
            return NULL;
        }
    }
#endif

#ifdef GDK_WINDOWING_WAYLAND
    if (GDK_IS_WAYLAND_DISPLAY(dpy)) {
        VNC_DEBUG("Using Wayland evdev virtual keycode mapping");
        *maplen = G_N_ELEMENTS(keymap_xorgevdev2qnum);
        return keymap_xorgevdev2qnum;
    }
#endif

#ifdef GDK_WINDOWING_WIN32
    if (GDK_IS_WIN32_DISPLAY(dpy)) {
        VNC_DEBUG("Using Win32 virtual keycode mapping");
        *maplen = G_N_ELEMENTS(keymap_win322qnum);
        return keymap_win322qnum;
    }
#endif

#ifdef GDK_WINDOWING_QUARTZ
    if (GDK_IS_QUARTZ_DISPLAY(dpy)) {
        VNC_DEBUG("Using OS-X virtual keycode mapping");
        *maplen = G_N_ELEMENTS(keymap_osx2qnum);
        return keymap_osx2qnum;
    }
#endif

#ifdef GDK_WINDOWING_BROADWAY
    if (GDK_IS_BROADWAY_DISPLAY(dpy)) {
        g_warning("experimental: using broadway, x11 virtual keysym \n"
                  "mapping - with very limited support. See also \n"
                  "https://bugzilla.gnome.org/show_bug.cgi?id=700105");
        *maplen = G_N_ELEMENTS(keymap_x112qnum);
        return keymap_x112qnum;
    }
#endif

    g_warning("Unsupported GDK Windowing platform.\n"
              "Disabling extended keycode tables.\n"
              "Please report to gtk-vnc-list@gnome.org\n"
              "including the following information:\n"
              "\n"
              "  - Operating system\n"
              "  - GDK Windowing system build\n");
    return NULL;
}

guint16 vnc_display_keymap_gdk2rfb(const guint16 *keycode_map,
                                   size_t keycode_maplen,
                                   guint16 keycode)
{
    if (!keycode_map)
        return 0;
    if (keycode >= keycode_maplen)
        return 0;
    return keycode_map[keycode];
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
