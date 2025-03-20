# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.24.5")

# Extract version components
string(REGEX MATCH "^([0-9]+)\\.[0-9]+\\.[0-9]+" _ ${PIM_VERSION})
set(PIM_VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "^[0-9]+\\.([0-9]+)\\.[0-9]+" _ ${PIM_VERSION})
set(PIM_VERSION_MINOR ${CMAKE_MATCH_1})
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.([0-9]+)" _ ${PIM_VERSION})
set(PIM_VERSION_PATCH ${CMAKE_MATCH_1})

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

option(FORCE_DISABLE_AKONADI_SEARCH "Disable features and API that require akonadi-search, e.g. because xapian isn't available" ON)
option(BUILD_AGENTS "Build the Akonadi agents" OFF)

if(NOT FORCE_DISABLE_AKONADI_SEARCH)
    find_package(KF5AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG)
    set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE OPTIONAL PURPOSE "Provides search capabilities in KMail and Akonadi")
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
find_package(KF5ConfigWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)

# Optional dependencies
find_package(KF5KCMUtils ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF5KCMUtils PROPERTIES 
    DESCRIPTION "KDE Config Module Utils" 
    URL "https://invent.kde.org/frameworks/kcmutils" 
    TYPE OPTIONAL
    PURPOSE "Required for configuration modules"
)

find_package(KPim5IdentityManagementCore ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG)
set_package_properties(KPim5IdentityManagementCore PROPERTIES
    DESCRIPTION "KDE PIM Identity Management Core Library"
    URL "https://invent.kde.org/pim/kidentitymanagement"
    TYPE OPTIONAL
    PURPOSE "Required for identity management features"
)

find_package(KPim5AkonadiAgentBase ${AKONADI_VERSION} CONFIG)
set_package_properties(KPim5AkonadiAgentBase PROPERTIES
    DESCRIPTION "Akonadi Agent Base Library"
    URL "https://invent.kde.org/pim/akonadi"
    TYPE OPTIONAL
    PURPOSE "Required for Akonadi agents"
)

find_package(KF5TextCustomEditor ${KTEXTADDONS_MIN_VERSION} CONFIG)
set_package_properties(KF5TextCustomEditor PROPERTIES
    DESCRIPTION "KF5 Text Custom Editor"
    URL "https://invent.kde.org/libraries/ktextaddons"
    TYPE OPTIONAL
    PURPOSE "Required for text editor features"
)

find_package(KF5StatusNotifierItem ${KF_MIN_VERSION} CONFIG)
set_package_properties(KF5StatusNotifierItem PROPERTIES DESCRIPTION "KDE Status Notifier Item" URL "https://invent.kde.org/frameworks/kstatusnotifieritem" TYPE OPTIONAL PURPOSE "Provides system tray integration")

find_package(KPim5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KPim5AkonadiMime ${AKONADI_MIMELIB_VERSION} CONFIG REQUIRED)
find_package(KPim5AkonadiContact ${AKONADI_CONTACT_VERSION} CONFIG REQUIRED)
find_package(KPim5CalendarUtils ${CALENDARUTILS_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5Ldap ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MessageCore ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MessageList ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MessageViewer ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MessageComposer ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MimeTreeParser ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5TemplateParser ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5PimCommon ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5LibKSieve ${LIBKSIEVE_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5KontactInterface ${KONTACTINTERFACE_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5MailCommon ${MAILCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim5Gravatar ${LIBGRAVATAR_VERSION} CONFIG REQUIRED)
find_package(KPim5Tnef ${KTNEF_LIB_VERSION} CONFIG REQUIRED)

# Optional KPim dependencies
find_package(KPim5IdentityManagementCore ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG)
set_package_properties(KPim5IdentityManagementCore PROPERTIES
    DESCRIPTION "KDE PIM Identity Management Core Library"
    URL "https://invent.kde.org/pim/kidentitymanagement"
    TYPE OPTIONAL
    PURPOSE "Required for identity management features"
)

find_package(KPim5AkonadiAgentBase ${AKONADI_VERSION} CONFIG)
set_package_properties(KPim5AkonadiAgentBase PROPERTIES
    DESCRIPTION "Akonadi Agent Base Library"
    URL "https://invent.kde.org/pim/akonadi"
    TYPE OPTIONAL
    PURPOSE "Required for Akonadi agents"
)

find_package(KF5TextCustomEditor ${KTEXTADDONS_MIN_VERSION} CONFIG)
set_package_properties(KF5TextCustomEditor PROPERTIES
    DESCRIPTION "KF5 Text Custom Editor"
    URL "https://invent.kde.org/libraries/ktextaddons"
    TYPE OPTIONAL
    PURPOSE "Required for text editor features"
)

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

# Find optional libraries and set configuration values
find_package(KF5Activities CONFIG)
if(KF5Activities_FOUND)
    set(KMAIL_HAVE_ACTIVITY_SUPPORT 1)
else()
    set(KMAIL_HAVE_ACTIVITY_SUPPORT 0)
endif()

configure_file(config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-enterprise.h)
configure_file(kmail-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kmail-version.h)
configure_file(config-kmail.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-kmail.h @ONLY)

add_definitions(-DTRANSLATION_DOMAIN=\"kmail\")
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)

if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif()

add_subdirectory(src)
if(BUILD_AGENTS)
    add_subdirectory(agents)
else()
    message(STATUS "Skipping agents as BUILD_AGENTS is OFF")
endif()

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

# Documentation temporarily disabled due to entity reference issues in translated files
# We'll fix this properly later
#if(KF5DocTools_FOUND)
#    # Set languages to exclude from documentation due to XML entity issues
#    set(KDOCTOOLS_LANG_EXCLUDE_LIST "sl;pt_BR" CACHE STRING "List of languages to exclude from documentation building")
#    kdoctools_install(po EXCLUDED_LANGUAGES ${KDOCTOOLS_LANG_EXCLUDE_LIST})
#    add_subdirectory(doc)
#endif()

ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

# Add linting targets
find_program(CLANG_FORMAT clang-format)
find_program(CLANG_TIDY clang-tidy)
find_program(CPPCHECK cppcheck)
find_program(FLAKE8 flake8)

if(CLANG_FORMAT)
    add_custom_target(format
        COMMAND ${CMAKE_SOURCE_DIR}/tools/lint.sh
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running linters on the codebase"
        VERBATIM)
endif()

if(CLANG_FORMAT)
    add_custom_target(format-fix
        COMMAND find src agents -name "*.cpp" -o -name "*.h" | xargs ${CLANG_FORMAT} -i
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Fixing code format with clang-format"
        VERBATIM)
endif()

# Define build flags for optional components
if(KF5KCMUtils_FOUND)
    add_definitions(-DHAVE_KCMUTILS)
endif()

if(KPim5IdentityManagementCore_FOUND)
    add_definitions(-DHAVE_IDENTITYMANAGEMENT_CORE)
endif()

if(KPim5AkonadiAgentBase_FOUND)
    add_definitions(-DHAVE_AKONADI_AGENT_BASE)
endif()

if(KF5TextCustomEditor_FOUND)
    add_definitions(-DHAVE_TEXT_CUSTOM_EDITOR)
endif()

if(KF5StatusNotifierItem_FOUND)
    add_definitions(-DHAVE_STATUS_NOTIFIER_ITEM)
endif()
