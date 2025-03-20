/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <Akonadi/Item>
#include <KActionCollection>
#include <KMime/Message>
#include <QObject>
#include <QPointer>

class QAction;
class KActionMenu;
class KToggleAction;

namespace KMail {

class KMMainWidget;
class KMMainWidgetAIFilter;
class KMMessage;

/**
 * @brief Extension for KMMainWidget that adds AI-powered features
 *
 * This class extends KMMainWidget with AI-powered features such as
 * AI-generated reply drafts, email summarization, and task extraction.
 */
class AIMainWidgetExtension : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param mainWidget The parent KMMainWidget
     */
    explicit AIMainWidgetExtension(KMMainWidget* mainWidget);

    /**
     * Destructor
     */
    ~AIMainWidgetExtension() override;

    /**
     * Handle a new message
     * @param message The new message
     */
    void handleNewMessage(KMMessage* message);

    /**
     * Update the AI actions
     */
    void updateActions();

private Q_SLOTS:
    /**
     * Generate an AI reply draft for the selected message
     */
    void slotGenerateAIReply();

    /**
     * Summarize the selected message using AI
     */
    void slotSummarizeEmail();

    /**
     * Prioritize the selected message using AI
     */
    void slotPrioritizeEmail();

    /**
     * Extract tasks from the selected message using AI
     */
    void slotExtractTasks();

private:
    /**
     * Create the AI actions
     */
    void createActions();

    /**
     * Setup connections
     */
    void setupConnections();

    /**
     * The parent KMMainWidget
     */
    QPointer<KMMainWidget> m_mainWidget;

    /**
     * The AI filter
     */
    KMMainWidgetAIFilter* m_aiFilter;

    /**
     * The AI menu
     */
    KActionMenu* m_aiMenu;

    /**
     * The AI generate reply action
     */
    QAction* m_generateReplyAction;

    /**
     * The AI summarize action
     */
    QAction* m_summarizeAction;

    /**
     * The AI prioritize action
     */
    QAction* m_prioritizeAction;

    /**
     * The AI extract tasks action
     */
    QAction* m_extractTasksAction;
};

} // namespace KMail
