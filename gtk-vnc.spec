# -*- rpm-spec -*-

Summary: A GTK widget for VNC clients
Name: gtk-vnc
Version: 0.3.3
Release: 1%{?dist}%{?extra_release}
License: LGPL
Group: Development/Libraries
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
URL: http://gtk-vnc.sf.net/
BuildRequires: gtk2-devel pygtk2-devel python-devel zlib-devel

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

%prep
%setup -q

%build
%configure
make

%install
rm -fr %{buildroot}
%makeinstall
rm -f %{buildroot}%{_libdir}/*.a
rm -f %{buildroot}%{_libdir}/*.la
rm -f %{buildroot}%{_libdir}/python*/site-packages/*.a
rm -f %{buildroot}%{_libdir}/python*/site-packages/*.la

%clean
rm -fr %{buildroot}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

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

%changelog
* Thu Sep 13 2007 Daniel P. Berrange <berrange@redhat.com> - 0.2.0-1
- Support client cursor offload
- Fix mixed endian client/server handling
- Fix colour depths conversions

* Wed Aug 15 2007 Daniel P. Berrange <berrange@redhat.com> - 0.1.0-1
- Initial official release
