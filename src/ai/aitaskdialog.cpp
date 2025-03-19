#include "aitaskdialog.h"
#include "localaiservice.h"
#include "../kmmessage.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QHeaderView>

namespace KMail {

AITaskDialog::AITaskDialog(KMMessage *message, QWidget *parent)
    : QDialog(parent)
    , m_message(message)
    , m_aiService(new LocalAIService(this))
    , m_isExtracting(false)
{
    setupUi();
    createConnections();
    
    // Extract tasks automatically
    slotExtractTasks();
}

AITaskDialog::~AITaskDialog()
{
}

void AITaskDialog::setupUi()
{
    setWindowTitle(i18n("AI Task Extraction"));
    setModal(true);
    resize(600, 400);

    auto mainLayout = new QVBoxLayout(this);

    // Add status label
    m_statusLabel = new QLabel(i18n("Extracting tasks from email..."), this);
    mainLayout->addWidget(m_statusLabel);

    // Add task list
    m_taskList = new QTreeWidget(this);
    m_taskList->setHeaderLabels({i18n("Task"), i18n("Due Date"), i18n("Priority")});
    m_taskList->setRootIsDecorated(false);
    m_taskList->setAllColumnsShowFocus(true);
    m_taskList->setSelectionMode(QAbstractItemView::NoSelection);
    m_taskList->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_taskList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_taskList->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    mainLayout->addWidget(m_taskList);

    // Add progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(0); // Indeterminate
    m_progressBar->hide();
    mainLayout->addWidget(m_progressBar);

    // Add selection buttons
    auto selectionLayout = new QHBoxLayout;
    m_selectAllButton = new QPushButton(i18n("Select All"), this);
    m_selectNoneButton = new QPushButton(i18n("Select None"), this);
    selectionLayout->addWidget(m_selectAllButton);
    selectionLayout->addWidget(m_selectNoneButton);
    selectionLayout->addStretch();
    mainLayout->addLayout(selectionLayout);

    // Add bottom buttons
    auto buttonLayout = new QHBoxLayout;
    
    m_extractButton = new QPushButton(i18n("Extract Again"), this);
    m_acceptButton = new QPushButton(i18n("Add to KOrganizer"), this);
    m_cancelButton = new QPushButton(i18n("Cancel"), this);
    
    m_acceptButton->setDefault(true);
    
    buttonLayout->addWidget(m_extractButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_acceptButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);

    // Initially disable buttons until we extract tasks
    m_acceptButton->setEnabled(false);
    m_selectAllButton->setEnabled(false);
    m_selectNoneButton->setEnabled(false);
}

void AITaskDialog::createConnections()
{
    connect(m_extractButton, &QPushButton::clicked,
            this, &AITaskDialog::slotExtractTasks);
    connect(m_selectAllButton, &QPushButton::clicked,
            this, &AITaskDialog::slotSelectAll);
    connect(m_selectNoneButton, &QPushButton::clicked,
            this, &AITaskDialog::slotSelectNone);
    connect(m_acceptButton, &QPushButton::clicked,
            this, &AITaskDialog::slotAccept);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
            
    connect(m_aiService, &LocalAIService::tasksExtracted,
            this, &AITaskDialog::slotTasksExtracted);
    connect(m_aiService, &LocalAIService::error,
            this, &AITaskDialog::slotError);
}

void AITaskDialog::slotExtractTasks()
{
    if (m_isExtracting) {
        return;
    }

    m_isExtracting = true;
    m_extractButton->setEnabled(false);
    m_acceptButton->setEnabled(false);
    m_selectAllButton->setEnabled(false);
    m_selectNoneButton->setEnabled(false);
    m_taskList->clear();
    m_progressBar->show();
    m_statusLabel->setText(i18n("Extracting tasks from email..."));

    QString content = extractEmailContent();
    m_aiService->extractTasks(content);
}

void AITaskDialog::slotTasksExtracted(const QList<ExtractedTask> &tasks)
{
    m_isExtracting = false;
    m_extractButton->setEnabled(true);
    m_progressBar->hide();

    if (tasks.isEmpty()) {
        m_statusLabel->setText(i18n("No tasks found in this email."));
        m_acceptButton->setEnabled(false);
        m_selectAllButton->setEnabled(false);
        m_selectNoneButton->setEnabled(false);
        return;
    }

    m_tasks = tasks;
    updateTaskList(tasks);
    
    m_statusLabel->setText(i18n("%1 tasks found. Select the tasks you want to add to KOrganizer:", tasks.size()));
    m_acceptButton->setEnabled(true);
    m_selectAllButton->setEnabled(true);
    m_selectNoneButton->setEnabled(true);
}

void AITaskDialog::updateTaskList(const QList<ExtractedTask> &tasks)
{
    m_taskList->clear();
    
    for (const ExtractedTask &task : tasks) {
        auto item = new QTreeWidgetItem(m_taskList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, task.selected ? Qt::Checked : Qt::Unchecked);
        item->setText(0, task.description);
        item->setText(1, task.dueDate.toString(Qt::DefaultLocaleLongDate));
        item->setText(2, task.priority);
    }

    m_taskList->resizeColumnToContents(0);
    m_taskList->resizeColumnToContents(1);
    m_taskList->resizeColumnToContents(2);
}

void AITaskDialog::slotError(const QString &error)
{
    m_isExtracting = false;
    m_extractButton->setEnabled(true);
    m_acceptButton->setEnabled(false);
    m_selectAllButton->setEnabled(false);
    m_selectNoneButton->setEnabled(false);
    m_progressBar->hide();

    m_statusLabel->setText(i18n("Error extracting tasks"));
    KMessageBox::error(this, i18n("Failed to extract tasks: %1", error));
}

void AITaskDialog::slotAccept()
{
    QList<ExtractedTask> selectedTasks;
    
    for (int i = 0; i < m_taskList->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_taskList->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked) {
            selectedTasks.append(m_tasks[i]);
        }
    }

    if (selectedTasks.isEmpty()) {
        KMessageBox::error(this, i18n("Please select at least one task to add to KOrganizer."));
        return;
    }

    accept();
}

void AITaskDialog::slotSelectAll()
{
    for (int i = 0; i < m_taskList->topLevelItemCount(); ++i) {
        m_taskList->topLevelItem(i)->setCheckState(0, Qt::Checked);
    }
}

void AITaskDialog::slotSelectNone()
{
    for (int i = 0; i < m_taskList->topLevelItemCount(); ++i) {
        m_taskList->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
    }
}

QList<ExtractedTask> AITaskDialog::selectedTasks() const
{
    QList<ExtractedTask> selectedTasks;
    
    for (int i = 0; i < m_taskList->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = m_taskList->topLevelItem(i);
        if (item->checkState(0) == Qt::Checked) {
            selectedTasks.append(m_tasks[i]);
        }
    }

    return selectedTasks;
}

QString AITaskDialog::extractEmailContent() const
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
