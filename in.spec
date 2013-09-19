Name:		icmpecho
Version:	@@VERSION@@
Release:	1%{?dist}
Summary:	Packet encode/decode of ICMPv4/6 echo and reply.

Group:		zenoss
License:	Commercial
URL:		http://www.zenoss.com/
Source:		%{name}-%{version}.tar.gz
Prefix:		@@PREFIX@@

%description

%prep
%setup -q -n %{name}

%build
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}@@PREFIX@@

%files
%attr(4755, root, root) @@PREFIX@@/bin/pyraw
%attr(-, -, -) @@PREFIX@@/lib/python/icmpecho*

%changelog

