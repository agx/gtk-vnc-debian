
EXTRA_DIST = libgvnc_sym.version libgvncpulse_sym.version libgtk-vnc_sym.version vncmarshal.txt

lib_LTLIBRARIES = libgvnc-1.0.la

BUILT_SOURCES =
if USE_VERSION_DEFS
GVNC_VERSION_FILE = $(builddir)/libgvnc_sym.def
GVNCPULSE_VERSION_FILE = $(builddir)/libgvncpulse_sym.def
GTK_VNC_VERSION_FILE = $(builddir)/libgtk-vnc_sym.def
BUILT_SOURCES += $(GVNC_VERSION_FILE) $(GTK_VNC_VERSION_FILE) $(GVNCPULSE_VERSION_FILE)
else
GVNC_VERSION_FILE = $(srcdir)/libgvnc_sym.version
GVNCPULSE_VERSION_FILE = $(srcdir)/libgvncpulse_sym.version
GTK_VNC_VERSION_FILE = $(srcdir)/libgtk-vnc_sym.version
endif

%.def: %.version Makefile
if HAVE_GTK_2
	$(AM_V_GEN)rm -f -- $@-tmp $@ ; \
	printf 'EXPORTS\n' > $@-tmp && \
	sed -e '/^$$/d; /#/d; /:/d; /\}/d; /\*/d; /LIBVIRT_/d; s/[ \t]*\(.*\)\;/    \1/g' $< >> $@-tmp && \
	chmod a-w $@-tmp && \
	mv $@-tmp $@
else
	$(AM_V_GEN)rm -f -- $@-tmp $@ ; \
	printf 'EXPORTS\n' > $@-tmp && \
	sed -e '/^$$/d; /#/d; /:/d; /\}/d; /\*/d; /vnc_image_framebuffer_*/d; /LIBVIRT_/d; s/[ \t]*\(.*\)\;/    \1/g' $< >> $@-tmp && \
	chmod a-w $@-tmp && \
	mv $@-tmp $@
endif


libgvnc_1_0_la_LIBADD = \
			$(GOBJECT_LIBS) \
			$(GIO_LIBS) \
			$(GTHREAD_LIBS) \
			$(GDK_PIXBUF_LIBS) \
			$(LIBGCRYPT_LIBS) \
			$(GNUTLS_LIBS) \
			$(SASL_LIBS)
libgvnc_1_0_la_CFLAGS = \
			$(GOBJECT_CFLAGS) \
			$(GIO_CFLAGS) \
			$(GTHREAD_CFLAGS) \
			$(GDK_PIXBUF_CFLAGS) \
			$(LIBGCRYPT_CFLAGS) \
			$(GNUTLS_CFLAGS) \
			$(SASL_CFLAGS) \
			$(WARN_CFLAGS) \
			-DSYSCONFDIR=\""$(sysconfdir)"\" \
			-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\" \
			-DG_LOG_DOMAIN=\"gtk-vnc\"
libgvnc_1_0_la_LDFLAGS = \
			$(VERSION_SCRIPT_FLAGS)$(GVNC_VERSION_FILE) \
			-version-info 0:1:0 $(NO_UNDEFINED_FLAGS)

libgvnc_1_0_ladir = $(includedir)/gvnc-1.0
libgvnc_1_0_la_HEADERS = \
			gvnc.h \
			vncpixelformat.h \
			vncaudioformat.h \
			vncaudiosample.h \
			vncaudio.h \
			vncbaseaudio.h \
			vncframebuffer.h \
			vncbaseframebuffer.h \
			vnccursor.h \
			vnccolormap.h \
			vncconnection.h \
			vncutil.h \
			$(NULL)
nodist_libgvnc_1_0_la_HEADERS = \
			vncconnectionenums.h \
			$(NULL)

