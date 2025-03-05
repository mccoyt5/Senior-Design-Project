#include "DockWidget.h"

DockWidget::DockWidget()
{
    this->setFeatures(this->features() & ~QDockWidget::DockWidgetClosable);
}
