// Add the implementation of slotUpdateAIActions to KMMainWidget.cpp
void KMMainWidget::slotUpdateAIActions()
{
    if (m_aiExtension) {
        m_aiExtension->updateActions(currentSelection());
    }
}

// Add the signal connection to the KMMainWidget constructor
// Add after the line "readConfig();"

// Initialize AI extension
KMail::initializeAIExtension(this, actionCollection);

// Connect AI signals
connect(this, &KMMainWidget::selectionChanged, this, &KMMainWidget::slotUpdateAIActions);
