#ifndef ACTION_H
#define ACTION_H

#include <QAction>

class Action: public QAction
{
    Q_OBJECT

    using QAction::QAction;

    public slots:
        void quit();
};

#endif // ACTION_H
