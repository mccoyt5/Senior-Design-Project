#include "Widget.h"

Widget::Widget()
{
    grid = new QGridLayout(this);

    processes = new QTableWidget(this);
    processes->setColumnCount(4);
    QStringList columnLabels = {"PID", "Process Name", "Connections", "Notes"};
    processes->setHorizontalHeaderLabels(columnLabels);
    processes->setMinimumWidth(1000);
    processes->setMinimumHeight(450);
    processes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    processes->setSelectionBehavior(QAbstractItemView::SelectRows);

    populateProcessTable();

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

void Widget::populateProcessTable()
{
    // Start fresh
    processes->clearContents();
    processes->setRowCount(0);

    HANDLE captureProcesses = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (captureProcesses == INVALID_HANDLE_VALUE)
    {
        QMessageBox::critical(this, "Error", "Something went wrong when getting running processes");
        // Exit on OK?
    }

    PROCESSENTRY32 process;
    process.dwSize = sizeof(PROCESSENTRY32);

    // Get first process
    if (Process32First(captureProcesses, &process))
    {
        do {
            processes->insertRow(processes->rowCount());
            QString pid = QString::number(process.th32ProcessID);
            QString pName = QString::fromWCharArray(process.szExeFile);

            // PID Column
            QTableWidgetItem *processID = new QTableWidgetItem(pid);
            processID->setFlags(processID->flags() & ~Qt::ItemIsEditable);
            processes->setItem(processes->rowCount()-1, 0, processID);

            // Process Name Column
            QTableWidgetItem *processName = new QTableWidgetItem(pName);
            processName->setFlags(processName->flags() & ~Qt::ItemIsEditable);
            processes->setItem(processes->rowCount()-1, 1, processName);

            // Connections Column
            int connectionCount = getProcessConnections(process.th32ProcessID);
            QTableWidgetItem *connections = new QTableWidgetItem(QString::number(connectionCount));
            connections->setFlags(connections->flags() & ~Qt::ItemIsEditable);
            processes->setItem(processes->rowCount()-1, 2, connections);
        } while (Process32Next(captureProcesses, &process));
    }
    CloseHandle(captureProcesses);
}

void Widget::chooseFileToExecute()
{
    *file = QFileDialog::getOpenFileName(this, "Choose a File", "", "All Files (*)");
    selectedFile->setText("File to Execute: " + *file);
    // would proceed to execute and analyze file here
}

int Widget::getProcessConnections(DWORD processID)
{
    int connectionCount = 0;

    // Get TCP connections (IPv4)
    PMIB_TCPTABLE_OWNER_PID tcpTable = NULL;
    DWORD tcpTableSize = 0;
    DWORD result;

    // First call to get required buffer size (A lot of these functions work like this)
    result = GetExtendedTcpTable(NULL, &tcpTableSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
    if (result == ERROR_INSUFFICIENT_BUFFER)
    {
        tcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(tcpTableSize);
        if (tcpTable != NULL)
        {
            result = GetExtendedTcpTable(tcpTable, &tcpTableSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
            if (result == NO_ERROR)
            {
                // Count the connections of the given process
                for (DWORD i = 0; i < tcpTable->dwNumEntries; i++)
                {
                    if (tcpTable->table[i].dwOwningPid == processID) connectionCount++;
                }
            }
            free(tcpTable);
            tcpTable = NULL;
        }
    }

    // Now, get the UDP connections
    PMIB_UDPTABLE_OWNER_PID udpTable = NULL;
    DWORD udpTableSize = 0;

    result = GetExtendedUdpTable(NULL, &udpTableSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0);
    if (result == ERROR_INSUFFICIENT_BUFFER)
    {
        udpTable = (PMIB_UDPTABLE_OWNER_PID)malloc(udpTableSize);
        result = GetExtendedUdpTable(udpTable, &udpTableSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0);
        if (result != NO_ERROR)
        {
            for(DWORD i = 0; i < udpTable->dwNumEntries; i++)
            {
                if (udpTable->table[i].dwOwningPid == processID) connectionCount++;
            }
        }
        free(udpTable);
        udpTable = NULL;
    }

    // IPv6 -- Only difference is AF_INET -> AF_INET6
    // Haven't decided to include this yet. I'm just sticking with IPv4 for now.
    // tcpTableSize = 0;
    // result = GetExtendedTcpTable(NULL, &tcpTableSize, TRUE, AF_INET6, TCP_TABLE_OWNER_PID_ALL, 0);
    // if (result == ERROR_INSUFFICIENT_BUFFER)
    // {
    //     tcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(tcpTableSize);
    //     if (tcpTable != NULL)
    //     {
    //         result = GetExtendedTcpTable(tcpTable, &tcpTableSize, TRUE, AF_INET6, TCP_TABLE_OWNER_PID_ALL, 0);
    //         if (result == NO_ERROR)
    //         {
    //             // Count the connections of the given process
    //             for (DWORD i = 0; i < tcpTable->dwNumEntries; i++)
    //             {
    //                 if (tcpTable->table[i].dwOwningPid == processID) connectionCount++;
    //             }
    //         }
    //         free(tcpTable);
    //         tcpTable = NULL;
    //     }
    // }

    // udpTableSize = 0;
    // result = GetExtendedUdpTable(NULL, &udpTableSize, TRUE, AF_INET6, UDP_TABLE_OWNER_PID, 0);
    // if (result == ERROR_INSUFFICIENT_BUFFER)
    // {
    //     udpTable = (PMIB_UDPTABLE_OWNER_PID)malloc(udpTableSize);
    //     result = GetExtendedUdpTable(udpTable, &udpTableSize, TRUE, AF_INET6, UDP_TABLE_OWNER_PID, 0);
    //     if (result != NO_ERROR)
    //     {
    //         for(DWORD i = 0; i < udpTable->dwNumEntries; i++)
    //         {
    //             if (udpTable->table[i].dwOwningPid == processID) connectionCount++;
    //         }
    //     }
    //     free(udpTable);
    //     udpTable = NULL;
    // }

    return connectionCount;
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

    // add proper error handling
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
        PIMAGE_NT_HEADERS pimage_nt_header = (PIMAGE_NT_HEADERS)((u_char*)pimage_dos_header+pimage_dos_header->e_lfanew);
        PIMAGE_SECTION_HEADER textSectionHeader = (PIMAGE_SECTION_HEADER)((uintptr_t)pimage_nt_header + sizeof(DWORD) + (uintptr_t)(sizeof(IMAGE_FILE_HEADER)) + (uintptr_t)pimage_nt_header->FileHeader.SizeOfOptionalHeader);

        DWORD virtualSize = textSectionHeader->Misc.VirtualSize;
        DWORD actualSize = textSectionHeader->SizeOfRawData;

        // need to get imports

        emit analyzeNewFile(QString::fromWCharArray(module.szExePath), QString::number(virtualSize), QString::number(actualSize));
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
    populateProcessTable();
    selectedFile->setText("Selected Process: No process selected");
}

void Widget::sendTable()
{
    emit saveTable(processes);
}
