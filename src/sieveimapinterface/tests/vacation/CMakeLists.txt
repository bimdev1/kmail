# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_executable(vacationmultiservertest main.cpp)
target_link_libraries(vacationmultiservertest  KPim5::KSieveUi KF5::I18n KPim5::AkonadiCore kmailprivate)


target_link_libraries(vacationmultiservertest qt6keychain)
