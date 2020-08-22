#include "private/qhttpclient_private.hpp"

#include <QTimerEvent>
///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace client {
///////////////////////////////////////////////////////////////////////////////

QHttpClient::QHttpClient(QObject *parent, qhttp::TBackend backendType)
    : QObject(parent), d_ptr(new QHttpClientPrivate(this, backendType)) {}

QHttpClient::QHttpClient(QHttpClientPrivate &dd, QObject *parent)
    : QObject(parent), d_ptr(&dd) {}

QHttpClient::~QHttpClient() = default;

quint32 QHttpClient::timeOut() const { return this->d_ptr->itimeOut; }

void QHttpClient::setTimeOut(quint32 t) { this->d_ptr->itimeOut = t; }

bool QHttpClient::isOpen() const { return this->d_ptr->isocket->isOpen(); }

void QHttpClient::killConnection() {
  this->d_ptr->iconnectingTimer.stop();
  this->d_ptr->itimer.stop();
  this->d_ptr->isocket->close();
}

TBackend QHttpClient::backendType() const {
  return (dynamic_cast<details::QHttpTcpSocket *>(
              this->d_ptr->isocket->isocket) != nullptr)
             ? ETcpSocket
             : ELocalSocket;
}

QTcpSocket *QHttpClient::tcpSocket() const {
  return backendType() == ETcpSocket
             ? dynamic_cast<QTcpSocket *>(this->d_ptr->isocket->isocket)
             : nullptr;
}

QLocalSocket *QHttpClient::localSocket() const {
  return backendType() == ELocalSocket
             ? dynamic_cast<QLocalSocket *>(this->d_ptr->isocket->isocket)
             : nullptr;
}

details::QHttpAbstractSocket *QHttpClient::abstractSocket() const {
  return this->d_ptr->isocket.data();
}

void QHttpClient::setConnectingTimeOut(quint32 timeout) {
  if (timeout == 0) {
    this->d_ptr->iconnectingTimer.stop();

  } else {
    this->d_ptr->iconnectingTimer.start(static_cast<int>(timeout),
                                        Qt::CoarseTimer, this);
  }
}

bool QHttpClient::request(THttpMethod method, QUrl url,
                          const TRequestHandler &reqHandler,
                          const TResponseHandler &resHandler) {
  this->d_ptr->ireqHandler = nullptr;
  this->d_ptr->irespHandler = nullptr;

  // if url is a local file (UNIX socket) the host could be empty!
  if (!url.isValid() || url.isEmpty() /*||    url.host().isEmpty()*/)
    return false;

  // process handlers
  if (resHandler) {
    this->d_ptr->irespHandler = resHandler;

    if (reqHandler)
      this->d_ptr->ireqHandler = reqHandler;
    else
      d_ptr->ireqHandler = [](QHttpRequest *req) -> void {
        req->addHeader("Connection", "close");
        req->end();
      };
  }

  auto requestCreator = [this, method, url]() {
    // create request object
    if (this->d_ptr->ilastRequest)
      this->d_ptr->ilastRequest->deleteLater();

    this->d_ptr->ilastRequest = new QHttpRequest(this);
    QObject::connect(this->d_ptr->ilastRequest, &QHttpRequest::done,
                     [this](bool wasTheLastPacket) {
                       this->d_ptr->ikeepAlive = !wasTheLastPacket;
                     });

    this->d_ptr->ilastRequest->d_ptr->imethod = method;
    this->d_ptr->ilastRequest->d_ptr->iurl = url;
  };

  // connecting to host/server must be the last thing. (after all function
  // handlers and ...) check for type
  if (url.scheme().toLower() == QLatin1String("file")) {
    // d->isocket->ibackendType = ELocalSocket;
    this->d_ptr->initializeSocket();

    requestCreator();

    if (this->d_ptr->isocket->isOpen())
      this->d_ptr->onConnected();
    else
      this->d_ptr->isocket->connectTo(url.path());

  } else {
    // d->isocket->ibackendType = ETcpSocket;
    this->d_ptr->initializeSocket();

    requestCreator();

    if (this->d_ptr->isocket->isOpen())
      this->d_ptr->onConnected();
    else
      this->d_ptr->isocket->connectTo(url.host(),
                                      static_cast<quint16>(url.port(80)));
  }

  return true;
}

