#pragma once

#include <KLocalizedString>
#include <QSpinBox>

// Compatibility header for KLocalization
namespace KLocalization {
static inline void setupSpinBoxFormatString(QSpinBox* spinBox, const KLocalizedString& format)
{
    spinBox->setSuffix(format.subs(spinBox->value()).toString());
    QObject::connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     [spinBox, format](int value) { spinBox->setSuffix(format.subs(value).toString()); });
}
} // namespace KLocalization