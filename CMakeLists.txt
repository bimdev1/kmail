# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.24.5")

project(kmail VERSION ${PIM_VERSION})

include(CheckIncludeFiles)

set(KF_MIN_VERSION "5.116.0")
set(QT_REQUIRED_VERSION "5.15.2")
set(CMAKE_CXX_STANDARD 17)
find_package(ECM ${KF_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
include(CheckFunctionExists)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMFeatureSummary)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)
set(RELEASE_SERVICE_VERSION "23.08.5")

# Update it for each release
set(KMAIL_RELEASE_VERSION_DATE "23.08.05") # yy.MM.dd

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")

set(AKONADI_MIMELIB_VERSION "5.24.5")
set(AKONADI_CONTACT_VERSION "5.24.5")
set(CALENDARUTILS_LIB_VERSION "5.24.5")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.24.5")
set(KLDAP_LIB_VERSION "5.24.5")
set(KMAILTRANSPORT_LIB_VERSION "5.24.5")
set(KONTACTINTERFACE_LIB_VERSION "5.24.5")
set(KMIME_LIB_VERSION "5.24.5")
set(KPIMTEXTEDIT_LIB_VERSION "5.24.5")
set(AKONADI_VERSION "5.24.5")
set(KTNEF_LIB_VERSION "5.24.5")
set(GPG_ERROR_REQUIRED_VERSION "1.36")

set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(KDEPIM_LIB_SOVERSION "5")

set(KTEXTADDONS_MIN_VERSION "1.5.4")
option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED DBus Network Test Widgets WebEngineWidgets)
set(LIBGRAVATAR_VERSION "5.24.5")
set(MAILCOMMON_LIB_VERSION "5.24.5")
set(MESSAGELIB_LIB_VERSION "5.24.5")
set(LIBKLEO_LIB_VERSION "5.24.5")
set(PIMCOMMON_LIB_VERSION "5.24.5")
set(LIBKDEPIM_LIB_VERSION "5.24.5")
set(LIBKSIEVE_LIB_VERSION "5.24.5")
set(AKONADI_SEARCH_VERSION "5.24.5")
set(QT5KEYCHAIN_LIB_VERSION "0.14.2")

option(FORCE_DISABLE_AKONADI_SEARCH "Disable features and API that require akonadi-search, e.g. because xapian isn't available" OFF)
if(UNIX)
    if(NOT FORCE_DISABLE_AKONADI_SEARCH)
        find_package(KF5AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG REQUIRED)
        set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")
    endif()
else()
    if(NOT FORCE_DISABLE_AKONADI_SEARCH)
        find_package(KF5AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG)
        set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE OPTIONAL PURPOSE "Provides search capabilities in KMail and Akonadi")
    endif()
endif()

if(NOT KF5AkonadiSearch_FOUND)
    set(KMAIL_FORCE_DISABLE_AKONADI_SEARCH TRUE)
endif()

find_package(KF5I18n ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Crash ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5GuiAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5IconThemes ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5JobWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Notifications ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5NotifyConfig ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Parts ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Sonnet ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5TextWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5WindowSystem ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5XmlGui ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarCore ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5StatusNotifierItem ${KF_MIN_VERSION} CONFIG REQUIRED)

find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADI_MIMELIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiContact ${AKONADI_CONTACT_VERSION} CONFIG REQUIRED)
find_package(KF5CalendarUtils ${CALENDARUTILS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5LDAP ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5LibKleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MessageCore ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MessageList ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MessageViewer ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MessageComposer ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MimeTreeParser ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5TemplateParser ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommon ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5LibKdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5KSieve ${LIBKSIEVE_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5KontactInterface ${KONTACTINTERFACE_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailCommon ${MAILCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Gravatar ${LIBGRAVATAR_VERSION} CONFIG REQUIRED)
find_package(KF5Tnef ${KTNEF_LIB_VERSION} CONFIG REQUIRED)

find_package(Gpgmepp ${GPGME_REQUIRED_VERSION} CONFIG REQUIRED)
find_package(QGpgme ${GPGME_REQUIRED_VERSION} CONFIG REQUIRED)
find_package(Gpgmepp ${GPGME_REQUIRED_VERSION} CONFIG REQUIRED)

find_package(KF5TextEditTextToSpeech ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5TextWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5TextUtils ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)

find_package(Qt5Keychain ${QT5KEYCHAIN_LIB_VERSION} CONFIG)
set_package_properties(Qt5Keychain PROPERTIES
    DESCRIPTION "Qt API to store passwords and other secret data securely"
    URL "https://github.com/frankosterfeld/qtkeychain"
    TYPE OPTIONAL
)

find_package(Cups)
set_package_properties(Cups PROPERTIES DESCRIPTION "Common UNIX Printing System"
    URL "http://www.cups.org"
    TYPE OPTIONAL
)

configure_file(config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-enterprise.h)
configure_file(kmail-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kmail-version.h)

add_definitions(-DTRANSLATION_DOMAIN=\"kmail\")
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)

if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif()

add_subdirectory(src)
add_subdirectory(agents)
if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

if(KF5DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()

ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
