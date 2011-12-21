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

#ifndef VNC_AUDIO_SAMPLE_H
#define VNC_AUDIO_SAMPLE_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define VNC_TYPE_AUDIO_SAMPLE            (vnc_audio_sample_get_type ())

typedef struct _VncAudioSample VncAudioSample;

struct _VncAudioSample {
    guint8 *data;
    guint32 length;
    guint32 capacity;
};

GType vnc_audio_sample_get_type(void);

VncAudioSample *vnc_audio_sample_new(guint32 capacity);
VncAudioSample *vnc_audio_sample_copy(VncAudioSample *sample);
void vnc_audio_sample_free(VncAudioSample *sample);

G_END_DECLS

#endif /* VNC_AUDIO_SAMPLE_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
