/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aireplydraftwidget.h"
#include "../aimanager.h"
#include "../aiserviceinterface.h"
#include "kmail_debug.h"

#include <KLocalizedString>
#include <KConfigGroup>
#include <KSharedConfig>
#include <Akonadi/MessageQueueJob>
#include <MessageComposer/Util>

#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

namespace KMail {

AIReplyDraftWidget::AIReplyDraftWidget(QWidget *parent)
    : QWidget(parent)
    , m_isGenerating(false)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Title label
    m_titleLabel = new QLabel(i18n("AI-Generated Reply Draft"), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    mainLayout->addWidget(m_titleLabel);

    // Reply text edit
    m_replyTextEdit = new QTextEdit(this);
    m_replyTextEdit->setReadOnly(false);
    m_replyTextEdit->setPlaceholderText(i18n("Click 'Generate Reply' to create an AI-generated draft"));
    mainLayout->addWidget(m_replyTextEdit);

    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0); // Indeterminate
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);

    // Buttons layout
    auto buttonsLayout = new QHBoxLayout();

    // Generate button
    auto generateButton = new QPushButton(i18n("Generate Reply"), this);
    connect(generateButton, &QPushButton::clicked, this, &AIReplyDraftWidget::generateReply);
    buttonsLayout->addWidget(generateButton);

    // Regenerate button
    m_regenerateButton = new QPushButton(i18n("Regenerate"), this);
    connect(m_regenerateButton, &QPushButton::clicked, this, &AIReplyDraftWidget::regenerateReply);
    m_regenerateButton->setEnabled(false);
    buttonsLayout->addWidget(m_regenerateButton);

    // Insert button
    m_insertButton = new QPushButton(i18n("Insert"), this);
    connect(m_insertButton, &QPushButton::clicked, this, &AIReplyDraftWidget::insertReply);
    m_insertButton->setEnabled(false);
    buttonsLayout->addWidget(m_insertButton);

    mainLayout->addLayout(buttonsLayout);

    // Set initial state
    updateUIState();
}

AIReplyDraftWidget::~AIReplyDraftWidget()
{
}

void AIReplyDraftWidget::setOriginalMessage(const KMime::Message::Ptr &message)
{
    m_originalMessage = message;
    m_replyText.clear();
    m_replyTextEdit->clear();
    updateUIState();
}

QString AIReplyDraftWidget::replyText() const
{
    return m_replyTextEdit->toPlainText();
}

void AIReplyDraftWidget::generateReply()
{
    if (!m_originalMessage) {
        qCWarning(KMAIL_LOG) << "No original message set for AI reply generation";
        return;
    }

    AIServiceInterface *service = aiService();
    if (!service) {
        qCWarning(KMAIL_LOG) << "AI service not available";
        return;
    }

    // Extract email content from the original message
    QString emailContent;
    if (m_originalMessage->contentType()->isMultipart()) {
        // Handle multipart messages
        KMime::Content *textPart = MessageComposer::Util::findTextPart(m_originalMessage.data(), true);
        if (textPart) {
            emailContent = textPart->decodedText();
        }
    } else {
        // Handle simple text messages
        emailContent = m_originalMessage->body();
    }

    if (emailContent.isEmpty()) {
        qCWarning(KMAIL_LOG) << "Failed to extract email content for AI reply generation";
        return;
    }

    // Get user email history for style analysis
    QStringList userHistory = getUserEmailHistory();

    // Update UI state
    m_isGenerating = true;
    m_progressBar->setVisible(true);
    updateUIState();

    // Connect to the replyGenerated signal
    connect(service, &AIServiceInterface::replyGenerated, this, &AIReplyDraftWidget::handleGeneratedReply, Qt::UniqueConnection);

    // Generate the reply asynchronously
    QTimer::singleShot(0, this, [this, service, emailContent, userHistory]() {
        QString reply = service->generateReply(emailContent, userHistory);
        if (reply.isEmpty()) {
            // If the service didn't emit the signal, emit it manually
            Q_EMIT service->replyGenerated(i18n("I've reviewed your email and will respond shortly."));
        }
    });
}

void AIReplyDraftWidget::handleGeneratedReply(const QString &reply)
{
    // Disconnect from the signal
    if (AIServiceInterface *service = aiService()) {
        disconnect(service, &AIServiceInterface::replyGenerated, this, &AIReplyDraftWidget::handleGeneratedReply);
    }

    // Update the reply text
    m_replyText = reply;
    m_replyTextEdit->setText(reply);

    // Update UI state
    m_isGenerating = false;
    m_progressBar->setVisible(false);
    updateUIState();

    // Emit signal
    Q_EMIT replyGenerationComplete();
}

void AIReplyDraftWidget::insertReply()
{
    QString text = m_replyTextEdit->toPlainText();
    if (!text.isEmpty()) {
        Q_EMIT insertReplyRequested(text);
    }
}

void AIReplyDraftWidget::regenerateReply()
{
    // Clear the current reply
    m_replyText.clear();
    m_replyTextEdit->clear();

    // Generate a new reply
    generateReply();
}

void AIReplyDraftWidget::updateUIState()
{
    bool hasReply = !m_replyTextEdit->toPlainText().isEmpty();
    bool hasOriginalMessage = m_originalMessage != nullptr;

    m_insertButton->setEnabled(hasReply && !m_isGenerating);
    m_regenerateButton->setEnabled(hasOriginalMessage && !m_isGenerating);
    m_replyTextEdit->setReadOnly(m_isGenerating);
}

AIServiceInterface *AIReplyDraftWidget::aiService() const
{
    if (AIManager::self()->isEnabled()) {
        return AIManager::self()->service();
    }
    return nullptr;
}

QStringList AIReplyDraftWidget::getUserEmailHistory() const
{
    // In a real implementation, this would retrieve actual sent emails
    // For now, return an empty list
    return QStringList();
}

} // namespace KMail
