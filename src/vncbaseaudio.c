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

#include "vncbaseaudio.h"
#include "vncutil.h"
#include "coroutine.h"

#define VNC_BASE_AUDIO_GET_PRIVATE(obj)                                 \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), VNC_TYPE_BASE_AUDIO, VncBaseAudioPrivate))

struct _VncBaseAudioPrivate {
    gboolean unused;
};


static void vnc_base_audio_interface_init(gpointer g_iface,
                                          gpointer iface_data);

G_DEFINE_TYPE_EXTENDED(VncBaseAudio, vnc_base_audio, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(VNC_TYPE_AUDIO, vnc_base_audio_interface_init));


static void vnc_base_audio_class_init(VncBaseAudioClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_signal_new("vnc-audio-playback-start",
                 G_OBJECT_CLASS_TYPE (object_class),
                 G_SIGNAL_RUN_FIRST,
                 G_STRUCT_OFFSET (VncBaseAudioClass, playback_start),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__BOXED,
                 G_TYPE_NONE,
                 1,
                 VNC_TYPE_AUDIO_FORMAT);

    g_signal_new("vnc-audio-playback-stop",
                 G_OBJECT_CLASS_TYPE (object_class),
                 G_SIGNAL_RUN_FIRST,
                 G_STRUCT_OFFSET (VncBaseAudioClass, playback_stop),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__VOID,
                 G_TYPE_NONE,
                 0);

    g_signal_new("vnc-audio-playback-data",
                 G_OBJECT_CLASS_TYPE (object_class),
                 G_SIGNAL_RUN_FIRST,
                 G_STRUCT_OFFSET (VncBaseAudioClass, playback_data),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__BOXED,
                 G_TYPE_NONE,
                 1,
                 VNC_TYPE_AUDIO_SAMPLE);

    g_type_class_add_private(klass, sizeof(VncBaseAudioPrivate));
}


void vnc_base_audio_init(VncBaseAudio *fb)
{
    VncBaseAudioPrivate *priv;

    priv = fb->priv = VNC_BASE_AUDIO_GET_PRIVATE(fb);

    memset(priv, 0, sizeof(*priv));
}


/**
 * vnc_base_audio_new:
 *
 * Create a new generic VNC audio object, which will emit
 * signals when key audio events occur. Callers can connect
 * to these signals to provide playback facilities.
 *
 * Returns: (transfer full): a new audio object
 */
VncBaseAudio *vnc_base_audio_new(void)
{
    return VNC_BASE_AUDIO(g_object_new(VNC_TYPE_BASE_AUDIO,
                                       NULL));
}


static gboolean vnc_base_audio_playback_start(VncAudio *audio,
                                              VncAudioFormat *format)
{
    g_signal_emit_by_name(audio, "vnc-audio-playback-start", format);
    return TRUE;
}

static gboolean vnc_base_audio_playback_stop(VncAudio *audio)
{
    g_signal_emit_by_name(audio, "vnc-audio-playback-stop");
    return TRUE;
}

static gboolean vnc_base_audio_playback_data(VncAudio *audio,
                                             VncAudioSample *sample)
{
    g_signal_emit_by_name(audio, "vnc-audio-playback-data", sample);
    return TRUE;
}

static void vnc_base_audio_interface_init(gpointer g_iface,
                                          gpointer iface_data G_GNUC_UNUSED)
{
    VncAudioInterface *iface = g_iface;

    iface->playback_start = vnc_base_audio_playback_start;
    iface->playback_stop = vnc_base_audio_playback_stop;
    iface->playback_data = vnc_base_audio_playback_data;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
