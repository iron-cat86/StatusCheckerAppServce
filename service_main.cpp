#include <QCoreApplication>
#include "simpleserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    SimpleServer server;
    server.startServer(8080); // Запускаем на порту 8080
    return a.exec();
}
