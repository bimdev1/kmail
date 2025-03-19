#include "aireplydialog.h"
#include "localaiservice.h"
#include "../kmmessage.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>

namespace KMail {

AIReplyDialog::AIReplyDialog(KMMessage *originalMessage, QWidget *parent)
    : QDialog(parent)
    , m_originalMessage(originalMessage)
    , m_aiService(new LocalAIService(this))
    , m_isGenerating(false)
{
    setupUi();
    createConnections();
    
    // Generate initial reply
    slotGenerateReply();
}

AIReplyDialog::~AIReplyDialog()
{
}

void AIReplyDialog::setupUi()
{
    setWindowTitle(i18n("AI-Generated Reply"));
    setModal(true);
    resize(600, 400);

    auto mainLayout = new QVBoxLayout(this);

    // Add description label
    auto descLabel = new QLabel(i18n("Review and edit the AI-generated reply:"), this);
    mainLayout->addWidget(descLabel);

    // Add reply editor
    m_replyEdit = new QTextEdit(this);
    m_replyEdit->setAcceptRichText(false);
    mainLayout->addWidget(m_replyEdit);

    // Add progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0); // Indeterminate
    m_progressBar->hide();
    mainLayout->addWidget(m_progressBar);

    // Add buttons
    auto buttonLayout = new QHBoxLayout;
    
    m_generateButton = new QPushButton(i18n("Regenerate"), this);
    m_acceptButton = new QPushButton(i18n("Accept"), this);
    m_cancelButton = new QPushButton(i18n("Cancel"), this);
    
    m_acceptButton->setDefault(true);
    
    buttonLayout->addWidget(m_generateButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_acceptButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void AIReplyDialog::createConnections()
{
    connect(m_generateButton, &QPushButton::clicked,
            this, &AIReplyDialog::slotGenerateReply);
    connect(m_acceptButton, &QPushButton::clicked,
            this, &AIReplyDialog::slotAccept);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
            
    connect(m_aiService, &LocalAIService::replyGenerated,
            this, &AIReplyDialog::slotReplyGenerated);
    connect(m_aiService, &LocalAIService::error,
            this, &AIReplyDialog::slotError);
}

void AIReplyDialog::slotGenerateReply()
{
    if (m_isGenerating) {
        return;
    }

    m_isGenerating = true;
    m_generateButton->setEnabled(false);
    m_acceptButton->setEnabled(false);
    m_replyEdit->setReadOnly(true);
    m_progressBar->show();

    QString content = extractEmailContent();
    m_aiService->generateReply(content);
}

void AIReplyDialog::slotReplyGenerated(const QString &reply)
{
    m_replyEdit->setPlainText(reply);
    
    m_isGenerating = false;
    m_generateButton->setEnabled(true);
    m_acceptButton->setEnabled(true);
    m_replyEdit->setReadOnly(false);
    m_progressBar->hide();
}

void AIReplyDialog::slotError(const QString &error)
{
    m_isGenerating = false;
    m_generateButton->setEnabled(true);
    m_acceptButton->setEnabled(true);
    m_replyEdit->setReadOnly(false);
    m_progressBar->hide();

    KMessageBox::error(this, i18n("Failed to generate reply: %1", error));
}

void AIReplyDialog::slotAccept()
{
    if (m_replyEdit->toPlainText().isEmpty()) {
        KMessageBox::error(this, i18n("Cannot accept an empty reply."));
        return;
    }
    accept();
}

QString AIReplyDialog::replyText() const
{
    return m_replyEdit->toPlainText();
}

QString AIReplyDialog::extractEmailContent() const
{
    if (!m_originalMessage) {
        return QString();
    }

    QString content;
    content += QStringLiteral("Subject: ") + m_originalMessage->subject() + QStringLiteral("\n\n");
    content += m_originalMessage->plainTextContent();
    return content;
}

} // namespace KMail
