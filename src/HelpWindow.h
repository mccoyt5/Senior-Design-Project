#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QDialog>
#include <QLabel>

class HelpWindow: public QDialog
{
    Q_OBJECT

    public:
        HelpWindow();

    private:
        QLabel *faq;
};

#endif // HELPWINDOW_H
