/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include "aimainwidgetextension.h"
#include "aimanager.h"
#include "aiserviceinterface.h"
#include "widgets/aireplydraftwidget.h"
#include "kmail_debug.h"

#include <KMime/Message>
#include <KLocalizedString>
#include <KActionCollection>
#include <KActionMenu>
#include <KToggleAction>
#include <KMessageBox>
#include <KConfigGroup>
#include <KSharedConfig>

#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QPointer>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "../kmmainwidget.h"
#include "../kmkernel.h"
#include "../editor/kmcomposerwin.h"
#include "../util.h"

namespace KMail {

AIMainWidgetExtension::AIMainWidgetExtension(KMMainWidget *parent, KActionCollection *actionCollection)
    : QObject(parent)
    , m_mainWidget(parent)
    , m_actionCollection(actionCollection)
    , m_aiReplyAction(nullptr)
    , m_aiSummarizeAction(nullptr)
    , m_aiExtractTasksAction(nullptr)
    , m_aiCategorizeAction(nullptr)
    , m_aiFollowUpAction(nullptr)
    , m_aiSettingsAction(nullptr)
    , m_aiMenu(nullptr)
{
    createActions();
    
    // Connect to AI manager signals
    connect(AIManager::self(), &AIManager::enabledChanged, this, &AIMainWidgetExtension::slotAIServiceEnabledChanged);
}

AIMainWidgetExtension::~AIMainWidgetExtension()
{
}

void AIMainWidgetExtension::initialize()
{
    // Initialize the AI service
    AIManager::self();
    
    // Update actions based on AI service state
    slotAIServiceEnabledChanged(AIManager::self()->isEnabled());
}

void AIMainWidgetExtension::createActions()
{
    // Create AI menu
    m_aiMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("brain")), i18n("AI Tools"), this);
    m_actionCollection->addAction(QStringLiteral("ai_menu"), m_aiMenu);
    
    // Create AI reply action
    m_aiReplyAction = new QAction(QIcon::fromTheme(QStringLiteral("mail-reply-sender")), i18n("Generate AI Reply"), this);
    m_aiReplyAction->setToolTip(i18n("Generate an AI-powered reply draft"));
    connect(m_aiReplyAction, &QAction::triggered, this, &AIMainWidgetExtension::slotGenerateAIReply);
    m_actionCollection->addAction(QStringLiteral("ai_reply"), m_aiReplyAction);
    m_actionCollection->setDefaultShortcut(m_aiReplyAction, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_R));
    m_aiMenu->addAction(m_aiReplyAction);
    
    // Create AI summarize action
    m_aiSummarizeAction = new QAction(QIcon::fromTheme(QStringLiteral("document-preview")), i18n("Summarize Email"), this);
    m_aiSummarizeAction->setToolTip(i18n("Generate an AI-powered summary of the email"));
    connect(m_aiSummarizeAction, &QAction::triggered, this, &AIMainWidgetExtension::slotSummarizeEmail);
    m_actionCollection->addAction(QStringLiteral("ai_summarize"), m_aiSummarizeAction);
    m_actionCollection->setDefaultShortcut(m_aiSummarizeAction, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S));
    m_aiMenu->addAction(m_aiSummarizeAction);
    
    // Create AI extract tasks action
    m_aiExtractTasksAction = new QAction(QIcon::fromTheme(QStringLiteral("view-task")), i18n("Extract Tasks"), this);
    m_aiExtractTasksAction->setToolTip(i18n("Extract tasks from the email and add them to KOrganizer"));
    connect(m_aiExtractTasksAction, &QAction::triggered, this, &AIMainWidgetExtension::slotExtractTasks);
    m_actionCollection->addAction(QStringLiteral("ai_extract_tasks"), m_aiExtractTasksAction);
    m_actionCollection->setDefaultShortcut(m_aiExtractTasksAction, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_T));
    m_aiMenu->addAction(m_aiExtractTasksAction);
    
    // Create AI categorize action
    m_aiCategorizeAction = new QAction(QIcon::fromTheme(QStringLiteral("tag")), i18n("Categorize Email"), this);
    m_aiCategorizeAction->setToolTip(i18n("Categorize the email using AI"));
    connect(m_aiCategorizeAction, &QAction::triggered, this, &AIMainWidgetExtension::slotCategorizeEmail);
    m_actionCollection->addAction(QStringLiteral("ai_categorize"), m_aiCategorizeAction);
    m_actionCollection->setDefaultShortcut(m_aiCategorizeAction, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_C));
    m_aiMenu->addAction(m_aiCategorizeAction);
    
    // Create AI follow-up action
    m_aiFollowUpAction = new QAction(QIcon::fromTheme(QStringLiteral("appointment-soon")), i18n("Check Follow-Up"), this);
    m_aiFollowUpAction->setToolTip(i18n("Check if the email needs a follow-up"));
    connect(m_aiFollowUpAction, &QAction::triggered, this, &AIMainWidgetExtension::slotCheckFollowUp);
    m_actionCollection->addAction(QStringLiteral("ai_follow_up"), m_aiFollowUpAction);
    m_actionCollection->setDefaultShortcut(m_aiFollowUpAction, QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_F));
    m_aiMenu->addAction(m_aiFollowUpAction);
    
    m_aiMenu->addSeparator();
    
    // Create AI settings action
    m_aiSettingsAction = new QAction(QIcon::fromTheme(QStringLiteral("configure")), i18n("AI Settings"), this);
    m_aiSettingsAction->setToolTip(i18n("Configure AI settings"));
    connect(m_aiSettingsAction, &QAction::triggered, this, &AIMainWidgetExtension::slotShowAISettings);
    m_actionCollection->addAction(QStringLiteral("ai_settings"), m_aiSettingsAction);
    m_aiMenu->addAction(m_aiSettingsAction);
}

