SUBDIRS = src tools examples po vapi
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

pkgconfig_DATA = $(PACKAGE)-$(GTK_VNC_API_VERSION).pc gvnc-1.0.pc
pkgconfigdir = $(libdir)/pkgconfig

if HAVE_PULSEAUDIO
pkgconfig_DATA += gvncpulse-1.0.pc
endif

EXTRA_DIST =				\
	gvnc-1.0.pc.in			\
	gvncpulse-1.0.pc.in		\
	$(PACKAGE)-1.0.pc.in		\
	$(PACKAGE)-2.0.pc.in		\
	$(PACKAGE).spec.in		\
	$(PACKAGE).spec			\
	mingw-$(PACKAGE).spec.in	\
	intltool-extract.in		\
	intltool-merge.in		\
	intltool-update.in		\
	cfg.mk				\
	GNUmakefile			\
	maint.mk			\
	AUTHORS.in			\
	ChangeLog-old			\
	$(NULL)

DISTCLEANFILES =			\
	intltool-extract		\
	intltool-merge			\
	intltool-update

MAINTAINERCLEANFILES = 		\
	INSTALL			\
	compile			\
	aclocal.m4		\
	config.guess		\
	config.h.in		\
	config.sub		\
	depcomp			\
	gtk-doc.make		\
	install-sh		\
	ltmain.sh		\
	missing			\
	mkinstalldirs		\
	omf.make		\
	py-compile		\
	xmldocs.make		\
	m4/libtool.m4			\
	m4/lt~obsolete.m4		\
	m4/ltoptions.m4			\
	m4/ltsugar.m4			\
	m4/ltversion.m4			\
	build-aux/compile		\
	build-aux/config.guess		\
	build-aux/config.sub		\
	build-aux/depcomp		\
	build-aux/install-sh		\
	build-aux/ltmain.sh		\
	build-aux/missing		\
	build-aux/mkinstalldirs		\
	`find "$(srcdir)" -type f -name Makefile.in -print`

GITIGNOREFILES = plugin/gtk-vnc-plugin.la plugin/gtk-vnc-plugin.so

gen_start_date = 2008-12-07
dist-hook: gen-ChangeLog gen-AUTHORS

.PHONY: gen-ChangeLog
gen-ChangeLog:
	$(AM_V_GEN)if test -d $(srcdir)/.git; then	\
	  $(top_srcdir)/build-aux/gitlog-to-changelog	\
	    --since=$(gen_start_date) > $(distdir)/cl-t;\
	  rm -f $(distdir)/ChangeLog;			\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;	\
	fi

.PHONY: gen-AUTHORS
gen-AUTHORS:
	$(AM_V_GEN)if test -d $(srcdir)/.git; then 			\
	    out="`cd $(srcdir) && git log --pretty=format:'%aN <%aE>' | sort -u`" && \
	    perl -p -e "s/#authorslist#// and print '$$out'"		\
	      < $(srcdir)/AUTHORS.in > $(distdir)/AUTHORS-tmp &&	\
	    mv -f $(distdir)/AUTHORS-tmp $(distdir)/AUTHORS ;		\
	fi

-include $(top_srcdir)/git.mk
