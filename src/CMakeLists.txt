# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_definitions(-DTRANSLATION_DOMAIN=\"kmail\")

qt5_generate_dbus_interface(kmkernel.h org.kde.kmail.kmail.xml OPTIONS -a)
qt5_generate_dbus_interface(editor/kmcomposerwin.h org.kde.kmail.mailcomposer.xml OPTIONS -a)

add_custom_target(kmail_xml ALL
    DEPENDS ${kmail_BINARY_DIR}/src/org.kde.kmail.kmail.xml
    DEPENDS ${kmail_BINARY_DIR}/src/org.kde.kmail.mailcomposer.xml
    COMMENT "Helper target for XML stuff. The Kontact plugin, KOrganizer and others depend on it."
)

add_subdirectory(pics)
add_subdirectory(icons)
add_subdirectory(kontactplugin)
add_subdirectory(ai)

########### kmailprivate ###############




set(kmail_debugcategory_SRCS)
ecm_qt_declare_logging_category(kmail_debugcategory_SRCS HEADER kmail_debug.h IDENTIFIER KMAIL_LOG CATEGORY_NAME org.kde.pim.kmail
    DESCRIPTION "kmail (kmail)"
    OLD_CATEGORY_NAMES log_kmail
    EXPORT KMAIL
)

ecm_qt_declare_logging_category(kmail_debugcategory_SRCS HEADER kmail_undo_send_debug.h IDENTIFIER KMAIL_UNDO_SEND_LOG CATEGORY_NAME org.kde.pim.kmail.undo.send
    DESCRIPTION "kmail undo send (kmail)"
    OLD_CATEGORY_NAMES log_undo_send_kmail
    EXPORT KMAIL
)

ecm_qt_declare_logging_category(kmail_debugcategory_SRCS HEADER kmail_activities_debug.h IDENTIFIER KMAIL_ACTIVITIES_LOG CATEGORY_NAME org.kde.pim.kmail.activities
    DESCRIPTION "kmail activities (kmail)"
    EXPORT KMAIL
)


add_library(kmailprivate)
if(TARGET KF5::UserFeedbackWidgets)
    target_sources(kmailprivate PRIVATE
        userfeedback/accountinfosource.cpp
        userfeedback/accountinfosource.h
        userfeedback/plugininfosource.cpp
        userfeedback/plugininfosource.h
        userfeedback/kmailuserfeedbackprovider.cpp
        userfeedback/kmailuserfeedbackprovider.h
)
endif()

