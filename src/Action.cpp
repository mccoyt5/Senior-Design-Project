#include "Action.h"

void Action::help()
{
    HelpWindow *helpWindow = new HelpWindow();
    helpWindow->setMinimumWidth(1000);
    helpWindow->setMinimumHeight(700);
    helpWindow->exec();
}

void Action::quit()
{
    exit(0);
}
