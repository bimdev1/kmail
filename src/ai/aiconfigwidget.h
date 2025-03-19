/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;

namespace KMail {

class AIConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AIConfigWidget(QWidget *parent = nullptr);
    ~AIConfigWidget() override = default;

    void loadSettings();
    void saveSettings();

Q_SIGNALS:
    void configChanged();
    void testConnectionFinished(bool success, const QString &message);

private Q_SLOTS:
    void slotTestConnection();
    void slotApiKeyChanged();
    void updateTestButton();

private:
    void setupUi();
    void createConnections();

    QLineEdit *m_apiKeyEdit;
    QPushButton *m_testButton;
    QLabel *m_statusLabel;
    bool m_isValidating;
};

} // namespace KMail
