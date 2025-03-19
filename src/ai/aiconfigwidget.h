/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <QWidget>
#include <KCModule>

class QCheckBox;
class QSpinBox;
class QComboBox;

namespace KMail {

/**
 * @brief Configuration widget for AI features
 * 
 * This widget provides a configuration UI for AI features in KMail.
 */
class AIConfigWidget : public KCModule
{
    Q_OBJECT

public:
    explicit AIConfigWidget(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    ~AIConfigWidget() override;

    void load() override;
    void save() override;
    void defaults() override;

private:
    /**
     * Create the UI components
     */
    void createUI();

    /**
     * Enable or disable the UI components based on the AI enabled state
     */
    void updateUIState();

    /**
     * Checkbox to enable/disable AI features
     */
    QCheckBox *m_enableAICheckbox;

    /**
     * Checkbox to enable/disable AI inbox categorization
     */
    QCheckBox *m_enableCategorization;

    /**
     * Checkbox to enable/disable AI reply drafting
     */
    QCheckBox *m_enableReplyDrafting;

    /**
     * Checkbox to enable/disable AI task extraction
     */
    QCheckBox *m_enableTaskExtraction;

    /**
     * Checkbox to enable/disable AI follow-up reminders
     */
    QCheckBox *m_enableFollowUpReminders;

    /**
     * Spin box to set the maximum number of emails to analyze for user style
     */
    QSpinBox *m_maxEmailsForStyle;

    /**
     * Combo box to select the AI model quality
     */
    QComboBox *m_modelQuality;
};

} // namespace KMail
