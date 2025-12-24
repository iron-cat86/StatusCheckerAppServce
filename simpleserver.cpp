#include "simpleserver.h"
#include "logger.h"
#include <QSettings>

SimpleServer::SimpleServer(QString config, QObject *parent) : QObject(parent)
{
    serviceConfig = config;
    loadConfig();

    if (!Logger::getInstance().init(serviceLog)) {
        qWarning("Failed to initialize logger file!");
    }
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &SimpleServer::onNewConnection);
}

SimpleServer::~SimpleServer()
{
    Logger::getInstance().close();
}

void SimpleServer::loadConfig()
{
    QSettings *settings = new QSettings(serviceConfig, QSettings::IniFormat, this);
    port = settings->value("Settings/ServicePort", 8080).toInt();
    serviceLog = settings->value("Settings/ServiceLog", "service_log.log").toString();
}

void SimpleServer::startServer()
{
    if (tcpServer->listen(QHostAddress::LocalHost, port)) {
        QString result = "TCP Server listening on http://localhost:" + QString::number(port);
        Logger::getInstance().log(result);
        qDebug() << result;
    } else {
        QString result = "TCP Server failed to start:" + tcpServer->errorString();
        Logger::getInstance().log(result);
        qDebug() << result;
    }
}

void SimpleServer::onNewConnection()
{
    QTcpSocket *socket = tcpServer->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &SimpleServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &SimpleServer::onSocketDisconnected);
}

void SimpleServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray requestData = socket->readAll();
    QString requestString(requestData);

    //("GET /?requestID=123 HTTP/1.1")
    QString firstLine = requestString.split('\n').value(0);

    QString requestId = "N/A"; // Значение по умолчанию, если ID не найден

    if (firstLine.contains("requestID=")) {
        int startIndex = firstLine.indexOf("requestID=") + QString("requestID=").length();
        int endIndex = firstLine.indexOf(" ", startIndex);

        if (endIndex == -1) { // Если пробела после ID нет (конец строки)
            endIndex = firstLine.length();
        }

        requestId = firstLine.mid(startIndex, endIndex - startIndex);
    }

    int status = QRandomGenerator::global()->bounded(2);
    QString responseBody = QString::number(status);
    QByteArray responseData;

    responseData.append("HTTP/1.1 200 OK\r\n");
    responseData.append("Content-Type: text/plain\r\n");
    responseData.append(QString("X-Request-ID: %1\r\n").arg(requestId).toUtf8());

    responseData.append(QString("Content-Length: %1\r\n").arg(responseBody.length()).toUtf8());

    responseData.append("Connection: close\r\n");
    responseData.append("\r\n");
    responseData.append(responseBody.toUtf8());

    socket->write(responseData);
    socket->flush();
    socket->disconnectFromHost(); // Отправляем данные и закрываем сокет
    QString result =  "Responded on ID=" + requestId + " with status:" + QString::number(status) + " (Press Ctrl+C to stop gracefully)";
    Logger::getInstance().log(result);
    qDebug() << result;
}

void SimpleServer::onSocketDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}
