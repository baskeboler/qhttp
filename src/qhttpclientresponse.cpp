#include "private/qhttpclientresponse_private.hpp"
#include "qhttpclient.hpp"
///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace client {
///////////////////////////////////////////////////////////////////////////////
QHttpResponse::QHttpResponse(QHttpClient *cli)

    : QHttpAbstractInput(cli), d_ptr(new QHttpResponsePrivate(cli, this)) {
  d_ptr->initialize();
}

QHttpResponse::QHttpResponse(QHttpResponsePrivate &dd, QHttpClient *cli)
    : QHttpAbstractInput(cli), d_ptr(&dd) {
  d_ptr->initialize();
}

QHttpResponse::~QHttpResponse() = default;

TStatusCode QHttpResponse::status() const { return this->d_ptr->istatus; }

const QString &QHttpResponse::statusString() const {
  return this->d_ptr->icustomStatusMessage;
}

const QString &QHttpResponse::httpVersion() const {
  return this->d_ptr->iversion;
}

const Headers &QHttpResponse::headers() const { return this->d_ptr->iheaders; }

bool QHttpResponse::isSuccessful() const { return this->d_ptr->isuccessful; }

void QHttpResponse::collectData(int atMost) {
  this->d_ptr->collectData(atMost);
}

const QByteArray &QHttpResponse::body() const { return d_func()->ibody; }

QHttpClient *QHttpResponse::connection() const { return this->d_ptr->iclient; }

///////////////////////////////////////////////////////////////////////////////
} // namespace client
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
