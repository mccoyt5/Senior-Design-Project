#ifdef QT_NO_DEBUG_OUTPUT
#undef QT_NO_DEBUG_OUTPUT
#endif

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);
    MainWindow *window = new MainWindow();
    window->show();
    return app.exec();
}
