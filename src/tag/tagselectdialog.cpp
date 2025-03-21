/*
   SPDX-FileCopyrightText: 2011-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tagselectdialog.h"

#include "kmail_debug.h"

#include <MailCommon/AddTagDialog>

#include <KListWidgetSearchLine>
#include <KLocalizedString>
#include <QIcon>

#include <Akonadi/TagAttribute>
#include <Akonadi/TagFetchJob>
#include <Akonadi/TagFetchScope>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>
using namespace Qt::Literals::StringLiterals;

namespace {
static const char myTagSelectDialogGroupName[] = "TagSelectDialog";
}

TagSelectDialog::TagSelectDialog(QWidget* parent, int numberOfSelectedMessages, const Akonadi::Item& selectedItem)
    : QDialog(parent), mNumberOfSelectedMessages(numberOfSelectedMessages), mSelectedItem(selectedItem),
      mListTag(new QListWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Select Tags"));
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    auto user1Button = new QPushButton;
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &TagSelectDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &TagSelectDialog::reject);
    user1Button->setText(i18n("Add New Tag…"));
    setModal(true);

    auto mainWidget = new QWidget(this);
    mainLayout->addWidget(mainWidget);
    mainLayout->addWidget(buttonBox);

    auto vbox = new QVBoxLayout;
    mainWidget->setLayout(vbox);
    vbox->setContentsMargins({});
    mListTag->setObjectName("listtag"_L1);
    auto listWidgetSearchLine = new KListWidgetSearchLine(this, mListTag);
    listWidgetSearchLine->setObjectName("searchline"_L1);

    listWidgetSearchLine->setPlaceholderText(i18nc("@info:placeholder", "Search tag…"));
    listWidgetSearchLine->setClearButtonEnabled(true);

    vbox->addWidget(listWidgetSearchLine);
    vbox->addWidget(mListTag);

    createTagList(false);
    connect(user1Button, &QPushButton::clicked, this, &TagSelectDialog::slotAddNewTag);

    readConfig();
}

TagSelectDialog::~TagSelectDialog()
{
    writeConfig();
}

void TagSelectDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(500, 300));
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myTagSelectDialogGroupName));
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void TagSelectDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myTagSelectDialogGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
}

void TagSelectDialog::slotAddNewTag()
{
    QPointer<MailCommon::AddTagDialog> dialog = new MailCommon::AddTagDialog(mActionCollectionList, this);
    dialog->setTags(mTagList);
    if (dialog->exec()) {
        mCurrentSelectedTags = selectedTag();
        mListTag->clear();
        mTagList.clear();
        createTagList(true);
    }
    delete dialog;
}

void TagSelectDialog::createTagList(bool updateList)
{
    auto fetchJob = new Akonadi::TagFetchJob(this);
    fetchJob->setProperty("updatelist", updateList);
    fetchJob->fetchScope().fetchAttribute<Akonadi::TagAttribute>();
    connect(fetchJob, &Akonadi::TagFetchJob::result, this, &TagSelectDialog::slotTagsFetched);
}

void TagSelectDialog::setActionCollection(const QList<KActionCollection*>& actionCollectionList)
{
    mActionCollectionList = actionCollectionList;
}

void TagSelectDialog::slotTagsFetched(KJob* job)
{
    if (job->error()) {
        qCWarning(KMAIL_LOG) << "Failed to load tags " << job->errorString();
        return;
    }
    auto fetchJob = static_cast<Akonadi::TagFetchJob*>(job);
    bool updatelist = fetchJob->property("updatelist").toBool();

    const Akonadi::Tag::List lstTags = fetchJob->tags();
    for (const Akonadi::Tag& akonadiTag : lstTags) {
        mTagList.append(MailCommon::Tag::fromAkonadi(akonadiTag));
    }

    std::sort(mTagList.begin(), mTagList.end(), MailCommon::Tag::compare);

    for (const MailCommon::Tag::Ptr& tag : std::as_const(mTagList)) {
        auto item = new QListWidgetItem(QIcon::fromTheme(tag->iconName), tag->tagName, mListTag);
        item->setData(UrlTag, tag->tag().url().url());
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked);
        mListTag->addItem(item);

        if (updatelist) {
            const bool select = mCurrentSelectedTags.contains(tag->tag());
            item->setCheckState(select ? Qt::Checked : Qt::Unchecked);
        } else {
            if (mNumberOfSelectedMessages == 1) {
                const bool hasTag = mSelectedItem.hasTag(tag->tag());
                item->setCheckState(hasTag ? Qt::Checked : Qt::Unchecked);
            } else {
                item->setCheckState(Qt::Unchecked);
            }
        }
    }
}

Akonadi::Tag::List TagSelectDialog::selectedTag() const
{
    Akonadi::Tag::List lst;
    const int numberOfItems(mListTag->count());
    for (int i = 0; i < numberOfItems; ++i) {
        QListWidgetItem* item = mListTag->item(i);
        if (item->checkState() == Qt::Checked) {
            lst.append(Akonadi::Tag::fromUrl(QUrl(item->data(UrlTag).toString())));
        }
    }
    return lst;
}

#include "moc_tagselectdialog.cpp"
