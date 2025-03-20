#pragma once

#include <KConfigDialog>
#include <KPageDialog>

// Compatibility header for KCMultiDialog
namespace KDELibs4Support {
// This is a simplified compatibility class for KCMultiDialog
// Original class from KDELibs4Support would have been more complex
class KCMultiDialog : public KPageDialog
{
public:
    explicit KCMultiDialog(QWidget* parent = nullptr) : KPageDialog(parent) { setFaceType(KPageDialog::List); }

    void addModule(const QString& moduleName)
    {
        // In Qt5 implementation, this would actually load KCM modules
        // This is just a stub for compatibility
    }
};
} // namespace KDELibs4Support