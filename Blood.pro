#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T21:20:10
#
#-------------------------------------------------

QT       += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Blood
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += C++14

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    base.cpp \
    patientsetting.cpp \
    paintscene.cpp \
    figure.cpp \
    romb.cpp \
    square.cpp \
    triangle.cpp \
    tablesetting.cpp \
    imagesetting.cpp \
    markerwindow.cpp \
    characteristicssetting.cpp

HEADERS += \
        mainwindow.h \
    base.h \
    structsfordb.h \
    patientsetting.h \
    paintscene.h \
    figure.h \
    romb.h \
    square.h \
    triangle.h \
    tablesetting.h \
    imagesetting.h \
    markerwindow.h \
    characteristicssetting.h

FORMS += \
        mainwindow.ui \
    patientsetting.ui \
    tablesetting.ui \
    imagesetting.ui \
    markerwindow.ui \
    characteristicssetting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
