#pragma once

#include <KPim5/KIdentityManagement/kidentitymanagement/identitymanager.h>

// Compatibility header for KIdentityManagementCore/IdentityManager
namespace KIdentityManagementCore {
// Just use the regular KIdentityManagement::IdentityManager
using IdentityManager = KIdentityManagement::IdentityManager;
} // namespace KIdentityManagementCore