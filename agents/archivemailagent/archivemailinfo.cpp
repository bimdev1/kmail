/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "archivemailinfo.h"
using namespace Qt::Literals::StringLiterals;

#include <KLocalizedString>
#include <QDir>
#include "archivemailagent_debug.h"

ArchiveMailInfo::ArchiveMailInfo() = default;

ArchiveMailInfo::ArchiveMailInfo(const KConfigGroup& config)
{
    readConfig(config);
}

ArchiveMailInfo::ArchiveMailInfo(const ArchiveMailInfo& info)
    : mLastDateSaved(info.lastDateSaved()), mArchiveAge(info.archiveAge()), mArchiveType(info.archiveType()),
      mArchiveUnit(info.archiveUnit()), mSaveCollectionId(info.saveCollectionId()), mPath(info.url()),
      mRanges(info.range()), mMaximumArchiveCount(info.maximumArchiveCount()),
      mSaveSubCollection(info.saveSubCollection()), mIsEnabled(info.isEnabled()), mUseRange(info.useRange())
{
}

ArchiveMailInfo::~ArchiveMailInfo() = default;

ArchiveMailInfo& ArchiveMailInfo::operator=(const ArchiveMailInfo& old)
{
    mLastDateSaved = old.lastDateSaved();
    mArchiveAge = old.archiveAge();
    mArchiveType = old.archiveType();
    mArchiveUnit = old.archiveUnit();
    mSaveCollectionId = old.saveCollectionId();
    mMaximumArchiveCount = old.maximumArchiveCount();
    mSaveSubCollection = old.saveSubCollection();
    mPath = old.url();
    mIsEnabled = old.isEnabled();
    mUseRange = old.useRange();
    mRanges = old.range();
    return *this;
}

QString normalizeFolderName(const QString& folderName)
{
    QString adaptFolderName(folderName);
    adaptFolderName.replace(QLatin1Char('/'), QLatin1Char('_'));
    return adaptFolderName;
}

QString ArchiveMailInfo::dirArchive(bool& dirExit) const
{
    const QDir dir(url().path());
    QString dirPath = url().path();
    if (!dir.exists()) {
        dirExit = false;
        dirPath = QDir::homePath();
        qCWarning(ARCHIVEMAILAGENT_LOG) << " Path doesn't exist" << dir.path();
    } else {
        dirExit = true;
    }
    return dirPath;
}

QList<int> ArchiveMailInfo::range() const
{
    return mRanges;
}

void ArchiveMailInfo::setRange(const QList<int>& newRanges)
{
    mRanges = newRanges;
}

bool ArchiveMailInfo::useRange() const
{
    return mUseRange;
}

void ArchiveMailInfo::setUseRange(bool newUseRange)
{
    mUseRange = newUseRange;
}

QUrl ArchiveMailInfo::realUrl(const QString& folderName, bool& dirExist) const
{
    const int numExtensions = 4;
    // The extensions here are also sorted, like the enum order of BackupJob::ArchiveType
    const char* extensions[numExtensions] = {".zip", ".tar", ".tar.bz2", ".tar.gz"};
    const QString dirPath = dirArchive(dirExist);

    const QString path = dirPath + QLatin1Char('/') +
                         i18nc("Start of the filename for a mail archive file", "Archive") + QLatin1Char('_') +
                         normalizeFolderName(folderName) + QLatin1Char('_') +
                         QDate::currentDate().toString(Qt::ISODate) + QString::fromLatin1(extensions[mArchiveType]);
    const QUrl real(QUrl::fromLocalFile(path));
    return real;
}

