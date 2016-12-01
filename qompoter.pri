qhttp-lib {
    DEFINES += QOMP_QHTTP
    QOMP_NAME_QHTTP=qhttp

    LIBNAME = $$QOMP_NAME_QHTTP
    IMPORT_INCLUDEPATH = $$PWD/$$QOMP_NAME_QHTTP/include
    IMPORT_LIBPATH = $$PWD/$$LIBPATH
    INCLUDEPATH += $$IMPORT_INCLUDEPATH
    LIBS += -L$$IMPORT_LIBPATH -l$$getLibName($${LIBNAME}, "Qt")
}

qhttp {
    DEFINES += QOMP_QHTTP
    QOMP_NAME_QHTTP=qhttp

    HEADERS += \
        $$PWD/http-parser/http_parser.h \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpfwd.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpabstracts.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserverconnection.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserverrequest.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserverresponse.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserver.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpbase.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpserverconnection_private.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpserverrequest_private.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpserverresponse_private.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpserver_private.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpclientrequest_private.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpclientresponse_private.hpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/private/qhttpclient_private.hpp \

    SOURCES  += \
        $$PWD/http-parser/http_parser.c \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpabstracts.cpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserverconnection.cpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserverrequest.cpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserverresponse.cpp \
        $$PWD/$$QOMP_NAME_QHTTP/src/qhttpserver.cpp
    
    contains(DEFINES, QHTTP_HAS_CLIENT) {
        HEADERS += \
            $$PWD/$$QOMP_NAME_QHTTP/src/qhttpclient.hpp \
            $$PWD/$$QOMP_NAME_QHTTP/src/qhttpclientresponse.hpp \
            $$PWD/$$QOMP_NAME_QHTTP/src/qhttpclientrequest.hpp

        SOURCES += \
            $$PWD/$$QOMP_NAME_QHTTP/src/qhttpclientrequest.cpp \
            $$PWD/$$QOMP_NAME_QHTTP/src/qhttpclientresponse.cpp \
            $$PWD/$$QOMP_NAME_QHTTP/src/qhttpclient.cpp
    }

    INCLUDEPATH += \
        $$PWD/$$QOMP_NAME_QHTTP \
        $$PWD/$$QOMP_NAME_QHTTP/src \
        $$PWD/$$QOMP_NAME_QHTTP/vendor
}
