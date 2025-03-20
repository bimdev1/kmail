#pragma once

#include <MailTransport/TransportManagementWidget>

// Compatibility header for TransportManagementWidgetNg
namespace MailTransport {
// Just use the regular TransportManagementWidget as a drop-in replacement
using TransportManagementWidgetNg = TransportManagementWidget;
} // namespace MailTransport