#include "CaptureWindow.h"
#include <ConnectionsWindow.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

ConnectionsWindow::ConnectionsWindow(QString pid) : processPid(pid)
{
    setWindowTitle("Process Connections");
    resize(800, 500);

    layout = new QVBoxLayout(this);
    connectionsTable = new QTableWidget(this);
    connectionsTable->setColumnCount(6);
    QStringList headers = {"Protocol", "Local Address", "Local Port", "Remote Address", "Remote Port", "Status"};
    connectionsTable->setHorizontalHeaderLabels(headers);
    connectionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connectionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(connectionsTable);
    setLayout(layout);

    connect(connectionsTable, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onRowDoubleClicked(int,int)));

    loadConnections();
}

ConnectionsWindow::~ConnectionsWindow()
{
}

QString ConnectionsWindow::getStatusString(DWORD state)
{
    switch (state) {
    case MIB_TCP_STATE_CLOSED:
        return "CLOSED";
    case MIB_TCP_STATE_LISTEN:
        return "LISTENING";
    case MIB_TCP_STATE_SYN_SENT:
        return "SYN_SENT";
    case MIB_TCP_STATE_SYN_RCVD:
        return "SYN_RECEIVED";
    case MIB_TCP_STATE_ESTAB:
        return "ESTABLISHED";
    case MIB_TCP_STATE_FIN_WAIT1:
        return "FIN_WAIT1";
    case MIB_TCP_STATE_FIN_WAIT2:
        return "FIN_WAIT2";
    case MIB_TCP_STATE_CLOSE_WAIT:
        return "CLOSE_WAIT";
    case MIB_TCP_STATE_CLOSING:
        return "CLOSING";
    case MIB_TCP_STATE_LAST_ACK:
        return "LAST_ACK";
    case MIB_TCP_STATE_TIME_WAIT:
        return "TIME_WAIT";
    case MIB_TCP_STATE_DELETE_TCB:
        return "DELETE_TCB";
    default:
        return "UNKNOWN";
    }
}

void ConnectionsWindow::onRowDoubleClicked(int row, int column)
{
    // Get the connection details
    QString protocol = connectionsTable->item(row, 0)->text();
    QString localAddr = connectionsTable->item(row, 1)->text();
    QString localPort = connectionsTable->item(row, 2)->text();
    QString remoteAddr = connectionsTable->item(row, 3)->text();
    QString remotePort = connectionsTable->item(row, 4)->text();

    // Create and then show the capture window
    CaptureWindow *captureWindow = new CaptureWindow(protocol, localAddr, localPort, remoteAddr, remotePort);

    captureWindow->setWindowTitle("Port Capture - " + localPort);
    captureWindow->setWindowFlags(Qt::Window);
    captureWindow->show();
}

void ConnectionsWindow::loadConnections()
{
    connectionsTable->clearContents();
    connectionsTable->setRowCount(0);

    DWORD pidFilter = processPid.toUInt();

    // init Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return;
    }

    // TCP table with process IDs (IPv4)
    DWORD size = 0;
    DWORD result = GetExtendedTcpTable(NULL, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);

    if (result == ERROR_INSUFFICIENT_BUFFER) {
        std::vector<char> buffer(size);
        PMIB_TCPTABLE_OWNER_PID tcpTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(&buffer[0]);

        if (GetExtendedTcpTable(tcpTable, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
            for (DWORD i = 0; i < tcpTable->dwNumEntries; i++) {
                if (tcpTable->table[i].dwOwningPid == pidFilter) {
                    int row = connectionsTable->rowCount();
                    connectionsTable->insertRow(row);

                    // convert IP addresses
                    IN_ADDR localAddr, remoteAddr;
                    localAddr.S_un.S_addr = tcpTable->table[i].dwLocalAddr;
                    remoteAddr.S_un.S_addr = tcpTable->table[i].dwRemoteAddr;

                    char localAddrStr[INET_ADDRSTRLEN];
                    char remoteAddrStr[INET_ADDRSTRLEN];

                    inet_ntop(AF_INET, &localAddr, localAddrStr, INET_ADDRSTRLEN);
                    inet_ntop(AF_INET, &remoteAddr, remoteAddrStr, INET_ADDRSTRLEN);

                    // Protocol
                    connectionsTable->setItem(row, 0, new QTableWidgetItem("TCP (IPv4)"));

                    // Local address and port
                    connectionsTable->setItem(row, 1, new QTableWidgetItem(QString(localAddrStr)));
                    connectionsTable->setItem(row, 2, new QTableWidgetItem(QString::number(ntohs(tcpTable->table[i].dwLocalPort))));

                    // Remote address and port
                    connectionsTable->setItem(row, 3, new QTableWidgetItem(QString(remoteAddrStr)));
                    connectionsTable->setItem(row, 4, new QTableWidgetItem(QString::number(ntohs(tcpTable->table[i].dwRemotePort))));

                    // Status
                    connectionsTable->setItem(row, 5, new QTableWidgetItem(getStatusString(tcpTable->table[i].dwState)));
                }
            }
        }
    }

    // now, UDP table with process IDs (IPv4) (no connection state for UDP)
    size = 0;
    result = GetExtendedUdpTable(NULL, &size, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0);

    if (result == ERROR_INSUFFICIENT_BUFFER) {
        std::vector<char> buffer(size);
        PMIB_UDPTABLE_OWNER_PID udpTable = reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>(&buffer[0]);

        if (GetExtendedUdpTable(udpTable, &size, FALSE, AF_INET, UDP_TABLE_OWNER_PID, 0) == NO_ERROR) {
            for (DWORD i = 0; i < udpTable->dwNumEntries; i++) {
                if (udpTable->table[i].dwOwningPid == pidFilter) {
                    int row = connectionsTable->rowCount();
                    connectionsTable->insertRow(row);

                    // Convert IP address
                    IN_ADDR localAddr;
                    localAddr.S_un.S_addr = udpTable->table[i].dwLocalAddr;

                    char localAddrStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &localAddr, localAddrStr, INET_ADDRSTRLEN);

                    // Protocol
                    connectionsTable->setItem(row, 0, new QTableWidgetItem("UDP (IPv4)"));

                    // Local address and port
                    connectionsTable->setItem(row, 1, new QTableWidgetItem(QString(localAddrStr)));
                    connectionsTable->setItem(row, 2, new QTableWidgetItem(QString::number(ntohs(udpTable->table[i].dwLocalPort))));

                    // Remote (N/A for UDP)
                    connectionsTable->setItem(row, 3, new QTableWidgetItem("*"));
                    connectionsTable->setItem(row, 4, new QTableWidgetItem("*"));

                    // Status (N/A for UDP)
                    connectionsTable->setItem(row, 5, new QTableWidgetItem("STATELESS"));
                }
            }
        }
    }

    WSACleanup();
}
