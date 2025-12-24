#include "logger.h"

Logger::~Logger() {
    close();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

bool Logger::init(const QString& fileName) {
    if (file.isOpen()) {
        file.close();
    }
    file.setFileName(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        stream.setDevice(&file);
        return true;
    }
    return false;
}

void Logger::log(const QString& message) {
    QMutexLocker locker(&mutex);
    if (file.isOpen()) {
        stream << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") 
               << " - " << message << "\n";
        stream.flush();
    }
}

void Logger::close() {
    QMutexLocker locker(&mutex);
    if (file.isOpen()) {
        file.close();
    }
}
