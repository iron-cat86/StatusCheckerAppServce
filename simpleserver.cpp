#include "simpleserver.h"

SimpleServer::SimpleServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &SimpleServer::onNewConnection);
}

void SimpleServer::startServer(quint16 port)
{
    if (tcpServer->listen(QHostAddress::LocalHost, port)) {
        qDebug() << "TCP Server listening on http://localhost:" << port;
    } else {
        qDebug() << "TCP Server failed to start:" << tcpServer->errorString();
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
    qDebug() << "Responded with status:" << status;
}

void SimpleServer::onSocketDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    socket->deleteLater();
}
