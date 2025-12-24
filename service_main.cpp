#include <QCoreApplication>
#include <QDebug>
#include "simpleserver.h"
#include <csignal>

QCoreApplication *app_ptr = nullptr;

void handle_sigint(int signal) {
    if (signal == SIGINT) {
        qDebug() << "Received SIGINT (Ctrl+C). Initiating graceful shutdown...";
        if (app_ptr) {
            app_ptr->quit();
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    app_ptr = &a;

    std::signal(SIGINT, handle_sigint);

    SimpleServer *server;

    if(argc > 1)
        server = new SimpleServer(argv[1]);
    else
        server = new SimpleServer();
    server->startServer();

    qDebug() << "Service is running. Press Ctrl+C to stop gracefully.";

    int returnCode = a.exec();

    qDebug() << "Service shutdown complete.";
    delete server;
    return returnCode;
}
