# SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>
# SPDX-License-Identifier: BSD-3-Clause
add_definitions(-DTRANSLATION_DOMAIN=\"akonadi_archivemail_agent\")

add_library(archivemailagent STATIC)
set_property(TARGET archivemailagent PROPERTY POSITION_INDEPENDENT_CODE ON)
target_sources(archivemailagent PRIVATE
    archivemailkernel.cpp
    archivemailmanager.cpp
    archivemailinfo.cpp
    job/archivejob.cpp
    archivemailagentutil.cpp
    archivemailkernel.h
    archivemailmanager.h
    archivemailinfo.h
    job/archivejob.h
    archivemailagentutil.h
)

kconfig_add_kcfg_files(archivemailagent
    settings/archivemailagentsettings.kcfgc
)


qt5_add_dbus_adaptor(libarchivemailagent_SRCS org.freedesktop.Akonadi.ArchiveMailAgent.xml archivemailagent.h ArchiveMailAgent)
target_sources(archivemailagent PRIVATE ${libarchivemailagent_SRCS})

ecm_qt_declare_logging_category(archivemailagent HEADER archivemailagent_debug.h IDENTIFIER ARCHIVEMAILAGENT_LOG CATEGORY_NAME org.kde.pim.archivemailagent
        DESCRIPTION "kmail (archivemailagent)"
        OLD_CATEGORY_NAMES log_archivemailagent
        EXPORT KMAIL
)


ki18n_wrap_ui(archivemailagent ui/archivemailwidget.ui)

#if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
#    set_target_properties(archivemailagent PROPERTIES UNITY_BUILD ON)
#endif()
target_link_libraries(archivemailagent
    KPim5::MailCommon
    KF5::I18n
    KF5::Notifications
    KF5::KIOWidgets
)

if(TARGET KPim5::AkonadiAgentWidgetBase)
    target_link_libraries(archivemailagent KPim5::AkonadiAgentWidgetBase)
endif()

########################### Agent executable ################################

add_executable(akonadi_archivemail_agent archivemailagent.cpp archivemailagent.h)

target_link_libraries(akonadi_archivemail_agent
    archivemailagent
    KPim5::AkonadiCore
    KPim5::AkonadiMime
    KF5::DBusAddons
    KF5::XmlGui
    KF5::KIOWidgets
    KF5::Notifications
    KPim5::MailCommon
    KF5::I18n
)

if(TARGET KPim5::IdentityManagementCore)
    target_link_libraries(akonadi_archivemail_agent KPim5::IdentityManagementCore)
endif()

if(TARGET KPim5::AkonadiAgentWidgetBase)
    target_link_libraries(akonadi_archivemail_agent KPim5::AkonadiAgentWidgetBase)
endif()

if(APPLE)
    set_target_properties(akonadi_archivemail_agent PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${kmail_SOURCE_DIR}/agents/Info.plist.template)
    set_target_properties(akonadi_archivemail_agent PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.Akonadi.archivemail")
    set_target_properties(akonadi_archivemail_agent PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "KDE Akonadi Email Archiver")
endif()

install(TARGETS akonadi_archivemail_agent ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES archivemailagent.desktop DESTINATION "${KDE_INSTALL_DATAROOTDIR}/akonadi/agents")
install(FILES akonadi_archivemail_agent.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})


if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

########## Configuration ##############


kcoreaddons_add_plugin(archivemailagentconfig
    INSTALL_NAMESPACE pim5/akonadi/config
)
target_sources(archivemailagentconfig PRIVATE
    addarchivemaildialog.cpp
    addarchivemaildialog.h

    addarchivemailwidget.h
    addarchivemailwidget.cpp

    archivemailrangewidget.h
    archivemailrangewidget.cpp

    widgets/hourcombobox.h
    widgets/hourcombobox.cpp

    archivemailwidget.cpp
    archivemailwidget.h

    widgets/formatcombobox.cpp
    widgets/formatcombobox.h

    widgets/unitcombobox.cpp
    widgets/unitcombobox.h
)
target_link_libraries(archivemailagentconfig
    archivemailagent
    KPim5::AkonadiCore
    KF5::XmlGui
    KF5::I18n
)
#if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
#    set_target_properties(archivemailagentconfig PROPERTIES UNITY_BUILD ON)
#endif()


install(FILES settings/archivemailagentsettings.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})
