#ifndef SIMPLESERVER_H
#define SIMPLESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QRandomGenerator>
#include <QDebug>

class SimpleServer : public QObject
{
    Q_OBJECT
public:
    explicit SimpleServer(QObject *parent = nullptr);
    ~SimpleServer();
    void startServer(quint16 port);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onSocketDisconnected();

private:
    QTcpServer *tcpServer;
};

#endif // SIMPLESERVER_H
