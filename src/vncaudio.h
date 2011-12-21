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

#ifndef VNC_AUDIO_H
#define VNC_AUDIO_H

#include <glib-object.h>

#include <vncaudioformat.h>
#include <vncaudiosample.h>

G_BEGIN_DECLS

#define VNC_TYPE_AUDIO            (vnc_audio_get_type ())
#define VNC_AUDIO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), VNC_TYPE_AUDIO, VncAudio))
#define VNC_IS_AUDIO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VNC_TYPE_AUDIO))
#define VNC_AUDIO_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), VNC_TYPE_AUDIO, VncAudioInterface))


typedef struct _VncAudio VncAudio; /* Dummy object */
typedef struct _VncAudioInterface VncAudioInterface;

struct _VncAudioInterface {
    GTypeInterface parent;

    gboolean (*playback_start)(VncAudio *audio, VncAudioFormat *format);
    gboolean (*playback_stop)(VncAudio *audio);
    gboolean (*playback_data)(VncAudio *audio, VncAudioSample *sample);
};

GType vnc_audio_get_type(void) G_GNUC_CONST;

void vnc_audio_playback_start(VncAudio *aud,
                              VncAudioFormat *format);
void vnc_audio_playback_stop(VncAudio *aud);
void vnc_audio_playback_data(VncAudio *aud,
                             VncAudioSample *sample);

G_END_DECLS

#endif /* VNC_AUDIO_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
