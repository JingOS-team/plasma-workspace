set(statuspanel_engine_SRCS
    statuspanel.cpp
)


set(krunner_xml ${plasma-workspace_SOURCE_DIR}/krunner/dbus/org.kde.krunner.App.xml)
qt5_add_dbus_interface(statuspanel_engine_SRCS ${krunner_xml} krunner_interface)

add_library(plasma_engine_statuspanel MODULE ${statuspanel_engine_SRCS})

target_link_libraries(plasma_engine_statuspanel
    KF5::Plasma
    KF5::Service
    KF5::CoreAddons
    Qt::DBus
    KF5::Solid
)

kcoreaddons_desktop_to_json(plasma_engine_statuspanel plasma-dataengine-statuspanel.desktop)

install(TARGETS plasma_engine_statuspanel DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/dataengine)
install(FILES plasma-dataengine-statuspanel.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )
