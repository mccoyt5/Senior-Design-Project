#include "MainWindow.h"

MainWindow::MainWindow()
{
    Widget *mainMenu = new Widget();
    this->setCentralWidget(mainMenu);
    DockWidget *analyzedFile = new DockWidget();
    QScrollArea *scrollArea = new QScrollArea(analyzedFile);
    AnalysisWidget *analysis = new AnalysisWidget();
    analyzedFile->setWidget(analysis);
    scrollArea->setWidget(analysis);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(500);
    scrollArea->setFixedHeight(600);
    analyzedFile->setFixedWidth(500);
    analyzedFile->setFixedHeight(600);
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
    connect(mainMenu, &Widget::cantAnalyzeFile, analysis, &AnalysisWidget::updateDockError);
    connect(saveTableAction, &Action::getTable, mainMenu, &Widget::sendTable);
    connect(mainMenu, &Widget::saveTable, saveTableAction, &Action::processTable);
}
