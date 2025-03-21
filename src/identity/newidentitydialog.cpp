/*
 *   kmail: KDE mail client
 *   SPDX-FileCopyrightText: 2000 Espen Sand <espen@kde.org>
 *   SPDX-FileCopyrightText: 2001-2003 Marc Mutz <mutz@kde.org>
 *   Contains code segments and ideas from earlier kmail dialog code.
 *   SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "newidentitydialog.h"

#include <KIdentityManagementCore/IdentityManager>
#include <KLineEditEventHandler>
#include <KLocalizedString>
#include <KSeparator>
#include <PimCommon/PimUtil>
#include <QComboBox>
#include <QLineEdit>

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

#include <cassert>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace KMail;

NewIdentityDialog::NewIdentityDialog(KIdentityManagementCore::IdentityManager* manager, QWidget* parent)
    : QDialog(parent), mIdentityManager(manager)
{
    setWindowTitle(i18nc("@title:window", "New Identity"));
    auto buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help, this);
    auto mainLayout = new QVBoxLayout(this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &NewIdentityDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &NewIdentityDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Help), &QPushButton::clicked, this, &NewIdentityDialog::slotHelp);

    auto page = new QWidget(this);
    mainLayout->addWidget(page);
    mainLayout->addWidget(buttonBox);
    auto vlay = new QVBoxLayout(page);
    vlay->setContentsMargins({});

    // row 0: line edit with label
    auto hlay = new QHBoxLayout(); // inherits spacing
    vlay->addLayout(hlay);
    mLineEdit = new QLineEdit(page);
    mLineEdit->setFocus();
    mLineEdit->setClearButtonEnabled(true);
    KLineEditEventHandler::catchReturnKey(mLineEdit);
    auto l = new QLabel(i18nc("@label:textbox", "&New identity:"), page);
    l->setBuddy(mLineEdit);
    hlay->addWidget(l);
    hlay->addWidget(mLineEdit, 1);
    connect(mLineEdit, &QLineEdit::textChanged, this, &NewIdentityDialog::slotEnableOK);

    mButtonGroup = new QButtonGroup(page);

    // row 1: radio button
    auto radio = new QRadioButton(i18nc("@option:radio", "&With empty fields"), page);
    radio->setChecked(true);
    vlay->addWidget(radio);
    mButtonGroup->addButton(radio, static_cast<int>(Empty));

    // row 2: radio button
    radio = new QRadioButton(i18nc("@option:radio", "&Use System Settings values"), page);
    vlay->addWidget(radio);
    mButtonGroup->addButton(radio, static_cast<int>(ControlCenter));

    // row 3: radio button
    radio = new QRadioButton(i18nc("@option:radio", "&Duplicate existing identity"), page);
    vlay->addWidget(radio);
    mButtonGroup->addButton(radio, static_cast<int>(ExistingEntry));

    // row 4: combobox with existing identities and label
    hlay = new QHBoxLayout(); // inherits spacing
    vlay->addLayout(hlay);
    mComboBox = new QComboBox(page);
    mComboBox->addItems(manager->shadowIdentities());
    mComboBox->setEnabled(false);
    auto label = new QLabel(i18nc("@label:textbox", "&Existing identities:"), page);
    label->setBuddy(mComboBox);
    label->setEnabled(false);
    hlay->addWidget(label);
    hlay->addWidget(mComboBox, 1);

    vlay->addWidget(new KSeparator(this));
    vlay->addStretch(1); // spacer

    // enable/disable combobox and label depending on the third radio
    // button's state:
    connect(radio, &QRadioButton::toggled, label, &QLabel::setEnabled);
    connect(radio, &QRadioButton::toggled, mComboBox, &QComboBox::setEnabled);

    mOkButton->setEnabled(false); // since line edit is empty

    resize(400, 180);
}

void NewIdentityDialog::slotHelp()
{
    PimCommon::Util::invokeHelp(QStringLiteral("kmail2/configure-identity.html"),
                                QStringLiteral("configure-identity-newidentitydialog"));
}

NewIdentityDialog::DuplicateMode NewIdentityDialog::duplicateMode() const
{
    const int id = mButtonGroup->checkedId();
    assert(id == static_cast<int>(Empty) || id == static_cast<int>(ControlCenter) ||
           id == static_cast<int>(ExistingEntry));
    return static_cast<DuplicateMode>(id);
}

void NewIdentityDialog::slotEnableOK(const QString& proposedIdentityName)
{
    // OK button is disabled if
    const QString name = proposedIdentityName.trimmed();
    // name isn't empty
    if (name.isEmpty()) {
        mOkButton->setEnabled(false);
        return;
    }
    // or name doesn't yet exist.
    if (!mIdentityManager->isUnique(name)) {
        mOkButton->setEnabled(false);
        return;
    }
    mOkButton->setEnabled(true);
}

QString NewIdentityDialog::identityName() const
{
    return mLineEdit->text();
}

QString NewIdentityDialog::duplicateIdentity() const
{
    return mComboBox->currentText();
}

#include "moc_newidentitydialog.cpp"
