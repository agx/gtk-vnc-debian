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

#ifndef VNC_AUDIO_PULSE_H
#define VNC_AUDIO_PULSE_H

#include <glib-object.h>

#include <vncbaseaudio.h>

G_BEGIN_DECLS

#define VNC_TYPE_AUDIO_PULSE            (vnc_audio_pulse_get_type ())
#define VNC_AUDIO_PULSE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), VNC_TYPE_AUDIO_PULSE, VncAudioPulse))
#define VNC_AUDIO_PULSE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), VNC_TYPE_AUDIO_PULSE, VncAudioPulseClass))
#define VNC_IS_AUDIO_PULSE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VNC_TYPE_AUDIO_PULSE))
#define VNC_IS_AUDIO_PULSE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VNC_TYPE_AUDIO_PULSE))
#define VNC_AUDIO_PULSE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), VNC_TYPE_AUDIO_PULSE, VncAudioPulseClass))


typedef struct _VncAudioPulse VncAudioPulse;
typedef struct _VncAudioPulsePrivate VncAudioPulsePrivate;
typedef struct _VncAudioPulseClass VncAudioPulseClass;

struct _VncAudioPulse
{
    VncBaseAudio parent;

    VncAudioPulsePrivate *priv;
};

struct _VncAudioPulseClass
{
    VncBaseAudioClass parent_class;

};


GType vnc_audio_pulse_get_type(void) G_GNUC_CONST;

VncAudioPulse *vnc_audio_pulse_new(void);

G_END_DECLS

#endif /* VNC_AUDIO_PULSE_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
