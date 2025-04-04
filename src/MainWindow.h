#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Widget.h"
#include <QToolBar>
#include "Action.h"
#include "DockWidget.h"
#include "AnalysisWidget.h"
#include <QScrollArea>

class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();
};

#endif // MAINWINDOW_H