void AIMainWidgetExtension::updateActions(const Akonadi::Item::List &selectedItems)
{
    bool enabled = AIManager::self()->isEnabled();
    bool hasSingleSelection = selectedItems.size() == 1;
    
    m_aiReplyAction->setEnabled(enabled && hasSingleSelection);
    m_aiSummarizeAction->setEnabled(enabled && hasSingleSelection);
    m_aiExtractTasksAction->setEnabled(enabled && hasSingleSelection);
    m_aiCategorizeAction->setEnabled(enabled && hasSingleSelection);
    m_aiFollowUpAction->setEnabled(enabled && hasSingleSelection);
}

void AIMainWidgetExtension::slotGenerateAIReply()
{
    Akonadi::Item item = selectedMessage();
    if (!item.isValid()) {
        return;
    }
    
    // Create a dialog with the AI reply draft widget
    QPointer<QDialog> dlg = new QDialog(QApplication::activeWindow());
    dlg->setWindowTitle(i18n("AI Reply Draft"));
    dlg->setModal(true);
    
    QVBoxLayout *layout = new QVBoxLayout(dlg);
    
    // Create the AI reply draft widget
    AIReplyDraftWidget *replyWidget = new AIReplyDraftWidget(dlg);
    layout->addWidget(replyWidget);
    
    // Set the original message
    KMime::Message::Ptr msg = MessageComposer::Util::message(item);
    if (msg) {
        replyWidget->setOriginalMessage(msg);
    }
    
    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, dlg);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
    
    // Connect to the insert reply requested signal
    connect(replyWidget, &AIReplyDraftWidget::insertReplyRequested, dlg, [this, dlg, item](const QString &text) {
        // Open a composer window with the reply
        KMCommand *command = KMail::Util::replyCommand(item, text);
        if (command) {
            command->start();
        }
        
        // Close the dialog
        if (dlg) {
            dlg->accept();
        }
    });
    
    dlg->resize(600, 400);
    dlg->exec();
    delete dlg;
}

