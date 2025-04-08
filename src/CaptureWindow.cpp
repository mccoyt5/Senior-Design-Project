#include "CaptureWindow.h"

static void packet_handler(u_char *user, const struct pcap_pkthdr *header, const u_char *packet)
{
    CaptureWindow *window = reinterpret_cast<CaptureWindow*>(user);
    QMetaObject::invokeMethod(window, "processPacket", Qt::QueuedConnection,
                              Q_ARG(const pcap_pkthdr*, header),
                              Q_ARG(const u_char*, packet));
}

CaptureWindow::CaptureWindow(QString protocol, QString localAddr, QString localPort,
                             QString remoteAddr, QString remotePort, QWidget *parent)
    : QWidget(parent),
    protocol(protocol),
    localAddress(localAddr),
    localPort(localPort),
    remoteAddress(remoteAddr),
    remotePort(remotePort),
    isCapturing(false)
{
    // Initialize member variables first
    packetCount = 0;

    // Set up the UI
    setWindowTitle("Port Traffic Capture");
    resize(600, 400);

    mainLayout = new QVBoxLayout(this);

    // connection info display
    QString connectionInfo = protocol + " connection: " +
                             localAddress + ":" + localPort;
    if (remoteAddress != "*" && remotePort != "*") {
        connectionInfo += " <-> " + remoteAddress + ":" + remotePort;
    }
    connectionInfoLabel = new QLabel(connectionInfo, this);
    mainLayout->addWidget(connectionInfoLabel);

    // button layout
    buttonLayout = new QHBoxLayout();

    listDevicesButton = new QPushButton("List Devices", this);
    startCaptureButton = new QPushButton("Start Capture", this);
    stopCaptureButton = new QPushButton("Stop Capture", this);
    stopCaptureButton->setEnabled(false);

    buttonLayout->addWidget(listDevicesButton);
    buttonLayout->addWidget(startCaptureButton);
    buttonLayout->addWidget(stopCaptureButton);
    mainLayout->addLayout(buttonLayout);

    // capture display area
    captureDisplay = new QTextEdit(this);
    captureDisplay->setReadOnly(true);
    captureDisplay->setFont(QFont("Courier New", 10));
    mainLayout->addWidget(captureDisplay);

    // connect the stuff
    // connect(listDevicesButton, SIGNAL(clicked()), this, SLOT(openDevicesWindow()));
    connect(startCaptureButton, SIGNAL(clicked()), this, SLOT(startCapture()));
    connect(stopCaptureButton, SIGNAL(clicked()), this, SLOT(stopCapture()));

    captureDisplay->append("Ready to capture traffic on port " + localPort);
    captureDisplay->append("Click 'Start Capture' to begin monitoring network activity");
}

CaptureWindow::~CaptureWindow()
{
    if (isCapturing) {
        stopCapture();
    }
}

pcap_if_t* CaptureWindow::findMatchingInterface(pcap_if_t *alldevs, const QString &targetAddress)
{
    pcap_if_t *matchedDevice = nullptr;

    // First try: Find exact interface with matching address
    for (pcap_if_t *device = alldevs; device != nullptr; device = device->next) {
        for (pcap_addr_t *address = device->addresses; address != nullptr; address = address->next) {
            // Check if this is an IPv4 address
            if (address->addr && address->addr->sa_family == AF_INET) {
                char addrStr[INET_ADDRSTRLEN];
                struct sockaddr_in *addr = (struct sockaddr_in*)address->addr;
                inet_ntop(AF_INET, &(addr->sin_addr), addrStr, INET_ADDRSTRLEN);

                if (QString(addrStr) == targetAddress) {
                    // found it
                    return device;
                }
            }
            // Check if this is an IPv6 address
            else if (address->addr && address->addr->sa_family == AF_INET6) {
                char addrStr[INET6_ADDRSTRLEN];
                struct sockaddr_in6 *addr = (struct sockaddr_in6*)address->addr;
                inet_ntop(AF_INET6, &(addr->sin6_addr), addrStr, INET6_ADDRSTRLEN);

                if (QString(addrStr) == targetAddress) {
                    return device;
                }
            }
        }
    }

    // Second try: For loopback addresses
    if (targetAddress == "127.0.0.1" || targetAddress == "::1") {
        for (pcap_if_t *device = alldevs; device != nullptr; device = device->next) {
            if (device->flags & PCAP_IF_LOOPBACK) {
                matchedDevice = device;
                break;
            }
        }
    }

    // Third try: For "0.0.0.0" (any address) or cases where we need to find default interface
    if (!matchedDevice) {
        for (pcap_if_t *device = alldevs; device != nullptr; device = device->next) {
            // Skip loopback interfaces for default
            if (device->flags & PCAP_IF_LOOPBACK)
                continue;

            // Prefer interfaces that are up and running
            if ((device->flags & PCAP_IF_UP) && (device->flags & PCAP_IF_RUNNING)) {
                matchedDevice = device;
                break;
            }
        }
    }

    // Last resort: Just use the first non-loopback interface
    if (!matchedDevice) {
        for (pcap_if_t *device = alldevs; device != nullptr; device = device->next) {
            if (!(device->flags & PCAP_IF_LOOPBACK)) {
                matchedDevice = device;
                break;
            }
        }
    }

    // If still nothing, use the first available interface
    if (!matchedDevice && alldevs) {
        matchedDevice = alldevs;
    }

    return matchedDevice;
}