target_sources(kmailprivate PRIVATE
    ${kmail_debugcategory_SRCS}
    attributes/taskattribute.cpp
    folderarchive/folderarchiveaccountinfo.cpp
    folderarchive/folderarchiveutil.cpp
    folderarchive/folderarchivecache.cpp
    folderarchive/folderarchiveagentcheckcollection.cpp
    folderarchive/folderarchivemanager.cpp
    folderarchive/folderarchiveagentjob.cpp
    collectionpage/collectiontemplatespage.cpp
    collectionpage/collectionviewpage.cpp
    collectionpage/collectionquotapage.cpp
    collectionpage/collectionquotawidget.cpp
    collectionpage/collectionmailinglistpage.cpp
    collectionpage/collectionshortcutpage.cpp
    configuredialog/configagentdelegate.cpp
    configuredialog/configuredialoglistview.cpp
    configuredialog/configuredialog.cpp
    configuredialog/configuredialog_p.cpp
    configuredialog/configuremiscpage.cpp
    configuredialog/configuresecuritypage.cpp
    configuredialog/configurecomposerpage.cpp
    configuredialog/configureappearancepage.cpp
    configuredialog/configureaccountpage.cpp
    configuredialog/configurepluginpage.cpp
    configuredialog/colorlistbox.cpp
    configuredialog/configureplugins/configurepluginslistwidget.cpp
    searchdialog/kmsearchmessagemodel.cpp
    searchdialog/searchpatternwarning.cpp
    searchdialog/kmailsearchpatternedit.cpp
    searchdialog/searchwindowdialog.cpp
    searchdialog/searchdescriptionattribute.cpp
    searchdialog/incompleteindexdialog.cpp
    identity/identitydialog.cpp
    identity/xfaceconfigurator.cpp
    identity/encodedimagepicker.cpp
    identity/newidentitydialog.cpp
    identity/identityeditvcarddialog.cpp
    identity/identityaddvcarddialog.cpp
    identity/identityinvalidfolder.cpp
    identity/identityfolderrequester.cpp

    identity/identitytreengwidget.h
    identity/identitytreengwidget.cpp
    identity/identityngpage.cpp
    identity/identityngpage.h
    identity/identityexpirespamfolderdialog.h
    identity/identityexpirespamfolderdialog.cpp

    editor/kmcomposereditorng.cpp
    editor/encryptionstate.cpp
    editor/subjectlineeditwithautocorrection.cpp
    editor/subjectlineeditwithautocorrection.h
    editor/composer.cpp
    editor/kmcomposerwin.cpp
    editor/attachment/attachmentcontroller.cpp
    editor/attachment/attachmentview.cpp
    editor/widgets/cryptostateindicatorwidget.cpp
    editor/validatesendmailshortcut.cpp
    editor/kmcomposerglobalaction.cpp
    editor/kmcomposerupdatetemplatejob.cpp
    editor/kmcomposercreatenewcomposerjob.cpp
    editor/pimmessagebox.cpp
    editor/warningwidgets/externaleditorwarning.cpp
    editor/warningwidgets/attachmentmissingwarning.cpp
    editor/warningwidgets/incorrectidentityfolderwarning.cpp
    editor/warningwidgets/attachmentaddedfromexternalwarning.cpp
    editor/warningwidgets/toomanyrecipientswarning.cpp
    editor/warningwidgets/nearexpirywarning.cpp
    editor/potentialphishingemail/potentialphishingemailwarning.cpp
    editor/potentialphishingemail/potentialphishingemailjob.cpp
    editor/potentialphishingemail/potentialphishingdetaildialog.cpp
    editor/potentialphishingemail/potentialphishingdetailwidget.cpp
    dialog/archivefolderdialog.cpp
    dialog/addemailtoexistingcontactdialog.cpp
    dialog/kmknotify.cpp
    dialog/spellcheckerconfigdialog.cpp
    job/addressvalidationjob.cpp
    job/createnewcontactjob.cpp
    job/addemailtoexistingcontactjob.cpp
    job/createtaskjob.cpp
    job/savedraftjob.cpp
    job/removeduplicatemailjob.cpp
    job/createfollowupreminderonexistingmessagejob.cpp
    job/removecollectionjob.cpp
    job/saveasfilejob.cpp
    job/markallmessagesasreadinfolderandsubfolderjob.cpp
    job/removeduplicatemessageinfolderandsubfolderjob.cpp
    job/handleclickedurljob.cpp
    job/composenewmessagejob.cpp
    job/opencomposerjob.cpp
    job/newmessagejob.cpp
    job/opencomposerhiddenjob.cpp
    job/fillcomposerjob.cpp
    job/createreplymessagejob.cpp
    job/createforwardmessagejob.cpp
    job/dndfromarkjob.cpp
    widgets/collectionpane.cpp
    widgets/vacationscriptindicatorwidget.cpp
    widgets/displaymessageformatactionmenu.cpp
    widgets/kactionmenutransport.cpp
    widgets/kactionmenuaccount.cpp
    widgets/zoomlabelwidget.cpp
    tag/tagactionmanager.cpp
    tag/tagselectdialog.cpp
    tag/tagmonitormanager.cpp
    plugininterface/kmailplugininterface.cpp
    plugininterface/kmailplugincheckbeforedeletingmanagerinterface.cpp
    editor/plugininterface/kmailplugineditormanagerinterface.cpp
    editor/plugininterface/kmailplugineditorcheckbeforesendmanagerinterface.cpp
    editor/plugininterface/kmailplugineditorinitmanagerinterface.cpp
    editor/plugininterface/kmailplugineditorconverttextmanagerinterface.cpp
    editor/plugininterface/kmailplugingrammareditormanagerinterface.cpp

    sieveimapinterface/kmailsieveimapinstanceinterface.cpp
    sieveimapinterface/kmsieveimappasswordprovider.cpp
    undosend/undosendcombobox.cpp
    undosend/undosendmanager.cpp
    undosend/undosendcreatejob.cpp
    kmmainwin.cpp
    settings/kmailsettings.cpp
    kmreaderwin.cpp
    kmsystemtray.cpp
    unityservicemanager.cpp
    undostack.cpp
    kmkernel.cpp
    commandlineinfo.h
    commandlineinfo.cpp
    kmcommands.cpp
    kmreadermainwin.cpp
    kmmainwidget.cpp
    aboutdata.cpp
    mailserviceimpl.cpp
    secondarywindow.cpp
    util.cpp
    kmail_options.h
    messageactions.cpp
    foldershortcutactionmanager.cpp
    kmlaunchexternalcomponent.cpp
    manageshowcollectionproperties.cpp
    kmail.qrc

    attributes/taskattribute.h
    folderarchive/folderarchiveaccountinfo.h
    folderarchive/folderarchiveutil.h
    folderarchive/folderarchivecache.h
    folderarchive/folderarchiveagentcheckcollection.h
    folderarchive/folderarchivemanager.h
    folderarchive/folderarchiveagentjob.h
    collectionpage/collectiontemplatespage.h
    collectionpage/collectionviewpage.h
    collectionpage/collectionquotapage.h
    collectionpage/collectionquotawidget.h
    collectionpage/collectionmailinglistpage.h
    collectionpage/collectionshortcutpage.h
    configuredialog/configagentdelegate.h
    configuredialog/configuredialoglistview.h
    configuredialog/configuredialog.h
    configuredialog/configuredialog_p.h
    configuredialog/configuremiscpage.h
    configuredialog/configuresecuritypage.h
    configuredialog/configurecomposerpage.h
    configuredialog/configureappearancepage.h
    configuredialog/configureaccountpage.h
    configuredialog/configurepluginpage.h
    configuredialog/colorlistbox.h
    configuredialog/configureplugins/configurepluginslistwidget.h
    searchdialog/kmsearchmessagemodel.h
    searchdialog/searchpatternwarning.h
    searchdialog/kmailsearchpatternedit.h
    searchdialog/searchwindowdialog.h
    searchdialog/searchdescriptionattribute.h
    searchdialog/incompleteindexdialog.h
    identity/identitydialog.h
    identity/xfaceconfigurator.h
    identity/encodedimagepicker.h
    identity/newidentitydialog.h
    identity/identityeditvcarddialog.h
    identity/identityaddvcarddialog.h
    identity/identityinvalidfolder.h
    identity/identityfolderrequester.h

    editor/kmcomposereditorng.h
    editor/composer.h
    editor/encryptionstate.h
    editor/kmcomposerwin.h
    editor/attachment/attachmentcontroller.h
    editor/attachment/attachmentview.h
    editor/widgets/cryptostateindicatorwidget.h
    editor/validatesendmailshortcut.h
    editor/kmcomposerglobalaction.h
    editor/kmcomposerupdatetemplatejob.h
    editor/kmcomposercreatenewcomposerjob.h
    editor/pimmessagebox.h
    editor/warningwidgets/externaleditorwarning.h
    editor/warningwidgets/attachmentmissingwarning.h
    editor/warningwidgets/incorrectidentityfolderwarning.h
    editor/warningwidgets/attachmentaddedfromexternalwarning.h
    editor/warningwidgets/toomanyrecipientswarning.h
    editor/warningwidgets/nearexpirywarning.h
    editor/potentialphishingemail/potentialphishingemailwarning.h
    editor/potentialphishingemail/potentialphishingemailjob.h
    editor/potentialphishingemail/potentialphishingdetaildialog.h
    editor/potentialphishingemail/potentialphishingdetailwidget.h
    dialog/archivefolderdialog.h
    dialog/addemailtoexistingcontactdialog.h
    dialog/kmknotify.h
    dialog/spellcheckerconfigdialog.h
    job/addressvalidationjob.h
    job/createnewcontactjob.h
    job/addemailtoexistingcontactjob.h
    job/createtaskjob.h
    job/savedraftjob.h
    job/removeduplicatemailjob.h
    job/createfollowupreminderonexistingmessagejob.h
    job/removecollectionjob.h
    job/saveasfilejob.h
    job/markallmessagesasreadinfolderandsubfolderjob.h
    job/removeduplicatemessageinfolderandsubfolderjob.h
    job/handleclickedurljob.h
    job/composenewmessagejob.h
    job/opencomposerjob.h
    job/newmessagejob.h
    job/opencomposerhiddenjob.h
    job/fillcomposerjob.h
    job/createreplymessagejob.h
    job/createforwardmessagejob.h
    job/dndfromarkjob.h
    widgets/collectionpane.h
    widgets/vacationscriptindicatorwidget.h
    widgets/displaymessageformatactionmenu.h
    widgets/kactionmenutransport.h
    widgets/kactionmenuaccount.h
    widgets/zoomlabelwidget.h
    tag/tagactionmanager.h
    tag/tagselectdialog.h
    tag/tagmonitormanager.h
    plugininterface/kmailplugininterface.h
    plugininterface/kmailplugincheckbeforedeletingmanagerinterface.h
    editor/plugininterface/kmailplugineditormanagerinterface.h
    editor/plugininterface/kmailplugineditorcheckbeforesendmanagerinterface.h
    editor/plugininterface/kmailplugineditorinitmanagerinterface.h
    editor/plugininterface/kmailplugineditorconverttextmanagerinterface.h
    editor/plugininterface/kmailplugingrammareditormanagerinterface.h

    sieveimapinterface/kmailsieveimapinstanceinterface.h
    sieveimapinterface/kmsieveimappasswordprovider.h
    undosend/undosendcombobox.h
    undosend/undosendmanager.h
    undosend/undosendcreatejob.h
    kmmainwin.h
    settings/kmailsettings.h
    kmreaderwin.h
    kmsystemtray.h
    unityservicemanager.h
    undostack.h
    kmkernel.h
    kmcommands.h
    kmreadermainwin.h
    kmmainwidget.h
    aboutdata.h
    mailserviceimpl.h
    secondarywindow.h
    util.h
    messageactions.h
    foldershortcutactionmanager.h
    kmlaunchexternalcomponent.h
    manageshowcollectionproperties.h
    historyswitchfolder/collectionswitchertreeview.cpp
    historyswitchfolder/collectionswitchertreeview.h
    historyswitchfolder/collectionswitchertreeviewmanager.h
    historyswitchfolder/collectionswitchertreeviewmanager.cpp
    historyswitchfolder/collectionswitchermodel.h
    historyswitchfolder/collectionswitchermodel.cpp

    historyclosedreader/historyclosedreaderinfo.h
    historyclosedreader/historyclosedreaderinfo.cpp
    historyclosedreader/historyclosedreadermanager.h
    historyclosedreader/historyclosedreadermanager.cpp
    historyclosedreader/historyclosedreadermenu.h
    historyclosedreader/historyclosedreadermenu.cpp
    ai/aiserviceinterface.h
    ai/localaiservice.cpp
    ai/localaiservice.h
    ai/aimanager.cpp
    ai/aimanager.h
    ai/aiconfigwidget.cpp
    ai/aiconfigwidget.h
    ai/aimainwidgetextension.cpp
    ai/aimainwidgetextension.h
    ai/kmainwidgetaiintegration.cpp
    ai/kmainwidgetaiintegration.h
    ai/widgets/aicategoryindicatorwidget.cpp
    ai/widgets/aicategoryindicatorwidget.h
    ai/widgets/aireplydraftwidget.cpp
    ai/widgets/aireplydraftwidget.h
)

