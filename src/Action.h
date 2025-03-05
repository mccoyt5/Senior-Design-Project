#ifndef ACTION_H
#define ACTION_H

#include <QAction>
#include "HelpWindow.h"

class Action: public QAction
{
    Q_OBJECT

    using QAction::QAction;

    public slots:
        void help();
        void quit();
};

#endif // ACTION_H
