#!/bin/bash

# Function to apply a patch to kmmainwidget.cpp
apply_patch() {
    local patch_file=$1
    local temp_file=$(mktemp)
    
    # Create backup
    cp ../kmmainwidget.cpp ../kmmainwidget.cpp.bak
    
    # Read the patch file
    while IFS= read -r line; do
        if [[ $line == "// Add to kmmainwidget.cpp after the includes" ]]; then
            # Find the last include and add new includes after it
            sed -n '/^#include/p' ../kmmainwidget.cpp > "$temp_file"
            echo "#include \"ai/kmainwidgetaiintegration.h\"" >> "$temp_file"
            echo "#include \"ai/aimainwidgetextension.h\"" >> "$temp_file"
            sed '1,/^#include.*$/d' ../kmmainwidget.cpp >> "$temp_file"
            mv "$temp_file" ../kmmainwidget.cpp
        elif [[ $line == "// Add member variable to KMMainWidget class private section" ]]; then
            # Add member variable to private section
            sed -i '/private:/a \    AIMainWidgetExtension* m_aiExtension;' ../kmmainwidget.cpp
        elif [[ $line == "// Add to KMMainWidget constructor initialization list" ]]; then
            # Add to constructor initialization list
            sed -i '/KMMainWidget::KMMainWidget.*:/a \    , m_aiExtension(nullptr)' ../kmmainwidget.cpp
        elif [[ $line == "// Add to KMMainWidget constructor body after readConfig();" ]]; then
            # Add after readConfig()
            sed -i '/readConfig()/a \    KMail::initializeAIExtension(this, actionCollection());' ../kmmainwidget.cpp
        elif [[ $line == "// Add to KMMainWidget destructor" ]]; then
            # Add to destructor
            sed -i '/KMMainWidget::~KMMainWidget()/a \    KMail::cleanupAIExtension(this);' ../kmmainwidget.cpp
        elif [[ $line == "// Add implementation of slotUpdateAIActions" ]]; then
            # Add new method implementation
            cat >> ../kmmainwidget.cpp << 'EOL'

void KMMainWidget::slotUpdateAIActions()
{
    if (m_aiExtension) {
        m_aiExtension->updateActions(currentSelection());
    }
}
EOL
        elif [[ $line == "// Add signal connection in KMMainWidget constructor after initializing AI extension" ]]; then
            # Add signal connection
            sed -i '/initializeAIExtension/a \    connect(this, \&KMMainWidget::selectionChanged, this, \&KMMainWidget::slotUpdateAIActions);' ../kmmainwidget.cpp
        elif [[ $line == "// Add to KMMainWidget::slotMessageSelected at the end" ]]; then
            # Add to slotMessageSelected
            sed -i '/void KMMainWidget::slotMessageSelected/,/^}/ s/^}$/    Q_EMIT selectionChanged();\n}/' ../kmmainwidget.cpp
        fi
    done < "$patch_file"
}

# Apply the patches
apply_patch "kmainwidget_ai_patch5.cpp"

echo "Patches applied successfully!"
