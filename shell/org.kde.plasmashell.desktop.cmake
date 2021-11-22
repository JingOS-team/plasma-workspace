[Desktop Entry]
Exec=@CMAKE_INSTALL_PREFIX@/bin/plasmashell
X-DBUS-StartupType=Unique
Name=JingOS Desktop
Name[da]=JingOS Desktop
Name[en_GB]=JingOS Desktop
Name[en_US]=JingOS Desktop
Name[zh_CN]=JingOS桌面
Type=Application
X-KDE-StartupNotify=false
X-DBUS-ServiceName=org.kde.plasmashell
OnlyShowIn=KDE;
X-KDE-autostart-phase=0
Icon=plasmashell
NoDisplay=true
X-systemd-skip=true

X-KDE-Wayland-Interfaces=org_kde_plasma_window_management,org_kde_kwin_keystate,zkde_screencast_unstable_v1
X-KDE-DBUS-Restricted-Interfaces=org.kde.kwin.Screenshot
