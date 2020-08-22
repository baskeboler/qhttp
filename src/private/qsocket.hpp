/** @file qsocket.hpp
 *
 * @copyright (C) 2016
 * @date 2016.05.26
 * @version 1.0.0
 * @author amir zamani <azadkuh@live.com>
 *
 */

#ifndef __QHTTP_SOCKET_HPP__
#define __QHTTP_SOCKET_HPP__

#include "qhttpfwd.hpp"

#include <QTcpSocket>
#include <QLocalSocket>
#include <QWebSocketServer>
#include <QUrl>
///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace details {
///////////////////////////////////////////////////////////////////////////////

/** an adapter for different socket types.
 * the main purpose of QHttp was to create a small HTTP server with ability to
 * support UNIX sockets (QLocalSocket)
 */
class QSocket
{
public:
    inline void close() {
        if ( itcpSocket )
            itcpSocket->close();

        if ( ilocalSocket )
            ilocalSocket->close();

	lastWrittenData.clear();
	lastReadData.clear();
    }

    inline void release() {
        close();
        if ( itcpSocket )
            itcpSocket->deleteLater();

        if ( ilocalSocket )
            ilocalSocket->deleteLater();

        itcpSocket   = nullptr;
        ilocalSocket = nullptr;

	lastWrittenData.clear();
	lastReadData.clear();
    }

    inline void flush() {
        if ( itcpSocket )
            itcpSocket->flush();

        else if ( ilocalSocket )
            ilocalSocket->flush();
    }

    inline bool isOpen() const {
        if ( ibackendType == ETcpSocket    &&    itcpSocket )
            return itcpSocket->isOpen()
                && itcpSocket->state() == QTcpSocket::ConnectedState;

        else if ( ibackendType == ELocalSocket    &&    ilocalSocket )
            return ilocalSocket->isOpen()
                && ilocalSocket->state() == QLocalSocket::ConnectedState;

        return false;
    }

    inline void connectTo(const QUrl& url) {
        if ( ilocalSocket )
            ilocalSocket->connectToServer(url.path());
    }

    inline void connectTo(const QString& host, quint16 port) {
        if ( itcpSocket )
            itcpSocket->connectToHost(host, port);
    }

    inline qint64 readRaw(char* buffer, qint64 maxlen) {
        if ( itcpSocket ) {
            itcpSocket->startTransaction();
            return itcpSocket->read(buffer, maxlen);
        }

        else if ( ilocalSocket ) {
            ilocalSocket->startTransaction();
            return ilocalSocket->read(buffer, maxlen);
        }

        return 0;
    }

    inline QByteArray readRaw() {
        if ( itcpSocket ) {
            itcpSocket->startTransaction();
            return itcpSocket->readAll();
        }

        else if ( ilocalSocket ) {
            ilocalSocket->startTransaction();
            return ilocalSocket->readAll();
        }

        return 0;
    }

    inline void writeRaw(const QByteArray& data) {
        if ( itcpSocket )
            itcpSocket->write(data);

        else if ( ilocalSocket )
            ilocalSocket->write(data);
    }

    inline qint64 bytesAvailable() {
        if ( itcpSocket )
            return itcpSocket->bytesAvailable();

        else if ( ilocalSocket )
            return ilocalSocket->bytesAvailable();

        return 0;
    }

    inline void disconnectAllQtConnections() {
        if ( itcpSocket )
            QObject::disconnect(itcpSocket, 0, 0, 0);

        if ( ilocalSocket )
            QObject::disconnect(ilocalSocket, 0, 0, 0);
    }

    inline void startTransaction() {
        if ( itcpSocket )
            itcpSocket->startTransaction();

        if ( ilocalSocket )
            ilocalSocket->startTransaction();
    }

    inline void rollbackTransaction() {
        if ( itcpSocket )
            itcpSocket->rollbackTransaction();

        if ( ilocalSocket )
            ilocalSocket->rollbackTransaction();
    }

    inline void commitTransaction() {
        if ( itcpSocket )
            itcpSocket->commitTransaction();

        if ( ilocalSocket )
            ilocalSocket->commitTransaction();
    }

public:
    TBackend          ibackendType = ETcpSocket;
    QTcpSocket*       itcpSocket   = nullptr;
    QLocalSocket*     ilocalSocket = nullptr;
    QWebSocketServer* iwebSocket   = nullptr;
    QByteArray lastWrittenData;
    QByteArray lastReadData;
}; // class QSocket

///////////////////////////////////////////////////////////////////////////////
} // namespace details
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
#endif // __QHTTP_SOCKET_HPP__