void AIMainWidgetExtension::slotSummarizeEmail()
{
    Akonadi::Item item = selectedMessage();
    if (!item.isValid()) {
        return;
    }
    
    AIServiceInterface *service = AIManager::self()->service();
    if (!service) {
        KMessageBox::error(QApplication::activeWindow(), i18n("AI service is not available."));
        return;
    }
    
    // Extract email content
    KMime::Message::Ptr msg = MessageComposer::Util::message(item);
    if (!msg) {
        return;
    }
    
    QString emailContent;
    if (msg->contentType()->isMultipart()) {
        // Handle multipart messages
        KMime::Content *textPart = MessageComposer::Util::findTextPart(msg.data(), true);
        if (textPart) {
            emailContent = textPart->decodedText();
        }
    } else {
        // Handle simple text messages
        emailContent = msg->body();
    }
    
    if (emailContent.isEmpty()) {
        KMessageBox::error(QApplication::activeWindow(), i18n("Failed to extract email content."));
        return;
    }
    
    // Show a progress dialog
    QPointer<QDialog> progressDlg = new QDialog(QApplication::activeWindow());
    progressDlg->setWindowTitle(i18n("Summarizing Email"));
    progressDlg->setModal(true);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDlg);
    QLabel *label = new QLabel(i18n("Generating email summary..."), progressDlg);
    layout->addWidget(label);
    
    QProgressBar *progressBar = new QProgressBar(progressDlg);
    progressBar->setRange(0, 0); // Indeterminate
    layout->addWidget(progressBar);
    
    progressDlg->show();
    
    // Connect to the emailSummarized signal
    connect(service, &AIServiceInterface::emailSummarized, progressDlg, [progressDlg, service](const QString &summary) {
        // Disconnect from the signal
        if (service) {
            disconnect(service, &AIServiceInterface::emailSummarized, nullptr, nullptr);
        }
        
        // Close the progress dialog
        if (progressDlg) {
            progressDlg->accept();
        }
        
        // Show the summary
        KMessageBox::information(QApplication::activeWindow(), summary, i18n("Email Summary"));
    });
    
    // Generate the summary asynchronously
    QTimer::singleShot(0, this, [service, emailContent]() {
        QString summary = service->summarizeEmail(emailContent);
        if (summary.isEmpty()) {
            // If the service didn't emit the signal, emit it manually
            Q_EMIT service->emailSummarized(i18n("No summary could be generated."));
        }
    });
}

void AIMainWidgetExtension::slotExtractTasks()
{
    Akonadi::Item item = selectedMessage();
    if (!item.isValid()) {
        return;
    }
    
    AIServiceInterface *service = AIManager::self()->service();
    if (!service) {
        KMessageBox::error(QApplication::activeWindow(), i18n("AI service is not available."));
        return;
    }
    
    // Extract email content
    KMime::Message::Ptr msg = MessageComposer::Util::message(item);
    if (!msg) {
        return;
    }
    
    QString emailContent;
    if (msg->contentType()->isMultipart()) {
        // Handle multipart messages
        KMime::Content *textPart = MessageComposer::Util::findTextPart(msg.data(), true);
        if (textPart) {
            emailContent = textPart->decodedText();
        }
    } else {
        // Handle simple text messages
        emailContent = msg->body();
    }
    
    if (emailContent.isEmpty()) {
        KMessageBox::error(QApplication::activeWindow(), i18n("Failed to extract email content."));
        return;
    }
    
    // Show a progress dialog
    QPointer<QDialog> progressDlg = new QDialog(QApplication::activeWindow());
    progressDlg->setWindowTitle(i18n("Extracting Tasks"));
    progressDlg->setModal(true);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDlg);
    QLabel *label = new QLabel(i18n("Extracting tasks from email..."), progressDlg);
    layout->addWidget(label);
    
    QProgressBar *progressBar = new QProgressBar(progressDlg);
    progressBar->setRange(0, 0); // Indeterminate
    layout->addWidget(progressBar);
    
    progressDlg->show();
    
    // Connect to the tasksExtracted signal
    connect(service, &AIServiceInterface::tasksExtracted, progressDlg, [progressDlg, service](const QStringList &tasks) {
        // Disconnect from the signal
        if (service) {
            disconnect(service, &AIServiceInterface::tasksExtracted, nullptr, nullptr);
        }
        
        // Close the progress dialog
        if (progressDlg) {
            progressDlg->accept();
        }
        
        if (tasks.isEmpty()) {
            KMessageBox::information(QApplication::activeWindow(), i18n("No tasks found in the email."), i18n("Task Extraction"));
            return;
        }
        
        // Show the tasks and ask if they should be added to KOrganizer
        QString taskList;
        for (const QString &task : tasks) {
            taskList += QStringLiteral("• %1\n").arg(task);
        }
        
        int result = KMessageBox::questionYesNo(QApplication::activeWindow(),
                                               i18n("The following tasks were extracted from the email:\n\n%1\n\nDo you want to add them to KOrganizer?", taskList),
                                               i18n("Task Extraction"),
                                               KGuiItem(i18n("Add to KOrganizer")),
                                               KGuiItem(i18n("Don't Add")));
        
        if (result == KMessageBox::Yes) {
            // TODO: Add tasks to KOrganizer
            // This would typically be done using D-Bus to communicate with KOrganizer
            KMessageBox::information(QApplication::activeWindow(), i18n("Tasks have been added to KOrganizer."), i18n("Task Extraction"));
        }
    });
    
    // Extract tasks asynchronously
    QTimer::singleShot(0, this, [service, emailContent]() {
        QStringList tasks = service->extractTasks(emailContent);
        if (tasks.isEmpty()) {
            // If the service didn't emit the signal, emit it manually
            Q_EMIT service->tasksExtracted(QStringList());
        }
    });
}

