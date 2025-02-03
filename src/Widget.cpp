#include "Widget.h"

Widget::Widget()
{
    grid = new QGridLayout(this);

    processes = new QTableWidget(this);
    processes->setColumnCount(3);
    QStringList columnLabels = {"PID", "Process Name", "Notes"};
    processes->setHorizontalHeaderLabels(columnLabels);
    processes->setMinimumWidth(1000);
    processes->setMinimumHeight(450);
    processes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    processes->setSelectionBehavior(QAbstractItemView::SelectRows);

    // need to add exception handling
    HANDLE captureProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);
    while(Process32Next(captureProcesses, &process) != FALSE)
    {
        processes->insertRow(processes->rowCount());
        QString pid = QString::number(process.th32ProcessID);
        QString pName = QString::fromWCharArray(process.szExeFile);
        processes->setItem(processes->rowCount()-1, 0, new QTableWidgetItem(pid));
        processes->setItem(processes->rowCount()-1, 1, new QTableWidgetItem(pName));
    }
    CloseHandle(captureProcesses);

    chooseFile = new QPushButton(this);
    chooseFile->setText("Choose File to Execute");

    file = new QString();

    analyzeButton = new QPushButton(this);
    analyzeButton->setText("Analyze");

    selectedFile = new QLabel(this);
    selectedFile->setText("Selected Process: No process selected");

    refreshProcesses = new QPushButton(this);
    refreshProcesses->setText("Refresh Table");

    grid->addWidget(processes);
    grid->addWidget(selectedFile);
    grid->addWidget(refreshProcesses);
    grid->addWidget(chooseFile);
    grid->addWidget(analyzeButton);

    connect(processes, SIGNAL(itemSelectionChanged()), this, SLOT(selectedProcessChanged()));
    connect(chooseFile, SIGNAL(clicked()), this, SLOT(chooseFileToExecute()));
    connect(analyzeButton, SIGNAL(clicked()), this, SLOT(analyzeFile()));
    connect(refreshProcesses, SIGNAL(clicked()), this, SLOT(refreshTable()));
}

void Widget::chooseFileToExecute()
{
    *file = QFileDialog::getOpenFileName(this, "Choose a File", "", "All Files (*)");
    selectedFile->setText("File to Execute: " + *file);
    // would proceed to execute and analyze file here
}

void Widget::analyzeFile()
{
    // function to be called when analyzing a file from the processes list
}

void Widget::selectedProcessChanged()
{
    QModelIndex index = processes->selectionModel()->currentIndex();
    QString rowNum = QString::number(index.row());
    QTableWidgetItem *processName = new QTableWidgetItem();
    processName = processes->item(index.row(), 1);
    selectedFile->setText("Selected Process: " + processName->text());
    processes->setCurrentCell(rowNum.toInt(), 0);
}

void Widget::refreshTable()
{
        processes->clearContents();
        processes->setRowCount(0);

        //same code as in initialization, create function for it
        HANDLE captureProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 process;
        process.dwSize = sizeof(PROCESSENTRY32);
        while(Process32Next(captureProcesses, &process) != FALSE)
        {
            processes->insertRow(processes->rowCount());
            QString pid = QString::number(process.th32ProcessID);
            QString pName = QString::fromWCharArray(process.szExeFile);
            processes->setItem(processes->rowCount()-1, 0, new QTableWidgetItem(pid));
            processes->setItem(processes->rowCount()-1, 1, new QTableWidgetItem(pName));
        }
        CloseHandle(captureProcesses);

        selectedFile->setText("Selected Process: No process selected");
}