if(KMAIL_HAVE_ACTIVITY_SUPPORT)
    target_sources(kmailprivate PRIVATE
        activities/activitiesmanager.h
        activities/activitiesmanager.cpp

        activities/transportactivities.h
        activities/transportactivities.cpp

        activities/identityactivities.h
        activities/identityactivities.cpp

	activities/ldapactivities.cpp
	activities/ldapactivities.h

	activities/accountactivities.h
	activities/accountactivities.cpp
)
    if(BUILD_TESTING)
       add_subdirectory(activities/autotests)
    endif()
endif()

if(KPim5AkonadiSearch_FOUND)
    target_sources(kmailprivate PRIVATE
        search/checkindexingmanager.cpp
        search/checkindexingjob.cpp
        search/checkindexingmanager.h
        search/checkindexingjob.h
    )
endif()


# Note: These DBus adaptor functions are temporarily commented out due to compatibility issues
# qt5_add_dbus_adaptor(kmailprivate_LIB_SRCS ${kmail_BINARY_DIR}/src/org.kde.kmail.kmail.xml kmkernel.h KMKernel)
# qt5_add_dbus_adaptor(kmailprivate_LIB_SRCS ${MAILTRANSPORT_DBUS_SERVICE} mailserviceimpl.h KMail::MailServiceImpl)

qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    ${kmail_BINARY_DIR}/src/org.kde.kmail.kmail.xml
)
qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    ../agents/archivemailagent/org.freedesktop.Akonadi.ArchiveMailAgent.xml
)
qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    ../agents/sendlateragent/org.freedesktop.Akonadi.SendLaterAgent.xml
)

qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    ../agents/mailmergeagent/org.freedesktop.Akonadi.MailMergeAgent.xml
)

qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    dbusinterface/org.freedesktop.Akonadi.NewMailNotifier.xml
)

qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    ../agents/followupreminderagent/org.freedesktop.Akonadi.FollowUpReminder.xml
)

qt5_add_dbus_interfaces(kmailprivate_LIB_SRCS
    ../agents/mailfilteragent/org.freedesktop.Akonadi.MailFilterAgent.xml
)


ki18n_wrap_ui(kmailprivate
    ui/smimeconfiguration.ui
    ui/miscpagemaintab.ui
    ui/securitypageencryptiontab.ui
    ui/securitypagegeneraltab.ui
    ui/securitypagemdntab.ui
    ui/accountspagereceivingtab.ui
    ui/searchwindow.ui
    ui/incompleteindexdialog.ui
    ui/xfaceconfigurator.ui
    ui/encodedimagepicker.ui
    ui/identityngpage.ui
)

