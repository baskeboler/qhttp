#include "private/qhttpserverrequest_private.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace server {
///////////////////////////////////////////////////////////////////////////////
QHttpRequest::QHttpRequest(QHttpConnection *conn)
    : QHttpAbstractInput(conn), d_ptr(new QHttpRequestPrivate(conn, this)) {
  d_ptr->initialize();
}

QHttpRequest::QHttpRequest(QHttpRequestPrivate &dd, QHttpConnection *conn)
    : QHttpAbstractInput(conn), d_ptr(&dd) {
  d_ptr->initialize();
}

QHttpRequest::~QHttpRequest() = default;

THttpMethod QHttpRequest::method() const { return this->d_ptr->imethod; }

const QString QHttpRequest::methodString() const {
  return http_method_str(static_cast<http_method>(this->d_ptr->imethod));
}

const QUrl &QHttpRequest::url() const { return this->d_ptr->iurl; }

const QString &QHttpRequest::httpVersion() const {
  return this->d_ptr->iversion;
}

const Headers &QHttpRequest::headers() const { return this->d_ptr->iheaders; }

const QString &QHttpRequest::remoteAddress() const {
  return this->d_ptr->iremoteAddress;
}

quint16 QHttpRequest::remotePort() const { return this->d_ptr->iremotePort; }

bool QHttpRequest::isSuccessful() const { return this->d_ptr->isuccessful; }

void QHttpRequest::collectData(int atMost) { this->d_ptr->collectData(atMost); }

const QByteArray &QHttpRequest::body() const { return d_func()->ibody; }

QHttpConnection *QHttpRequest::connection() const {
  return d_ptr ? this->d_ptr->iconnection : nullptr;
}

void QHttpRequest::addUserDefinedData(const QString &key, const QString value) {
  if (d_ptr)
    this->d_ptr->iuserDefinedValues.append(qMakePair(key, value));
}

QList<QPair<QString, QString>> QHttpRequest::userDefinedValues() const {
  return d_ptr ? this->d_ptr->iuserDefinedValues
               : QList<QPair<QString, QString>>();
}

///////////////////////////////////////////////////////////////////////////////
} // namespace server
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
