/*
 *   kmail: KDE mail client
 *   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "configuredialoglistview.h"

#include <KLocalizedString>
#include <QMenu>

ListView::ListView(QWidget* parent) : QTreeWidget(parent)
{
    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setRootIsDecorated(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &ListView::customContextMenuRequested, this, &ListView::slotContextMenu);
}

void ListView::resizeEvent(QResizeEvent* e)
{
    QTreeWidget::resizeEvent(e);
    resizeColums();
}

void ListView::showEvent(QShowEvent* e)
{
    QTreeWidget::showEvent(e);
    resizeColums();
}

void ListView::resizeColums()
{
    const int c = columnCount();
    if (c == 0) {
        return;
    }

    const int w1 = viewport()->width();
    const int w2 = w1 / c;
    const int w3 = w1 - (c - 1) * w2;

    for (int i = 0; i < c - 1; ++i) {
        setColumnWidth(i, w2);
    }
    setColumnWidth(c - 1, w3);
}

void ListView::slotContextMenu(QPoint pos)
{
    QMenu menu(this);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18nc("@action", "Add"), this, &ListView::addHeader);
    if (currentItem()) {
        menu.addAction(QIcon::fromTheme(QStringLiteral("list-remove")), i18nc("@action", "Remove"), this,
                       &ListView::removeHeader);
    }
    menu.exec(viewport()->mapToGlobal(pos));
}

#include "moc_configuredialoglistview.cpp"
