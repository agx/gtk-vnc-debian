# -*- rpm-spec -*-

%global with_gir 0
%if 0%{?fedora} >= 12 || 0%{?rhel} >= 7
%global with_gir 1
%endif

%global with_gtk3 0
%if 0%{?fedora} >= 15 || 0%{?rhel} >= 7
%global with_gtk3 1
%endif

%global with_vala 0
%if 0%{with_gtk3}
%global with_vala 1
%endif

%global with_pulse 1

Summary: A GTK2 widget for VNC clients
Name: gtk-vnc
Version: 0.5.2
Release: 1%{?dist}%{?extra_release}
License: LGPLv2+
Group: Development/Libraries
Source: http://ftp.gnome.org/pub/GNOME/sources/%{name}/0.5/%{name}-%{version}.tar.xz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
URL: http://live.gnome.org/gtk-vnc
Requires: gvnc = %{version}-%{release}
BuildRequires: gtk2-devel >= 2.14
BuildRequires: pygtk2-devel python-devel zlib-devel
BuildRequires: gnutls-devel cyrus-sasl-devel intltool
%if %{with_gir}
BuildRequires: gobject-introspection-devel
%if 0%{?fedora} && 0%{?fedora} < 14
BuildRequires: gir-repository-devel
%endif
%endif
%if %{with_gtk3}
BuildRequires: gtk3-devel
%endif
%if %{with_vala}
BuildRequires: vala-tools
%endif
%if %{with_pulse}
BuildRequires: pulseaudio-libs-devel
%endif
BuildRequires: /usr/bin/pod2man

%description
gtk-vnc is a VNC viewer widget for GTK2. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

%package devel
Summary: Development files to build GTK2 applications with gtk-vnc
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: pkgconfig
Requires: gtk2-devel

%description devel
gtk-vnc is a VNC viewer widget for GTK2. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

Libraries, includes, etc. to compile with the gtk-vnc library

%package python
Summary: Python bindings for the gtk-vnc library
Group: Development/Libraries
Requires: %{name} = %{version}

%description python
gtk-vnc is a VNC viewer widget for GTK2. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

A module allowing use of the GTK-VNC widget from python

%package -n gvnc
Summary: A GObject for VNC connections

%description -n gvnc
gvnc is a GObject for managing a VNC connection. It provides all the
infrastructure required to build a VNC client without having to deal
with the raw protocol itself.

%package -n gvnc-devel
Summary: Libraries, includes, etc. to compile with the gvnc library
Group: Development/Libraries
Requires: gvnc = %{version}-%{release}
Requires: pkgconfig

%description -n gvnc-devel
gvnc is a GObject for managing a VNC connection. It provides all the
infrastructure required to build a VNC client without having to deal
with the raw protocol itself.

Libraries, includes, etc. to compile with the gvnc library

%if %{with_pulse}
%package -n gvncpulse
Summary: A Pulse Audio bridge for VNC connections

%description -n gvncpulse
gvncpulse is a bridge to the Pulse Audio system for VNC.
It allows VNC clients to play back audio on the local
system

%package -n gvncpulse-devel
Summary: Libraries, includes, etc. to compile with the gvncpulse library
Group: Development/Libraries
Requires: gvncpulse = %{version}-%{release}
Requires: pkgconfig

%description -n gvncpulse-devel
gvncpulse is a bridge to the Pulse Audio system for VNC.
It allows VNC clients to play back audio on the local
system

Libraries, includes, etc. to compile with the gvnc library
%endif

%package -n gvnc-tools
Summary: Command line VNC tools
Group: Applications/Internet

%description -n gvnc-tools
Provides useful command line utilities for interacting with
VNC servers. Includes the gvnccapture program for capturing
screenshots of a VNC desktop

%if %{with_gtk3}
%package -n gtk-vnc2
Summary: A GTK3 widget for VNC clients
Group: Applications/Internet
Requires: gvnc = %{version}-%{release}

%description -n gtk-vnc2
gtk-vnc is a VNC viewer widget for GTK3. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

%package -n gtk-vnc2-devel
Summary: Development files to build GTK3 applications with gtk-vnc
Group: Development/Libraries
Requires: gtk-vnc2 = %{version}-%{release}
Requires: pkgconfig
Requires: gtk3-devel

%description -n gtk-vnc2-devel
gtk-vnc is a VNC viewer widget for GTK3. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

Libraries, includes, etc. to compile with the gtk-vnc library
%endif

%prep
%setup -q -n gtk-vnc-%{version} -c
%if %{with_gtk3}
cp -a gtk-vnc-%{version} gtk-vnc2-%{version}
%endif

%build
%if %{with_gir}
%define gir_arg --enable-introspection=yes
%else
%define gir_arg --enable-introspection=no
%endif

cd gtk-vnc-%{version}
%configure --with-gtk=2.0 %{gir_arg}
%__make %{?_smp_mflags} V=1
cd ..

%if %{with_gtk3}
cd gtk-vnc2-%{version}
%configure --with-gtk=3.0 %{gir_arg}
%__make %{?_smp_mflags} V=1
cd ..
%endif

