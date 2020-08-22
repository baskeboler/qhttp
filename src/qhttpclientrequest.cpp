#include "private/qhttpclientrequest_private.hpp"
#include "qhttpclient.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace client {
///////////////////////////////////////////////////////////////////////////////
QHttpRequest::QHttpRequest(QHttpClient *cli)

    : QHttpAbstractOutput(cli), d_ptr(new QHttpRequestPrivate(cli, this)) {
  d_ptr->initialize();
}

QHttpRequest::QHttpRequest(QHttpRequestPrivate &dd, QHttpClient *cli)
    : QHttpAbstractOutput(cli), d_ptr(&dd) {
  d_ptr->initialize();
}

QHttpRequest::~QHttpRequest() = default;

void QHttpRequest::setVersion(const QString &versionString) {
  this->d_ptr->iversion = versionString;
}

void QHttpRequest::addHeader(const QByteArray &field, const QByteArray &value) {
  this->d_ptr->addHeader(field, value);
}

Headers &QHttpRequest::headers() { return this->d_ptr->iheaders; }

void QHttpRequest::write(const QByteArray &data) {
  this->d_ptr->writeData(data);
}

void QHttpRequest::end(const QByteArray &data) {
  if (this->d_ptr->endPacket(data))
    emit done(!this->d_ptr->ikeepAlive);
}

QHttpClient *QHttpRequest::connection() const { return this->d_ptr->iclient; }

///////////////////////////////////////////////////////////////////////////////
QByteArray QHttpRequestPrivate::makeTitle() {

  QByteArray title;
  title.reserve(512);
  title.append(qhttp::Stringify::toString(imethod)).append(" ");

  QByteArray path = iurl.path(QUrl::FullyEncoded).toLatin1();
  if (path.size() == 0)
    path = "/";
  title.append(path);

  if (iurl.hasQuery())
    title.append("?").append(iurl.query(QUrl::FullyEncoded).toLatin1());

  title.append(" HTTP/").append(iversion.toLatin1()).append("\r\n");

  return title;
}

void QHttpRequestPrivate::prepareHeadersToWrite() {

  if (!iheaders.contains("Host")) {
    int port = iurl.port();
    if (port == -1 || port == 0) {
      if (iurl.scheme().toLower() == QString("http"))
        port = 80; // HTTP
      else
        port = 443; // HTTPS
    }

    iheaders.insert("Host",
                    QString("%1:%2").arg(iurl.host()).arg(port).toLatin1());
  }
}

///////////////////////////////////////////////////////////////////////////////
} // namespace client
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
