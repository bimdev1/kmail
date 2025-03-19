// Add to KMMainWidget::slotMessageSelected in kmmainwidget.cpp
// Add at the end of the method before the closing brace

    // Notify AI extension about selection change
    Q_EMIT selectionChanged();
