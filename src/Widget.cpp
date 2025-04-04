#include "Widget.h"
#include <QMessageBox>

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
        QTableWidgetItem *processID = new QTableWidgetItem(pid);
        processID->setFlags(processID->flags() & ~Qt::ItemIsEditable);
        processes->setItem(processes->rowCount()-1, 0, processID);
        QTableWidgetItem *processName = new QTableWidgetItem(pName);
        processName->setFlags(processName->flags() & ~Qt::ItemIsEditable);
        processes->setItem(processes->rowCount()-1, 1, processName);
    }
    CloseHandle(captureProcesses);

    chooseFile = new QPushButton(this);
    chooseFile->setText("Choose File to Execute");

    file = new QString();

    analyzeButton = new QPushButton(this);
    analyzeButton->setText("Analyze Selected Process");

    openConnectionsButton = new QPushButton(this);
    openConnectionsButton->setText("Open Process Connections");

    selectedFile = new QLabel(this);
    selectedFile->setText("Selected Process: No process selected");

    refreshProcesses = new QPushButton(this);
    refreshProcesses->setText("Refresh Table");

    processNameToAnalyze = "";
    pidToAnalyze = "";

    grid->addWidget(processes);
    grid->addWidget(selectedFile);
    grid->addWidget(analyzeButton);
    grid->addWidget(openConnectionsButton);
    grid->addWidget(refreshProcesses);
    grid->addWidget(chooseFile);

    connect(processes, SIGNAL(itemSelectionChanged()), this, SLOT(selectedProcessChanged()));
    connect(chooseFile, SIGNAL(clicked()), this, SLOT(chooseFileToExecute()));
    connect(analyzeButton, SIGNAL(clicked()), this, SLOT(analyzeFile()));
    connect(openConnectionsButton, SIGNAL(clicked()), this, SLOT(openConnectionsWindow()));
    connect(refreshProcesses, SIGNAL(clicked()), this, SLOT(refreshTable()));
}

