#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QGridLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <windows.h>
#include <tlhelp32.h>

class Widget: public QWidget
{
    Q_OBJECT

    public:
        Widget();

    public slots:
        void chooseFileToExecute();
        void analyzeFile();
        void selectedProcessChanged();
        void refreshTable();

    private:
        QPushButton *chooseFile;
        QPushButton *analyzeButton;
        QLabel *selectedFile;
        QString *file;
        QTableWidget *processes;
        QGridLayout *grid;
        QPushButton *refreshProcesses;
        //QTableWidgetItem *processName;
};

#endif // WIDGET_H
