TEMPLATE = app
TARGET = YAHOS

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += network

INCLUDEPATH += "C:/Development/npcap-sdk-1.15/Include"

SOURCES += \
    Action.cpp \
    AnalysisWidget.cpp \
    CaptureWindow.cpp \
    ConnectionsWindow.cpp \
    DevicesWindow.cpp \
    DockWidget.cpp \
    HelpWindow.cpp \
    MainWindow.cpp \
    Widget.cpp \
    main.cpp

HEADERS += \
    Action.h \
    AnalysisWidget.h \
    CaptureWindow.h \
    ConnectionsWindow.h \
    DevicesWindow.h \
    DockWidget.h \
    HelpWindow.h \
    MainWindow.h \
    Widget.h

LIBS += -liphlpapi -lws2_32
LIBS += -L"C:/Development/npcap-sdk-1.15/Lib/x64" -lwpcap -lPacket

DISTFILES += \
    app.manifest \
    app.rc

RC_FILE = app.rc
