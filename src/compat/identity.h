#pragma once

#include <KPim5/KIdentityManagement/kidentitymanagement/identity.h>

// Compatibility header for KIdentityManagementCore/Identity
namespace KIdentityManagementCore {
// Just use the regular KIdentityManagement::Identity
using Identity = KIdentityManagement::Identity;
} // namespace KIdentityManagementCore