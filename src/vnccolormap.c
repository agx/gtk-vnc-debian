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

#include <string.h>

#include "vnccolormap.h"

GType vnc_color_map_get_type(void)
{
    static GType color_map_type = 0;

    if (G_UNLIKELY(color_map_type == 0)) {
        color_map_type = g_boxed_type_register_static
            ("VncColorMap",
             (GBoxedCopyFunc)vnc_color_map_copy,
             (GBoxedFreeFunc)vnc_color_map_free);
    }

    return color_map_type;
}


/**
 * vnc_color_map_new:
 * @offset: the offset at which the color map starts
 * @size: the number of entries
 *
 * Allocate a new colour map object able to store colour
 * map entries with indexes in the range @offset to
 * @offset + @size.
 *
 * The color map must be freed with vnc_color_map_free
 * when no longer required.
 *
 * Returns: (transfer full): the new color map
 */
VncColorMap *vnc_color_map_new(guint16 offset, guint16 size)
{
    VncColorMap *map;

    map = g_slice_new0(VncColorMap);
    map->offset = offset;
    map->size = size;
    map->colors = g_new0(VncColorMapEntry, size);

    return map;
}


/**
 * vnc_color_map_copy:
 * @map: (transfer none): the original color map
 *
 * Allocate a new color map initializing it with a
 * copy of the data stored in @map.
 *
 * Returns: (transfer full): the new color map
 */
VncColorMap *vnc_color_map_copy(VncColorMap *map)
{
    VncColorMap *ret;

    ret = g_slice_dup(VncColorMap, map);
    ret->colors = g_new0(VncColorMapEntry, map->size);
    memcpy(ret->colors, map->colors,
           sizeof(VncColorMapEntry) * ret->size);

    return ret;
}

/**
 * vnc_color_map_free:
 * @map: the color map object
 *
 * Release the memory associated with the
 * color map @map
 */
void vnc_color_map_free(VncColorMap *map)
{
    g_slice_free(VncColorMap, map);
}


/**
 * vnc_color_map_set:
 * @map: the color map object
 * @idx: the index to set
 * @red: the new red value
 * @green: the new green value
 * @blue: the new blue value
 *
 * Update the RGB value associated with the
 * color map entry at position @idx.
 *
 * Returns: TRUE if @idx was in range, FALSE otherwise
 */
gboolean vnc_color_map_set(VncColorMap *map,
                           guint16 idx,
                           guint16 red,
                           guint16 green,
                           guint16 blue)
{
    if (idx >= (map->size + map->offset))
        return FALSE;

    map->colors[idx - map->offset].red = red;
    map->colors[idx - map->offset].green = green;
    map->colors[idx - map->offset].blue = blue;

    return TRUE;
}


/**
 * vnc_color_map_lookup:
 * @map: the color map object
 * @idx: the index to set
 * @red: pointer to hold the red value
 * @green: pointer to hold the green value
 * @blue: pointer to hold the blue value
 *
 * Lookup the RGB values associated with the
 * colour map entry at position @idx
 *
 * Returns: TRUE if @idx was in range, FALSE otherwise
 */
gboolean vnc_color_map_lookup(VncColorMap *map,
                              guint16 idx,
                              guint16 *red,
                              guint16 *green,
                              guint16 *blue)
{
    if (idx >= (map->size + map->offset))
        return FALSE;

    *red = map->colors[idx - map->offset].red;
    *green = map->colors[idx - map->offset].green;
    *blue = map->colors[idx - map->offset].blue;

    return TRUE;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
