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

    signals:
        void analyzeNewFile(QString path, QString virtualSize, QString actualSize);

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
        QString processNameToAnalyze;
        QString pidToAnalyze;
};

#endif // WIDGET_H
