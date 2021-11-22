ecm_find_qmlmodule(org.kde.plasma.core 2.0)

# KI18N Translation Domain for this library.
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_translations\")

########### next target ###############

set(kcm_translations_PART_SRCS translations.cpp translationsmodel.cpp translationssettings.cpp translatetool.cpp
    regionmanager.cpp
    resources.qrc)

kcmutils_generate_module_data(
    kcm_translations_PART_SRCS
    MODULE_DATA_HEADER translationsdata.h
    MODULE_DATA_CLASS_NAME TranslationsData
    SETTINGS_HEADERS translationssettings.h
    SETTINGS_CLASSES TranslationsSettings
)

kconfig_add_kcfg_files(kcm_translations_PART_SRCS translationssettingsbase.kcfgc GENERATE_MOC)

add_library(kcm_translations MODULE ${kcm_translations_PART_SRCS})

target_link_libraries(kcm_translations
    PW::KWorkspace
    KF5::I18n
    KF5::KCMUtils
    KF5::QuickAddons
    KF5::ConfigCore
)

kcoreaddons_desktop_to_json(kcm_translations "kcm_translations.desktop")

########### install files ###############

set(DATA_INSTALL_DIR share CACHE PATH "Data file installation directory")

install(TARGETS kcm_translations DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
install(FILES kcm_translations.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR})
install(FILES plasmashell.mo DESTINATION ${DATA_INSTALL_DIR}/locale/zh_CN/LC_MESSAGES/)

kpackage_install_package(package kcm_translations kcms)

