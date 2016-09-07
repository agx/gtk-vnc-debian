/*
 * GTK VNC Widget
 *
 * Copyright (C) 2010 Daniel P. Berrange <dan@berrange.com>
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

#include <stdlib.h>
#include <string.h>

#include "vncutil.h"


static gboolean debugFlag = FALSE;

/**
 * vnc_util_set_debug:
 * @enabled: TRUE to turn on verbose debugging
 *
 * Control whether the VNC code emits verbose debug
 * messages on stderr
 */
void vnc_util_set_debug(gboolean enabled)
{
#if GLIB_CHECK_VERSION(2, 31, 0)
    if (enabled) {
        const gchar *doms = g_getenv("G_MESSAGES_DEBUG");
        if (!doms) {
            g_setenv("G_MESSAGES_DEBUG", G_LOG_DOMAIN, 1);
        } else if (!strstr(doms, G_LOG_DOMAIN)) {
            gchar *newdoms = g_strdup_printf("%s %s", doms, G_LOG_DOMAIN);
            g_setenv("G_MESSAGES_DEBUG", newdoms, 1);
            g_free(newdoms);
        }
    }
#endif
    debugFlag = enabled;
}


/**
 * vnc_util_get_debug:
 *
 * Determine whether the VNC code will emit verbose
 * debug messages
 *
 * Returns: TRUE if debugging is enabled, FALSE otherwise
 */
gboolean vnc_util_get_debug(void)
{
    return debugFlag;
}

/**
 * vnc_util_get_version:
 *
 * Get the encoded version number of the library release.
 * The major, minor and micro components are encoded in
 * 8-bits each.
 *
 * Returns: the library version number
 */
gint vnc_util_get_version(void)
{
    return (VERSION_MAJOR << 16) |
        (VERSION_MINOR << 8) |
        VERSION_MICRO;
}


/**
 * vnc_util_check_version:
 * @major: the desired major version
 * @minor: the desired minor version
 * @micro: the desired micro version
 *
 * Check whether the library is at least as new as the
 * version (@major, @minor, @micro)
 *
 * Returns: TRUE if the library is at least as new as the requested version
 */
gboolean vnc_util_check_version(gint major, gint minor, gint micro)
{
    return ((VERSION_MAJOR > major) || \
            ((VERSION_MAJOR == major) && (VERSION_MINOR > minor)) || \
            ((VERSION_MAJOR == major) && (VERSION_MINOR == minor) && \
             (VERSION_MICRO >= micro)));
}


/**
 * vnc_util_get_version_string:
 *
 * Get the library version number in a printable
 * string format
 *
 * Returns: (transfer none): the version string
 */
const gchar *vnc_util_get_version_string(void)
{
    return VERSION;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
