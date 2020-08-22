#include "private/qhttpserver_private.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace qhttp {
namespace server {
///////////////////////////////////////////////////////////////////////////////

QHttpServer::QHttpServer(QObject *parent)
    : QObject(parent), d_ptr(new QHttpServerPrivate) {
    connect(&d_func()->iwsServer, &QWebSocketServer::newConnection, this, &QHttpServer::forwardWsConnection);
}

QHttpServer::QHttpServer(QHttpServerPrivate &dd, QObject *parent)
    : QObject(parent), d_ptr(&dd) {
}

QHttpServer::~QHttpServer() {
    stopListening();
}

void
QHttpServer::setSslConfig(ssl::Config scnf) {
    d_func()->isslConfig = std::move(scnf);
}

const ssl::Config&
QHttpServer::sslConfig() const {
    return d_func()->isslConfig;
}

bool
QHttpServer::listen(const QString &socketOrPort, const TServerHandler &handler) {
    Q_D(QHttpServer);

    bool isNumber   = false;
    quint16 tcpPort = socketOrPort.toUShort(&isNumber);
    if ( isNumber    &&    tcpPort > 0 )
        return listen(QHostAddress::Any, tcpPort, handler);

    d->initialize(ELocalSocket, this);
    d->ihandler = handler;
    return d->ilocalServer->listen(socketOrPort);
}

bool
QHttpServer::listen(const QHostAddress& address, quint16 port, const qhttp::server::TServerHandler& handler) {
    Q_D(QHttpServer);

    d->initialize(ETcpSocket, this);
    d->ihandler = handler;
    return d->itcpServer->listen(address, port);
}

bool
QHttpServer::isListening() const {
    const Q_D(QHttpServer);

    if ( d->ibackend == ETcpSocket    &&    d->itcpServer )
        return d->itcpServer->isListening();

    else if ( d->ibackend == ELocalSocket    &&    d->ilocalServer )
        return d->ilocalServer->isListening();

    return false;
}

void
QHttpServer::stopListening() {
    Q_D(QHttpServer);

    if ( d->itcpServer )
        d->itcpServer->close();

    if ( d->ilocalServer ) {
        d->ilocalServer->close();
        QLocalServer::removeServer( d->ilocalServer->fullServerName() );
    }
}

quint32
QHttpServer::timeOut() const {
    return d_func()->itimeOut;
}

void
QHttpServer::setTimeOut(quint32 newValue) {
    d_func()->itimeOut = newValue;
}

TBackend
QHttpServer::backendType() const {
    return d_func()->ibackend;
}

void
QHttpServer::setProxyHeader(const QByteArray &header)
{
    d_func()->iproxyHeader = header.toLower();
}

void QHttpServer::forwardWsConnection()
{
    if (d_func()->iwsServer.hasPendingConnections())
    {
        emit newWsConnection(d_func()->iwsServer.nextPendingConnection());
    }
}

QTcpServer*
QHttpServer::tcpServer() const {
    Q_D(const QHttpServer);

    if (d->ibackend == ETcpSocket)
        return d->itcpServer.data();

    return nullptr;
}

QLocalServer*
QHttpServer::localServer() const {
    Q_D(const QHttpServer);

    if (d->ibackend == ELocalSocket)
        return d->ilocalServer.data();

    return nullptr;
}

void
QHttpServer::incomingConnection(qintptr handle) {
    Q_D(QHttpServer);

    QHttpConnection* conn = new QHttpConnection(this);
    conn->setSocketDescriptor(handle, backendType());
    conn->setTimeOut(d_func()->itimeOut);
    conn->setProxyHeader(d_func()->iproxyHeader);

    connect(conn, &QHttpConnection::newWebsocketUpgrade,
            this, &QHttpServer::onWebsocketUpgrade);

    emit newConnection(conn);

    if ( d->ihandler )
        QObject::connect(conn, &QHttpConnection::newRequest, d->ihandler);
    else
        incomingConnection(conn);
}

void QHttpServer::onWebsocketUpgrade(QTcpSocket *socket)
{
    socket->setParent(&d_func()->iwsServer);
    d_func()->iwsServer.handleConnection(socket);
    forwardWsConnection();
}

void
QHttpServer::incomingConnection(QHttpConnection *connection) {
    QObject::connect(connection,  &QHttpConnection::newRequest,
                     this,        &QHttpServer::newRequest);
}

///////////////////////////////////////////////////////////////////////////////
} // namespace server
} // namespace qhttp
///////////////////////////////////////////////////////////////////////////////
