#pragma once

#include <QDialog>

class QTextEdit;
class QPushButton;
class QProgressBar;

namespace KMail {

class LocalAIService;
class KMMessage;

class AIReplyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIReplyDialog(KMMessage* originalMessage, QWidget* parent = nullptr);
    ~AIReplyDialog() override;

    QString replyText() const;

private Q_SLOTS:
    void slotGenerateReply();
    void slotReplyGenerated(const QString& reply);
    void slotError(const QString& error);
    void slotAccept();

private:
    void setupUi();
    void createConnections();
    QString extractEmailContent() const;

    KMMessage* m_originalMessage;
    LocalAIService* m_aiService;

    QTextEdit* m_replyEdit;
    QPushButton* m_generateButton;
    QPushButton* m_acceptButton;
    QPushButton* m_cancelButton;
    QProgressBar* m_progressBar;
    bool m_isGenerating;
};