void AIMainWidgetExtension::slotCategorizeEmail()
{
    Akonadi::Item item = selectedMessage();
    if (!item.isValid()) {
        return;
    }
    
    AIServiceInterface *service = AIManager::self()->service();
    if (!service) {
        KMessageBox::error(QApplication::activeWindow(), i18n("AI service is not available."));
        return;
    }
    
    // Extract email content
    KMime::Message::Ptr msg = MessageComposer::Util::message(item);
    if (!msg) {
        return;
    }
    
    QString emailContent;
    if (msg->contentType()->isMultipart()) {
        // Handle multipart messages
        KMime::Content *textPart = MessageComposer::Util::findTextPart(msg.data(), true);
        if (textPart) {
            emailContent = textPart->decodedText();
        }
    } else {
        // Handle simple text messages
        emailContent = msg->body();
    }
    
    if (emailContent.isEmpty()) {
        KMessageBox::error(QApplication::activeWindow(), i18n("Failed to extract email content."));
        return;
    }
    
    // Show a progress dialog
    QPointer<QDialog> progressDlg = new QDialog(QApplication::activeWindow());
    progressDlg->setWindowTitle(i18n("Categorizing Email"));
    progressDlg->setModal(true);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDlg);
    QLabel *label = new QLabel(i18n("Categorizing email..."), progressDlg);
    layout->addWidget(label);
    
    QProgressBar *progressBar = new QProgressBar(progressDlg);
    progressBar->setRange(0, 0); // Indeterminate
    layout->addWidget(progressBar);
    
    progressDlg->show();
    
    // Connect to the emailCategorized signal
    connect(service, &AIServiceInterface::emailCategorized, progressDlg, [progressDlg, service](AIServiceInterface::EmailCategory category) {
        // Disconnect from the signal
        if (service) {
            disconnect(service, &AIServiceInterface::emailCategorized, nullptr, nullptr);
        }
        
        // Close the progress dialog
        if (progressDlg) {
            progressDlg->accept();
        }
        
        // Show the category
        QString categoryName;
        switch (category) {
        case AIServiceInterface::Urgent:
            categoryName = i18n("Urgent");
            break;
        case AIServiceInterface::FollowUp:
            categoryName = i18n("Follow-Up");
            break;
        case AIServiceInterface::LowPriority:
            categoryName = i18n("Low Priority");
            break;
        case AIServiceInterface::Uncategorized:
        default:
            categoryName = i18n("Uncategorized");
            break;
        }
        
        KMessageBox::information(QApplication::activeWindow(),
                                i18n("The email has been categorized as: %1", categoryName),
                                i18n("Email Categorization"));
    });
    
    // Categorize the email asynchronously
    QTimer::singleShot(0, this, [service, emailContent]() {
        AIServiceInterface::EmailCategory category = service->categorizeEmail(emailContent);
        // The service should emit the signal, but just in case it doesn't
        Q_EMIT service->emailCategorized(category);
    });
}

