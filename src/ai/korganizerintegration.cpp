#include "korganizerintegration.h"
#include "aitaskdialog.h"

#include <KLocalizedString>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QUuid>

namespace KMail {

KOrganizerIntegration::KOrganizerIntegration(QObject* parent) : QObject(parent) {}

KOrganizerIntegration::~KOrganizerIntegration() {}

bool KOrganizerIntegration::addTask(const ExtractedTask& task)
{
    if (!connectToKOrganizer()) {
        return false;
    }

    QDBusInterface iface(QStringLiteral("org.kde.korganizer"), QStringLiteral("/Calendar"),
                         QStringLiteral("org.kde.calendar.Calendar"));

    if (!iface.isValid()) {
        emit error(i18n("Failed to connect to KOrganizer: %1", iface.lastError().message()));
        return false;
    }

    QString incidenceXML = createIncidenceXML(task);
    QDBusReply<bool> reply = iface.call(QStringLiteral("addIncidence"), incidenceXML);

    if (!reply.isValid()) {
        emit error(i18n("Failed to add task to KOrganizer: %1", reply.error().message()));
        return false;
    }

    if (reply.value()) {
        emit taskAdded(task.description);
        return true;
    }

    emit error(i18n("KOrganizer rejected the task"));
    return false;
}

bool KOrganizerIntegration::addTasks(const QList<ExtractedTask>& tasks)
{
    bool allSuccess = true;
    for (const ExtractedTask& task : tasks) {
        if (!addTask(task)) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

bool KOrganizerIntegration::connectToKOrganizer()
{
    // Check if KOrganizer is running
    QDBusInterface iface(QStringLiteral("org.kde.korganizer"), QStringLiteral("/MainApplication"),
                         QStringLiteral("org.qtproject.Qt.QApplication"));

    if (!iface.isValid()) {
        // Try to start KOrganizer
        QDBusInterface launcher(QStringLiteral("org.kde.klauncher5"), QStringLiteral("/KLauncher"),
                                QStringLiteral("org.kde.KLauncher"));

        if (!launcher.isValid()) {
            emit error(i18n("Failed to connect to KLauncher"));
            return false;
        }

        QDBusReply<int> reply =
            launcher.call(QStringLiteral("start_service_by_desktop_name"), QStringLiteral("org.kde.korganizer.desktop"),
                          QStringList(), QStringList(), QString(), QString());

        if (!reply.isValid() || reply.value() != 0) {
            emit error(i18n("Failed to start KOrganizer"));
            return false;
        }

        // Wait for KOrganizer to start
        QThread::sleep(1);
    }

    return true;
}

QString KOrganizerIntegration::createIncidenceXML(const ExtractedTask& task) const
{
    // Create a unique ID for the task
    QString uid = QUuid::createUuid().toString();
    uid.remove('{').remove('}');

    // Convert priority to KOrganizer format (1-9, with 1 being highest)
    int priority = 5; // Default to medium priority
    if (task.priority.toLower() == QLatin1String("high")) {
        priority = 1;
    } else if (task.priority.toLower() == QLatin1String("low")) {
        priority = 9;
    }

    // Create the iCal XML
    QString xml = QStringLiteral("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                 "<icalendar xmlns=\"urn:ietf:params:xml:ns:icalendar-2.0\">\n"
                                 " <vtodo>\n"
                                 "  <uid>%1</uid>\n"
                                 "  <dtstamp>%2</dtstamp>\n"
                                 "  <summary>%3</summary>\n"
                                 "  <due>%4</due>\n"
                                 "  <priority>%5</priority>\n"
                                 "  <status>NEEDS-ACTION</status>\n"
                                 " </vtodo>\n"
                                 "</icalendar>")
                      .arg(uid)
                      .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
                      .arg(task.description.toHtmlEscaped())
                      .arg(task.dueDate.toString(Qt::ISODate))
                      .arg(priority);

    return xml;
}

} // namespace KMail
