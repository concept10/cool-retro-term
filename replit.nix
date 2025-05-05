
{ pkgs }: {
  deps = [
    pkgs.qt5.qtbase
    pkgs.qt5.qtdeclarative
    pkgs.qt5.qtquickcontrols
    pkgs.qt5.qtgraphicaleffects
    pkgs.qt5.qttools
    pkgs.gcc
    pkgs.gnumake
    pkgs.qmake
    pkgs.pkg-config
  ];
}