libgvnc_1_0_la_SOURCES = \
			coroutine.h \
			d3des.h d3des.c \
			dh.h dh.c \
			vncpixelformat.h vncpixelformat.c \
			vncaudioformat.h vncaudioformat.c \
			vncaudiosample.h vncaudiosample.c \
			vncaudio.h vncaudio.c \
			vncbaseaudio.h vncbaseaudio.c \
			vncframebuffer.h vncframebuffer.c \
			vncbaseframebufferblt.h \
			vncbaseframebuffer.h vncbaseframebuffer.c \
			vnccursor.h vnccursor.c \
			vnccolormap.h vnccolormap.c \
			vncconnection.h vncconnection.c \
			vncconnectionblt.h \
			vncmarshal.h vncmarshal.c \
			vncutil.h vncutil.c
nodist_libgvnc_1_0_la_SOURCES = \
			vncconnectionenums.h vncconnectionenums.c \
			$(NULL)

if HAVE_PULSEAUDIO
lib_LTLIBRARIES += libgvncpulse-1.0.la

gvncpulse_includedir = $(includedir)/gvncpulse-1.0/
gvncpulse_include_HEADERS = \
		       gvncpulse.h \
                       vncaudiopulse.h

libgvncpulse_1_0_la_SOURCES = \
                       vncaudiopulse.h vncaudiopulse.c
libgvncpulse_1_0_la_LIBADD = \
                       $(GOBJECT_LIBS) \
                       $(PULSEAUDIO_LIBS) \
                       libgvnc-1.0.la
libgvncpulse_1_0_la_CFLAGS = \
                       $(GOBJECT_CFLAGS) \
                       $(PULSEAUDIO_CFLAGS) \
                       $(WARN_CFLAGS) \
                       -DSYSCONFDIR=\""$(sysconfdir)"\" \
                       -DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\" \
                       -DG_LOG_DOMAIN=\"gtk-vnc\"
libgvncpulse_1_0_la_LDFLAGS = \
                       $(VERSION_SCRIPT_FLAGS)$(GVNCPULSE_VERSION_FILE) \
                       -version-info 0:1:0 $(NO_UNDEFINED_FLAGS)
endif

if WITH_UCONTEXT
libgvnc_1_0_la_SOURCES += continuation.h continuation.c coroutine_ucontext.c
EXTRA_DIST += coroutine_gthread.c
else
libgvnc_1_0_la_SOURCES += coroutine_gthread.c
EXTRA_DIST += continuation.h continuation.c coroutine_ucontext.c
endif

gtk_vnc_LIBADD = \
			$(GTK_LIBS) \
			$(X11_LIBS) \
			libgvnc-1.0.la
gtk_vnc_CFLAGS = \
			$(GTK_CFLAGS) \
			$(X11_CFLAGS) \
			$(WARN_CFLAGS) \
			-DSYSCONFDIR=\""$(sysconfdir)"\" \
			-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\" \
			-DG_LOG_DOMAIN=\"gtk-vnc\"

gtk_vnc_HDRS = \
			gtk-vnc.h \
			vncdisplay.h \
			vncgrabsequence.h \
			vncimageframebuffer.h \
			$(NULL)

nodist_gtk_vnc_HDRS = \
			vncdisplayenums.h \
			$(NULL)

gtk_vnc_SOURCES = \
			vnccairoframebuffer.h vnccairoframebuffer.c \
			vncdisplay.h vncdisplay.c \
			vncdisplaykeymap.h vncdisplaykeymap.c \
			vncgrabsequence.h vncgrabsequence.c \
			vncmarshal.h vncmarshal.c
			$(NULL)
nodist_gtk_vnc_SOURCES = \
			vncdisplayenums.h vncdisplayenums.c \
			$(NULL)

if HAVE_GTK_2
gtk_vnc_SOURCES += \
			vncimageframebuffer.h vncimageframebuffer.c
else
EXTRA_DIST +=		vncimageframebuffer.h vncimageframebuffer.c
endif

gtk_vnc_LDFLAGS = \
			$(VERSION_SCRIPT_FLAGS)$(GTK_VNC_VERSION_FILE) \
			$(NO_UNDEFINED_FLAGS)

