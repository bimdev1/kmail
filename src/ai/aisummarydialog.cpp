#include "aisummarydialog.h"
#include "localaiservice.h"
#include "../kmmessage.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QClipboard>
#include <QApplication>

namespace KMail {

AISummaryDialog::AISummaryDialog(KMMessage *message, QWidget *parent)
    : QDialog(parent)
    , m_message(message)
    , m_aiService(new LocalAIService(this))
    , m_isGenerating(false)
{
    setupUi();
    createConnections();
    
    // Generate summary automatically
    slotGenerateSummary();
}

AISummaryDialog::~AISummaryDialog()
{
}

void AISummaryDialog::setupUi()
{
    setWindowTitle(i18n("AI Email Summary"));
    setModal(true);
    resize(600, 400);

    auto mainLayout = new QVBoxLayout(this);

    // Add summary editor
    m_summaryEdit = new QTextEdit(this);
    m_summaryEdit->setReadOnly(true);
    m_summaryEdit->setPlaceholderText(i18n("Generating summary..."));
    mainLayout->addWidget(m_summaryEdit);

    // Add progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0); // Indeterminate
    m_progressBar->hide();
    mainLayout->addWidget(m_progressBar);

    // Add buttons
    auto buttonLayout = new QHBoxLayout;
    
    m_generateButton = new QPushButton(i18n("Regenerate"), this);
    m_copyButton = new QPushButton(i18n("Copy to Clipboard"), this);
    m_closeButton = new QPushButton(i18n("Close"), this);
    
    m_closeButton->setDefault(true);
    
    buttonLayout->addWidget(m_generateButton);
    buttonLayout->addWidget(m_copyButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);

    // Initially disable buttons until we generate the summary
    m_generateButton->setEnabled(false);
    m_copyButton->setEnabled(false);
}

void AISummaryDialog::createConnections()
{
    connect(m_generateButton, &QPushButton::clicked,
            this, &AISummaryDialog::slotGenerateSummary);
    connect(m_copyButton, &QPushButton::clicked,
            this, &AISummaryDialog::slotCopyToClipboard);
    connect(m_closeButton, &QPushButton::clicked,
            this, &QDialog::accept);
            
    connect(m_aiService, &LocalAIService::summaryGenerated,
            this, &AISummaryDialog::slotSummaryGenerated);
    connect(m_aiService, &LocalAIService::error,
            this, &AISummaryDialog::slotError);
}

void AISummaryDialog::slotGenerateSummary()
{
    if (m_isGenerating) {
        return;
    }

    m_isGenerating = true;
    m_generateButton->setEnabled(false);
    m_copyButton->setEnabled(false);
    m_summaryEdit->clear();
    m_summaryEdit->setPlaceholderText(i18n("Generating summary..."));
    m_progressBar->show();

    QString content = extractEmailContent();
    m_aiService->summarizeEmail(content);
}

void AISummaryDialog::slotSummaryGenerated(const QString &summary)
{
    m_isGenerating = false;
    m_generateButton->setEnabled(true);
    m_copyButton->setEnabled(true);
    m_progressBar->hide();

    m_summaryEdit->setPlainText(summary);
}

void AISummaryDialog::slotError(const QString &error)
{
    m_isGenerating = false;
    m_generateButton->setEnabled(true);
    m_copyButton->setEnabled(false);
    m_progressBar->hide();

    m_summaryEdit->clear();
    m_summaryEdit->setPlaceholderText(i18n("Failed to generate summary"));
    KMessageBox::error(this, i18n("Failed to generate summary: %1", error));
}

void AISummaryDialog::slotCopyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_summaryEdit->toPlainText());
    m_mainWidget->showStatusMessage(i18n("Summary copied to clipboard"));
}

QString AISummaryDialog::extractEmailContent() const
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
