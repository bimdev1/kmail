# SPDX-FileCopyrightText: none
# SPDX-License-Identifier: BSD-3-Clause

# Only build these agents if the dependencies are available
if(TARGET KPim5::AkonadiAgentBase)
    add_subdirectory(sendlateragent)
    add_subdirectory(archivemailagent)
    add_subdirectory(followupreminderagent)
    add_subdirectory(unifiedmailboxagent)
    add_subdirectory(mailmergeagent)
    
    if(TARGET KPim5::IdentityManagementCore)
        add_subdirectory(mailfilteragent)
    else()
        message(STATUS "Skipping mailfilteragent due to missing IdentityManagementCore")
    endif()
else()
    message(STATUS "Skipping agents due to missing AkonadiAgentBase")
endif()
