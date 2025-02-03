TEMPLATE = app
TARGET = YAHOS

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    Action.cpp \
    Widget.cpp \
    main.cpp

HEADERS += \
    Action.h \
    Widget.h
