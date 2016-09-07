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

#include "vncframebuffer.h"

/**
 * vnc_framebuffer_get_width:
 * @fb: (transfer none): the framebuffer object
 *
 * Query the width of the remote framebuffer
 *
 * Returns: the framebuffer width
 */
guint16 vnc_framebuffer_get_width(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->get_width(fb);
}

/**
 * vnc_framebuffer_get_height:
 * @fb: (transfer none): the framebuffer object
 *
 * Query the height of the remote framebuffer
 *
 * Returns: the frambuffer height
 */
guint16 vnc_framebuffer_get_height(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->get_height(fb);
}

/**
 * vnc_framebuffer_get_rowstride:
 * @fb: (transfer none): the framebuffer object
 *
 * Get the number of bytes per line of the framebuffer
 *
 * Returns: the framebuffer row stride
 */
int vnc_framebuffer_get_rowstride(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->get_rowstride(fb);
}

/**
 * vnc_framebuffer_get_buffer:
 * @fb: (transfer none): the framebuffer object
 *
 * Get a pointer to the framebuffer contents
 *
 * Returns: (transfer none)(array): the framebuffer data
 */
guint8 *vnc_framebuffer_get_buffer(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->get_buffer(fb);
}

/**
 * vnc_framebuffer_get_local_format:
 * @fb: (transfer none): the framebuffer object
 *
 * Get the pixel format used to store the framebuffer locally
 *
 * Returns: (transfer none): the local pixel format
 */
const VncPixelFormat *vnc_framebuffer_get_local_format(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->get_local_format(fb);
}

/**
 * vnc_framebuffer_remote_format:
 * @fb: (transfer none): the framebuffer object
 *
 * Get the pixel format used by the remote desktop sending
 * framebuffer updates.
 *
 * Returns: (transfer none): the remote pixel format
 */
const VncPixelFormat *vnc_framebuffer_get_remote_format(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->get_remote_format(fb);
}

/**
 * vnc_framebuffer_perfect_format_match:
 * @fb: (transfer none): the framebuffer object
 *
 * Determine if the local and remote pixel formats match
 *
 * Returns: TRUE if the local and remote pixel formats match
 */
gboolean vnc_framebuffer_perfect_format_match(VncFramebuffer *fb)
{
    return VNC_FRAMEBUFFER_GET_INTERFACE(fb)->perfect_format_match(fb);
}

/**
 * vnc_framebuffer_set_pixel_at:
 * @fb: (transfer none): the framebuffer object
 * @src: (array)(transfer none): the new pixel data
 * @x: the horizontal pixel to set
 * @y: the vertical pixel to set
 *
 * Sets a pixel in the framebuffer at (@x, @y) to the
 * value in @src. The number of bytes in @src is
 * determined by the remote pixel format
 */
void vnc_framebuffer_set_pixel_at(VncFramebuffer *fb,
                                  guint8 *src,
                                  guint16 x, guint16 y)
{
    VNC_FRAMEBUFFER_GET_INTERFACE(fb)->set_pixel_at(fb, src, x, y);
}

/**
 * vnc_framebuffer_fill:
 * @fb: (transfer none): the framebuffer object
 * @src: (array)(transfer none): the new pixel data
 * @x: the horizontal pixel to start filling
 * @y: the vertical pixel to start filling
 * @width: the number of pixels to fill horizontally
 * @height: the number of pixels to fill vertically
 *
 * Fill all the pixels in the range (@x, @y) to
 * (@x + @width, @y + @height) to the value in
 * @src. The number of bytes in @src is
 * determined by the remote pixel format
 */
void vnc_framebuffer_fill(VncFramebuffer *fb,
                          guint8 *src,
                          guint16 x, guint16 y,
                          guint16 width, guint16 height)
{
    VNC_FRAMEBUFFER_GET_INTERFACE(fb)->fill(fb, src, x, y, width, height);
}

/**
 * vnc_framebuffer_copyrect:
 * @fb: (transfer none): the framebuffer object
 * @srcx: the horizontal starting pixel
 * @srcy: the vertical starting pixel
 * @dstx: the horizontal target pixel
 * @dsty: the vertical target pixel
 * @width: the width of the region
 * @height: the height of the region
 *
 * Copies data from the range (@srcx, @srcy) to
 * (@srcx+@width, @srcy+@height) over to the
 * range starting at (@dstx, @dsty).
 */
void vnc_framebuffer_copyrect(VncFramebuffer *fb,
                              guint16 srcx, guint16 srcy,
                              guint16 dstx, guint16 dsty,
                              guint16 width, guint16 height)
{
    VNC_FRAMEBUFFER_GET_INTERFACE(fb)->copyrect(fb, srcx, srcy, dstx, dsty, width, height);
}

/**
 * vnc_framebuffer_:
 * @fb: (transfer none): the framebuffer object
 * @src: (array)(transfer none): the new pixel data
 * @rowstride: the number of bytes per row
 * @x: the horizontal pixel to start filling
 * @y: the vertical pixel to start filling
 * @width: the number of pixels to fill horizontally
 * @height: the number of pixels to fill vertically
 *
 * Fill all the pixels in the range (@x, @y) to
 * (@x + @width, @y + @height) to the value in
 * @src. The number of bytes in @src is
 * determined by the remote pixel format
 */
void vnc_framebuffer_blt(VncFramebuffer *fb,
                         guint8 *src,
                         int rowstride,
                         guint16 x, guint16 y,
                         guint16 width, guint16 height)
{
    VNC_FRAMEBUFFER_GET_INTERFACE(fb)->blt(fb, src, rowstride, x, y, width, height);
}

/**
 * vnc_framebuffer_rgb24_blt:
 * @fb: (transfer none): the framebuffer object
 * @src: (array)(transfer none): the new pixel data
 * @rowstride: the number of bytes per row
 * @x: the horizontal pixel to start filling
 * @y: the vertical pixel to start filling
 * @width: the number of pixels to fill horizontally
 * @height: the number of pixels to fill vertically
 *
 * Fill all the pixels in the range (@x, @y) to
 * (@x + @width, @y + @height) to the value in
 * @src. The number of bytes in @src is always
 * 3 as it must be in plain RGB24 format.
 */
void vnc_framebuffer_rgb24_blt(VncFramebuffer *fb,
                               guint8 *src,
                               int rowstride,
                               guint16 x, guint16 y,
                               guint16 width, guint16 height)
{
    VNC_FRAMEBUFFER_GET_INTERFACE(fb)->rgb24_blt(fb, src, rowstride, x, y, width, height);
}


/**
 * vnc_framebuffer_set_color_map:
 * @fb: (transfer none): the framebuffer object
 * @map: (transfer none): the new color map
 *
 * Set the color map to use for the framebuffer
 */
void vnc_framebuffer_set_color_map(VncFramebuffer *fb,
                                   VncColorMap *map)
{
    VNC_FRAMEBUFFER_GET_INTERFACE(fb)->set_color_map(fb, map);
}


GType
vnc_framebuffer_get_type (void)
{
    static GType framebuffer_type = 0;

    if (!framebuffer_type) {
        framebuffer_type =
            g_type_register_static_simple (G_TYPE_INTERFACE, "VncFramebuffer",
                                           sizeof (VncFramebufferInterface),
                                           NULL, 0, NULL, 0);

        g_type_interface_add_prerequisite (framebuffer_type, G_TYPE_OBJECT);
    }

    return framebuffer_type;
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
