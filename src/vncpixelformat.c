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

#include "vncpixelformat.h"

GType vnc_pixel_format_get_type(void)
{
    static GType pixel_format_type = 0;

    if (G_UNLIKELY(pixel_format_type == 0)) {
        pixel_format_type = g_boxed_type_register_static
            ("VncPixelFormat",
             (GBoxedCopyFunc)vnc_pixel_format_copy,
             (GBoxedFreeFunc)vnc_pixel_format_free);
    }

    return pixel_format_type;
}


/**
 * vnc_pixel_format_new:
 *
 * Allocate a new VNC pixel format struct whose
 * contents is initialized to all zeros. The
 * struct must be released using vnc_pixel_format_free
 * when no longer required
 *
 * Returns: (transfer full): the new pixel format struct
 */
VncPixelFormat *vnc_pixel_format_new(void)
{
    VncPixelFormat *format;

    format = g_slice_new0(VncPixelFormat);

    return format;
}

/**
 * vnc_pixel_format_copy:
 * @srcFormat: the format to copy
 *
 * Allocate a new VNC pixel format struct whose
 * contents is initialized with the data found
 * in @srcFormat. The struct must be released using
 * vnc_pixel_format_free when no longer required.
 *
 * Returns: (transfer full): the new pixel format struct
 */
VncPixelFormat *vnc_pixel_format_copy(VncPixelFormat *srcFormat)
{
    VncPixelFormat *format;

    format = g_slice_dup(VncPixelFormat, srcFormat);

    return format;
}


/**
 * vnc_pixel_format_free:
 * @format: the format to free
 *
 * Release the memory associated with @format
 */
void vnc_pixel_format_free(VncPixelFormat *format)
{
    g_slice_free(VncPixelFormat, format);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
