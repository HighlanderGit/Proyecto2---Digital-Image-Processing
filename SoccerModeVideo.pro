
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

DISTFILES += \
    vlcsnap-00001.png \
    vlcsnap-00003.png \
    vlcsnap-00011.png \
    vlcsnap-00014.png \
    vlcsnap-00029.png \
    vlcsnap-00053.png \
    vlcsnap-00057.png \
    vlcsnap-00060.png \
    Readme Installation guide and use.txt \
    video.mp4 \
    Readme Installation guide and use.txt \
    Readme Installation guide and use

