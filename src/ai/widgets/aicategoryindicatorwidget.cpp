/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aicategoryindicatorwidget.h"

#include <QHBoxLayout>
#include <QStyle>
#include <QApplication>

namespace KMail {

AICategoryIndicatorWidget::AICategoryIndicatorWidget(QWidget *parent)
    : QWidget(parent)
    , m_category(AIServiceInterface::Uncategorized)
    , m_label(new QLabel(this))
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_label);

    m_label->setAlignment(Qt::AlignCenter);
    m_label->setTextFormat(Qt::RichText);

    updateWidget();
}

AICategoryIndicatorWidget::~AICategoryIndicatorWidget()
{
}

void AICategoryIndicatorWidget::setCategory(AIServiceInterface::EmailCategory category)
{
    if (m_category == category) {
        return;
    }

    m_category = category;
    updateWidget();
}

AIServiceInterface::EmailCategory AICategoryIndicatorWidget::category() const
{
    return m_category;
}

void AICategoryIndicatorWidget::updateWidget()
{
    QString categoryText;
    QString categoryColor;
    QString categoryIcon;

    switch (m_category) {
    case AIServiceInterface::Urgent:
        categoryText = tr("Urgent");
        categoryColor = QStringLiteral("#ff0000"); // Red
        categoryIcon = QStringLiteral("dialog-warning");
        break;
    case AIServiceInterface::FollowUp:
        categoryText = tr("Follow-Up");
        categoryColor = QStringLiteral("#ff9900"); // Orange
        categoryIcon = QStringLiteral("appointment-soon");
        break;
    case AIServiceInterface::LowPriority:
        categoryText = tr("Low Priority");
        categoryColor = QStringLiteral("#0000ff"); // Blue
        categoryIcon = QStringLiteral("mail-mark-unread");
        break;
    case AIServiceInterface::Uncategorized:
    default:
        categoryText = tr("Uncategorized");
        categoryColor = QStringLiteral("#999999"); // Gray
        categoryIcon = QStringLiteral("mail-message");
        break;
    }

    // Get the icon as HTML
    QIcon icon = QIcon::fromTheme(categoryIcon);
    QPixmap pixmap = icon.pixmap(16, 16);
    QString iconHtml;
    if (!pixmap.isNull()) {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");
        iconHtml = QStringLiteral("<img src=\"data:image/png;base64,%1\" width=\"16\" height=\"16\">").arg(QString::fromLatin1(byteArray.toBase64()));
    }

    // Set the label text
    m_label->setText(QStringLiteral("<span style=\"color: %1;\">%2 %3</span>").arg(categoryColor, iconHtml, categoryText));

    // Set tooltip
    m_label->setToolTip(tr("AI-detected email category: %1").arg(categoryText));
}

} // namespace KMail
