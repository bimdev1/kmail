// Add to KMMainWidget.h in the Q_SIGNALS: section
// Add after the line "void recreateGui();"

    /**
     * Emitted when the selection in the message list changes.
     * Used by the AI extension to update its actions.
     */
    void selectionChanged();
