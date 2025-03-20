# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
########### next target ###############

set(kontact_kmail_plugins_interface_common_SRCS)

ecm_qt_declare_logging_category(kontact_kmail_plugins_interface_common_SRCS HEADER kmailplugin_debug.h IDENTIFIER KMAILPLUGIN_LOG CATEGORY_NAME org.kde.pim.kmail_plugin
        DESCRIPTION "kmail (kmail kontact plugins)"
        OLD_CATEGORY_NAMES log_kmail_plugin
        EXPORT KMAIL
)


set(kontact_kmail_plugins_interface_SRCS)
qt5_add_dbus_interfaces(kontact_kmail_plugins_interface_SRCS ${kmail_BINARY_DIR}/src/org.kde.kmail.kmail.xml)
add_library(kontact_kmailplugin MODULE)
target_sources(kontact_kmailplugin PRIVATE kmail_plugin.cpp summarywidget.cpp kmail_plugin.h summarywidget.h ${kontact_kmail_plugins_interface_SRCS} ${kontact_kmail_plugins_interface_common_SRCS})
add_dependencies(kontact_kmailplugin kmail_xml)

target_link_libraries(kontact_kmailplugin KPim5::Mime KF5::I18n KPim5::KontactInterface KF5::CalendarCore KPim5::CalendarUtils KPim5::AkonadiCore KF5::Contacts KPim5::AkonadiWidgets Qt::DBus)

########### next target ###############

if(TARGET KF5::KCMUtils)
    add_library(kcmkmailsummary MODULE kmailsummary.cpp)

    target_link_libraries(kcmkmailsummary
        KF5::KCMUtils
        KF5::I18n
        KF5::IconThemes
        KF5::KIOCore
    )

    install(TARGETS kcmkmailsummary DESTINATION ${KDE_INSTALL_PLUGINDIR})
endif()

########### install files ###############

install(TARGETS kontact_kmailplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim5/kontact)
