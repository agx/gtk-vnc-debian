
if WITH_EXAMPLES
bin_PROGRAMS = gvncviewer
else
noinst_PROGRAMS = gvncviewer
endif

gvncviewer_SOURCES = gvncviewer.c
gvncviewer_LDADD = ../src/libgtk-vnc-$(GTK_VNC_API_VERSION).la \
                   ../src/libgvnc-1.0.la \
		   $(GTK_CFLAGS) \
		   $(GTK_LIBS) \
		   $(VIEW_LIBS)
if HAVE_PULSEAUDIO
gvncviewer_LDADD += ../src/libgvncpulse-1.0.la
endif
gvncviewer_CFLAGS = $(GTK_CFLAGS) $(WARNING_CFLAGS) \
		    $(VIEW_CFLAGS) -I$(top_srcdir)/src/

EXTRA_DIST = gvncviewer.py gvncviewer.js

-include $(top_srcdir)/git.mk