void CaptureWindow::startCapture()
{
    // Clear any previous capture data
    packetMutex.lock();
    capturedPackets.clear();
    packetMutex.unlock();

    // Clear display and show header
    captureDisplay->clear();
    captureDisplay->append("Starting packet capture on " + protocol + " " +
                           localAddress + ":" + localPort);
    captureDisplay->append("Timestamp\t\t\tDirection\tSize\tInfo");
    captureDisplay->append("----------------------------------------------------------");

    // Find and open the interface
    findAndOpenInterface();

    if (captureHandle == nullptr) {
        return;
    }

    // Reset packet count
    packetCount = 0;
    isCapturing = true;
    stopCapturing = false;
    startCaptureButton->setEnabled(false);
    stopCaptureButton->setEnabled(true);

    // Start packet capture in a separate thread
    captureThread = new QThread();
    captureThread->start();

    // Move the capture loop to the new thread
    QObject *worker = new QObject();
    worker->moveToThread(captureThread);

    // Connect to start the capture loop
    connect(captureThread, &QThread::started, worker, [this, worker]() {
        // The main capture loop
        while (!stopCapturing && captureHandle) {
            pcap_dispatch(captureHandle, 1, packet_handler, reinterpret_cast<u_char*>(this));
            // Small delay to prevent high CPU usage
            QThread::msleep(10);
        }

        worker->deleteLater();
    });

    // Show a notification
    QMessageBox::information(this, "Capture Started",
                             "Network capture started on port " + localPort);
}

void CaptureWindow::stopCapture()
{
    if (!isCapturing) {
        return;
    }

    // First, mark that we're no longer capturing
    isCapturing = false;

    // Signal the capture thread to stop
    stopCapturing = true;

    // Break the pcap loop to unblock the thread
    if (captureHandle) {
        pcap_breakloop(captureHandle);
    }

    // Wait for the thread to finish - important to do this BEFORE opening any dialogs
    if (captureThread && captureThread->isRunning()) {
        if (!captureThread->wait(2000)) {
            // If thread doesn't quit on its own, terminate it
            captureThread->terminate();
            captureThread->wait(1000);
        }
        delete captureThread;
        captureThread = nullptr;
    }

    // Close the capture handle
    if (captureHandle) {
        pcap_close(captureHandle);
        captureHandle = nullptr;
    }

    // Make a thread-safe copy of packet data before showing dialog
    int finalPacketCount = 0;
    bool hasPackets = false;

    // Use a separate scope for the mutex lock to ensure it's released before dialog
    {
        QMutexLocker locker(&packetMutex);
        finalPacketCount = packetCount;
        hasPackets = !capturedPackets.isEmpty();
    } // mutex unlocked here when locker goes out of scope

    // Only show save dialog if we have packets
    if (hasPackets) {
        // Now it's safe to show dialog since all threads are stopped
        QString filePath = QFileDialog::getSaveFileName(this,
                                                        "Save Capture File", "", "Pcap Files (*.pcap)");

        if (!filePath.isEmpty()) {
            if (saveCaptureToPcapFile(filePath)) {
                captureDisplay->append("Capture saved to: " + filePath);
            } else {
                QMessageBox::critical(this, "Error", "Failed to save capture file");
            }
        }
    } else {
        captureDisplay->append("No packets were captured.");
    }

    captureDisplay->append("----------------------------------------------------------");
    captureDisplay->append("Packet capture stopped. Captured " +
                           QString::number(finalPacketCount) + " packets.");

    // Update UI
    startCaptureButton->setEnabled(true);
    stopCaptureButton->setEnabled(false);
}

bool CaptureWindow::saveCaptureToPcapFile(const QString &filePath)
{
    // Make a local copy of the packets to minimize mutex lock time
    QVector<PacketData> packetsCopy;

    // Lock the mutex only while copying the vector, not during file operations
    {
        QMutexLocker locker(&packetMutex);
        if (capturedPackets.isEmpty()) {
            return false;
        }
        packetsCopy = capturedPackets;
    } // mutex unlocked here

    // Create a new pcap file
    pcap_t *pd = pcap_open_dead(DLT_EN10MB, 65535);
    if (pd == nullptr) {
        return false;
    }

    // Open the file for writing
    pcap_dumper_t *dumper = pcap_dump_open(pd, filePath.toStdString().c_str());
    if (dumper == nullptr) {
        pcap_close(pd);
        return false;
    }

    // Write each packet to the file from our local copy
    for (int i = 0; i < packetsCopy.size(); i++) {
        const PacketData &packet = packetsCopy.at(i);
        pcap_dump((u_char*)dumper, &packet.header,
                  reinterpret_cast<const u_char*>(packet.data.constData()));
    }

    // Close the file
    pcap_dump_close(dumper);
    pcap_close(pd);

    return true;
}

