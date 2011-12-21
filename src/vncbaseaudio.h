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

#ifndef VNC_BASE_AUDIO_H
#define VNC_BASE_AUDIO_H

#include <glib-object.h>

#include <vncaudio.h>

G_BEGIN_DECLS

#define VNC_TYPE_BASE_AUDIO            (vnc_base_audio_get_type ())
#define VNC_BASE_AUDIO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), VNC_TYPE_BASE_AUDIO, VncBaseAudio))
#define VNC_BASE_AUDIO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), VNC_TYPE_BASE_AUDIO, VncBaseAudioClass))
#define VNC_IS_BASE_AUDIO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VNC_TYPE_BASE_AUDIO))
#define VNC_IS_BASE_AUDIO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VNC_TYPE_BASE_AUDIO))
#define VNC_BASE_AUDIO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), VNC_TYPE_BASE_AUDIO, VncBaseAudioClass))


typedef struct _VncBaseAudio VncBaseAudio;
typedef struct _VncBaseAudioPrivate VncBaseAudioPrivate;
typedef struct _VncBaseAudioClass VncBaseAudioClass;

struct _VncBaseAudio
{
    GObject parent;

    VncBaseAudioPrivate *priv;
};

struct _VncBaseAudioClass
{
    GObjectClass parent_class;

    gboolean (*playback_start)(VncBaseAudio *audio, VncAudioFormat *format);
    gboolean (*playback_stop)(VncBaseAudio *audio);
    gboolean (*playback_data)(VncBaseAudio *audio, VncAudioSample *sample);
};


GType vnc_base_audio_get_type(void) G_GNUC_CONST;

VncBaseAudio *vnc_base_audio_new(void);

G_END_DECLS

#endif /* VNC_BASE_AUDIO_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
