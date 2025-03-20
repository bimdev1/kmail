#pragma once

#include <QTextEdit>

// Compatibility header for TextCustomEditor/PlainTextEditorWidget
namespace TextCustomEditor {
// Simple replacement for PlainTextEditorWidget
class PlainTextEditorWidget : public QTextEdit
{
public:
    explicit PlainTextEditorWidget(QWidget* parent = nullptr) : QTextEdit(parent) {}
};
} // namespace TextCustomEditor