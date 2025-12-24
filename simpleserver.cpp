#include "simpleserver.h"
#include "logger.h"

SimpleServer::SimpleServer(QObject *parent) : QObject(parent)
{
    if (!Logger::getInstance().init("service_log.log")) {
        qWarning("Failed to initialize logger file!");
    }
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &SimpleServer::onNewConnection);
}

SimpleServer::~SimpleServer()
{
    Logger::getInstance().close();
}

void SimpleServer::startServer(quint16 port)
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

    int status = QRandomGenerator::global()->bounded(2);
    QString responseBody = QString::number(status);
    QByteArray responseData;

    responseData.append("HTTP/1.1 200 OK\r\n");
    responseData.append("Content-Type: text/plain\r\n");

    responseData.append(QString("Content-Length: %1\r\n").arg(responseBody.length()).toUtf8());

    responseData.append("Connection: close\r\n");
    responseData.append("\r\n");
    responseData.append(responseBody.toUtf8());

    socket->write(responseData);
    socket->flush();
    socket->disconnectFromHost(); // Отправляем данные и закрываем сокет
    QString result =  "Responded with status:" + QString::number(status) + " (Press Ctrl+C to stop gracefully)";
    Logger::getInstance().log(result);
    qDebug() << result;
}

void SimpleServer::onSocketDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}
