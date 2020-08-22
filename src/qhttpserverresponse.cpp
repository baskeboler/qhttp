#include "private/qhttpserverresponse_private.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace server {
///////////////////////////////////////////////////////////////////////////////
QHttpResponse::QHttpResponse(QHttpConnection* conn)
    : QHttpAbstractOutput(conn) , d_ptr(new QHttpResponsePrivate(conn, this)) {
    d_ptr->initialize();
}

QHttpResponse::QHttpResponse(QHttpResponsePrivate& dd, QHttpConnection* conn)
    : QHttpAbstractOutput(conn) , d_ptr(&dd) {
    d_ptr->initialize();
}

QHttpResponse::~QHttpResponse() = default;

void
QHttpResponse::setStatusCode(TStatusCode code) {
    d_ptr->istatus   = code;
}

TStatusCode
QHttpResponse::statusCode() const {
    return d_ptr->istatus;
}

void
QHttpResponse::setVersion(const QString &versionString) {
    this->d_ptr->iversion  = versionString;
}

void
QHttpResponse::addHeader(const QByteArray &field, const QByteArray &value) {
    this->d_ptr->addHeader(field, value);
}

Headers&
QHttpResponse::headers() {
    return this->d_ptr->iheaders;
}

void
QHttpResponse::write(const QByteArray &data) {
    this->d_ptr->writeData(data);
}

void
QHttpResponse::end(const QByteArray &data) {
    if ( this->d_ptr->endPacket(data) )
        emit done(!this->d_ptr->ikeepAlive);
}

QHttpConnection*
QHttpResponse::connection() const {
    return this->d_ptr->iconnection;
}

///////////////////////////////////////////////////////////////////////////////
QByteArray
QHttpResponsePrivate::makeTitle() {

    QString title = QString("HTTP/%1 %2 %3\r\n")
                    .arg(iversion)
                    .arg(istatus)
                    .arg(Stringify::toString(istatus));

    return title.toLatin1();
}

void
QHttpResponsePrivate::prepareHeadersToWrite() {

    if ( !iheaders.contains("date") ) {
        // Sun, 06 Nov 1994 08:49:37 GMT - RFC 822. Use QLocale::c() so english is used for month and
        // day.
        QString dateString = QLocale::c().
            toString(QDateTime::currentDateTimeUtc(),
                    "ddd, dd MMM yyyy hh:mm:ss")
            .append(" GMT");
        addHeader("date", dateString.toLatin1());
    }
}

///////////////////////////////////////////////////////////////////////////////
} // namespace server
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
