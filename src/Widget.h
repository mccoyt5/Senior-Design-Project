#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QGridLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <windows.h>
#include <tlhelp32.h>
#include "ConnectionsWindow.h"

class Widget: public QWidget
{
    Q_OBJECT

    public:
        Widget();

    signals:
        void analyzeNewFile(QString path, QString virtualSize, QString actualSize, std::vector<char*> dlls);
        void cantAnalyzeFile(QString name);
        void saveTable(QTableWidget *processes);

    public slots:
        int getProcessConnections(DWORD processID);
        void populateProcessTable();
        void chooseFileToExecute();
        void openConnectionsWindow();
        void analyzeFile();
        void selectedProcessChanged();
        void refreshTable();
        void sendTable();

    private:
        QPushButton *chooseFile;
        QPushButton *analyzeButton;
        QPushButton *openConnectionsButton;
        QLabel *selectedFile;
        QString *file;
        QTableWidget *processes;
        QGridLayout *grid;
        QPushButton *refreshProcesses;
        QString processNameToAnalyze;
        QString pidToAnalyze;
};

#endif // WIDGET_H
