#pragma once

#include <QLineEdit>
#include "textaddonswidgetslineeditcatchreturnkey.h"

// Compatibility header for KLineEditEventHandler
namespace KLineEditEventHandler {
inline void catchReturnKey(QLineEdit* edit)
{
    // Create the event handler and make it a child of the line edit
    // so it will be deleted automatically with the line edit
    new TextAddonsWidgets::LineEditCatchReturnKey(edit, edit);
}
} // namespace KLineEditEventHandler