/*
 * This file is part of KMail, the KDE mail client.
 * SPDX-FileCopyrightText: 2025 KMail AI Integration Team
 * SPDX-License-Identifier: GPL-2.0-only
 */

#pragma once

#include "aiserviceinterface.h"
#include <QObject>
#include <QPointer>
#include <memory>

namespace KMail {

/**
 * @brief Manager for AI services in KMail
 * 
 * This class manages the AI services used in KMail.
 * It provides a singleton instance and methods to access the AI services.
 */
class AIManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Get the singleton instance of the AI manager
     * @return The singleton instance
     */
    static AIManager *self();

    /**
     * Destroy the singleton instance
     */
    static void destroy();

    /**
     * Get the AI service
     * @return The AI service
     */
    AIServiceInterface *service() const;

    /**
     * Check if AI features are enabled
     * @return true if AI features are enabled, false otherwise
     */
    bool isEnabled() const;

    /**
     * Enable or disable AI features
     * @param enabled true to enable AI features, false to disable
     */
    void setEnabled(bool enabled);

Q_SIGNALS:
    /**
     * Emitted when the AI service is initialized
     * @param success true if initialization was successful, false otherwise
     */
    void initialized(bool success);

    /**
     * Emitted when the AI service is enabled or disabled
     * @param enabled true if AI features are enabled, false otherwise
     */
    void enabledChanged(bool enabled);

private:
    /**
     * Constructor
     * @param parent The parent object
     */
    explicit AIManager(QObject *parent = nullptr);

    /**
     * Destructor
     */
    ~AIManager() override;

    /**
     * Initialize the AI service
     * @return true if initialization was successful, false otherwise
     */
    bool initialize();

    /**
     * The singleton instance
     */
    static AIManager *s_self;

    /**
     * The AI service
     */
    std::unique_ptr<AIServiceInterface> m_service;

    /**
     * Flag indicating whether AI features are enabled
     */
    bool m_enabled;
};

} // namespace KMail
