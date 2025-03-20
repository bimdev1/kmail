#pragma once

// Compatibility header for TextAddonsWidgets/LineEditCatchReturnKey
// Since we already have our own implementation in klineediteventhandler.h,
// this is just an empty file to satisfy imports
namespace TextAddonsWidgets {
class LineEditCatchReturnKey
{
public:
    LineEditCatchReturnKey(QObject* parent, QObject* receiver) {}
};
} // namespace TextAddonsWidgets