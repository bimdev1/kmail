/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aimainwidgetextension.h"
#include "kmmainwidgetaifilter.h"
#include "aireplydialog.h"
#include "aitaskdialog.h"
#include "../kmmainwidget.h"
#include "../kmmessage.h"

#include <KActionMenu>
#include <KLocalizedString>
#include <KActionCollection>
#include <QAction>
#include "korganizerintegration.h"

namespace KMail {

AIMainWidgetExtension::AIMainWidgetExtension(KMMainWidget *mainWidget)
    : QObject(mainWidget)
    , m_mainWidget(mainWidget)
    , m_aiFilter(new KMMainWidgetAIFilter(mainWidget))
    , m_aiMenu(nullptr)
    , m_generateReplyAction(nullptr)
    , m_summarizeAction(nullptr)
    , m_prioritizeAction(nullptr)
    , m_extractTasksAction(nullptr)
{
    createActions();
    setupConnections();
}

AIMainWidgetExtension::~AIMainWidgetExtension()
{
    delete m_aiFilter;
}

void AIMainWidgetExtension::createActions()
{
    // Create AI menu
    m_aiMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("brain")), i18n("AI Tools"), this);
    
    // Create actions
    m_generateReplyAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-reply-sender")),
                                      i18n("Generate AI Reply"), this);
    m_summarizeAction = new QAction(QIcon::fromTheme(QStringLiteral("view-list-text")),
                                  i18n("Summarize Email"), this);
    m_prioritizeAction = new QAction(QIcon::fromTheme(QStringLiteral("flag")),
                                   i18n("Prioritize Email"), this);
    m_extractTasksAction = new QAction(QIcon::fromTheme(QStringLiteral("view-task")),
                                     i18n("Extract Tasks"), this);

    // Add actions to menu
    m_aiMenu->addAction(m_generateReplyAction);
    m_aiMenu->addAction(m_summarizeAction);
    m_aiMenu->addAction(m_prioritizeAction);
    m_aiMenu->addAction(m_extractTasksAction);

    // Add menu to main widget's action collection
    m_mainWidget->actionCollection()->addAction(QStringLiteral("ai_tools"), m_aiMenu);
}

void AIMainWidgetExtension::setupConnections()
{
    connect(m_generateReplyAction, &QAction::triggered,
            this, &AIMainWidgetExtension::slotGenerateAIReply);
    connect(m_summarizeAction, &QAction::triggered,
            this, &AIMainWidgetExtension::slotSummarizeEmail);
    connect(m_prioritizeAction, &QAction::triggered,
            this, &AIMainWidgetExtension::slotPrioritizeEmail);
    connect(m_extractTasksAction, &QAction::triggered,
            this, &AIMainWidgetExtension::slotExtractTasks);
}

void AIMainWidgetExtension::handleNewMessage(KMMessage *message)
{
    if (message) {
        m_aiFilter->processNewMessage(message);
    }
}

void AIMainWidgetExtension::updateActions()
{
    bool hasSelection = m_mainWidget->currentMessage() != nullptr;
    
    m_generateReplyAction->setEnabled(hasSelection);
    m_summarizeAction->setEnabled(hasSelection);
    m_prioritizeAction->setEnabled(hasSelection);
    m_extractTasksAction->setEnabled(hasSelection);
}

void AIMainWidgetExtension::slotGenerateAIReply()
{
    KMMessage *currentMessage = m_mainWidget->currentMessage();
    if (!currentMessage) {
        return;
    }

    auto dialog = new AIReplyDialog(currentMessage, m_mainWidget);
    if (dialog->exec() == QDialog::Accepted) {
        QString replyText = dialog->replyText();
        
        // Create a new message as a reply
        KMMessage *reply = currentMessage->createReply();
        if (reply) {
            reply->setBody(replyText.toUtf8());
            m_mainWidget->slotCompose(reply); // Open composer with the reply
        }
    }
    delete dialog;
}

void AIMainWidgetExtension::slotSummarizeEmail()
{
    KMMessage *currentMessage = m_mainWidget->currentMessage();
    if (!currentMessage) {
        return;
    }

    auto dialog = new AISummaryDialog(currentMessage, m_mainWidget);
    dialog->exec();
    delete dialog;
}

void AIMainWidgetExtension::slotPrioritizeEmail()
{
    if (auto message = m_mainWidget->currentMessage()) {
        m_aiFilter->processNewMessage(message);
    }
}

void AIMainWidgetExtension::slotExtractTasks()
{
    KMMessage *currentMessage = m_mainWidget->currentMessage();
    if (!currentMessage) {
        return;
    }

    auto dialog = new AITaskDialog(currentMessage, m_mainWidget);
    if (dialog->exec() == QDialog::Accepted) {
        QList<ExtractedTask> tasks = dialog->selectedTasks();
        
        // Create tasks in KOrganizer
        KOrganizerIntegration korg(this);
        connect(&korg, &KOrganizerIntegration::taskAdded,
                m_mainWidget, &KMMainWidget::showStatusMessage);
        connect(&korg, &KOrganizerIntegration::error,
                [this](const QString &error) {
                    KMessageBox::error(m_mainWidget, error);
                });
        
        korg.addTasks(tasks);
    }
    delete dialog;
}

} // namespace KMail
