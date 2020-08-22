QT          += core network websockets
QT          -= gui
CONFIG      += console
osx:CONFIG  -= app_bundle

TEMPLATE     = app

PRJDIR       = ../..
include(../qmake/configs.pri)
include(../qhttp.pri)
INCLUDEPATH += $$PWD/../include
INCLUDEPATH += $$PWD/../src

#equals(ENABLE_QHTTP_CLIENT, "1") {
#    DEFINES *= QHTTP_HAS_CLIENT
#}

#CONFIG      += c++11 # c++14

#INCLUDEPATH += $$PWD/../include

##LIBS        += $$OUT_PWD/../../src/libqhttp.a

#QT          += core network
#QT          -= gui
#CONFIG      += console
#osx:CONFIG  -= app_bundle

#TEMPLATE     = app

#<<<<<<<< HEAD:example/helloworld/helloworld.pro
#HEADERS   +=

#SOURCES   += main.cpp

#include($$PWD/../example.pri)
#========
#PRJDIR       = ../..
#include(../qmake/configs.pri)

#LIBS      += -lqhttp

#>>>>>>>> QHttp/master:examples/example.pri
#>>>>>>> QHttp/master
