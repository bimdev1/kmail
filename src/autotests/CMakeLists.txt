# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DTEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
include_directories(
    BEFORE
    ${kmail_SOURCE_DIR}
    ${kmail_BINARY_DIR}
)

########### dbus test ###############
add_executable(displaymessageformatactionmenutest displaymessageformatactionmenutest.cpp displaymessageformatactionmenutest.h)
add_test(NAME displaymessageformatactionmenutest COMMAND displaymessageformatactionmenutest)
ecm_mark_as_test(kmail-displaymessageformatactionmenutest)
target_link_libraries(displaymessageformatactionmenutest Qt::Test  KPim5::MessageViewer KF5::I18n kmailprivate)

#####
add_executable(identityaddvcarddialogtest  identityaddvcarddialogtest.cpp identityaddvcarddialogtest.h)
ecm_mark_as_test(kmail-identityaddvcarddialogtest)
target_link_libraries(identityaddvcarddialogtest Qt::Test Qt::Widgets KF5::Completion KF5::KIOWidgets KF5::I18n kmailprivate)

#####
add_executable(createfollowupreminderonexistingmessagejobtest createfollowupreminderonexistingmessagejobtest.cpp createfollowupreminderonexistingmessagejobtest.h)
ecm_mark_as_test(kmail_createfollowupreminderonexistingmessagejobtest)
target_link_libraries(createfollowupreminderonexistingmessagejobtest Qt::Test Qt::Widgets KPim5::AkonadiCore KF5::CalendarCore KPim5::Mime KPim5::MessageComposer kmailprivate)
#####

add_executable(cryptostateindicatorwidgettest cryptostateindicatorwidgettest.cpp cryptostateindicatorwidgettest.h)
add_test(NAME cryptostateindicatorwidgettest COMMAND cryptostateindicatorwidgettest)
ecm_mark_as_test(cryptostateindicatorwidgettest)
target_link_libraries(cryptostateindicatorwidgettest Qt::Test  KPim5::MessageCore Qt::Widgets KF5::I18n kmailprivate)
#####

add_executable(encryptionstatetest encryptionstatetest.cpp encryptionstatetest.h)
add_test(NAME encryptionstatetest COMMAND encryptionstatetest)
ecm_mark_as_test(encryptionstatetest)
target_link_libraries(encryptionstatetest Qt::Test kmailprivate)

add_executable(kactionmenutransporttest kactionmenutransporttest.cpp kactionmenutransporttest.h)
add_test(NAME kactionmenutransporttest COMMAND kactionmenutransporttest)
ecm_mark_as_test(kactionmenutransporttest)
target_link_libraries(kactionmenutransporttest Qt::Test  KPim5::AkonadiMime KF5::WidgetsAddons KF5::I18n KF5::ConfigGui kmailprivate)

add_akonadi_isolated_test_advanced( kmcomposerwintest.cpp "kmcomposerwintest.h"
	"Qt::Test;KPim5::IdentityManagementWidgets;KPim5::Libkleo;KPim5::MessageCore;KPim5::MessageComposer;KPim5::TemplateParser;KF5::XmlGui;Qt::Widgets;KF5::I18n;kmailprivate;KF5::Codecs")

add_akonadi_isolated_test_advanced( tagselectdialogtest.cpp  "tagselectdialogtest.h" "kmailprivate;KPim5::MailCommon;KPim5::Libkdepim;KF5::ItemViews;KPim5::TemplateParser;KF5::XmlGui;KF5::Completion;KF5::I18n")

if(TARGET KPim5::IdentityManagementCore)
    # Original tests that depend on IdentityManagementCore
    add_akonadi_isolated_test_advanced(kmcommandstest.cpp "" "kmailprivate;Qt::Test;KF5::I18n")
else()
    # Dummy test when dependencies aren't available
    add_executable(kmcommandstest dummy_test.cpp)
    target_link_libraries(kmcommandstest Qt::Test)
    add_test(NAME kmcommandstest COMMAND kmcommandstest)
endif()

add_executable(commandlineinfotest commandlineinfotest.cpp commandlineinfotest.h)
add_test(NAME commandlineinfotest COMMAND commandlineinfotest)
ecm_mark_as_test(commandlineinfotest)
target_link_libraries(commandlineinfotest Qt::Test  KPim5::AkonadiMime KF5::WidgetsAddons KF5::I18n KF5::ConfigGui kmailprivate)
