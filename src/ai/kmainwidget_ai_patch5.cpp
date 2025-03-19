// Add to kmmainwidget.cpp after the includes
#include "ai/kmainwidgetaiintegration.h"
#include "ai/aimainwidgetextension.h"

// Add member variable to KMMainWidget class private section
AIMainWidgetExtension* m_aiExtension;

// Add to KMMainWidget constructor initialization list
, m_aiExtension(nullptr)

// Add to KMMainWidget constructor body after readConfig();
KMail::initializeAIExtension(this, actionCollection());

// Add to KMMainWidget destructor
KMail::cleanupAIExtension(this);

// Add implementation of slotUpdateAIActions
void KMMainWidget::slotUpdateAIActions()
{
    if (m_aiExtension) {
        m_aiExtension->updateActions(currentSelection());
    }
}

// Add signal connection in KMMainWidget constructor after initializing AI extension
connect(this, &KMMainWidget::selectionChanged,
        this, &KMMainWidget::slotUpdateAIActions);

// Add to KMMainWidget::slotMessageSelected at the end
Q_EMIT selectionChanged();
