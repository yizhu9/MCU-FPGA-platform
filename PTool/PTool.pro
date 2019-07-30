#-------------------------------------------------
#
# Project created by QtCreator 2017-03-04T14:21:06
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET          = PTool
TEMPLATE        = app
MOC_DIR         = temp/moc
RCC_DIR         = temp/rcc
UI_DIR          = temp/ui
OBJECTS_DIR     = temp/obj
DESTDIR         = bin
win32:RC_FILE   = other/main.rc
PRECOMPILED_HEADER  = head.h

INCLUDEPATH     += $$PWD
INCLUDEPATH     += $$PWD/qcustomplot
INCLUDEPATH     += $$PWD/qextserialport
CONFIG          += qt warn_off

include ($$PWD/qcustomplot/qcustomplot.pri)
include ($$PWD/qextserialport/qextserialport.pri)

SOURCES += \
    main.cpp\
    quiwidget.cpp \
    frmmain.cpp

HEADERS  += \
    quiwidget.h \
    frmmain.h

FORMS    += \
    frmmain.ui

RESOURCES += \
    other/main.qrc \
    other/qss.qrc
