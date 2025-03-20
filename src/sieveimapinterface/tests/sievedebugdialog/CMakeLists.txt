# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause
add_executable(sievedebugdialog sievedebugdialog.cpp)

target_link_libraries(sievedebugdialog
  KPim5::KSieve
  KPim5::KSieveUi
  KPim5::AkonadiCore
  KF5::SyntaxHighlighting
  kmailprivate
)

target_link_libraries(sievedebugdialog qt6keychain)
