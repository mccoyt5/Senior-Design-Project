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

    // Help Action
    Action *helpAction = new Action("Help");
    Action::connect(helpAction, SIGNAL(triggered()), helpAction, SLOT(help()));

    // Quit Action
    Action *quitAction = new Action("Quit");
    Action::connect(quitAction, SIGNAL(triggered()), quitAction, SLOT(quit()));

    // Add all actions to toolbar
    toolBar->addAction(helpAction);
    toolBar->addAction(quitAction);

    this->addToolBar(toolBar);

    connect(mainMenu, &Widget::analyzeNewFile, analysis, &AnalysisWidget::updateDock);
}
