/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aiconfigwidget.h"
#include "aimanager.h"

#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>

namespace KMail {

AIConfigWidget::AIConfigWidget(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
{
    createUI();
    load();
}

AIConfigWidget::~AIConfigWidget()
{
}

void AIConfigWidget::createUI()
{
    auto mainLayout = new QVBoxLayout(this);

    // Main AI enable/disable checkbox
    m_enableAICheckbox = new QCheckBox(i18n("Enable AI features"), this);
    m_enableAICheckbox->setToolTip(i18n("Enable or disable all AI features in KMail"));
    mainLayout->addWidget(m_enableAICheckbox);

    // Description label
    auto descLabel = new QLabel(i18n("KMail AI features provide intelligent email management, "
                                     "including inbox categorization, reply drafting, task extraction, "
                                     "and follow-up reminders."), this);
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);

    // Feature group
    auto featureGroup = new QGroupBox(i18n("AI Features"), this);
    auto featureLayout = new QVBoxLayout(featureGroup);

    m_enableCategorization = new QCheckBox(i18n("Enable inbox categorization"), this);
    m_enableCategorization->setToolTip(i18n("Automatically categorize incoming emails based on urgency and context"));
    featureLayout->addWidget(m_enableCategorization);

    m_enableReplyDrafting = new QCheckBox(i18n("Enable reply drafting"), this);
    m_enableReplyDrafting->setToolTip(i18n("Suggest AI-generated replies that match your writing style"));
    featureLayout->addWidget(m_enableReplyDrafting);

    m_enableTaskExtraction = new QCheckBox(i18n("Enable task extraction"), this);
    m_enableTaskExtraction->setToolTip(i18n("Identify actionable tasks in emails and send them to KOrganizer"));
    featureLayout->addWidget(m_enableTaskExtraction);

    m_enableFollowUpReminders = new QCheckBox(i18n("Enable follow-up reminders"), this);
    m_enableFollowUpReminders->setToolTip(i18n("Notify when an email needs a response"));
    featureLayout->addWidget(m_enableFollowUpReminders);

    mainLayout->addWidget(featureGroup);

    // Advanced settings group
    auto advancedGroup = new QGroupBox(i18n("Advanced Settings"), this);
    auto advancedLayout = new QFormLayout(advancedGroup);

    m_maxEmailsForStyle = new QSpinBox(this);
    m_maxEmailsForStyle->setRange(1, 100);
    m_maxEmailsForStyle->setValue(10);
    m_maxEmailsForStyle->setToolTip(i18n("Maximum number of previous emails to analyze for your writing style"));
    advancedLayout->addRow(i18n("Emails to analyze for style:"), m_maxEmailsForStyle);

    m_modelQuality = new QComboBox(this);
    m_modelQuality->addItem(i18n("Fast (Lower quality)"), QStringLiteral("fast"));
    m_modelQuality->addItem(i18n("Balanced"), QStringLiteral("balanced"));
    m_modelQuality->addItem(i18n("High quality (Slower)"), QStringLiteral("high"));
    m_modelQuality->setToolTip(i18n("Select the quality level of AI models (higher quality requires more resources)"));
    advancedLayout->addRow(i18n("AI model quality:"), m_modelQuality);

    mainLayout->addWidget(advancedGroup);

    // Add stretch to push everything to the top
    mainLayout->addStretch();

    // Connect signals
    connect(m_enableAICheckbox, &QCheckBox::toggled, this, &AIConfigWidget::updateUIState);
    connect(m_enableAICheckbox, &QCheckBox::toggled, this, &AIConfigWidget::changed);
    connect(m_enableCategorization, &QCheckBox::toggled, this, &AIConfigWidget::changed);
    connect(m_enableReplyDrafting, &QCheckBox::toggled, this, &AIConfigWidget::changed);
    connect(m_enableTaskExtraction, &QCheckBox::toggled, this, &AIConfigWidget::changed);
    connect(m_enableFollowUpReminders, &QCheckBox::toggled, this, &AIConfigWidget::changed);
    connect(m_maxEmailsForStyle, QOverload<int>::of(&QSpinBox::valueChanged), this, &AIConfigWidget::changed);
    connect(m_modelQuality, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AIConfigWidget::changed);
}

void AIConfigWidget::updateUIState()
{
    bool enabled = m_enableAICheckbox->isChecked();
    m_enableCategorization->setEnabled(enabled);
    m_enableReplyDrafting->setEnabled(enabled);
    m_enableTaskExtraction->setEnabled(enabled);
    m_enableFollowUpReminders->setEnabled(enabled);
    m_maxEmailsForStyle->setEnabled(enabled);
    m_modelQuality->setEnabled(enabled);
}

void AIConfigWidget::load()
{
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("AI"));
    
    // Load main AI enabled state
    m_enableAICheckbox->setChecked(config.readEntry(QStringLiteral("Enabled"), false));
    
    // Load feature states
    m_enableCategorization->setChecked(config.readEntry(QStringLiteral("EnableCategorization"), true));
    m_enableReplyDrafting->setChecked(config.readEntry(QStringLiteral("EnableReplyDrafting"), true));
    m_enableTaskExtraction->setChecked(config.readEntry(QStringLiteral("EnableTaskExtraction"), true));
    m_enableFollowUpReminders->setChecked(config.readEntry(QStringLiteral("EnableFollowUpReminders"), true));
    
    // Load advanced settings
    m_maxEmailsForStyle->setValue(config.readEntry(QStringLiteral("MaxEmailsForStyle"), 10));
    
    QString modelQuality = config.readEntry(QStringLiteral("ModelQuality"), QStringLiteral("balanced"));
    int modelIndex = m_modelQuality->findData(modelQuality);
    if (modelIndex >= 0) {
        m_modelQuality->setCurrentIndex(modelIndex);
    }
    
    updateUIState();
}

void AIConfigWidget::save()
{
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("AI"));
    
    // Save main AI enabled state
    config.writeEntry(QStringLiteral("Enabled"), m_enableAICheckbox->isChecked());
    
    // Save feature states
    config.writeEntry(QStringLiteral("EnableCategorization"), m_enableCategorization->isChecked());
    config.writeEntry(QStringLiteral("EnableReplyDrafting"), m_enableReplyDrafting->isChecked());
    config.writeEntry(QStringLiteral("EnableTaskExtraction"), m_enableTaskExtraction->isChecked());
    config.writeEntry(QStringLiteral("EnableFollowUpReminders"), m_enableFollowUpReminders->isChecked());
    
    // Save advanced settings
    config.writeEntry(QStringLiteral("MaxEmailsForStyle"), m_maxEmailsForStyle->value());
    config.writeEntry(QStringLiteral("ModelQuality"), m_modelQuality->currentData().toString());
    
    config.sync();
    
    // Update the AI manager
    AIManager::self()->setEnabled(m_enableAICheckbox->isChecked());
}

void AIConfigWidget::defaults()
{
    m_enableAICheckbox->setChecked(false);
    m_enableCategorization->setChecked(true);
    m_enableReplyDrafting->setChecked(true);
    m_enableTaskExtraction->setChecked(true);
    m_enableFollowUpReminders->setChecked(true);
    m_maxEmailsForStyle->setValue(10);
    int modelIndex = m_modelQuality->findData(QStringLiteral("balanced"));
    if (modelIndex >= 0) {
        m_modelQuality->setCurrentIndex(modelIndex);
    }
    
    updateUIState();
    Q_EMIT changed();
}

} // namespace KMail
