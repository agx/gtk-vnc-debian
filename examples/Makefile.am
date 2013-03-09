
if WITH_EXAMPLES
bin_PROGRAMS = gvncviewer
else
noinst_PROGRAMS = gvncviewer
endif

gvncviewer_SOURCES = gvncviewer.c
gvncviewer_LDADD = ../src/libgtk-vnc-$(GTK_VNC_API_VERSION).la \
                   ../src/libgvnc-1.0.la \
		   $(GIOUNIX_LIBS) \
		   $(GTK_LIBS) \
		   $(VIEW_LIBS)
if HAVE_PULSEAUDIO
gvncviewer_LDADD += ../src/libgvncpulse-1.0.la
endif
gvncviewer_CFLAGS = $(GTK_CFLAGS) \
		    $(GIOUNIX_CFLAGS) \
		    $(WARN_CFLAGS) \
		    $(VIEW_CFLAGS) -I$(top_srcdir)/src/

EXTRA_DIST = gvncviewer-bindings.py gvncviewer-introspection.py \
             gvncviewer.js gvncviewer.pl

-include $(top_srcdir)/git.mk
