# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
########### next target ###############

set(kontact_summaryplugin_PART_SRCS summaryview_plugin.cpp summaryview_part.cpp dropwidget.cpp summaryview.qrc
    summaryview_plugin.h summaryview_part.h dropwidget.h)

qt5_add_dbus_interfaces(kontact_summaryplugin_PART_SRCS ${kmail_BINARY_DIR}/src/org.kde.kmail.kmail.xml)

if(TARGET KF5::KCMUtils)
    add_library(kontact_summaryplugin MODULE ${kontact_summaryplugin_PART_SRCS})
    add_dependencies(kontact_summaryplugin kmail_xml)

    target_link_libraries(kontact_summaryplugin
        KF5::IconThemes
        KF5::KCMUtils
        KF5::I18n
        KF5::Parts
        KPim5::IdentityManagementCore
        KPim5::KontactInterface
        KPim5::PimCommon
        KF5::ConfigCore
    )

    add_library(kcmkontactsummary MODULE kcmkontactsummary.cpp kcmkontactsummary.h)

    target_link_libraries(kcmkontactsummary
        KF5::IconThemes
        KF5::KCMUtils
        KF5::I18n
        Qt::Widgets
    )

    install(TARGETS kontact_summaryplugin DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim5/kontact)
    install(TARGETS kcmkontactsummary DESTINATION ${KDE_INSTALL_PLUGINDIR}/pim5/kcms/summary)
endif()

########### next target ###############