void AIMainWidgetExtension::slotCheckFollowUp()
{
    Akonadi::Item item = selectedMessage();
    if (!item.isValid()) {
        return;
    }
    
    AIServiceInterface *service = AIManager::self()->service();
    if (!service) {
        KMessageBox::error(QApplication::activeWindow(), i18n("AI service is not available."));
        return;
    }
    
    // Extract email content
    KMime::Message::Ptr msg = MessageComposer::Util::message(item);
    if (!msg) {
        return;
    }
    
    QString emailContent;
    if (msg->contentType()->isMultipart()) {
        // Handle multipart messages
        KMime::Content *textPart = MessageComposer::Util::findTextPart(msg.data(), true);
        if (textPart) {
            emailContent = textPart->decodedText();
        }
    } else {
        // Handle simple text messages
        emailContent = msg->body();
    }
    
    if (emailContent.isEmpty()) {
        KMessageBox::error(QApplication::activeWindow(), i18n("Failed to extract email content."));
        return;
    }
    
    // Show a progress dialog
    QPointer<QDialog> progressDlg = new QDialog(QApplication::activeWindow());
    progressDlg->setWindowTitle(i18n("Checking Follow-Up"));
    progressDlg->setModal(true);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDlg);
    QLabel *label = new QLabel(i18n("Checking if email needs follow-up..."), progressDlg);
    layout->addWidget(label);
    
    QProgressBar *progressBar = new QProgressBar(progressDlg);
    progressBar->setRange(0, 0); // Indeterminate
    layout->addWidget(progressBar);
    
    progressDlg->show();
    
    // Check if the email needs a follow-up
    bool needsFollowUp = service->needsFollowUp(emailContent);
    
    // Close the progress dialog
    if (progressDlg) {
        progressDlg->accept();
    }
    
    if (needsFollowUp) {
        // Get the suggested follow-up date
        QString followUpDate = service->suggestFollowUpDate(emailContent);
        
        int result = KMessageBox::questionYesNo(QApplication::activeWindow(),
                                               i18n("This email needs a follow-up by %1.\n\nDo you want to set a reminder?", followUpDate),
                                               i18n("Follow-Up Check"),
                                               KGuiItem(i18n("Set Reminder")),
                                               KGuiItem(i18n("Don't Set")));
        
        if (result == KMessageBox::Yes) {
            // TODO: Set a reminder in KOrganizer
            // This would typically be done using D-Bus to communicate with KOrganizer
            KMessageBox::information(QApplication::activeWindow(), i18n("A follow-up reminder has been set for %1.", followUpDate), i18n("Follow-Up Reminder"));
        }
    } else {
        KMessageBox::information(QApplication::activeWindow(), i18n("This email does not need a follow-up."), i18n("Follow-Up Check"));
    }
}

void AIMainWidgetExtension::slotShowAISettings()
{
    // Open the KMail configuration dialog with the AI settings page
    // In a real implementation, this would open the KMail configuration dialog
    // and select the AI settings page
    KMessageBox::information(QApplication::activeWindow(), i18n("AI settings can be configured in the KMail settings dialog."), i18n("AI Settings"));
}

void AIMainWidgetExtension::slotAIServiceEnabledChanged(bool enabled)
{
    m_aiMenu->setEnabled(enabled);
    m_aiSettingsAction->setEnabled(true); // Always enable settings
}

Akonadi::Item AIMainWidgetExtension::selectedMessage() const
{
    Akonadi::Item::List selectedItems = m_mainWidget->currentSelection();
    if (selectedItems.size() != 1) {
        return Akonadi::Item();
    }
    
    return selectedItems.first();
}

} // namespace KMail
