# -*- rpm-spec -*-

# Plugin isn't ready for real world use yet - it needs
# a security audit at very least
%define with_plugin 0

Summary: A GTK widget for VNC clients
Name: gtk-vnc
Version: 0.3.6
Release: 1%{?dist}%{?extra_release}
License: LGPLv2+
Group: Development/Libraries
Source: http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
URL: http://gtk-vnc.sf.net/
BuildRequires: gtk2-devel pygtk2-devel python-devel zlib-devel
BuildRequires: gnutls-devel gtkglext-devel
%if %{with_plugin}
%if "%{fedora}" > "8"
BuildRequires: xulrunner-devel
%else
BuildRequires: firefox-devel
%endif
%endif

%description
gtk-vnc is a VNC viewer widget for GTK. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

%package devel
Summary: Libraries, includes, etc. to compile with the gtk-vnc library
Group: Development/Libraries
Requires: %{name} = %{version}
Requires: pkgconfig
Requires: pygtk2-devel gtk2-devel

%description devel
gtk-vnc is a VNC viewer widget for GTK. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

Libraries, includes, etc. to compile with the gtk-vnc library

%package python
Summary: Python bindings for the gtk-vnc library
Group: Development/Libraries
Requires: %{name} = %{version}

%description python
gtk-vnc is a VNC viewer widget for GTK. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

A module allowing use of the GTK-VNC widget from python

%if %{with_plugin}
%package plugin
Summary: Mozilla plugin for the gtk-vnc library
Group: Development/Libraries
Requires: %{name} = %{version}

%description plugin
gtk-vnc is a VNC viewer widget for GTK. It is built using coroutines
allowing it to be completely asynchronous while remaining single threaded.

This package provides a web browser plugin for Mozilla compatible
browsers.
%endif

%prep
%setup -q

%build
%if %{with_plugin}
%configure --enable-plugin=yes
%else
%configure
%endif
%__make %{?_smp_mflags}

%install
rm -fr %{buildroot}
%__make install DESTDIR=%{buildroot}
rm -f %{buildroot}%{_libdir}/*.a
rm -f %{buildroot}%{_libdir}/*.la
rm -f %{buildroot}%{_libdir}/python*/site-packages/*.a
rm -f %{buildroot}%{_libdir}/python*/site-packages/*.la
%if %{with_plugin}
rm -f %{buildroot}%{_libdir}/mozilla/plugins/%{name}-plugin.a
rm -f %{buildroot}%{_libdir}/mozilla/plugins/%{name}-plugin.la
%endif

%clean
rm -fr %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)
%doc AUTHORS ChangeLog NEWS README COPYING.LIB
%{_libdir}/lib*.so.*

%files devel
%defattr(-, root, root)
%doc AUTHORS ChangeLog NEWS README COPYING.LIB
%doc examples/gvncviewer.c
%{_libdir}/lib*.so
%dir %{_includedir}/%{name}-1.0/
%{_includedir}/%{name}-1.0/*.h
%{_libdir}/pkgconfig/%{name}-1.0.pc

%files python
%defattr(-, root, root)
%doc AUTHORS ChangeLog NEWS README COPYING.LIB
%doc examples/gvncviewer.py
%{_libdir}/python*/site-packages/gtkvnc.so

%if %{with_plugin}
%files plugin
%defattr(-, root, root)
%{_libdir}/mozilla/plugins/%{name}-plugin.so
%endif

%changelog
* Thu Sep 13 2007 Daniel P. Berrange <berrange@redhat.com> - 0.2.0-1
- Support client cursor offload
- Fix mixed endian client/server handling
- Fix colour depths conversions

* Wed Aug 15 2007 Daniel P. Berrange <berrange@redhat.com> - 0.1.0-1
- Initial official release