void CaptureWindow::openDevicesWindow()
{
    // Create window
    DevicesWindow *devicesWindow = new DevicesWindow();
    devicesWindow->setWindowFlags(Qt::Window);
    devicesWindow->show();
}

void CaptureWindow::findAndOpenInterface()
{
    char errbuf[PCAP_ERRBUF_SIZE] = {0};

    // Find the matching interface
    QString addrToMatch = localAddress;

    // For 0.0.0.0 or empty addresses, handle specially
    if (addrToMatch == "0.0.0.0" || addrToMatch.isEmpty()) {
        // Try to get the interface used for the remote connection
        if (remoteAddress != "*" && !remoteAddress.isEmpty() && remoteAddress != "0.0.0.0") {
            QUdpSocket testSocket;
            testSocket.connectToHost(remoteAddress, remotePort.toUInt());
            if (testSocket.waitForConnected(100)) {
                addrToMatch = testSocket.localAddress().toString();
            }
            testSocket.close();
        }
    }

    // Call our interface matching function
    pcap_if_t *alldevs = nullptr;
    pcap_if_t *matchedDevice = nullptr;
    // char errbuf[PCAP_ERRBUF_SIZE] = {0};
    std::memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    // Get the list of network interfaces
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        QMessageBox::critical(this, "Error", "Error finding devices: " + QString(errbuf));
        return;
    }

    // Find the matching interface using our helper function
    matchedDevice = findMatchingInterface(alldevs, addrToMatch);

    if (matchedDevice) {
        // Store the device name for logging
        selectedInterfaceName = QString(matchedDevice->name);
        captureDisplay->append("Using interface: " + selectedInterfaceName +
                               (matchedDevice->description ?
                                    " (" + QString(matchedDevice->description) + ")" : ""));

        // Open the device for capturing - use the actual device name from the matched device
        captureHandle = pcap_open_live(matchedDevice->name,
                                       65536, 1, 1000, errbuf);

        if (captureHandle == nullptr) {
            QMessageBox::critical(this, "Error", "Could not open device: " + QString(errbuf));
            pcap_freealldevs(alldevs);
            return;
        }

        // Set filter to capture only traffic for our port
        QString filterStr;

        // Create appropriate filter based on the connection we're monitoring
        if (remoteAddress != "*" && remotePort != "*") {
            // TCP/UDP connection to specific remote
            filterStr = QString("(host %1 and host %2) and (port %3 or port %4)")
                            .arg(localAddress)
                            .arg(remoteAddress)
                            .arg(localPort)
                            .arg(remotePort);
        } else {
            // Just filter on the local port
            filterStr = QString("port %1").arg(localPort);
        }

        struct bpf_program fcode;
        if (pcap_compile(captureHandle, &fcode, filterStr.toStdString().c_str(), 1, PCAP_NETMASK_UNKNOWN) < 0) {
            QMessageBox::warning(this, "Warning", "Could not compile filter: " + filterStr);
        } else {
            if (pcap_setfilter(captureHandle, &fcode) < 0) {
                QMessageBox::warning(this, "Warning", "Could not set filter");
            }
            pcap_freecode(&fcode);
        }
    } else {
        QMessageBox::critical(this, "Error", "Could not find suitable interface for capture");
    }

    // Free the device list
    pcap_freealldevs(alldevs);
}

void CaptureWindow::processPacket(const pcap_pkthdr *header, const u_char *packet)
{
    // Create a new packet data structure
    PacketData packetData;
    packetData.header = *header;

    // Copy packet data
    packetData.data = QByteArray(reinterpret_cast<const char*>(packet), header->caplen);

    // Lock the mutex to safely add to our vector
    packetMutex.lock();
    capturedPackets.append(packetData);
    packetMutex.unlock();

    // Update the display
    QDateTime timestamp = QDateTime::fromSecsSinceEpoch(header->ts.tv_sec);
    timestamp = timestamp.addMSecs(header->ts.tv_usec / 1000);
    QString timeStr = timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");

    // simplified - could use more parsing here
    QString direction = "IN";

    // Display packet info
    captureDisplay->append(QString("%1\t%2\t\t%3\t%4")
                               .arg(timeStr)
                               .arg(direction)
                               .arg(header->len)
                               .arg("Packet #" + QString::number(capturedPackets.size())));

    packetCount++;
}

void CaptureWindow::closeEvent(QCloseEvent *event)
{
    if (isCapturing) {
        // Ask user if they want to stop capturing
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Capture in Progress",
            "A capture is in progress. Do you want to stop and save?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {
            stopCapture();
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }

    // Make sure the capture thread is stopped
    stopCapturing = true;
    if (captureThread && captureThread->isRunning()) {
        captureThread->quit();
        captureThread->wait(1000);
        delete captureThread;
    }

    // Close any open handles
    if (captureHandle) {
        pcap_close(captureHandle);
    }

    event->accept();
}
