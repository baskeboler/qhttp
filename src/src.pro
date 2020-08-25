QT       += core network websockets
QT       -= gui

TARGET    = qhttp
TEMPLATE  = lib

PRJDIR    = ..
include($$PRJDIR/qmake/configs.pri)

win32:DEFINES *= QHTTP_EXPORT

# nodejs http_parser
SOURCES  += $$PRJDIR/3rdParty/http-parser/http_parser.c
SUBMODULE_HEADERS  += $$PRJDIR/3rdParty/http-parser/http_parser.h


SOURCES  += \
    qhttpabstracts.cpp \
    qhttpserverconnection.cpp \
    qhttpserverrequest.cpp \
    qhttpserverresponse.cpp \
    qhttpserver.cpp \
    qhttpsslsocket.cpp


DIST_HEADERS  += \
    $$PRJDIR/include/QHttp/qhttpfwd.hpp \
    $$PRJDIR/include/QHttp/qhttpabstracts.hpp \
    $$PRJDIR/include/QHttp/qhttpserverconnection.hpp \
    $$PRJDIR/include/QHttp/qhttpserverrequest.hpp \
    $$PRJDIR/include/QHttp/qhttpserverresponse.hpp \
    $$PRJDIR/include/QHttp/qhttpserver.hpp \
    $$PRJDIR/include/QHttp/QHttpServer


contains(DEFINES, QHTTP_HAS_CLIENT) {
    SOURCES += \
        qhttpclientrequest.cpp \
        qhttpclientresponse.cpp \
        qhttpclient.cpp

    DIST_HEADERS += \
        $$PRJDIR/include/QHttp/qhttpclient.hpp \
        $$PRJDIR/include/QHttp/qhttpclientresponse.hpp \
        $$PRJDIR/include/QHttp/qhttpclientrequest.hpp \
        $$PRJDIR/include/QHttp/QHttpClient

}

#HEADERS += $$PUBLIC_HEADERS

unix:!mac {
    CONFIG += create_pc create_prl no_install_prl

    headers.files = $$PUBLIC_HEADERS
    headers.path = $$PREFIX/include/qhttp/
    target.path = $$PREFIX/lib/
    INSTALLS += target headers

    QMAKE_PKGCONFIG_NAME = QHttp
    QMAKE_PKGCONFIG_DESCRIPTION = QHttp is a lightweight, asynchronous and fast HTTP library in c++14 / Qt5
    QMAKE_PKGCONFIG_PREFIX = $$PREFIX
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    QMAKE_PKGCONFIG_INCDIR = $$headers.path
    QMAKE_PKGCONFIG_VERSION = 2.0.0
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig

}

PRIVATE_HEADERS += \
    private/httpparser.hxx \
    private/httpreader.hxx \
    private/httpwriter.hxx \
    private/qhttpabstractsocket.hpp \
    private/qhttpbase.hpp \
    private/qhttpclient_private.hpp \
    private/qhttpclientrequest_private.hpp \
    private/qhttpclientresponse_private.hpp \
    private/qhttpserver_private.hpp \
    private/qhttpserverconnection_private.hpp \
    private/qhttpserverrequest_private.hpp \
    private/qhttpserverresponse_private.hpp

HEADERS += $$DIST_HEADERS \
           $$PRIVATE_HEADERS \
           $$SUBMODULE_HEADERS

QMAKE_CXXFLAGS += -Wno-unknown-pragmas -Wno-padded

include($$PRJDIR/qmake/install.pri)