if HAVE_GTK_2
lib_LTLIBRARIES += libgtk-vnc-1.0.la
libgtk_vnc_1_0_ladir = $(includedir)/gtk-vnc-$(GTK_VNC_API_VERSION)/
libgtk_vnc_1_0_la_LIBADD = $(gtk_vnc_LIBADD)
libgtk_vnc_1_0_la_CFLAGS = $(gtk_vnc_CFLAGS)
libgtk_vnc_1_0_la_LDFLAGS = $(gtk_vnc_LDFLAGS) -version-info 0:1:0
libgtk_vnc_1_0_la_SOURCES = $(gtk_vnc_SOURCES)
libgtk_vnc_1_0_la_HEADERS = $(gtk_vnc_HDRS)
nodist_libgtk_vnc_1_0_la_SOURCES = $(nodist_gtk_vnc_SOURCES)
nodist_libgtk_vnc_1_0_la_HEADERS = $(nodist_gtk_vnc_HDRS)
else
lib_LTLIBRARIES += libgtk-vnc-2.0.la
libgtk_vnc_2_0_ladir = $(includedir)/gtk-vnc-$(GTK_VNC_API_VERSION)/
libgtk_vnc_2_0_la_LIBADD = $(gtk_vnc_LIBADD)
libgtk_vnc_2_0_la_CFLAGS = $(gtk_vnc_CFLAGS)
libgtk_vnc_2_0_la_LDFLAGS = $(gtk_vnc_LDFLAGS) -version-info 0:2:0
libgtk_vnc_2_0_la_SOURCES = $(gtk_vnc_SOURCES)
libgtk_vnc_2_0_la_HEADERS = $(gtk_vnc_HDRS)
nodist_libgtk_vnc_2_0_la_SOURCES = $(nodist_gtk_vnc_SOURCES)
nodist_libgtk_vnc_2_0_la_HEADERS = $(nodist_gtk_vnc_HDRS)
endif

EXTRA_DIST += keymap-gen.pl keymaps.csv

KEYMAP_GEN = $(srcdir)/keymap-gen.pl

KEYMAPS = \
	vncdisplaykeymap_xorgevdev2rfb.c \
	vncdisplaykeymap_xorgkbd2rfb.c \
	vncdisplaykeymap_xorgxquartz2rfb.c \
	vncdisplaykeymap_xorgxwin2rfb.c \
	vncdisplaykeymap_osx2rfb.c \
	vncdisplaykeymap_win322rfb.c \
	vncdisplaykeymap_x112rfb.c

$(KEYMAPS): $(KEYMAP_GEN) keymaps.csv
BUILT_SOURCES += $(KEYMAPS)

# Avoid need for perl(Text::CSV) by end users
EXTRA_DIST += $(KEYMAPS)
MAINTAINERCLEANFILES = $(KEYMAPS)

vncdisplaykeymap.c: $(KEYMAPS)

vncdisplaykeymap_xorgevdev2rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv xorgevdev rfb > $@ || rm $@

vncdisplaykeymap_xorgkbd2rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv xorgkbd rfb > $@ || rm $@

vncdisplaykeymap_xorgxquartz2rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv xorgxquartz rfb > $@ || rm $@

vncdisplaykeymap_xorgxwin2rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv xorgxwin rfb > $@ || rm $@

vncdisplaykeymap_osx2rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv osx rfb > $@ || rm $@

vncdisplaykeymap_win322rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv win32 rfb > $@ || rm $@

vncdisplaykeymap_x112rfb.c:
	$(AM_V_GEN)$(PERL) $(KEYMAP_GEN) $(srcdir)/keymaps.csv x11 rfb > $@ || rm $@

