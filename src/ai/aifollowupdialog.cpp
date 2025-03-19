#include "aifollowupdialog.h"
#include "localaiservice.h"
#include "../kmmessage.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>

namespace KMail {

AIFollowUpDialog::AIFollowUpDialog(KMMessage *message, QWidget *parent)
    : QDialog(parent)
    , m_message(message)
    , m_aiService(new LocalAIService(this))
    , m_isChecking(false)
{
    setupUi();
    createConnections();
    
    // Check for follow-up automatically
    slotCheckFollowUp();
}

AIFollowUpDialog::~AIFollowUpDialog()
{
}

void AIFollowUpDialog::setupUi()
{
    setWindowTitle(i18n("AI Follow-Up Reminder"));
    setModal(true);
    resize(400, 200);

    auto mainLayout = new QVBoxLayout(this);

    // Add status label
    m_statusLabel = new QLabel(i18n("Checking if this email needs follow-up..."), this);
    mainLayout->addWidget(m_statusLabel);

    // Add date/time editor
    m_dateTimeEdit = new QDateTimeEdit(this);
    m_dateTimeEdit->setCalendarPopup(true);
    m_dateTimeEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    m_dateTimeEdit->setMinimumDateTime(QDateTime::currentDateTime());
    mainLayout->addWidget(m_dateTimeEdit);

    // Add progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0); // Indeterminate
    m_progressBar->hide();
    mainLayout->addWidget(m_progressBar);

    // Add buttons
    auto buttonLayout = new QHBoxLayout;
    
    m_checkButton = new QPushButton(i18n("Check Again"), this);
    m_acceptButton = new QPushButton(i18n("Set Reminder"), this);
    m_cancelButton = new QPushButton(i18n("Cancel"), this);
    
    m_acceptButton->setDefault(true);
    
    buttonLayout->addWidget(m_checkButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_acceptButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);

    // Initially disable accept button until we check
    m_acceptButton->setEnabled(false);
    m_dateTimeEdit->setEnabled(false);
}

void AIFollowUpDialog::createConnections()
{
    connect(m_checkButton, &QPushButton::clicked,
            this, &AIFollowUpDialog::slotCheckFollowUp);
    connect(m_acceptButton, &QPushButton::clicked,
            this, &AIFollowUpDialog::slotAccept);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
            
    connect(m_aiService, &LocalAIService::followUpChecked,
            this, &AIFollowUpDialog::slotFollowUpChecked);
    connect(m_aiService, &LocalAIService::error,
            this, &AIFollowUpDialog::slotError);
}

void AIFollowUpDialog::slotCheckFollowUp()
{
    if (m_isChecking) {
        return;
    }

    m_isChecking = true;
    m_checkButton->setEnabled(false);
    m_acceptButton->setEnabled(false);
    m_dateTimeEdit->setEnabled(false);
    m_progressBar->show();
    m_statusLabel->setText(i18n("Checking if this email needs follow-up..."));

    QString content = extractEmailContent();
    m_aiService->checkFollowUp(content);
}

void AIFollowUpDialog::slotFollowUpChecked(bool needsFollowUp, const QDateTime &suggestedTime)
{
    m_isChecking = false;
    m_checkButton->setEnabled(true);
    m_progressBar->hide();

    if (needsFollowUp) {
        m_statusLabel->setText(i18n("This email needs follow-up. Suggested time shown below:"));
        m_acceptButton->setEnabled(true);
        m_dateTimeEdit->setEnabled(true);
        m_dateTimeEdit->setDateTime(suggestedTime);
    } else {
        m_statusLabel->setText(i18n("This email does not need follow-up."));
        m_acceptButton->setEnabled(false);
        m_dateTimeEdit->setEnabled(false);
    }
}

void AIFollowUpDialog::slotError(const QString &error)
{
    m_isChecking = false;
    m_checkButton->setEnabled(true);
    m_acceptButton->setEnabled(false);
    m_dateTimeEdit->setEnabled(false);
    m_progressBar->hide();

    m_statusLabel->setText(i18n("Error checking follow-up status"));
    KMessageBox::error(this, i18n("Failed to check follow-up status: %1", error));
}

void AIFollowUpDialog::slotAccept()
{
    if (!m_dateTimeEdit->dateTime().isValid() || m_dateTimeEdit->dateTime() <= QDateTime::currentDateTime()) {
        KMessageBox::error(this, i18n("Please select a valid future date and time for the reminder."));
        return;
    }
    accept();
}

QDateTime AIFollowUpDialog::reminderDateTime() const
{
    return m_dateTimeEdit->dateTime();
}

QString AIFollowUpDialog::extractEmailContent() const
{
    if (!m_message) {
        return QString();
    }

    QString content;
    content += QStringLiteral("Subject: ") + m_message->subject() + QStringLiteral("\n\n");
    content += m_message->plainTextContent();
    return content;
}

} // namespace KMail
