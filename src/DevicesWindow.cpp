#include "DevicesWindow.h"

DevicesWindow::DevicesWindow()
{

    // Window setup
    setWindowTitle("Interface Adapter List");
    resize(600,400);

    mainLayout = new QVBoxLayout(this);

    // Display a table with columns: address, name, subnet?
    devicesTable = new QTableWidget(this);

    devicesTable->setColumnCount(4);
    QStringList headers = {"Name", "Address", "Subnet", "Description"};

    devicesTable->setHorizontalHeaderLabels(headers);
    devicesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mainLayout->addWidget(devicesTable);
    setLayout(mainLayout);

    loadDevices();
}

DevicesWindow::~DevicesWindow()
{
}


void DevicesWindow::loadDevices()
{
    pcap_if_t *all_devs;
    pcap_if_t *d;
    pcap_addr_t *a;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE];

    // get device list from local machine
    if (pcap_findalldevs(&all_devs, errbuf) == PCAP_ERROR)
    {
        QMessageBox::critical(this, "Error","Error when using pcap_findalldevs: " + QString(errbuf));
        return;
    }

    // Clean table
    devicesTable->clearContents();
    devicesTable->setRowCount(0);

    for (d=all_devs; d != NULL; d = d->next)
    {
        int row = devicesTable->rowCount();
        devicesTable->insertRow(row);

        devicesTable->setItem(row, 0, new QTableWidgetItem(d->name));

        // addresses
        // for (a=d->addresses;a;a=a->next)
        // {
        //     if (a->addr)
        //     {
        //         char iAddr[INET_ADDRSTRLEN];
        //         inet_ntop(AF_INET, &iAddr, ((struct sockaddr_in *)a->addr)->sin_addr.s_addr, INET_ADDRSTRLEN);

        //         devicesTable->setItem(row, 1, new QTableWidgetItem(QString(

        //                                           )));
        //     }
        // }

        if (d->description)
        {
            devicesTable->setItem(row, 3, new QTableWidgetItem(d->description));
        } else {
            devicesTable->setItem(row, 3, new QTableWidgetItem(""));
        }
        i++;
    }

    if (i == 0)
    {
        QMessageBox::warning(this, "No devices found", "No interfaces found! Make sure Npcap is installed.");
    }

    pcap_freealldevs(all_devs);
}

void DevicesWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Close Detected", "The windows close event was detected. Do you want to continue?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // Clean up and close
        event->accept();
    }
    else event->ignore();
}
