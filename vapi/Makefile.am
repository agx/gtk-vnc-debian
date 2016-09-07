NULL =

if WITH_VALA
vapidir = $(datadir)/vala/vapi
vapi_DATA =					\
	gtk-vnc-$(GTK_VNC_API_VERSION).deps	\
	gtk-vnc-$(GTK_VNC_API_VERSION).vapi	\
	gvnc-1.0.vapi				\
	$(NULL)

if HAVE_PULSEAUDIO
vapi_DATA += \
	gvncpulse-1.0.vapi
endif

gvnc-1.0.vapi: $(top_builddir)/src/GVnc-1.0.gir
	$(AM_V_GEN)$(VAPIGEN)			\
		--pkg gobject-2.0		\
		--pkg gio-2.0			\
		--library gvnc-1.0		\
		$<

if HAVE_PULSEAUDIO
gvncpulse-1.0.vapi: $(top_builddir)/src/GVncPulse-1.0.gir gvnc-1.0.vapi
	$(AM_V_GEN)$(VAPIGEN)			\
		--vapidir=$(builddir)		\
		--pkg gobject-2.0		\
		--pkg gio-2.0			\
		--pkg gvnc-1.0			\
		--library gvncpulse-1.0		\
		--girdir=$(top_builddir)/src	\
		$<
endif

gtk-vnc-$(GTK_VNC_API_VERSION).vapi: $(top_builddir)/src/GtkVnc-$(GTK_VNC_API_VERSION).gir gvnc-1.0.vapi
	$(AM_V_GEN)$(VAPIGEN)			\
		--vapidir=$(builddir)		\
		--pkg gobject-2.0		\
		--pkg gio-2.0			\
		--pkg gvnc-1.0			\
		--pkg gtk+-$(GTK_API_VERSION)	\
		--library gtk-vnc-$(GTK_VNC_API_VERSION) \
		--girdir=$(top_builddir)/src	\
		$<

gtk-vnc-$(GTK_VNC_API_VERSION).deps: gtk-vnc-$(GTK_VNC_API_VERSION).vapi
	@echo "gvnc-1.0" > $@
	@echo "gtk+-$(GTK_API_VERSION)" >> $@

endif

CLEANFILES = $(vapi_DATA)

-include $(top_srcdir)/git.mk
