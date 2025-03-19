/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include "../aiserviceinterface.h"
#include <QWidget>
#include <QLabel>

namespace KMail {

/**
 * @brief Widget that displays the AI category of an email
 * 
 * This widget displays the AI category of an email as a colored label.
 */
class AICategoryIndicatorWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent The parent widget
     */
    explicit AICategoryIndicatorWidget(QWidget *parent = nullptr);

    /**
     * Destructor
     */
    ~AICategoryIndicatorWidget() override;

    /**
     * Set the category of the email
     * @param category The category of the email
     */
    void setCategory(AIServiceInterface::EmailCategory category);

    /**
     * Get the category of the email
     * @return The category of the email
     */
    AIServiceInterface::EmailCategory category() const;

private:
    /**
     * Update the widget based on the current category
     */
    void updateWidget();

    /**
     * The category of the email
     */
    AIServiceInterface::EmailCategory m_category;

    /**
     * The label that displays the category
     */
    QLabel *m_label;
};

} // namespace KMail
