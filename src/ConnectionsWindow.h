#ifndef CONNECTIONSWINDOW_H
#define CONNECTIONSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <qheaderview.h>

class ConnectionsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionsWindow(QString pid = "");
    ~ConnectionsWindow();

private:
    QTableWidget *connectionsTable;
    QVBoxLayout *layout;
    QString processPid;

    void loadConnections();
    QString getStatusString(DWORD state);

private slots:
    void onRowDoubleClicked(int row, int column);
};

#endif // CONNECTIONSWINDOW_H
