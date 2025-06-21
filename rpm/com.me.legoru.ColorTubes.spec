Name:       com.me.legoru.ColorTubes

Summary:    ColorTubes
Version:    0.9
Release:    5
Group:      Qt/Qt
License:    MIT
URL:        https://github.com/goroleo/ColorTubes
Source0:    %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)

%description
Color sort game for Aurora OS

%prep
%autosetup

%build
%qmake5 \
    VERSION=%{version} \
    RELEASE=%{release}

%make_build

%install
%make_install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%defattr(644,root,root,-)
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
