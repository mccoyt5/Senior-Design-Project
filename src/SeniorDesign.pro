TEMPLATE = app
TARGET = YAHOS

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    Action.cpp \
    AnalysisWidget.cpp \
    DockWidget.cpp \
    HelpWindow.cpp \
    MainWindow.cpp \
    Widget.cpp \
    main.cpp

HEADERS += \
    Action.h \
    AnalysisWidget.h \
    DockWidget.h \
    HelpWindow.h \
    MainWindow.h \
    Widget.h
