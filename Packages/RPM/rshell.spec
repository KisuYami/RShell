Name:           RShell
Version:        1.5
Release:        1%{?dist}
Summary:        Minimal shell
License:        BSD3
Packager: 	    Reberti Carvalho Soares
URL:            https://github.com/KisuYami/RShell
Source0:        RShell-%{version}.tar.gz

BuildRequires:  gcc
BuildRequires:  libncurses6
BuildRequires:  libreadline8
BuildRequires:  readline-devel

%description
RShell  is  a  minimal shell program, do not expect scripting or some thing fancy, rshell is only a shell and nothing more.

%prep
%setup -q

%build
%{make_build}

%install
%{make_install} PREFIX=%{_prefix}

%files
/usr/bin/rshell
/usr/share/man/man1/rshell.1.gz
/usr/share/man/pt_BR/man1/rshell.1.gz