QStringList ArchiveMailInfo::listOfArchive(const QString& folderName, bool& dirExist) const
{
    const int numExtensions = 4;
    // The extensions here are also sorted, like the enum order of BackupJob::ArchiveType
    const char* extensions[numExtensions] = {".zip", ".tar", ".tar.bz2", ".tar.gz"};
    const QString dirPath = dirArchive(dirExist);

    QDir dir(dirPath);

    QStringList nameFilters;
    nameFilters << i18nc("Start of the filename for a mail archive file", "Archive") + QLatin1Char('_') +
                       normalizeFolderName(folderName) + QLatin1Char('_') + "*"_L1 +
                       QString::fromLatin1(extensions[mArchiveType]);
    const QStringList lst = dir.entryList(nameFilters, QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
    return lst;
}

bool ArchiveMailInfo::isValid() const
{
    return mSaveCollectionId != -1;
}

void ArchiveMailInfo::setArchiveAge(int age)
{
    mArchiveAge = age;
}

int ArchiveMailInfo::archiveAge() const
{
    return mArchiveAge;
}

void ArchiveMailInfo::setArchiveUnit(ArchiveMailInfo::ArchiveUnit unit)
{
    mArchiveUnit = unit;
}

ArchiveMailInfo::ArchiveUnit ArchiveMailInfo::archiveUnit() const
{
    return mArchiveUnit;
}

void ArchiveMailInfo::setArchiveType(MailCommon::BackupJob::ArchiveType type)
{
    mArchiveType = type;
}

MailCommon::BackupJob::ArchiveType ArchiveMailInfo::archiveType() const
{
    return mArchiveType;
}

void ArchiveMailInfo::setLastDateSaved(QDate date)
{
    mLastDateSaved = date;
}

QDate ArchiveMailInfo::lastDateSaved() const
{
    return mLastDateSaved;
}

void ArchiveMailInfo::readConfig(const KConfigGroup& config)
{
    mPath = QUrl::fromUserInput(config.readEntry("storePath"));

    if (config.hasKey(QStringLiteral("lastDateSaved"))) {
        mLastDateSaved = QDate::fromString(config.readEntry("lastDateSaved"), Qt::ISODate);
    }
    mSaveSubCollection = config.readEntry("saveSubCollection", false);
    mArchiveType = static_cast<MailCommon::BackupJob::ArchiveType>(
        config.readEntry("archiveType", (int)MailCommon::BackupJob::Zip));
    mArchiveUnit = static_cast<ArchiveUnit>(config.readEntry("archiveUnit", (int)ArchiveDays));
    Akonadi::Collection::Id tId = config.readEntry("saveCollectionId", mSaveCollectionId);
    mArchiveAge = config.readEntry("archiveAge", 1);
    mMaximumArchiveCount = config.readEntry("maximumArchiveCount", 0);
    mUseRange = config.readEntry("useRange", false);
    mRanges = config.readEntry("ranges", QList<int>());
    if (tId >= 0) {
        mSaveCollectionId = tId;
    }
    mIsEnabled = config.readEntry("enabled", true);
}

void ArchiveMailInfo::writeConfig(KConfigGroup& config)
{
    if (!isValid()) {
        return;
    }
    config.writeEntry("storePath", mPath.toLocalFile());

    if (mLastDateSaved.isValid()) {
        config.writeEntry("lastDateSaved", mLastDateSaved.toString(Qt::ISODate));
    }

    config.writeEntry("saveSubCollection", mSaveSubCollection);
    config.writeEntry("archiveType", static_cast<int>(mArchiveType));
    config.writeEntry("archiveUnit", static_cast<int>(mArchiveUnit));
    config.writeEntry("saveCollectionId", mSaveCollectionId);
    config.writeEntry("archiveAge", mArchiveAge);
    config.writeEntry("maximumArchiveCount", mMaximumArchiveCount);
    config.writeEntry("enabled", mIsEnabled);
    config.writeEntry("useRange", mUseRange);
    config.writeEntry("ranges", mRanges);
    config.sync();
}

QUrl ArchiveMailInfo::url() const
{
    return mPath;
}

void ArchiveMailInfo::setUrl(const QUrl& url)
{
    mPath = url;
}

bool ArchiveMailInfo::saveSubCollection() const
{
    return mSaveSubCollection;
}

void ArchiveMailInfo::setSaveSubCollection(bool saveSubCol)
{
    mSaveSubCollection = saveSubCol;
}

void ArchiveMailInfo::setSaveCollectionId(Akonadi::Collection::Id collectionId)
{
    mSaveCollectionId = collectionId;
}

Akonadi::Collection::Id ArchiveMailInfo::saveCollectionId() const
{
    return mSaveCollectionId;
}

int ArchiveMailInfo::maximumArchiveCount() const
{
    return mMaximumArchiveCount;
}

void ArchiveMailInfo::setMaximumArchiveCount(int max)
{
    mMaximumArchiveCount = max;
}

bool ArchiveMailInfo::isEnabled() const
{
    return mIsEnabled;
}

void ArchiveMailInfo::setEnabled(bool b)
{
    mIsEnabled = b;
}

bool ArchiveMailInfo::operator==(const ArchiveMailInfo& other) const
{
    return saveCollectionId() == other.saveCollectionId() && saveSubCollection() == other.saveSubCollection() &&
           url() == other.url() && archiveType() == other.archiveType() && archiveUnit() == other.archiveUnit() &&
           archiveAge() == other.archiveAge() && lastDateSaved() == other.lastDateSaved() &&
           maximumArchiveCount() == other.maximumArchiveCount() && isEnabled() == other.isEnabled() &&
           useRange() == other.useRange() && range() == other.range();
}

QDebug operator<<(QDebug d, const ArchiveMailInfo& t)
{
    d.space() << "mLastDateSaved" << t.lastDateSaved();
    d.space() << "mArchiveAge" << t.archiveAge();
    d.space() << "mArchiveType" << t.archiveType();
    d.space() << "mArchiveUnit" << t.archiveUnit();
    d.space() << "mSaveCollectionId" << t.saveCollectionId();
    d.space() << "mPath" << t.url();
    d.space() << "mRanges" << t.range();
    d.space() << "mMaximumArchiveCount" << t.maximumArchiveCount();
    d.space() << "mSaveSubCollection" << t.saveSubCollection();
    d.space() << "mIsEnabled" << t.isEnabled();
    d.space() << "mUseRange" << t.useRange();
    return d;
}
