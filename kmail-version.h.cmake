/*
  SPDX-FileCopyrightText: 2023-2024 KDE PIM Team

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

// Define the version for KMail
#define KMAIL_VERSION "${PIM_VERSION}"
#define KMAIL_RELEASE_VERSION "${KMAIL_RELEASE_VERSION_DATE}"

// KMail version as components
#define KMAIL_VERSION_MAJOR ${PIM_VERSION_MAJOR}
#define KMAIL_VERSION_MINOR ${PIM_VERSION_MINOR}
#define KMAIL_VERSION_MICRO ${PIM_VERSION_PATCH}

// Development status
#cmakedefine KDEPIM_DEV_VERSION "${KDEPIM_DEV_VERSION}"

// Full version string with development status
#define KDEPIM_VERSION "${KDEPIM_VERSION}" 