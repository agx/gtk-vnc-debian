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

#ifndef VNC_AUDIO_FORMAT_H
#define VNC_AUDIO_FORMAT_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define VNC_TYPE_AUDIO_FORMAT            (vnc_audio_format_get_type ())

typedef struct _VncAudioFormat VncAudioFormat;

struct _VncAudioFormat {
    guint8 format;
    guint8 nchannels;
    guint32 frequency;
};

typedef enum {
    VNC_AUDIO_FORMAT_RAW_U8,
    VNC_AUDIO_FORMAT_RAW_S8,
    VNC_AUDIO_FORMAT_RAW_U16,
    VNC_AUDIO_FORMAT_RAW_S16,
    VNC_AUDIO_FORMAT_RAW_U32,
    VNC_AUDIO_FORMAT_RAW_S32,
} VncAudioFormatType;

GType vnc_audio_format_get_type(void);

VncAudioFormat *vnc_audio_format_new(void);
VncAudioFormat *vnc_audio_format_copy(VncAudioFormat *format);
void vnc_audio_format_free(VncAudioFormat *format);

G_END_DECLS

#endif /* VNC_AUDIO_FORMAT_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
