#pragma once

#include <QDateTime>
#include <QDialog>
#include <QList>

class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QLabel;
class QProgressBar;

namespace KMail {

class LocalAIService;
class KMMessage;

struct ExtractedTask {
    QString description;
    QDateTime dueDate;
    QString priority;
    bool selected;

    ExtractedTask() : selected(true) {}
};

class AITaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AITaskDialog(KMMessage* message, QWidget* parent = nullptr);
    ~AITaskDialog() override;

    QList<ExtractedTask> selectedTasks() const;

private Q_SLOTS:
    void slotExtractTasks();
    void slotTasksExtracted(const QList<ExtractedTask>& tasks);
    void slotError(const QString& error);
    void slotAccept();
    void slotSelectAll();
    void slotSelectNone();

private:
    void setupUi();
    void createConnections();
    QString extractEmailContent() const;
    void updateTaskList(const QList<ExtractedTask>& tasks);

    KMMessage* m_message;
    LocalAIService* m_aiService;
    QList<ExtractedTask> m_tasks;

    QLabel* m_statusLabel;
    QTreeWidget* m_taskList;
    QPushButton* m_extractButton;
    QPushButton* m_selectAllButton;
    QPushButton* m_selectNoneButton;
    QPushButton* m_acceptButton;
    QPushButton* m_cancelButton;
    QProgressBar* m_progressBar;
    bool m_isExtracting;
};
