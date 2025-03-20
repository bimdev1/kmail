#pragma once

#include <QDateTime>
#include <QDialog>

class QDateTimeEdit;
class QPushButton;
class QLabel;
class QProgressBar;

namespace KMail {

class LocalAIService;
class KMMessage;

class AIFollowUpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIFollowUpDialog(KMMessage* message, QWidget* parent = nullptr);
    ~AIFollowUpDialog() override;

    QDateTime reminderDateTime() const;

private Q_SLOTS:
    void slotCheckFollowUp();
    void slotFollowUpChecked(bool needsFollowUp, const QDateTime& suggestedTime);
    void slotError(const QString& error);
    void slotAccept();

private:
    void setupUi();
    void createConnections();
    QString extractEmailContent() const;

    KMMessage* m_message;
    LocalAIService* m_aiService;

    QLabel* m_statusLabel;
    QDateTimeEdit* m_dateTimeEdit;
    QPushButton* m_checkButton;
    QPushButton* m_acceptButton;
    QPushButton* m_cancelButton;
    QProgressBar* m_progressBar;
    bool m_isChecking;
};