%install
rm -fr %{buildroot}
cd gtk-vnc-%{version}
%__make install DESTDIR=%{buildroot}
cd ..

%if %{with_gtk3}
cd gtk-vnc2-%{version}
%__make install DESTDIR=%{buildroot}
cd ..
%endif

rm -f %{buildroot}%{_libdir}/*.a
rm -f %{buildroot}%{_libdir}/*.la
rm -f %{buildroot}%{_libdir}/python*/site-packages/*.a
rm -f %{buildroot}%{_libdir}/python*/site-packages/*.la

%find_lang %{name}

%clean
rm -fr %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post -n gvnc -p /sbin/ldconfig

%postun -n gvnc -p /sbin/ldconfig

%post -n gvncpulse -p /sbin/ldconfig

%postun -n gvncpulse -p /sbin/ldconfig

%post -n gtk-vnc2 -p /sbin/ldconfig

%postun -n gtk-vnc2 -p /sbin/ldconfig

%files
%defattr(-, root, root)
%{_libdir}/libgtk-vnc-1.0.so.*
%if %{with_gir}
%{_libdir}/girepository-1.0/GtkVnc-1.0.typelib
%endif

%files devel
%defattr(-, root, root)
%doc gtk-vnc-%{version}/examples/gvncviewer.c
%if %{with_gir}
%doc gtk-vnc-%{version}/examples/gvncviewer.js
%doc gtk-vnc-%{version}/examples/gvncviewer.pl
%doc gtk-vnc-%{version}/examples/gvncviewer-introspection.py
%endif
%{_libdir}/libgtk-vnc-1.0.so
%dir %{_includedir}/%{name}-1.0/
%{_includedir}/%{name}-1.0/*.h
%{_libdir}/pkgconfig/%{name}-1.0.pc
%if %{with_gir}
%{_datadir}/gir-1.0/GtkVnc-1.0.gir
%endif

%files python
%defattr(-, root, root)
%doc gtk-vnc-%{version}/examples/gvncviewer-bindings.py
%{_libdir}/python*/site-packages/gtkvnc.so

%files -n gvnc -f %{name}.lang
%defattr(-, root, root)
%{_libdir}/libgvnc-1.0.so.*
%if %{with_gir}
%{_libdir}/girepository-1.0/GVnc-1.0.typelib
%endif
%if %{with_vala}
%{_datadir}/vala/vapi/gvnc-1.0.vapi
%endif

%files -n gvnc-devel
%defattr(-, root, root)
%{_libdir}/libgvnc-1.0.so
%dir %{_includedir}/gvnc-1.0/
%{_includedir}/gvnc-1.0/*.h
%{_libdir}/pkgconfig/gvnc-1.0.pc
%if %{with_gir}
%{_datadir}/gir-1.0/GVnc-1.0.gir
%endif

%if %{with_pulse}
%files -n gvncpulse -f %{name}.lang
%defattr(-, root, root)
%{_libdir}/libgvncpulse-1.0.so.*
%if %{with_gir}
%{_libdir}/girepository-1.0/GVncPulse-1.0.typelib
%endif
%if %{with_vala}
%{_datadir}/vala/vapi/gvncpulse-1.0.vapi
%endif

%files -n gvncpulse-devel
%defattr(-, root, root)
%{_libdir}/libgvncpulse-1.0.so
%dir %{_includedir}/gvncpulse-1.0/
%{_includedir}/gvncpulse-1.0/*.h
%{_libdir}/pkgconfig/gvncpulse-1.0.pc
%if %{with_gir}
%{_datadir}/gir-1.0/GVncPulse-1.0.gir
%endif
%endif

%files -n gvnc-tools
%defattr(-, root, root)
%doc gtk-vnc-%{version}/AUTHORS
%doc gtk-vnc-%{version}/ChangeLog
%doc gtk-vnc-%{version}/ChangeLog-old
%doc gtk-vnc-%{version}/NEWS
%doc gtk-vnc-%{version}/README
%doc gtk-vnc-%{version}/COPYING.LIB
%{_bindir}/gvnccapture
%{_mandir}/man1/gvnccapture.1*

%if %{with_gtk3}
%files -n gtk-vnc2
%defattr(-, root, root)
%{_libdir}/libgtk-vnc-2.0.so.*
%if %{with_gir}
%{_libdir}/girepository-1.0/GtkVnc-2.0.typelib
%endif
%if %{with_vala}
%{_datadir}/vala/vapi/gtk-vnc-2.0.deps
%{_datadir}/vala/vapi/gtk-vnc-2.0.vapi
%endif

%files -n gtk-vnc2-devel
%defattr(-, root, root)
%doc gtk-vnc2-%{version}/examples/gvncviewer.c
%if %{with_gir}
%doc gtk-vnc2-%{version}/examples/gvncviewer.js
%endif
%{_libdir}/libgtk-vnc-2.0.so
%dir %{_includedir}/%{name}-2.0/
%{_includedir}/%{name}-2.0/*.h
%{_libdir}/pkgconfig/%{name}-2.0.pc
%if %{with_gir}
%{_datadir}/gir-1.0/GtkVnc-2.0.gir
%endif
%endif

%changelog