vncdisplayenums.c: vncdisplay.h
	$(AM_V_GEN)glib-mkenums    --fhead "#include <glib-object.h>\n" \
			--fhead "#include \"vncdisplayenums.h\"\n\n" \
			--fprod "\n/* enumerations from \"vncdisplay.h\" */" \
			--fprod "\n#include \"vncdisplay.h\"\n" \
			--vhead "static const G@Type@Value _@enum_name@_values[] = {" \
			--vprod "  { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "  { 0, NULL, NULL }\n};\n\n" \
			--vtail "GType\n@enum_name@_get_type (void)\n{\n" \
			--vtail "  static GType type = 0;\n\n" \
			--vtail "  if (!type)\n" \
			--vtail "    type = g_@type@_register_static (\"@EnumName@\", _@enum_name@_values);\n\n" \
			--vtail "  return type;\n}\n\n" \
		$< > $@

vncdisplayenums.h: vncdisplay.h
	$(AM_V_GEN)glib-mkenums    --fhead "#ifndef VNC_DISPLAY_ENUMS_H\n" \
			--fhead "#define VNC_DISPLAY_ENUMS_H\n\n" \
			--fhead "G_BEGIN_DECLS\n\n" \
			--ftail "G_END_DECLS\n\n" \
			--ftail "#endif /* VNC_DISPLAY_ENUMS_H */\n" \
			--fprod "\n/* --- vncdisplay.h --- */" \
			--eprod "#define VNC_TYPE_@ENUMSHORT@ @enum_name@_get_type()\n" \
			--eprod "GType @enum_name@_get_type (void);\n" \
		$< >  $@

vncconnectionenums.c: vncconnection.h
	$(AM_V_GEN)glib-mkenums    --fhead "#include <glib-object.h>\n" \
			--fhead "#include \"vncconnectionenums.h\"\n\n" \
			--fprod "\n/* enumerations from \"vncconnection.h\" */" \
			--fprod "\n#include \"vncconnection.h\"\n" \
			--vhead "static const G@Type@Value _@enum_name@_values[] = {" \
			--vprod "  { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "  { 0, NULL, NULL }\n};\n\n" \
			--vtail "GType\n@enum_name@_get_type (void)\n{\n" \
			--vtail "  static GType type = 0;\n\n" \
			--vtail "  if (!type)\n" \
			--vtail "    type = g_@type@_register_static (\"@EnumName@\", _@enum_name@_values);\n\n" \
			--vtail "  return type;\n}\n\n" \
		$< > $@

vncconnectionenums.h: vncconnection.h
	$(AM_V_GEN)glib-mkenums    --fhead "#ifndef VNC_CONNECTION_ENUMS_H\n" \
			--fhead "#define VNC_CONNECTION_ENUMS_H\n\n" \
			--fhead "G_BEGIN_DECLS\n\n" \
			--ftail "G_END_DECLS\n\n" \
			--ftail "#endif /* VNC_CONNECTION_ENUMS_H */\n" \
			--fprod "\n/* --- vncconnection.h --- */" \
			--eprod "#define VNC_TYPE_@ENUMSHORT@ @enum_name@_get_type()\n" \
			--eprod "GType @enum_name@_get_type (void);\n" \
		$< >  $@

vncmarshal.c: vncmarshal.txt
	$(AM_V_GEN)echo "#include \"vncmarshal.h\"" > $@ && \
		glib-genmarshal --body $< >> $@ || (rm -f $@ && exit 1)

vncmarshal.h: vncmarshal.txt
	$(AM_V_GEN)glib-genmarshal --header $< > $@ || (rm -f $@ && exit 1)

vncdisplay.c:: vncmarshal.h vncdisplayenums.h

vncconnection.c:: vncmarshal.h vncconnectionenums.h

MARSHAL_FILES = vncmarshal.c vncmarshal.h
ENUM_FILES = \
	vncdisplayenums.h vncdisplayenums.c \
	vncconnectionenums.h vncconnectionenums.c

BUILT_SOURCES += $(MARSHAL_FILES) $(ENUM_FILES)

CLEANFILES = $(MARSHAL_FILES) $(ENUM_FILES)

if WITH_PYTHON
pyexec_LTLIBRARIES = gtkvnc.la

gtkvnc_la_LIBADD = libgtk-vnc-$(GTK_VNC_API_VERSION).la $(PYGTK_LIBS)
gtkvnc_la_CFLAGS = $(GTK_CFLAGS) $(PYTHON_INCLUDES) \
		   $(PYGTK_CFLAGS) $(WARN_PYTHON_CFLAGS)
gtkvnc_la_LDFLAGS = -module -avoid-version -fPIC
gtkvnc_la_SOURCES = vncmodule.c
nodist_gtkvnc_la_SOURCES = vncmodule.defs.c
BUILT_SOURCES += vncmodule.defs.c

CODEGENDIR = $(shell pkg-config --variable=codegendir pygobject-2.0)
DEFSDIR = $(shell pkg-config --variable=defsdir pygtk-2.0)

vnc.defs: vncdisplay.h vncdisplayenums.h
	$(AM_V_GEN)$(PYTHON) $(CODEGENDIR)/h2def.py $< > $@

vncmodule.defs.c: vnc.override vnc.defs
	$(AM_V_GEN)pygtk-codegen-2.0 --prefix gtkvnc \
			  --register $(DEFSDIR)/gdk-types.defs \
			  --register $(DEFSDIR)/gtk-types.defs \
			  --override $(srcdir)/vnc.override vnc.defs > $@

CLEANFILES += vnc.defs vncmodule.defs.c
EXTRA_DIST += vnc.override

else
EXTRA_DIST += vnc.override vncmodule.c
endif

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --symbol-prefix=vnc --warn-all \
			--identifier-prefix=Vnc \
			--add-include-path=$(top_srcdir) \
			--add-include-path=$(srcdir) \
			--add-include-path=$(builddir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(top_srcdir) \
			--includedir=$(srcdir)

if HAVE_INTROSPECTION
GVNC_INTROSPECTION_SRCS = \
			$(srcdir)/vncpixelformat.h \
			$(srcdir)/vncaudioformat.h $(srcdir)/vncaudioformat.c \
			$(srcdir)/vncaudiosample.h $(srcdir)/vncaudiosample.c \
			$(srcdir)/vncaudio.h $(srcdir)/vncaudio.c \
			$(srcdir)/vncbaseaudio.h $(srcdir)/vncbaseaudio.c \
			$(srcdir)/vncframebuffer.h $(srcdir)/vncframebuffer.c \
			$(srcdir)/vncbaseframebuffer.h $(srcdir)/vncbaseframebuffer.c \
			$(srcdir)/vnccolormap.h $(srcdir)/vnccolormap.c \
			$(srcdir)/vnccursor.h $(srcdir)/vnccursor.c \
			$(srcdir)/vncconnection.h $(srcdir)/vncconnection.c \
			$(builddir)/vncconnectionenums.h $(builddir)/vncconnectionenums.c \
			$(srcdir)/vncutil.h $(srcdir)/vncutil.c

GVNCPULSE_INTROSPECTION_SRCS = \
			$(srcdir)/vncaudiopulse.h $(srcdir)/vncaudiopulse.c

GTK_VNC_INTROSPECTION_SRCS = \
			$(srcdir)/vnccairoframebuffer.h $(srcdir)/vnccairoframebuffer.c \
			$(srcdir)/vncdisplay.h $(srcdir)/vncdisplay.c \
			$(srcdir)/vncgrabsequence.h $(srcdir)/vncgrabsequence.c \
			$(builddir)/vncdisplayenums.h $(builddir)/vncdisplayenums.c

if HAVE_GTK_2
GTK_VNC_INTROSPECTION_SRCS += \
			$(srcdir)/vncimageframebuffer.h $(srcdir)/vncimageframebuffer.c
endif

GVnc-1.0.gir: libgvnc-1.0.la
GVnc_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
GVnc_1_0_gir_PACKAGES = gobject-2.0 gio-2.0
GVnc_1_0_gir_EXPORT_PACKAGES = gvnc-1.0
GVnc_1_0_gir_LIBS = libgvnc-1.0.la
GVnc_1_0_gir_FILES = $(GVNC_INTROSPECTION_SRCS)
GVnc_1_0_gir_CFLAGS = $(libgvnc_1_0_la_CFLAGS) -I$(srcdir)
GVnc_1_0_gir_SCANNERFLAGS = --c-include="gvnc.h"
INTROSPECTION_GIRS += GVnc-1.0.gir

if HAVE_PULSEAUDIO
GVncPulse-1.0.gir: libgvncpulse-1.0.la GVnc-1.0.gir
GVncPulse_1_0_gir_INCLUDES = GObject-2.0
GVncPulse_1_0_gir_PACKAGES = gobject-2.0 libpulse-simple
GVncPulse_1_0_gir_EXPORT_PACKAGES = gvnc-1.0
GVncPulse_1_0_gir_LIBS = libgvnc-1.0.la libgvncpulse-1.0.la
GVncPulse_1_0_gir_FILES = $(GVNCPULSE_INTROSPECTION_SRCS)
GVncPulse_1_0_gir_CFLAGS = $(libgvnc_1_0_la_CFLAGS) $(libgvncpulse_1_0_la_CFLAGS) -I$(srcdir)
GVncPulse_1_0_gir_SCANNERFLAGS = --c-include="gvncpulse.h" \
	--include-uninstalled=$(top_builddir)/src/GVnc-1.0.gir
INTROSPECTION_GIRS += GVncPulse-1.0.gir
endif

if HAVE_GTK_2
GtkVnc-1.0.gir: libgtk-vnc-$(GTK_VNC_API_VERSION).la GVnc-1.0.gir
GtkVnc_1_0_gir_INCLUDES = GObject-2.0 Gtk-2.0
GtkVnc_1_0_gir_PACKAGES = gobject-2.0 gtk+-2.0
GtkVnc_1_0_gir_EXPORT_PACKAGES = gtk-vnc-$(GTK_VNC_API_VERSION)
GtkVnc_1_0_gir_LIBS = libgvnc-1.0.la libgtk-vnc-$(GTK_VNC_API_VERSION).la
GtkVnc_1_0_gir_FILES = $(GTK_VNC_INTROSPECTION_SRCS)
GtkVnc_1_0_gir_CFLAGS = $(libgvnc_1_0_la_CFLAGS) $(libgvncpulse_1_0_la_CFLAGS) -I$(srcdir)
GtkVnc_1_0_gir_SCANNERFLAGS = --c-include="gtk-vnc.h" \
	--include-uninstalled=$(top_builddir)/src/GVnc-1.0.gir
INTROSPECTION_GIRS += GtkVnc-1.0.gir
else
GtkVnc-2.0.gir: libgtk-vnc-2.0.la GVnc-1.0.gir
GtkVnc_2_0_gir_INCLUDES = GObject-2.0 Gtk-3.0
GtkVnc_2_0_gir_PACKAGES = gobject-2.0 gtk+-3.0
GtkVnc_2_0_gir_EXPORT_PACKAGES = gtk-vnc-$(GKT_VNC_API_VERSION)
GtkVnc_2_0_gir_LIBS = libgvnc-1.0.la libgtk-vnc-$(GTK_VNC_API_VERSION).la
GtkVnc_2_0_gir_FILES = $(GTK_VNC_INTROSPECTION_SRCS)
GtkVnc_2_0_gir_CFLAGS = $(libgvnc_1_0_la_CFLAGS) $(libgvncpulse_1_0_la_CFLAGS) -I$(srcdir)
GtkVnc_2_0_gir_SCANNERFLAGS = --c-include="gtk-vnc.h" \
	--include-uninstalled=$(top_builddir)/src/GVnc-1.0.gir
INTROSPECTION_GIRS += GtkVnc-2.0.gir
endif

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelibs_DATA)

endif # HAVE_INTROSPECTION

-include $(top_srcdir)/git.mk
