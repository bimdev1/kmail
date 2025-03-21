/*
 * SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "incompleteindexdialog.h"
#include "kmail_debug.h"
#include "kmkernel.h"
#include "ui_incompleteindexdialog.h"

#include <KDescendantsProxyModel>
#include <KLocalizedString>
#include <QAbstractItemView>
#include <QProgressDialog>

#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/EntityTreeModel>

#include <PimCommon/PimUtil>
#include <PimCommonAkonadi/MailUtil>

#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowConfig>
#include <chrono>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QTimer>
#include <QWindow>

using namespace std::chrono_literals;
Q_DECLARE_METATYPE(Qt::CheckState)
Q_DECLARE_METATYPE(QList<qint64>)

class SearchCollectionProxyModel : public QSortFilterProxyModel
{
public:
    explicit SearchCollectionProxyModel(const QList<qint64>& unindexedCollections, QObject* parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        mFilterCollections.reserve(unindexedCollections.size());
        for (qint64 col : unindexedCollections) {
            mFilterCollections.insert(col, true);
        }
    }

    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override
    {
        if (role == Qt::CheckStateRole) {
            if (index.isValid() && index.column() == 0) {
                const qint64 colId = collectionIdForIndex(index);
                return mFilterCollections.value(colId) ? Qt::Checked : Qt::Unchecked;
            }
        }

        return QSortFilterProxyModel::data(index, role);
    }

    bool setData(const QModelIndex& index, const QVariant& data, int role) override
    {
        if (role == Qt::CheckStateRole) {
            if (index.isValid() && index.column() == 0) {
                const qint64 colId = collectionIdForIndex(index);
                mFilterCollections[colId] = data.value<Qt::CheckState>();
                return true;
            }
        }

        return QSortFilterProxyModel::setData(index, data, role);
    }

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        if (index.isValid() && index.column() == 0) {
            return QSortFilterProxyModel::flags(index) | Qt::ItemIsUserCheckable;
        } else {
            return QSortFilterProxyModel::flags(index);
        }
    }

protected:
    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override
    {
        const QModelIndex source_idx = sourceModel()->index(source_row, 0, source_parent);
        const qint64 colId = sourceModel()->data(source_idx, Akonadi::EntityTreeModel::CollectionIdRole).toLongLong();
        return mFilterCollections.contains(colId);
    }

private:
    [[nodiscard]] qint64 collectionIdForIndex(const QModelIndex& index) const
    {
        return data(index, Akonadi::EntityTreeModel::CollectionIdRole).toLongLong();
    }

private:
    QHash<qint64, bool> mFilterCollections;
};

namespace {
static const char myIncompleteIndexDialogGroupName[] = "IncompleteIndexDialog";
}

IncompleteIndexDialog::IncompleteIndexDialog(const QList<qint64>& unindexedCollections, QWidget* parent)
    : QDialog(parent), mUi(new Ui::IncompleteIndexDialog)
{
    auto mainLayout = new QHBoxLayout(this);
    auto w = new QWidget(this);
    mainLayout->addWidget(w);
    qDBusRegisterMetaType<QList<qint64>>();

    mUi->setupUi(w);

    Akonadi::EntityTreeModel* etm = KMKernel::self()->entityTreeModel();
    auto mimeProxy = new Akonadi::EntityMimeTypeFilterModel(this);
    mimeProxy->addMimeTypeInclusionFilter(Akonadi::Collection::mimeType());
    mimeProxy->setSourceModel(etm);

    auto flatProxy = new KDescendantsProxyModel(this);
    flatProxy->setDisplayAncestorData(true);
    flatProxy->setAncestorSeparator(QStringLiteral(" / "));
    flatProxy->setSourceModel(mimeProxy);

    auto proxy = new SearchCollectionProxyModel(unindexedCollections, this);
    proxy->setSourceModel(flatProxy);

    mUi->collectionView->setModel(proxy);

    mUi->collectionView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(mUi->selectAllBtn, &QPushButton::clicked, this, &IncompleteIndexDialog::selectAll);
    connect(mUi->unselectAllBtn, &QPushButton::clicked, this, &IncompleteIndexDialog::unselectAll);
    mUi->buttonBox->button(QDialogButtonBox::Ok)->setText(i18n("Reindex"));
    mUi->buttonBox->button(QDialogButtonBox::Cancel)->setText(i18nc("@action:button", "Search Anyway"));
    connect(mUi->buttonBox, &QDialogButtonBox::accepted, this, &IncompleteIndexDialog::waitForIndexer);
    connect(mUi->buttonBox, &QDialogButtonBox::rejected, this, &IncompleteIndexDialog::reject);
    readConfig();
}

IncompleteIndexDialog::~IncompleteIndexDialog()
{
    writeConfig();
}

void IncompleteIndexDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(500, 400));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myIncompleteIndexDialogGroupName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void IncompleteIndexDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myIncompleteIndexDialogGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
    group.sync();
}

void IncompleteIndexDialog::selectAll()
{
    updateAllSelection(true);
}

void IncompleteIndexDialog::unselectAll()
{
    updateAllSelection(false);
}

void IncompleteIndexDialog::updateAllSelection(bool select)
{
    QAbstractItemModel* model = mUi->collectionView->model();
    for (int i = 0, cnt = model->rowCount(); i < cnt; ++i) {
        const QModelIndex idx = model->index(i, 0, QModelIndex());
        model->setData(idx, select ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
    }
}

QList<qlonglong> IncompleteIndexDialog::collectionsToReindex() const
{
    QList<qlonglong> res;

    QAbstractItemModel* model = mUi->collectionView->model();
    for (int i = 0, cnt = model->rowCount(); i < cnt; ++i) {
        const QModelIndex idx = model->index(i, 0, QModelIndex());
        if (model->data(idx, Qt::CheckStateRole).toInt() == Qt::Checked) {
            res.push_back(model->data(idx, Akonadi::EntityTreeModel::CollectionRole).value<Akonadi::Collection>().id());
        }
    }

    return res;
}

void IncompleteIndexDialog::waitForIndexer()
{
    mIndexer =
        new QDBusInterface(PimCommon::MailUtil::indexerServiceName(), QStringLiteral("/"),
                           QStringLiteral("org.freedesktop.Akonadi.Indexer"), QDBusConnection::sessionBus(), this);

    if (!mIndexer->isValid()) {
        qCWarning(KMAIL_LOG) << "Invalid indexer dbus interface ";
        accept();
        return;
    }
    mIndexingQueue = collectionsToReindex();
    if (mIndexingQueue.isEmpty()) {
        accept();
        return;
    }

    mProgressDialog = new QProgressDialog(this);
    mProgressDialog->setWindowTitle(i18nc("@title:window", "Indexing"));
    mProgressDialog->setMaximum(mIndexingQueue.size());
    mProgressDialog->setValue(0);
    mProgressDialog->setLabelText(i18n("Indexing Collections…"));
    connect(mProgressDialog, &QDialog::rejected, this, &IncompleteIndexDialog::slotStopIndexing);

    connect(mIndexer, SIGNAL(collectionIndexingFinished(qlonglong)), this,
            SLOT(slotCurrentlyIndexingCollectionChanged(qlonglong)));

    mIndexer->asyncCall(QStringLiteral("reindexCollections"), QVariant::fromValue(mIndexingQueue));
    mProgressDialog->show();
}

void IncompleteIndexDialog::slotStopIndexing()
{
    mProgressDialog->close();
    reject();
}

void IncompleteIndexDialog::slotCurrentlyIndexingCollectionChanged(qlonglong colId)
{
    const int idx = mIndexingQueue.indexOf(colId);
    if (idx > -1) {
        mIndexingQueue.removeAt(idx);
        mProgressDialog->setValue(mProgressDialog->maximum() - mIndexingQueue.size());

        if (mIndexingQueue.isEmpty()) {
            QTimer::singleShot(1s, this, &IncompleteIndexDialog::accept);
        }
    }
}

#include "moc_incompleteindexdialog.cpp"
