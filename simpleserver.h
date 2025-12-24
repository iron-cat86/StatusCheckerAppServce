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
    explicit SimpleServer(QString config = "config_service.ini", QObject *parent = nullptr);
    ~SimpleServer();
    void startServer();

private slots:
    void onNewConnection();
    void onReadyRead();
    void onSocketDisconnected();

private:
    void loadConfig();

    QString serviceConfig;
    QString serviceLog;
    QTcpServer *tcpServer;
    int port;
};

#endif // SIMPLESERVER_H
