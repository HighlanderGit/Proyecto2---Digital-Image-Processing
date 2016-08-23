
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = project
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp

HEADERS  += mainwindow.h \
    player.h
FORMS    += mainwindow.ui

# Opencv Library
INCLUDEPATH += "/usr/local/include/opencv2"
LIBS += `pkg-config --cflags --libs opencv`

# Qwt library
# CONFIG += qwt
# INCLUDEPATH +="/usr/local/qwt-6.1.0-rc3/include"
# LIBS += -L/usr/local/qwt-6.1.0-rc3/lib -lqwt