# KCFG files. The main kmail.kcfg is configured by CMake and put
# in the build directory.

if(KDEPIM_ENTERPRISE_BUILD)
    set(WARN_TOOMANY_RECIPIENTS_DEFAULT true)
else()
    set(WARN_TOOMANY_RECIPIENTS_DEFAULT false)
endif()

configure_file(settings/kmail.kcfg.in ${CMAKE_CURRENT_BINARY_DIR}/kmail.kcfg)

kconfig_add_kcfg_files(kmailprivate_LIB_SRCS
    settings/globalsettings_kmail.kcfgc
    editor/custommimeheader/custommimeheader.kcfgc
)

set(kmail_userfeedback_LIB)
if(TARGET KF5::UserFeedbackWidgets)
    set(kmail_userfeedback_LIB KF5::UserFeedbackWidgets)
endif()

target_sources(kmailprivate PRIVATE ${kmailprivate_LIB_SRCS})
target_sources(kmailprivate PRIVATE
    historyclosedreader/historyclosedreadermenu.cpp
    ai/aiserviceinterface.h
    ai/localaiservice.cpp
    ai/localaiservice.h
    ai/aimanager.cpp
    ai/aimanager.h
    ai/aiconfigwidget.cpp
    ai/aiconfigwidget.h
    ai/aimainwidgetextension.cpp
    ai/aimainwidgetextension.h
    ai/kmainwidgetaiintegration.cpp
    ai/kmainwidgetaiintegration.h
)

