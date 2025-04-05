#ifndef CAPTUREWINDOW_H
#define CAPTUREWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QString>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <pcap.h>
#include <QMessageBox>
#include <stdlib.h>
#include <QUdpSocket>
#include "DevicesWindow.h"
#include "qudpsocket.h"

class CaptureWindow : public QWidget
{
    Q_OBJECT

public:
    // Structure to hold packet data temporarily
    struct PacketData {
        struct pcap_pkthdr header;
        QByteArray data;
    };

    CaptureWindow(QString protocol, QString localAddr, QString localPort,
                  QString remoteAddr, QString remotePort, QWidget *parent = nullptr);
    ~CaptureWindow();

public slots:
    void processPacket(const pcap_pkthdr *header, const u_char *packet);

private slots:
    void startCapture();
    void stopCapture();
    void openDevicesWindow();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QLabel *connectionInfoLabel;
    QPushButton *listDevicesButton;
    QPushButton *startCaptureButton;
    QPushButton *stopCaptureButton;
    QTextEdit *captureDisplay;

    QString protocol;
    QString localAddress;
    QString localPort;
    QString remoteAddress;
    QString remotePort;
    QString selectedInterfaceName;

    bool isCapturing;
    QTimer *captureTimer;
    int packetCount;
    pcap_t *captureHandle = nullptr;
    QThread *captureThread = nullptr;
    QMutex packetMutex;
    QVector<struct PacketData> capturedPackets;
    bool stopCapturing = false;

    void findAndOpenInterface();
    bool saveCaptureToPcapFile(const QString &filePath);
    pcap_if_t* findMatchingInterface(pcap_if_t *alldevs, const QString &targetAddress);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // CAPTUREWINDOW_H
