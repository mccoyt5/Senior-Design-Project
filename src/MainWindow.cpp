#include "MainWindow.h"

MainWindow::MainWindow()
{
    Widget *mainMenu = new Widget();
    this->setCentralWidget(mainMenu);
    DockWidget *analyzedFile = new DockWidget();
    AnalysisWidget *analysis = new AnalysisWidget();
    analyzedFile->setWidget(analysis);
    this->addDockWidget(Qt::RightDockWidgetArea, analyzedFile);
    mainMenu->setMinimumWidth(1000);
    mainMenu->setMinimumHeight(500);

    // Create toolbar
    QToolBar *toolBar = new QToolBar();

    // Save Table Action
    Action *saveTableAction = new Action("Save Table");
    Action::connect(saveTableAction, SIGNAL(triggered()), saveTableAction, SLOT(saveTable()));

    // Help Action
    Action *helpAction = new Action("Help");
    Action::connect(helpAction, SIGNAL(triggered()), helpAction, SLOT(help()));

    // Quit Action
    Action *quitAction = new Action("Quit");
    Action::connect(quitAction, SIGNAL(triggered()), quitAction, SLOT(quit()));

    // Add all actions to toolbar
    toolBar->addAction(saveTableAction);
    toolBar->addAction(helpAction);
    toolBar->addAction(quitAction);

    this->addToolBar(toolBar);

    connect(mainMenu, &Widget::analyzeNewFile, analysis, &AnalysisWidget::updateDock);
    connect(saveTableAction, &Action::getTable, mainMenu, &Widget::sendTable);
    connect(mainMenu, &Widget::saveTable, saveTableAction, &Action::processTable);
}