if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(kmailprivate PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(kmailprivate BASE_NAME kmail)
target_link_libraries(kmailprivate
    PRIVATE
    KF5::I18n
    KF5::SonnetUi
    KPim5::Mime
    KPim5::AkonadiCore
    KPim5::AkonadiMime
    KPim5::MessageCore
    KPim5::MessageList 
    KPim5::MessageComposer
    KPim5::PimCommon
    KPim5::MailCommon
    KF5::WindowSystem
    KF5::Notifications
    KF5::GuiAddons
    KF5::Crash
    KF5::KIOFileWidgets
    KF5::NotifyConfig
    KF5::Codecs
    KF5::KIOCore
    KF5::Contacts
    KF5::XmlGui
    KF5::Completion
    KF5::SyntaxHighlighting
    KF5::GuiAddons
    KF5::TextUtils
    ${kmail_userfeedback_LIB}
)

# LibGpgError is optional
find_package(LibGpgError ${GPG_ERROR_REQUIRED_VERSION} CONFIG)
if(LibGpgError_FOUND)
    target_link_libraries(kmailprivate PRIVATE LibGpgError::LibGpgError)
endif()

# Optional dependencies - these might not be available in all environments
if(TARGET KPim5::Gravatar)
    target_link_libraries(kmailprivate PRIVATE KPim5::Gravatar)
endif()

if(TARGET KPim5::TemplateParser)
    target_link_libraries(kmailprivate PRIVATE KPim5::TemplateParser)
endif()

if(TARGET KPim5::IdentityManagementWidgets)
    target_link_libraries(kmailprivate PRIVATE KPim5::IdentityManagementWidgets)
endif()

if(TARGET KPim5::MailTransport)
    target_link_libraries(kmailprivate PRIVATE KPim5::MailTransport)
endif()

if(TARGET KPim5::Libkleo)
    target_link_libraries(kmailprivate PRIVATE KPim5::Libkleo)
endif()

if(TARGET KPim5::KSieveUi)
    target_link_libraries(kmailprivate PRIVATE KPim5::KSieveUi)
endif()

if(TARGET KPim5::PimTextEdit)
    target_link_libraries(kmailprivate PRIVATE KPim5::PimTextEdit)
endif()

if(TARGET KPim5::MessageViewer)
    target_link_libraries(kmailprivate PRIVATE KPim5::MessageViewer)
endif()

if(TARGET KPim5::LdapWidgets)
    target_link_libraries(kmailprivate PRIVATE KPim5::LdapWidgets)
endif()

if(TARGET KPim5::WebEngineViewer)
    target_link_libraries(kmailprivate PRIVATE KPim5::WebEngineViewer)
endif()

if(TARGET KF5::IconThemes)
    target_link_libraries(kmailprivate PRIVATE KF5::IconThemes)
endif()

if(TARGET KF5::IconWidgets)
    target_link_libraries(kmailprivate PRIVATE KF5::IconWidgets)
endif()

if(TARGET KF5::KCMUtils)
    target_link_libraries(kmailprivate PRIVATE KF5::KCMUtils)
endif()

if(TARGET KF5::TextEditTextToSpeech)
    target_link_libraries(kmailprivate PRIVATE KF5::TextEditTextToSpeech)
endif()

if(TARGET KF5::StatusNotifierItem)
    target_link_libraries(kmailprivate PRIVATE KF5::StatusNotifierItem)
endif()

if(KMAIL_HAVE_ACTIVITY_SUPPORT)
    target_link_libraries(kmailprivate PRIVATE KPim5::PimCommonActivities)
endif()

if(KPim5AkonadiSearch_FOUND)
    target_link_libraries(kmailprivate PRIVATE KPim5::AkonadiSearchDebug KPim5::AkonadiSearchPIM)
endif()

# Only link against qt5keychain if found
if(TARGET Qt5::Keychain)
    target_link_libraries(kmailprivate PRIVATE Qt5::Keychain)
else()
    message(STATUS "Qt5Keychain not found, some keychain functionality will be disabled")
endif()

target_include_directories(kmailprivate PUBLIC $<TARGET_PROPERTY:KPim5::MessageList,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(kmailprivate PUBLIC $<TARGET_PROPERTY:KPim5::MessageViewer,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(kmailprivate PUBLIC $<TARGET_PROPERTY:KPim5::MailCommon,INTERFACE_INCLUDE_DIRECTORIES>)

set_target_properties(kmailprivate
    PROPERTIES VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
)

########### kcm_kmail ###############
add_library(kcm_kmail_static STATIC ${kcm_kmail_PART_SRCS})
target_sources(kcm_kmail_static PRIVATE kcm_kmail.cpp)
target_link_libraries(kcm_kmail_static kmailprivate KF5::Completion KF5::I18n)

if(TARGET KF5::KCMUtils)
    target_link_libraries(kcm_kmail_static KF5::KCMUtils)
endif()

if(TARGET KF5::KCMUtils)
    kcoreaddons_add_plugin(kcm_kmail_accounts SOURCES configuredialog/kcm_kmail_accounts.cpp INSTALL_NAMESPACE "pim5/kcms/kmail")
    target_link_libraries(kcm_kmail_accounts kcm_kmail_static KPim5::AkonadiCore)

    kcoreaddons_add_plugin(kcm_kmail_appearance SOURCES configuredialog/kcm_kmail_appearance.cpp INSTALL_NAMESPACE "pim5/kcms/kmail")
    target_link_libraries(kcm_kmail_appearance kcm_kmail_static KPim5::AkonadiCore)

    kcoreaddons_add_plugin(kcm_kmail_composer SOURCES configuredialog/kcm_kmail_composer.cpp INSTALL_NAMESPACE "pim5/kcms/kmail")
    target_link_libraries(kcm_kmail_composer kcm_kmail_static KPim5::AkonadiCore)

    kcoreaddons_add_plugin(kcm_kmail_misc SOURCES configuredialog/kcm_kmail_misc.cpp INSTALL_NAMESPACE "pim5/kcms/kmail")
    target_link_libraries(kcm_kmail_misc kcm_kmail_static KPim5::AkonadiCore)

    kcoreaddons_add_plugin(kcm_kmail_plugins SOURCES configuredialog/kcm_kmail_plugins.cpp INSTALL_NAMESPACE "pim5/kcms/kmail")
    target_link_libraries(kcm_kmail_plugins kcm_kmail_static KPim5::AkonadiCore)

    kcoreaddons_add_plugin(kcm_kmail_security SOURCES configuredialog/kcm_kmail_security.cpp INSTALL_NAMESPACE "pim5/kcms/kmail")
    target_link_libraries(kcm_kmail_security kcm_kmail_static KPim5::AkonadiCore)
else()
    message(STATUS "KCMUtils not found, skipping building KCM modules")
endif()
########### kmailpart ###############

set(kmailpart_PART_SRCS kmail_part.cpp kmail_part.h ${kmail_debugcategory_SRCS})
qt5_generate_dbus_interface(kmail_part.h org.kde.kmail.kmailpart.xml OPTIONS -a)
# Note: This DBus adaptor function is temporarily commented out due to compatibility issues
# qt5_add_dbus_adaptor(kmailpart_PART_SRCS ${kmail_BINARY_DIR}/src/org.kde.kmail.kmailpart.xml kmail_part.h KMailPart)
qt5_add_dbus_interfaces(kmailpart_PART_SRCS ${kmail_BINARY_DIR}/src/org.kde.kmail.kmailpart.xml)
add_library(kmailpart MODULE ${kmailpart_PART_SRCS})

target_link_libraries(kmailpart
    kmailprivate
    KPim5::AkonadiCore
    KF5::I18n
    KPim5::PimCommon
    KF5::Parts
    KF5::IconThemes
    KPim5::TemplateParser
)

if(TARGET KF5::KCMUtils)
    target_link_libraries(kmailpart KF5::KCMUtils)
endif()

########### KMail executable ###############

set(kmail_SRCS main.cpp ${kmail_debugcategory_SRCS})
set(kmail_SRCS 
    ${kmail_SRCS}
    filter/kmmailfilter.cpp
    ai/kmmainwidgetaifilter.cpp
    ai/aitaskdialog.cpp
    ai/aireplydialog.cpp
    ai/aifollowupdialog.cpp
)
set(kmail_SRCS 
    ${kmail_SRCS}
    ai/korganizerintegration.cpp
)
set(kmail_SRCS 
    ${kmail_SRCS}
    ai/aisummarydialog.cpp
)
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kmail.png")
ecm_add_app_icon(kmail_SRCS ICONS ${ICONS_SRCS})
add_executable(kmail ${kmail_SRCS})
target_link_libraries(kmail
    KPim5::KontactInterface
    kmailprivate
    KPim5::AkonadiCore
    KPim5::PimCommon
    KF5::I18n
    KPim5::TemplateParser
    KPim5::Libkdepim
    KF5::Crash
    KPim5::WebEngineViewer
    ${kmail_userfeedback_LIB}
    KF5::WindowSystem
    KF5::IconThemes
)

if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
    add_subdirectory(folderarchive/autotests/)
    add_subdirectory(editor/potentialphishingemail/autotests)
    add_subdirectory(editor/warningwidgets/autotests)
    add_subdirectory(sieveimapinterface/tests/)
    add_subdirectory(undosend/autotests/)
    add_subdirectory(job/autotests/)
    add_subdirectory(historyswitchfolder/autotests/)
    add_subdirectory(historyclosedreader/autotests/)
endif()
########### install files ###############
install(TARGETS kmailprivate ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

install(TARGETS kmail ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(PROGRAMS data/org.kde.kmail2.desktop data/kmail_view.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kmail.kcfg
    DESTINATION ${KDE_INSTALL_KCFGDIR}
)
install(FILES data/org.kde.kmail2.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

install(FILES data/kmail2.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})
install(TARGETS
    kmailpart
    DESTINATION ${KDE_INSTALL_PLUGINDIR}
)
install(FILES
    ${kmail_BINARY_DIR}/src/org.kde.kmail.kmailpart.xml
    ${kmail_BINARY_DIR}/src/org.kde.kmail.kmail.xml
    DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR}
)

configure_file(data/org.kde.kmail.service.in org.kde.kmail.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kmail.service DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
