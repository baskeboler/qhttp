/** private imeplementation.
 * https://github.com/azadkuh/qhttp
 *
 * @author amir zamani
 * @version 2.0.0
 * @date 2014-07-11
 */

#ifndef QHTTPSERVER_CONNECTION_PRIVATE_HPP
#define QHTTPSERVER_CONNECTION_PRIVATE_HPP
///////////////////////////////////////////////////////////////////////////////

#include <QBasicTimer>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>

#if defined(QHTTP_HAS_SSL)
#include "qhttpsslsocket.hpp"
#endif

#include "qhttpserver.hpp"

#include "qhttpserverconnection.hpp"
#include "httpparser.hxx"
#include "qhttpserverrequest.hpp"
#include "qhttpserverresponse.hpp"

#include "private/qhttpserverrequest_private.hpp"
#include "private/qhttpserverresponse_private.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace server {
///////////////////////////////////////////////////////////////////////////////
class QHttpConnectionPrivate
    : public details::HttpRequestParser<QHttpConnectionPrivate> {
  Q_DECLARE_PUBLIC(QHttpConnection)

public:
  explicit QHttpConnectionPrivate(QHttpConnection *q, QHttpServer *server,
                                  TBackend backendType)
      : q_ptr(q), iserver(server) {
    if (backendType == ETcpSocket) {
      isocket.reset(new details::QHttpTcpSocket);
    } else {
      isocket.reset(new details::QHttpLocalSocket);
    }
    isocket->ibackendType = backendType;
    QObject::connect(q_func(), &QHttpConnection::disconnected,
                     [this]() { release(); });
  }

  virtual ~QHttpConnectionPrivate() = default;


   void createSocket(qintptr sokDesc) {
    isocket->init(
        q_func(), sokDesc, [this]() { onReadyRead(); },
        [this]() { onWriteReady(); },
        [this]() { emit q_func()->disconnected(); });

/*
    if (isocket->ibackendType == ETcpSocket) {
      QTcpSocket *sok = new QTcpSocket(q_func());
      sok->setSocketDescriptor(sokDesc);
      isocket->itcpSocket = sok;
    } else if (isocket->ibackendType == ELocalSocket) {
      QLocalSocket *sok = new QLocalSocket(q_func());
      sok->setSocketDescriptor(sokDesc);
      isocket->ilocalSocket = sok;
    }
*/
  }

#if 0
  void createSocket(qintptr sokDesc, TBackend bend) {
    isocket.ibackendType = bend;

    if (bend == ETcpSocket) {
      initTcpSocket(sokDesc);

    } else if (bend == ELocalSocket) {
      initLocalSocket(sokDesc);
    }
   }
#endif

  void release() {
    // if socket drops and http_parser can not call
    // messageComplete, dispatch the ilastRequest
    finalizeConnection();

    isocket->disconnectAllQtConnections();
    isocket->release();

    if (ilastRequest) {
      ilastRequest->deleteLater();
      ilastRequest = nullptr;
    }

    if (ilastResponse) {
      ilastResponse->deleteLater();
      ilastResponse = nullptr;
    }

    q_func()->deleteLater();
  }

public:
    void onReadyRead() {
        while (isocket->bytesAvailable() > 0) {
          QByteArray buffer(isocket->readRaw());

      parse(buffer.constData(), buffer.size());
      if (iparser.http_errno != 0) {
        release(); // release the socket if parsing failed
        return;
      }
    }
  }

  void onWriteReady() {
    auto bytesWritten = isocket->bytesToWrite();
    if (bytesWritten == 0 && ilastResponse)
      emit ilastResponse->allBytesWritten();
  }

  void finalizeConnection() {
    if (ilastRequest == nullptr)
      return;

    ilastRequest->d_func()->finalizeSending(
        [this] { emit ilastRequest->end(); });
  }

public:
  int messageBegin(http_parser *parser);
  int url(http_parser *parser, const char *at, size_t length);
  int status(http_parser *, const char *, size_t) {
    return 0; // not used in parsing incoming request.
  }
  int headerField(http_parser *parser, const char *at, size_t length);
  int headerValue(http_parser *parser, const char *at, size_t length);
  int headersComplete(http_parser *parser);
  int body(http_parser *parser, const char *at, size_t length);
  int messageComplete(http_parser *parser);


protected:
  QHttpConnection *const q_ptr;
  QHttpServer *const iserver;

  QByteArray itempUrl;

  // Since there can only be one request/response pair per connection at any
  // time even with pipelining.
  QHttpRequest *ilastRequest = nullptr;
  QHttpResponse *ilastResponse = nullptr;

  ServerHandler ihandler = nullptr;
private:
#if 0
  void initTcpSocket(qintptr sokDesc) {
#if defined(QHTTP_HAS_SSL)
    const auto &sslConfig = iserver->sslConfig();
    if (sslConfig.hasLocal()) {
      auto sslsok = new ssl::Socket(q_func());
      sslsok->setup(sslConfig);
      sslsok->setSocketDescriptor(sokDesc);
      sslsok->startServerEncryption();
      sslsok->setPeerVerifyMode(QSslSocket::VerifyNone);
      isocket.itcpSocket = sslsok;

    } else
#endif
    {
      QTcpSocket *sok = new QTcpSocket(q_func());
      sok->setSocketDescriptor(sokDesc);
      isocket.itcpSocket = sok;
    }

    QObject::connect(isocket.itcpSocket, &QTcpSocket::readyRead,
                     [this]() { onReadyRead(); });
    QObject::connect(isocket.itcpSocket, &QTcpSocket::bytesWritten, [this]() {
      auto btw = isocket.itcpSocket->bytesToWrite();
      if (btw == 0 && ilastResponse)
        emit ilastResponse->allBytesWritten();
    });
    QObject::connect(isocket.itcpSocket, &QTcpSocket::disconnected, q_func(),
                     &QHttpConnection::disconnected, Qt::QueuedConnection);
  }

  void initLocalSocket(qintptr sokDesc) {
    QLocalSocket *sok = new QLocalSocket(q_func());
    isocket.ilocalSocket = sok;
    sok->setSocketDescriptor(sokDesc);

    QObject::connect(sok, &QLocalSocket::readyRead,
                     [this]() { onReadyRead(); });
    QObject::connect(sok, &QLocalSocket::bytesWritten, [this]() {
      auto btw = isocket.ilocalSocket->bytesToWrite();
      if (btw == 0 && ilastResponse)
        emit ilastResponse->allBytesWritten();
    });
    QObject::connect(sok, &QLocalSocket::disconnected, q_func(),
                     &QHttpConnection::disconnected, Qt::QueuedConnection);
  }


    

    void finalizeConnection() {
        if ( ilastRequest == nullptr )
            return;

        ilastRequest->pPrivate->finalizeSending([this]{
            emit ilastRequest->end();
        });
    }
#endif
};

///////////////////////////////////////////////////////////////////////////////
} // namespace server
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
#endif // QHTTPSERVER_CONNECTION_PRIVATE_HPP
