#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , requestCounter(0)
{
    setupUiManual(); 
    
    netManager = new QNetworkAccessManager(this);
    connect(netManager, &QNetworkAccessManager::finished, 
            this, &MainWindow::handleNetworkReply);

    loadConfig();
    
    loadCounterFromFile();
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::sendRequest);
    
    if (pollInterval > 0) {
        timer->start(pollInterval);
    } else {
        logResult("Ошибка: некорректный интервал опроса в конфиге.");
    }

    connect(pushButtonReset, &QPushButton::clicked, this, &MainWindow::resetCounter);

    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
}

MainWindow::~MainWindow()
{
    saveCounterToFile();
}

void MainWindow::setupUiManual() {
    centerWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centerWidget);

    labelCounter = new QLabel(QString("Запросов: %1").arg(requestCounter), centerWidget);
    labelCounter->setAlignment(Qt::AlignCenter);
    labelCounter->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    pushButtonReset = new QPushButton("Сбросить счетчик", centerWidget);

    layout->addWidget(labelCounter);
    layout->addWidget(pushButtonReset);
    
    centerWidget->setLayout(layout);
    setCentralWidget(centerWidget);
    resize(400, 200);
}

void MainWindow::loadConfig() {
    config = new QSettings("config.ini", QSettings::IniFormat, this);
    serviceUrl = config->value("Settings/ServiceUrl", "http://localhost:8080").toString();
    pollInterval = config->value("Settings/PollIntervalMs", 5000).toInt();
}

void MainWindow::sendRequest() {
    netManager->get(QNetworkRequest(QUrl(serviceUrl)));
    requestCounter++;
    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
    saveCounterToFile();
}

void MainWindow::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QString result = QString::fromUtf8(responseData).trimmed();

        if (result == "1") {
            this->setStyleSheet("background-color: green;");
            logResult("Ответ 1 (GREEN)");
        } else if (result == "0") {
            this->setStyleSheet("background-color: red;");
            logResult("Ответ 0 (RED)");
        } else {
            this->setStyleSheet("background-color: gray;"); // Неизвестный ответ
            logResult("Неизвестный ответ: " + result);
        }
    } else {
        this->setStyleSheet("background-color: orange;");
        logResult("Ошибка сети: " + reply->errorString());
    }
    reply->deleteLater();
}

void MainWindow::resetCounter() {
    requestCounter = 0;
    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
    saveCounterToFile();
    logResult("Счетчик сброшен пользователем.");
}

void MainWindow::logResult(const QString &result) {
    QFile file("app_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") 
               << " - " << result << "\n";
        file.close();
    }
}

void MainWindow::saveCounterToFile() {
    QFile file("counter.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << requestCounter;
        file.close();
    }
}

void MainWindow::loadCounterFromFile() {
    QFile file("counter.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString val = stream.readAll();
        requestCounter = val.toInt();
        file.close();
    }
}
