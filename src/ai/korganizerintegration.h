#pragma once

#include <QObject>
#include <QDateTime>
#include <QString>

namespace KMail {

struct ExtractedTask;

class KOrganizerIntegration : public QObject
{
    Q_OBJECT

public:
    explicit KOrganizerIntegration(QObject *parent = nullptr);
    ~KOrganizerIntegration() override;

    bool addTask(const ExtractedTask &task);
    bool addTasks(const QList<ExtractedTask> &tasks);

Q_SIGNALS:
    void taskAdded(const QString &summary);
    void error(const QString &errorMessage);

private:
    bool connectToKOrganizer();
    QString createIncidenceXML(const ExtractedTask &task) const;
};
