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

gboolean vnc_util_get_debug(void)
{
    return debugFlag;
}

gint vnc_util_get_version(void)
{
    return (VERSION_MAJOR << 16) |
        (VERSION_MINOR << 8) |
        VERSION_MICRO;
}

gboolean vnc_util_check_version(gint major, gint minor, gint micro)
{
    return ((VERSION_MAJOR > major) || \
            ((VERSION_MAJOR == major) && (VERSION_MINOR > minor)) || \
            ((VERSION_MAJOR == major) && (VERSION_MINOR == minor) && \
             (VERSION_MICRO >= micro)));
}

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
