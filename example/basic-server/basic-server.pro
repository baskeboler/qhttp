QT          += core network websockets
QT          -= gui
CONFIG      += console
osx:CONFIG  -= app_bundle

TARGET       = basic-server
TEMPLATE     = app
DEFINES += QHTTP_HAS_SSL
include($$PWD/../../qhttp.pri)

HEADERS     +=

SOURCES     += main.cpp

#include($$PWD/../example.pri)
