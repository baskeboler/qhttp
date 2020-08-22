#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QUrlQuery>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <iostream>

#include "QHttp/QHttpServer"
#if defined(QHTTP_HAS_CLIENT)
#include "QHttp/QHttpClient"
#endif

#include "../include/ticktock.hxx"
#include "../include/unixcatcher.hpp"

///////////////////////////////////////////////////////////////////////////////
namespace {
///////////////////////////////////////////////////////////////////////////////
namespace client {
using namespace qhttp::client;
///////////////////////////////////////////////////////////////////////////////
struct Client {
  int start(const QString &ip, quint16 port, int count) {
    QObject::connect(&iclient, &QHttpClient::disconnected,
                     [this]() { finalize(); });

    QUrl url;
    url.setScheme("http");
    url.setHost(ip);
    url.setPort(port);

    iurl = url;
    icount = count;

    itick.tick();
    send();

    return qApp->exec();
  }

  void send() {
    iclient.post(
        iurl,
        [this](QHttpRequest *req) {
          QJsonObject root{
              {"name", "add"},
              {"stan", ++istan},
              {"args",
               QJsonArray{10, 14, -12}} // server computes sum of these values
          };

          auto body = QJsonDocument(root).toJson();
          req->addHeader("connection", "keep-alive");
          req->addHeaderValue("content-length", body.length());
          req->end(body);
        },
        [this](QHttpResponse *res) {
          res->collectData(512);

          res->onEnd([this, res]() { onIncomingData(res->body()); });
        });
  }

  void onIncomingData(const QByteArray &data) {
    auto root = QJsonDocument::fromJson(data).object();
    if (root.isEmpty()) {
      qDebug("the result is an invalid json\n");
      finalize();
      return;
    }

    if (root.value("stan").toInt() != istan) {
      qDebug("invalid stan number, %d != %d\n", istan,
             root.value("stan").toInt());
    }

    if (istan >= icount)
      finalize();
    else
      send();
  }

  void finalize() {
    qDebug("totally %d request/response pairs have been transmitted in %ld "
           "[mSec].\n",
           istan, itick.tock());

    QCoreApplication::quit();
  }

public:
  int icount = 0;
  int istan = 0; // system trace audit number
  QUrl iurl;
  QHttpClient iclient;
  am::TickTock itick;
}; // struct client
///////////////////////////////////////////////////////////////////////////////
} // namespace client
///////////////////////////////////////////////////////////////////////////////
namespace server {
using namespace qhttp::server;
///////////////////////////////////////////////////////////////////////////////
struct Server : public QHttpServer {
  using QHttpServer::QHttpServer;

  int start(quint16 port) {
    connect(this, &QHttpServer::newConnection, [this](QHttpConnection *) {
      printf("a new connection has been come!\n");
    });

    bool isListening = listen(
        QString::number(port), [this](QHttpRequest *req, QHttpResponse *res) {
          req->onData([this, req, res](QByteArray _data) {
            std::cout << _data.constData() << std::endl;
          });
          req->onEnd([this, req, res]() { process(req, res); });
        });

    if (!isListening) {
      qDebug("can not listen on %d!\n", port);
      return -1;
    }

    return qApp->exec();
  }

  void process(QHttpRequest *req, QHttpResponse *res) {

    auto root = QJsonDocument::fromJson(req->body()).object();

    if (root.isEmpty() ||
        root.value("name").toString() != QLatin1Literal("add")) {
      const static char KMessage[] = "Invalid json format!";
      res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
      res->addHeader("connection", "close");
      res->addHeaderValue("content-length", strlen(KMessage));
      res->end(KMessage);
      return;
    }

    int total = 0;
    auto args = root.value("args").toArray();
    for (const auto jv : args) {
      total += jv.toInt();
    }
    root["args"] = total;

    QByteArray body = QJsonDocument(root).toJson();
    res->addHeader("connection", "keep-alive");
    res->addHeaderValue("content-length", body.length());
    res->setStatusCode(qhttp::ESTATUS_OK);
    res->end(body);
  }
}; // struct server
///////////////////////////////////////////////////////////////////////////////
} // namespace server
///////////////////////////////////////////////////////////////////////////////
} // namespace
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  catchDefaultOsSignals();

  app.setApplicationName("keep-alive");
  app.setApplicationVersion("1.0.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("a server/client application to test the"
                                   " performance of keep-alive connections.");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("mode",
                               "mode: server or client. default: local");

  parser.addOption({{"p", "port"},
                    "tcp port number or UNIX socket path. default: 10022",
                    "number/path",
                    "10022"});
  parser.addOption(
      {{"i", "ip"}, "ip address of test server", "ip", "127.0.0.1"});
  parser.addOption(
      {{"c", "count"},
       "count of request/response pairs (only in client mode). default: 100",
       "number",
       "100"});
  parser.process(app);

  QStringList posList = parser.positionalArguments();
  if (posList.size() == 1) {
    const auto &mode = posList.at(0);

    if (mode == QLatin1Literal("server")) {
      server::Server server;
      return server.start(parser.value("port").toUShort());

    } else if (mode == QLatin1Literal("client")) {
      client::Client client;
      return client.start(parser.value("ip"), parser.value("port").toUShort(),
                          parser.value("count").toInt());
    }
  }

  parser.showHelp(0);
  return 0;
}
