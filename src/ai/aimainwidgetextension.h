/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <QObject>
#include <QPointer>
#include <KActionCollection>
#include <KMime/Message>
#include <Akonadi/Item>

class QAction;
class KActionMenu;
class KToggleAction;

namespace KMail {

class KMMainWidget;

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
     * @param parent The parent KMMainWidget
     * @param actionCollection The action collection to add actions to
     */
    explicit AIMainWidgetExtension(KMMainWidget *parent, KActionCollection *actionCollection);

    /**
     * Destructor
     */
    ~AIMainWidgetExtension() override;

    /**
     * Initialize the extension
     */
    void initialize();

    /**
     * Update the AI actions based on the current selection
     * @param selectedItems The currently selected items
     */
    void updateActions(const Akonadi::Item::List &selectedItems);

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
     * Extract tasks from the selected message using AI
     */
    void slotExtractTasks();

    /**
     * Categorize the selected message using AI
     */
    void slotCategorizeEmail();

    /**
     * Check if the selected message needs a follow-up
     */
    void slotCheckFollowUp();

    /**
     * Show the AI settings dialog
     */
    void slotShowAISettings();

    /**
     * Handle AI service enabled/disabled state changes
     * @param enabled true if AI service is enabled, false otherwise
     */
    void slotAIServiceEnabledChanged(bool enabled);

private:
    /**
     * Create the AI actions
     */
    void createActions();

    /**
     * Get the selected message
     * @return The selected message
     */
    Akonadi::Item selectedMessage() const;

    /**
     * The parent KMMainWidget
     */
    KMMainWidget *m_mainWidget;

    /**
     * The action collection
     */
    KActionCollection *m_actionCollection;

    /**
     * The AI reply action
     */
    QAction *m_aiReplyAction;

    /**
     * The AI summarize action
     */
    QAction *m_aiSummarizeAction;

    /**
     * The AI extract tasks action
     */
    QAction *m_aiExtractTasksAction;

    /**
     * The AI categorize action
     */
    QAction *m_aiCategorizeAction;

    /**
     * The AI follow-up action
     */
    QAction *m_aiFollowUpAction;

    /**
     * The AI settings action
     */
    QAction *m_aiSettingsAction;

    /**
     * The AI menu
     */
    KActionMenu *m_aiMenu;
};

} // namespace KMail
