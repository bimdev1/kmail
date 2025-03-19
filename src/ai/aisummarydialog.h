#pragma once

#include <QDialog>

class QTextEdit;
class QPushButton;
class QProgressBar;

namespace KMail {

class LocalAIService;
class KMMessage;

class AISummaryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AISummaryDialog(KMMessage *message, QWidget *parent = nullptr);
    ~AISummaryDialog() override;

private Q_SLOTS:
    void slotGenerateSummary();
    void slotSummaryGenerated(const QString &summary);
    void slotError(const QString &error);
    void slotCopyToClipboard();

private:
    void setupUi();
    void createConnections();
    QString extractEmailContent() const;

    KMMessage *m_message;
    LocalAIService *m_aiService;
    
    QTextEdit *m_summaryEdit;
    QPushButton *m_generateButton;
    QPushButton *m_copyButton;
    QPushButton *m_closeButton;
    QProgressBar *m_progressBar;
    bool m_isGenerating;
};