void QHttpClient::timerEvent(QTimerEvent *e) {
  if (e->timerId() == this->d_ptr->itimer.timerId()) {
    killConnection();

  } else if (e->timerId() == this->d_ptr->iconnectingTimer.timerId()) {
    this->d_ptr->iconnectingTimer.stop();
    emit connectingTimeOut();
  }
}

void QHttpClient::onRequestReady(QHttpRequest *req) { emit httpConnected(req); }

void QHttpClient::onResponseReady(QHttpResponse *res) { emit newResponse(res); }

const QByteArray &QHttpClient::replyData() {
  Q_D(QHttpClient);
  return d->isocket->lastReadData;
}

const QByteArray &QHttpClient::requestData() {
  return d_ptr->ilastRequest->d_ptr->isocket->lastWrittenData;
}

///////////////////////////////////////////////////////////////////////////////

// if server closes the connection, ends the response or by any other reason
// the socket disconnects, then the irequest and iresponse instances may have
// been deleted. In these situations reading more http body or emitting end()
// for incoming request are not possible:
// if ( ilastRequest == nullptr )
//     return 0;

int QHttpClientPrivate::messageBegin(http_parser *) {
  itempHeaderField.clear();
  itempHeaderValue.clear();

  return 0;
}

int QHttpClientPrivate::status(http_parser *parser, const char *at,
                               int length) {
  if (ilastResponse)
    ilastResponse->deleteLater();

  ilastResponse = new QHttpResponse(q_func());
  ilastResponse->d_ptr->istatus = static_cast<TStatusCode>(parser->status_code);
  ilastResponse->d_ptr->iversion =
      QString("%1.%2").arg(parser->http_major).arg(parser->http_minor);
  ilastResponse->d_ptr->icustomStatusMessage = QString::fromUtf8(at, length);

  return 0;
}

int QHttpClientPrivate::headerField(http_parser *, const char *at, int length) {
  if (ilastResponse == nullptr)
    return 0;

  // insert the header we parsed previously
  // into the header map
  if (!itempHeaderField.isEmpty() && !itempHeaderValue.isEmpty()) {
    // header names are always lower-cased
    ilastResponse->d_ptr->iheaders.insert(itempHeaderField.toLower(),
                                          itempHeaderValue);
    // clear header value. this sets up a nice
    // feedback loop where the next time
    // HeaderValue is called, it can simply append
    itempHeaderField.clear();
    itempHeaderValue.clear();
  }

  itempHeaderField.append(at, length);
  return 0;
}

int QHttpClientPrivate::headerValue(http_parser *, const char *at, int length) {

  itempHeaderValue.append(at, length);
  return 0;
}

int QHttpClientPrivate::headersComplete(http_parser *) {
  if (ilastResponse == nullptr)
    return 0;

  // Insert last remaining header
  ilastResponse->d_ptr->iheaders.insert(itempHeaderField.toLower(),
                                        itempHeaderValue);

  if (irespHandler)
    irespHandler(ilastResponse);
  else
    q_func()->onResponseReady(ilastResponse);

  return 0;
}

int QHttpClientPrivate::body(http_parser *, const char *at, int length) {
  if (ilastResponse == nullptr)
    return 0;

  ilastResponse->d_ptr->ireadState = QHttpResponsePrivate::EPartial;

  if (ilastResponse->d_ptr->icollectRequired) {
    if (!ilastResponse->d_ptr->append(at, length)) {
      // forcefully dispatch the ilastResponse
      finalizeConnection();
    }

    return 0;
  }

  emit ilastResponse->data(QByteArray(at, length));
  return 0;
}

int QHttpClientPrivate::messageComplete(http_parser *) {
  if (ilastResponse == nullptr)
    return 0;

  // response is done
  finalizeConnection();
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
} // namespace client
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
