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

#include "vncaudioformat.h"

GType vnc_audio_format_get_type(void)
{
    static GType audio_format_type = 0;

    if (G_UNLIKELY(audio_format_type == 0)) {
        audio_format_type = g_boxed_type_register_static
            ("VncAudioFormat",
             (GBoxedCopyFunc)vnc_audio_format_copy,
             (GBoxedFreeFunc)vnc_audio_format_free);
    }

    return audio_format_type;
}


/**
 * vnc_audio_format_new:
 *
 * Allocate a new VNC audio format struct whose
 * contents is initialized to all zeros. The
 * struct must be released using vnc_audio_format_free
 * when no longer required
 *
 * Returns: (transfer full): the new audio format struct
 */
VncAudioFormat *vnc_audio_format_new(void)
{
    VncAudioFormat *format;

    format = g_slice_new0(VncAudioFormat);

    return format;
}


/**
 * vnc_audio_format_copy:
 * @format: the format to copy
 *
 * Allocate a new VNC audio format struct whose
 * contents is initialized with the data found
 * in @format. The struct must be released using
 * vnc_audio_format_free when no longer required.
 *
 * Returns: (transfer full): the new audio format struct
 */
VncAudioFormat *vnc_audio_format_copy(VncAudioFormat *format)
{
    VncAudioFormat *ret;

    ret = g_slice_dup(VncAudioFormat, format);

    return ret;
}


/**
 * vnc_audio_format_free:
 * @format: the format to free
 *
 * Release the memory associated with @format
 */
void vnc_audio_format_free(VncAudioFormat *format)
{
    g_slice_free(VncAudioFormat, format);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
