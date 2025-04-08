#ifndef DEVICESWINDOW_H
#define DEVICESWINDOW_H

#include "qtablewidget.h"
#include "qwidget.h"
#include "qboxlayout.h"
#include "qheaderview.h"
#include <ws2tcpip.h>
#include <pcap.h>
#include <QMessageBox>
#include <QCloseEvent>

class DevicesWindow : public QWidget
{
    Q_OBJECT

public:
    DevicesWindow();
    ~DevicesWindow();

private:
    QVBoxLayout *mainLayout;
    QTableWidget *devicesTable;

    void loadDevices();

protected:
    void closeEvent(QCloseEvent *event);

};

#endif // DEVICESWINDOW_H
