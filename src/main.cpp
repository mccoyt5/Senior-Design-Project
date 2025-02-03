#ifdef QT_NO_DEBUG_OUTPUT
#undef QT_NO_DEBUG_OUTPUT
#endif

#include <QApplication>
#include <QMainWindow>
#include "Widget.h"
#include <QToolBar>
#include "Action.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);
    QMainWindow *window = new QMainWindow();
    Widget *mainMenu = new Widget();
    window->setCentralWidget(mainMenu);
    mainMenu->setMinimumWidth(1000);
    mainMenu->setMinimumHeight(500);
    QToolBar *toolBar = new QToolBar();
    QString fileButton = "File";
    QString windowButton = "Window";
    QString helpButton = "Help";
    Action *quitAction = new Action("Quit");
    Action::connect(quitAction, SIGNAL(triggered()), quitAction, SLOT(quit()));
    toolBar->addAction(fileButton);
    toolBar->addAction(windowButton);
    toolBar->addAction(helpButton);
    toolBar->addAction(quitAction);
    window->addToolBar(toolBar);
    window->show();
    return app.exec();
}
