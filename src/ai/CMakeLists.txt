# SPDX-FileCopyrightText: 2025 KMail AI Integration Team
# SPDX-License-Identifier: BSD-3-Clause

# AI module for KMail

# Install Python scripts
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python/
        DESTINATION ${KDE_INSTALL_DATADIR}/kmail/ai/python
        FILES_MATCHING PATTERN "*.py"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Add widgets subdirectory
add_subdirectory(widgets)
