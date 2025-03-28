#ifndef ACTION_H
#define ACTION_H

#include <QAction>
#include <QTableWidget>
#include <QFileDialog>
#include "HelpWindow.h"

class Action: public QAction
{
    Q_OBJECT

    using QAction::QAction;

    signals:
        void getTable();

    public slots:
        void saveTable();
        void processTable(QTableWidget *processes);
        void help();
        void quit();
};

#endif // ACTION_H
