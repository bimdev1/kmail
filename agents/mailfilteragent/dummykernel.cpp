#include "dummykernel.h"

#include <Akonadi/ChangeRecorder>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Session>
#include <KIdentityManagementCore/IdentityManager>
#include <KSharedConfig>
#include <MailCommon/FolderCollectionMonitor>
#include <MessageComposer/AkonadiSender>

DummyKernel::DummyKernel(QObject* parent)
    : QObject(parent), mMessageSender(new MessageComposer::AkonadiSender(this)),
      mIdentityManager(new KIdentityManagementCore::IdentityManager(true, this)),
      mCollectionModel(new Akonadi::EntityMimeTypeFilterModel(this))
{
    auto session = new Akonadi::Session(QByteArrayLiteral("MailFilter Kernel ETM"), this);

    mFolderCollectionMonitor = new MailCommon::FolderCollectionMonitor(session, this);

    mEntityTreeModel = new Akonadi::EntityTreeModel(folderCollectionMonitor(), this);
    mEntityTreeModel->setListFilter(Akonadi::CollectionFetchScope::Enabled);
    mEntityTreeModel->setItemPopulationStrategy(Akonadi::EntityTreeModel::LazyPopulation);

    mCollectionModel->setSourceModel(mEntityTreeModel);
    mCollectionModel->addMimeTypeInclusionFilter(Akonadi::Collection::mimeType());
    mCollectionModel->setHeaderGroup(Akonadi::EntityTreeModel::CollectionTreeHeaders);
    mCollectionModel->setSortCaseSensitivity(Qt::CaseInsensitive);
}

KIdentityManagementCore::IdentityManager* DummyKernel::identityManager()
{
    return mIdentityManager;
}

MessageComposer::MessageSender* DummyKernel::msgSender()
{
    return mMessageSender;
}

Akonadi::EntityMimeTypeFilterModel* DummyKernel::collectionModel() const
{
    return mCollectionModel;
}

KSharedConfig::Ptr DummyKernel::config()
{
    return KSharedConfig::openConfig();
}

void DummyKernel::syncConfig()
{
    Q_ASSERT(false);
}

MailCommon::JobScheduler* DummyKernel::jobScheduler() const
{
    Q_ASSERT(false);
    return nullptr;
}

Akonadi::ChangeRecorder* DummyKernel::folderCollectionMonitor() const
{
    return mFolderCollectionMonitor->monitor();
}

void DummyKernel::updateSystemTray()
{
    Q_ASSERT(false);
}

bool DummyKernel::showPopupAfterDnD()
{
    return false;
}

qreal DummyKernel::closeToQuotaThreshold()
{
    return 80;
}

QStringList DummyKernel::customTemplates()
{
    Q_ASSERT(false);
    return {};
}

bool DummyKernel::excludeImportantMailFromExpiry()
{
    Q_ASSERT(false);
    return true;
}

Akonadi::Collection::Id DummyKernel::lastSelectedFolder()
{
    Q_ASSERT(false);
    return Akonadi::Collection::Id();
}

void DummyKernel::setLastSelectedFolder(Akonadi::Collection::Id col)
{
    Q_UNUSED(col)
}

void DummyKernel::expunge(Akonadi::Collection::Id id, bool sync)
{
    Akonadi::Collection col(id);
    if (col.isValid()) {
        mFolderCollectionMonitor->expunge(Akonadi::Collection(col), sync);
    }
}

#include "moc_dummykernel.cpp"
