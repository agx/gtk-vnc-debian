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

#include "vncaudio.h"

/**
 * vnc_audio_playback_start:
 * @aud: (transfer none): the audio object
 * @format: (transfer none): the new audio format
 *
 * Indicate that the remote desktop is about to start
 * audio playback in format @format.
 */
void vnc_audio_playback_start(VncAudio *aud, VncAudioFormat *format)
{
    VNC_AUDIO_GET_INTERFACE(aud)->playback_start(aud, format);
}


/**
 * vnc_audio_playback_stop
 * @aud: (transfer none): the audio object
 *
 * Indicate that the remote desktop has completed
 * audio playback
 */
void vnc_audio_playback_stop(VncAudio *aud)
{
    VNC_AUDIO_GET_INTERFACE(aud)->playback_stop(aud);
}

/**
 * vnc_audio_playback_data:
 * @aud: (transfer none): the audio object
 * @sample: (transfer none): the audio sample
 *
 * Request playback of a single audio sample in @sample
 */
void vnc_audio_playback_data(VncAudio *aud, VncAudioSample *sample)
{
    VNC_AUDIO_GET_INTERFACE(aud)->playback_data(aud, sample);
}


GType
vnc_audio_get_type (void)
{
    static GType audio_type = 0;

    if (!audio_type) {
        audio_type =
            g_type_register_static_simple(G_TYPE_INTERFACE, "VncAudio",
                                          sizeof (VncAudioInterface),
                                          NULL, 0, NULL, 0);

        g_type_interface_add_prerequisite (audio_type, G_TYPE_OBJECT);
    }

    return audio_type;
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
