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

#include <pulse/simple.h>
#include <pulse/pulseaudio.h>

#include "vncaudiopulse.h"
#include "vncutil.h"

#define VNC_AUDIO_PULSE_GET_PRIVATE(obj)                                \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), VNC_TYPE_AUDIO_PULSE, VncAudioPulsePrivate))

struct _VncAudioPulsePrivate {
    pa_simple *pa;
};


G_DEFINE_TYPE(VncAudioPulse, vnc_audio_pulse, VNC_TYPE_BASE_AUDIO);


static gboolean vnc_audio_pulse_playback_start(VncAudio *audio,
                                               VncAudioFormat *format)
{
    VncAudioPulse *pulse = VNC_AUDIO_PULSE(audio);
    VncAudioPulsePrivate *priv = pulse->priv;
    pa_sample_spec pulse_spec;
    pa_buffer_attr buffer_attr;

    if (priv->pa)
        return FALSE;

    switch (format->format) {
    case VNC_AUDIO_FORMAT_RAW_U8:
        pulse_spec.format = PA_SAMPLE_U8;
        break;
    case VNC_AUDIO_FORMAT_RAW_S16:
        pulse_spec.format = PA_SAMPLE_S16LE;
        break;
    case VNC_AUDIO_FORMAT_RAW_S32:
        pulse_spec.format = PA_SAMPLE_S32LE;
        break;

    case VNC_AUDIO_FORMAT_RAW_S8:
    case VNC_AUDIO_FORMAT_RAW_U16:
    case VNC_AUDIO_FORMAT_RAW_U32:
    default:
        VNC_DEBUG("Unable to handle audio format %d", format->format);
        return FALSE;
    }
    pulse_spec.channels = format->nchannels;
    pulse_spec.rate = format->frequency;

    /* FIXME: we might want customizable latency */
    memset(&buffer_attr, 0, sizeof(buffer_attr));
    buffer_attr.maxlength = -1;
    buffer_attr.tlength = pa_usec_to_bytes(100 * PA_USEC_PER_MSEC, &pulse_spec);
    buffer_attr.prebuf = -1;
    buffer_attr.minreq = -1;

    priv->pa = pa_simple_new(NULL, "gvnc", PA_STREAM_PLAYBACK,
                             NULL, "VNC Remote Desktop",
                             &pulse_spec, NULL, &buffer_attr, NULL);
    return TRUE;
}

static gboolean vnc_audio_pulse_playback_stop(VncAudio *audio)
{
    VncAudioPulse *pulse = VNC_AUDIO_PULSE(audio);
    VncAudioPulsePrivate *priv = pulse->priv;

    if (!priv->pa)
        return FALSE;

    pa_simple_drain(priv->pa, NULL);
    pa_simple_free(priv->pa);
    priv->pa = NULL;

    return TRUE;
}

static gboolean vnc_audio_pulse_playback_data(VncAudio *audio,
                                              VncAudioSample *sample)
{
    VncAudioPulse *pulse = VNC_AUDIO_PULSE(audio);
    VncAudioPulsePrivate *priv = pulse->priv;

    if (!priv->pa)
        return FALSE;

    if (pa_simple_write(priv->pa, sample->data, sample->length, NULL) < 0)
        return FALSE;

    return TRUE;
}

static void vnc_audio_pulse_finalize(GObject *object)
{
    VncAudioPulse *ab = VNC_AUDIO_PULSE(object);
    VncAudioPulsePrivate *priv = ab->priv;

    VNC_DEBUG("Finalize VncAudioPulse=%p", ab);

    if (priv->pa)
        pa_simple_free(priv->pa);

    G_OBJECT_CLASS(vnc_audio_pulse_parent_class)->finalize (object);
}


static void vnc_audio_pulse_class_init(VncAudioPulseClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = vnc_audio_pulse_finalize;

    g_type_class_add_private(klass, sizeof(VncAudioPulsePrivate));
}


void vnc_audio_pulse_init(VncAudioPulse *fb)
{
    VncAudioPulsePrivate *priv;

    priv = fb->priv = VNC_AUDIO_PULSE_GET_PRIVATE(fb);

    memset(priv, 0, sizeof(*priv));

    g_signal_connect(G_OBJECT(fb), "vnc-audio-playback-start",
                     G_CALLBACK(vnc_audio_pulse_playback_start), NULL);
    g_signal_connect(G_OBJECT(fb), "vnc-audio-playback-stop",
                     G_CALLBACK(vnc_audio_pulse_playback_stop), NULL);
    g_signal_connect(G_OBJECT(fb), "vnc-audio-playback-data",
                     G_CALLBACK(vnc_audio_pulse_playback_data), NULL);
}


/**
 * vnc_audio_pulse_new:
 *
 * Create a new VNC audio object able to play
 * audio samples via pulseaudio
 *
 * Returns: (transfer full): the new audio object
 */
VncAudioPulse *vnc_audio_pulse_new(void)
{
    return VNC_AUDIO_PULSE(g_object_new(VNC_TYPE_AUDIO_PULSE,
                                        NULL));
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