void Widget::chooseFileToExecute()
{
    *file = QFileDialog::getOpenFileName(this, "Choose a File", "", "Executable files (*.exe)");
    selectedFile->setText("File to Execute: " + *file);
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if(CreateProcessA(file->toStdString().c_str(), NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        refreshTable();
    }
    else
    {
        QMessageBox::warning(this, "Warning", "Can't execute selected file.");
    }
}

void Widget::openConnectionsWindow()
{
    if (pidToAnalyze.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a process first.");
        return;
    }

    // Create a new connections window with the selected PID
    ConnectionsWindow *connectionsWindow = new ConnectionsWindow(pidToAnalyze);

    // Set window properties
    connectionsWindow->setWindowTitle("Process Connections - " + processNameToAnalyze);

    // Make it a separate window rather than embedded
    connectionsWindow->setWindowFlags(Qt::Window);

    // Show the window
    connectionsWindow->show();
}

void Widget::analyzeFile()
{
    bool stillRunning = false;
    HANDLE captureProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);
    while(Process32Next(captureProcesses, &process) != FALSE)
    {
        QString pid = QString::number(process.th32ProcessID);
        QString pName = QString::fromWCharArray(process.szExeFile);
        if(pid == pidToAnalyze && pName == processNameToAnalyze)
        {
            stillRunning = true;
            break;
        }
    }
    CloseHandle(captureProcesses);

    if(stillRunning)
    {
        HANDLE captureProcess = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, pidToAnalyze.toInt());
        MODULEENTRY32 module;
        module.dwSize = sizeof(MODULEENTRY32);
        Module32First(captureProcess, &module);
        CloseHandle(captureProcess);

        HANDLE pe = CreateFileW(LPCWSTR(module.szExePath), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        HANDLE fileMapping = CreateFileMapping(pe, NULL, PAGE_READONLY, 0, 0, NULL);
        CloseHandle(pe);
        LPVOID fileBase = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
        CloseHandle(fileMapping);
        PIMAGE_DOS_HEADER pimage_dos_header = (PIMAGE_DOS_HEADER)fileBase;
        if(pimage_dos_header != nullptr)
        {
            PIMAGE_NT_HEADERS pimage_nt_header = (PIMAGE_NT_HEADERS)((u_char*)pimage_dos_header+pimage_dos_header->e_lfanew);
            PIMAGE_SECTION_HEADER textSectionHeader = (PIMAGE_SECTION_HEADER)((uintptr_t)pimage_nt_header + sizeof(DWORD) + (uintptr_t)(sizeof(IMAGE_FILE_HEADER)) + (uintptr_t)pimage_nt_header->FileHeader.SizeOfOptionalHeader);
            int remainingSections = pimage_nt_header->FileHeader.NumberOfSections;
            while(true)
            {
                BYTE text[8] = {'.','t','e','x','t'};
                if(memcmp(textSectionHeader->Name, text, 8) == 0)
                {
                    break;
                }
                else if(remainingSections == 1)
                {
                    std::vector<char*> dlls;
                    emit analyzeNewFile(QString::fromWCharArray(module.szExePath), "No text section", "No text section", dlls);
                }
                else
                {
                    textSectionHeader = textSectionHeader + 1;
                    remainingSections--;
                }
            }

            DWORD virtualSize = textSectionHeader->Misc.VirtualSize;
            DWORD actualSize = textSectionHeader->SizeOfRawData;

            PIMAGE_SECTION_HEADER sectionHoldingImports = (PIMAGE_SECTION_HEADER)((uintptr_t)pimage_nt_header + sizeof(DWORD) + (uintptr_t)(sizeof(IMAGE_FILE_HEADER)) + (uintptr_t)pimage_nt_header->FileHeader.SizeOfOptionalHeader);
            remainingSections = pimage_nt_header->FileHeader.NumberOfSections;
            while(true)
            {
                if(sectionHoldingImports->VirtualAddress <= pimage_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress && (sectionHoldingImports->VirtualAddress + sectionHoldingImports->Misc.VirtualSize) > pimage_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)
                {
                    break;
                }
                else if(remainingSections == 1)
                {
                    std::vector<char*> dlls;
                    emit analyzeNewFile(QString::fromWCharArray(module.szExePath), QString::number(virtualSize), QString::number(actualSize), dlls);
                }
                else
                {
                    sectionHoldingImports = sectionHoldingImports + 1;
                    remainingSections--;
                }
            }

            PIMAGE_IMPORT_DESCRIPTOR pimage_import_descriptor = (PIMAGE_IMPORT_DESCRIPTOR)((uintptr_t)fileBase + sectionHoldingImports->PointerToRawData + pimage_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress - sectionHoldingImports->VirtualAddress);
            std::vector<char*> dlls;
            while(pimage_import_descriptor->Name != 0)
            {
                char* dllName = (char*)((uintptr_t)fileBase + sectionHoldingImports->PointerToRawData + pimage_import_descriptor->Name - sectionHoldingImports->VirtualAddress);
                dlls.push_back(dllName);
                pimage_import_descriptor += 1;
            }
            emit analyzeNewFile(QString::fromWCharArray(module.szExePath), QString::number(virtualSize), QString::number(actualSize), dlls);
        }
        else
        {
            emit cantAnalyzeFile(QString::fromWCharArray(process.szExeFile));
        }
    }
}

void Widget::selectedProcessChanged()
{
    QModelIndex index = processes->selectionModel()->currentIndex();
    QString rowNum = QString::number(index.row());
    QTableWidgetItem *processName = new QTableWidgetItem();
    processName = processes->item(index.row(), 1);
    selectedFile->setText("Selected Process: " + processName->text());
    processes->setCurrentCell(rowNum.toInt(), 0);
    processNameToAnalyze = processName->text();
    QTableWidgetItem *processID = new QTableWidgetItem();
    processID = processes->item(index.row(), 0);
    pidToAnalyze = processID->text();
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
            QTableWidgetItem *processID = new QTableWidgetItem(pid);
            processID->setFlags(processID->flags() & ~Qt::ItemIsEditable);
            processes->setItem(processes->rowCount()-1, 0, processID);
            QTableWidgetItem *processName = new QTableWidgetItem(pName);
            processName->setFlags(processName->flags() & ~Qt::ItemIsEditable);
            processes->setItem(processes->rowCount()-1, 1, processName);
        }
        CloseHandle(captureProcesses);

        selectedFile->setText("Selected Process: No process selected");
}

void Widget::sendTable()
{
    emit saveTable(processes);
}
