/*
 * Copyright (C) 2008  Anthony Liguori <anthony@codemonkey.ws>
 * Copyright (C) 2009-2010 Daniel P. Berrange <dan@berrange.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <config.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>

#include "vncdisplaykeymap.h"
#include "vncutil.h"

/*
 * This table is taken from QEMU x_keymap.c, under the terms:
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/* Compatability code to allow build on Gtk2 and Gtk3 */
#ifndef GDK_Tab
#define GDK_Tab GDK_KEY_Tab
#endif

/* keycode translation for sending ISO_Left_Send
 * to vncserver
 */
static struct {
    GdkKeymapKey *keys;
    gint n_keys;
    guint keyval;
} untranslated_keys[] = {{NULL, 0, GDK_Tab}};

static unsigned int ref_count_for_untranslated_keys = 0;

#ifdef GDK_WINDOWING_WAYLAND
#include <gdk/gdkwayland.h>
#endif

#ifdef GDK_WINDOWING_BROADWAY
#include <gdk/gdkbroadway.h>
#endif

#if defined(GDK_WINDOWING_X11) || defined(GDK_WINDOWING_WAYLAND)
/* Wayland or Xorg Linux + evdev (offset evdev keycodes) */
#include "vncdisplaykeymap_xorgevdev2rfb.c"
#endif

#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#include <X11/XKBlib.h>
#include <stdbool.h>
#include <string.h>

/* Xorg Linux + kbd (offset + mangled XT keycodes) */
#include "vncdisplaykeymap_xorgkbd2rfb.c"
/* Xorg OS-X aka XQuartz (offset OS-X keycodes) */
#include "vncdisplaykeymap_xorgxquartz2rfb.c"
/* Xorg Cygwin aka XWin (offset + mangled XT keycodes) */
#include "vncdisplaykeymap_xorgxwin2rfb.c"

/* Gtk2 compat */
#ifndef GDK_IS_X11_DISPLAY
#define GDK_IS_X11_DISPLAY(dpy) (dpy != NULL)
#endif
#endif

#ifdef GDK_WINDOWING_WIN32
/* Win32 native virtual keycodes */
#include "vncdisplaykeymap_win322rfb.c"

/* Gtk2 compat */
#ifndef GDK_IS_WIN32_DISPLAY
#define GDK_IS_WIN32_DISPLAY(dpy) (dpy != NULL)
#endif
#endif

#ifdef GDK_WINDOWING_BROADWAY
/* X11 keysyms */
#include "vncdisplaykeymap_x112rfb.c"

/* Gtk2 compat */
#ifndef GDK_IS_BROADWAY_DISPLAY
#define GDK_IS_BROADWAY_DISPLAY(dpy) (dpy != NULL)
#endif

#endif

#ifdef GDK_WINDOWING_QUARTZ
/* OS-X native keycodes */
#include "vncdisplaykeymap_osx2rfb.c"

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
            *maplen = G_N_ELEMENTS(keymap_xorgxwin2rfb);
            return keymap_xorgxwin2rfb;
        } else if (check_for_xquartz(dpy)) {
            VNC_DEBUG("Using xquartz keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgxquartz2rfb);
            return keymap_xorgxquartz2rfb;
        } else if (keycodes && STRPREFIX(keycodes, "evdev")) {
            VNC_DEBUG("Using evdev keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgevdev2rfb);
            return keymap_xorgevdev2rfb;
        } else if (keycodes && STRPREFIX(keycodes, "xfree86")) {
            VNC_DEBUG("Using xfree86 keycode mapping");
            *maplen = G_N_ELEMENTS(keymap_xorgkbd2rfb);
            return keymap_xorgkbd2rfb;
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
        *maplen = G_N_ELEMENTS(keymap_xorgevdev2rfb);
        return keymap_xorgevdev2rfb;
    }
#endif

#ifdef GDK_WINDOWING_WIN32
    if (GDK_IS_WIN32_DISPLAY(dpy)) {
        VNC_DEBUG("Using Win32 virtual keycode mapping");
        *maplen = G_N_ELEMENTS(keymap_win322rfb);
        return keymap_win322rfb;
    }
#endif

#ifdef GDK_WINDOWING_QUARTZ
    if (GDK_IS_QUARTZ_DISPLAY(dpy)) {
        VNC_DEBUG("Using OS-X virtual keycode mapping");
        *maplen = G_N_ELEMENTS(keymap_osx2rfb);
        return keymap_osx2rfb;
    }
#endif

#ifdef GDK_WINDOWING_BROADWAY
    if (GDK_IS_BROADWAY_DISPLAY(dpy)) {
        g_warning("experimental: using broadway, x11 virtual keysym \n"
                  "mapping - with very limited support. See also \n"
                  "https://bugzilla.gnome.org/show_bug.cgi?id=700105");
        *maplen = G_N_ELEMENTS(keymap_x112rfb);
        return keymap_x112rfb;
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

/* Set the keymap entries */
void vnc_display_keyval_set_entries(void)
{
    size_t i;
    if (ref_count_for_untranslated_keys == 0)
        for (i = 0; i < G_N_ELEMENTS(untranslated_keys); i++)
            gdk_keymap_get_entries_for_keyval(gdk_keymap_get_default(),
                                              untranslated_keys[i].keyval,
                                              &untranslated_keys[i].keys,
                                              &untranslated_keys[i].n_keys);
    ref_count_for_untranslated_keys++;
}

/* Free the keymap entries */
void vnc_display_keyval_free_entries(void)
{
    size_t i;

    if (ref_count_for_untranslated_keys == 0)
        return;

    ref_count_for_untranslated_keys--;
    if (ref_count_for_untranslated_keys == 0)
        for (i = 0; i < G_N_ELEMENTS(untranslated_keys); i++)
            g_free(untranslated_keys[i].keys);

}

/* Get the keyval from the keycode without the level. */
guint vnc_display_keyval_from_keycode(guint keycode, guint keyval)
{
    size_t i;
    for (i = 0; i < G_N_ELEMENTS(untranslated_keys); i++) {
        if (keycode == untranslated_keys[i].keys[0].keycode) {
            return untranslated_keys[i].keyval;
        }
    }

    return keyval;
}
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
