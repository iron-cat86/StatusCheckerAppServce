#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDateTime>
#include <QMutex>

class Logger {
public:
    static Logger& getInstance();

    bool init(const QString& fileName);
    void log(const QString& message);
    void close();

private:
    Logger() = default;
    ~Logger();

    QFile file;
    QTextStream stream;
    QMutex mutex;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

#endif // LOGGER_H
