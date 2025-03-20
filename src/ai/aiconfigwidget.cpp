/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aiconfigwidget.h"
#include "localaiservice.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

namespace KMail {

AIConfigWidget::AIConfigWidget(QWidget* parent) : QWidget(parent), m_isValidating(false)
{
    setupUi();
    createConnections();
    loadSettings();
}

void AIConfigWidget::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);

    // API Key section
    auto apiKeyLayout = new QHBoxLayout;
    auto apiKeyLabel = new QLabel(tr("DeepSeek API Key:"), this);
    m_apiKeyEdit = new QLineEdit(this);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText(tr("Enter your DeepSeek API key"));

    m_testButton = new QPushButton(tr("Test Connection"), this);
    m_testButton->setEnabled(false);

    apiKeyLayout->addWidget(apiKeyLabel);
    apiKeyLayout->addWidget(m_apiKeyEdit);
    apiKeyLayout->addWidget(m_testButton);

    // Status section
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);

    mainLayout->addLayout(apiKeyLayout);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addStretch();
}

void AIConfigWidget::createConnections()
{
    connect(m_apiKeyEdit, &QLineEdit::textChanged, this, &AIConfigWidget::slotApiKeyChanged);
    connect(m_testButton, &QPushButton::clicked, this, &AIConfigWidget::slotTestConnection);
}

void AIConfigWidget::loadSettings()
{
    QSettings settings(QStringLiteral("KMail"), QStringLiteral("AI"));
    const QString apiKey = settings.value(QStringLiteral("DeepSeekApiKey")).toString();
    m_apiKeyEdit->setText(apiKey);
    updateTestButton();
}

void AIConfigWidget::saveSettings()
{
    QSettings settings(QStringLiteral("KMail"), QStringLiteral("AI"));
    settings.setValue(QStringLiteral("DeepSeekApiKey"), m_apiKeyEdit->text());
    emit configChanged();
}

void AIConfigWidget::slotApiKeyChanged()
{
    updateTestButton();
}

void AIConfigWidget::updateTestButton()
{
    m_testButton->setEnabled(!m_apiKeyEdit->text().isEmpty() && !m_isValidating);
}

void AIConfigWidget::slotTestConnection()
{
    if (m_isValidating) {
        return;
    }

    m_isValidating = true;
    updateTestButton();
    m_statusLabel->setText(tr("Testing connection..."));

    // Create a temporary LocalAIService to test the connection
    auto service = new LocalAIService(this);
    service->setApiKey(m_apiKeyEdit->text());

    // Test the connection by trying to categorize a simple email
    connect(service, &LocalAIService::error, this, [this, service](const QString& error) {
        service->deleteLater();
        m_isValidating = false;
        updateTestButton();
        m_statusLabel->setText(tr("Connection failed: %1").arg(error));
        emit testConnectionFinished(false, error);
    });

    connect(service, &LocalAIService::emailCategorized, this, [this, service](EmailCategory) {
        service->deleteLater();
        m_isValidating = false;
        updateTestButton();
        m_statusLabel->setText(tr("Connection successful!"));
        emit testConnectionFinished(true, QString());
    });

    service->categorizeEmail(QStringLiteral("Test email for API validation"));
}

} // namespace KMail
