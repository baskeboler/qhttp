/** private imeplementation.
 * https://github.com/azadkuh/qhttp
 *
 * @author amir zamani
 * @version 2.0.0
 * @date 2014-07-11
 */

#ifndef QHTTPCLIENT_PRIVATE_HPP
#define QHTTPCLIENT_PRIVATE_HPP
///////////////////////////////////////////////////////////////////////////////

#include "qhttpclient.hpp"
#include "httpparser.hxx"
#include "qhttpclientrequest_private.hpp"
#include "qhttpclientresponse_private.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace client {
///////////////////////////////////////////////////////////////////////////////

class QHttpClientPrivate
    : public details::HttpResponseParser<QHttpClientPrivate> {
  Q_DECLARE_PUBLIC(QHttpClient)

public:
  explicit QHttpClientPrivate(QHttpClient *q, TBackend backendType) : q_ptr(q) {
    if (backendType == ETcpSocket)
      isocket.reset(new details::QHttpTcpSocket);
    else
      isocket.reset(new details::QHttpLocalSocket);

    QObject::connect(q_func(), &QHttpClient::disconnected,
                     [this]() { release(); });
  }

  virtual ~QHttpClientPrivate() = default;

  void release() {
    // if socket drops and http_parser can not call messageComplete,
    // dispatch the ilastResponse
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

    // must be called! or the later http_parser_execute() may fail
    http_parser_init(&iparser, HTTP_RESPONSE);
  }

  void initializeSocket() {
    if (isocket->isOpen()) {
      // no need to reconnect. do nothing and simply return
      if (ikeepAlive)
        return;

      // close previous connection now!
      // instead being called by emitted disconnected signal
      release();
    }

    ikeepAlive = false;

    isocket->init([this]() { onConnected(); }, [this]() { onReadyRead(); },
                  [this]() { onWriteReady(); },
                  [this]() { emit q_func()->disconnected(); });
  }

public:
  int messageBegin(http_parser *parser);
  int url(http_parser *, const char *, size_t) {
    return 0; // not used in parsing incoming respone.
  }
  int status(http_parser *parser, const char *at, int length);
  int headerField(http_parser *parser, const char *at, int length);
  int headerValue(http_parser *parser, const char *at, int length);
  int headersComplete(http_parser *parser);
  int body(http_parser *parser, const char *at, int length);
  int messageComplete(http_parser *parser);

protected:
  void onConnected() {
    iconnectingTimer.stop();

    if (itimeOut > 0)
      itimer.start(itimeOut, Qt::CoarseTimer, q_func());

    if (ireqHandler)
      ireqHandler(ilastRequest);
    else
      q_func()->onRequestReady(ilastRequest);
  }

  void onReadyRead() {
    while (isocket->bytesAvailable() > 0) {
      // char buffer[4097] = {0};
      // size_t readLength = static_cast<size_t>(isocket->readRaw(buffer,
      // 4096));
      // parse(buffer, readLength);
      QByteArray buffer(isocket->readRaw());
      parse(buffer.constData(), buffer.size());
      if (iparser.http_errno != 0) {
        release(); // release the socket if parsing failed
        return;
      }
    }
  }

  void onWriteReady() {
    if (isocket->bytesToWrite() == 0 && ilastRequest)
      emit ilastRequest->allBytesWritten();
  }

  void finalizeConnection() {
    if (ilastResponse == nullptr)
      return;

    ilastResponse->d_ptr->finalizeSending(
        [this] { emit ilastResponse->end(); });
  }

protected:
  QHttpClient *const q_ptr;

  QHttpRequest *ilastRequest = nullptr;
  QHttpResponse *ilastResponse = nullptr;
  TRequestHandler ireqHandler;
  TResponseHandler irespHandler;

  QBasicTimer iconnectingTimer;
};

///////////////////////////////////////////////////////////////////////////////
} // namespace client
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////

#endif // QHTTPCLIENT_PRIVATE_HPP
