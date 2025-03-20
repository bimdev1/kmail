/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <KMime/Message>
#include <QPointer>
#include <QWidget>

class QLabel;
class QTextEdit;
class QPushButton;
class QProgressBar;

namespace KMail {

class AIServiceInterface;

/**
 * @brief Widget for AI-generated reply drafts
 *
 * This widget displays AI-generated reply drafts and allows the user to
 * insert them into the compose window.
 */
class AIReplyDraftWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent The parent widget
     */
    explicit AIReplyDraftWidget(QWidget* parent = nullptr);

    /**
     * Destructor
     */
    ~AIReplyDraftWidget() override;

    /**
     * Set the original message to generate a reply for
     * @param message The original message
     */
    void setOriginalMessage(const KMime::Message::Ptr& message);

    /**
     * Get the generated reply text
     * @return The generated reply text
     */
    QString replyText() const;

Q_SIGNALS:
    /**
     * Emitted when the user clicks the "Insert" button
     * @param text The text to insert
     */
    void insertReplyRequested(const QString& text);

    /**
     * Emitted when the reply generation is complete
     */
    void replyGenerationComplete();

private Q_SLOTS:
    /**
     * Generate a reply to the original message
     */
    void generateReply();

    /**
     * Handle the generated reply
     * @param reply The generated reply
     */
    void handleGeneratedReply(const QString& reply);

    /**
     * Insert the generated reply into the compose window
     */
    void insertReply();

    /**
     * Regenerate the reply with a different style
     */
    void regenerateReply();

private:
    /**
     * Update the UI state based on the current state
     */
    void updateUIState();

    /**
     * Get the AI service
     * @return The AI service
     */
    AIServiceInterface* aiService() const;

    /**
     * Get a list of previous emails sent by the user
     * @return A list of previous emails
     */
    QStringList getUserEmailHistory() const;

    /**
     * The original message
     */
    KMime::Message::Ptr m_originalMessage;

    /**
     * The generated reply text
     */
    QString m_replyText;

    /**
     * The title label
     */
    QLabel* m_titleLabel;

    /**
     * The reply text edit
     */
    QTextEdit* m_replyTextEdit;

    /**
     * The insert button
     */
    QPushButton* m_insertButton;

    /**
     * The regenerate button
     */
    QPushButton* m_regenerateButton;

    /**
     * The progress bar
     */
    QProgressBar* m_progressBar;

    /**
     * Flag indicating whether a reply is being generated
     */
    bool m_isGenerating;
};

} // namespace KMail
