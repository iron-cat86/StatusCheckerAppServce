#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , requestCounter(0)
    , posCounter(0)
    , negCounter(0)
    , failedCounter(0)
{
    setupUiManual();
    
    netManager = new QNetworkAccessManager(this);
    connect(netManager, &QNetworkAccessManager::finished,
            this, &MainWindow::handleNetworkReply);

    loadConfig();
    if (!Logger::getInstance().init(appLog)) {
        qWarning("Failed to initialize logger file!");
    }
    loadCounterFromFile();
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::sendRequest);
    
    if (pollInterval > 0) {
        timer->start(pollInterval);
    } else {
        logResult("Ошибка: некорректный интервал опроса в конфиге.");
    }

    connect(pushButtonReset, &QPushButton::clicked, this, &MainWindow::resetCounter);
    connect(infoButton, &QPushButton::clicked, this, &MainWindow::on_infoButton_clicked);

    labelCounter->setText(QString("Всего запросов: %1").arg(requestCounter));
    labelPosCounter->setText(QString("Зеленных запросов: %1").arg(posCounter));
    labelNegCounter->setText(QString("Красных запросов: %1").arg(negCounter));
    labelFailedCounter->setText(QString("Неотвеченных запросов: %1").arg(failedCounter));
}

MainWindow::~MainWindow()
{
    Logger::getInstance().close();
    saveCounterToFile();
}

void MainWindow::setupUiManual() {
    setWindowTitle("Менеджер запросов");
    centerWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centerWidget);
    mainLayout->addStretch(); // Растяжка сверху

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(20);

    labelCounter = new QLabel(QString("Всего запросов: %1").arg(requestCounter), centerWidget);
    labelCounter->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(labelCounter, 0, 0, 1, 2);

    labelPosCounter   = new QLabel(QString("Зеленых запросов: %1").arg(posCounter), centerWidget);
    labelNegCounter   = new QLabel(QString("Краснных запросов: %1").arg(negCounter), centerWidget);
    labelFailedCounter= new QLabel(QString("Неотвеченных запросов: %1").arg(failedCounter), centerWidget);

    gridLayout->addWidget(labelPosCounter,   1, 0, Qt::AlignRight);
    gridLayout->addWidget(labelNegCounter,   1, 1, Qt::AlignLeft);
    gridLayout->addWidget(labelFailedCounter,2, 0, 1, 2, Qt::AlignCenter); // По центру под ними

    pushButtonReset = new QPushButton("Сбросить счетчики", centerWidget);
    infoButton = new QPushButton("Информация", centerWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(pushButtonReset);
    buttonLayout->addWidget(infoButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(buttonLayout);

    mainLayout->addStretch(); // Растяжка снизу
    centerWidget->setLayout(mainLayout);
    setCentralWidget(centerWidget);
    resize(400, 300);
}

void MainWindow::on_infoButton_clicked()
{
    QString infoText = "Приложение Менеджкер запросов (v1.0)\n\n"
                       "Оправляет http-запросы на независимый сервис на порту 8080 каждые 5 секунд по умолчанию\n"
                       "(или по настройкам из файла config.ini)\n\n"
                       "🟢 Зеленый: Сервис доступен (ответ 1)\n"
                       "🔴 Красный: Сервис доступен (ответ 0)\n"
                       "\U0001F7E0 Оранжевый: Сервис не доступен\n\n"
                       "Счетчики запросов хранятся в локальном файле counter.txt в формате <total>:<green>:<red>:<failed>, а также вы их видите на экране\n"
                       "Логи в файле app_log.txt.\n\n"
                       "Кнопка \"Cбросить счетчики\" обнуляет все счетчики\n\n"
                       "Разработчик: Анна Белова, Dec. 2025";

    QMessageBox::information(
        this,
        "Справка о приложении",
        infoText
    );
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    emit shown();
}

void MainWindow::loadConfig() {
    config = new QSettings("config.ini", QSettings::IniFormat, this);
    serviceUrl = config->value("Settings/ServiceUrl", "http://localhost:8080").toString();
    pollInterval = config->value("Settings/PollIntervalMs", 5000).toInt();
    requestTimeout = config->value("Settings/RequestTimeoutMs", 2000).toInt();
    appLog = config->value("Settings/AppLog", "app_log.log").toString();
}

void MainWindow::sendRequest() {
    QNetworkRequest request = QNetworkRequest(QUrl(serviceUrl));
    request.setTransferTimeout(requestTimeout);
    netManager->get(request);
    requestCounter++;
    labelCounter->setText(QString("Всего запросов: %1").arg(requestCounter));
    saveCounterToFile();
}

void MainWindow::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QString result = QString::fromUtf8(responseData).trimmed();

        if (result == "1") {
            this->setStyleSheet("background-color: green;");
            logResult("Ответ 1 (GREEN)");
            posCounter++;
            labelPosCounter->setText(QString("Зеленых запросов: %1").arg(posCounter));
        } else if (result == "0") {
            this->setStyleSheet("background-color: red;");
            logResult("Ответ 0 (RED)");
            negCounter++;
            labelNegCounter->setText(QString("Красных запросов: %1").arg(negCounter));
        } else {
            this->setStyleSheet("background-color: gray;"); // Неизвестный ответ
            logResult("Неизвестный ответ: " + result);
        }
    } else {
        this->setStyleSheet("background-color: orange;");
        logResult("Ошибка сети: " + reply->errorString());
        failedCounter++;
        labelFailedCounter->setText(QString("Неотвеченных запросов: %1").arg(failedCounter));
    }
    reply->deleteLater();
}

void MainWindow::resetCounter() {
    requestCounter = 0;
    posCounter = 0;
    negCounter = 0;
    failedCounter = 0;
    labelCounter->setText(QString("Всего запросов: %1").arg(requestCounter));
    labelPosCounter->setText(QString("Зеленых запросов: %1").arg(posCounter));
    labelNegCounter->setText(QString("Красных запросов: %1").arg(negCounter));
    labelFailedCounter->setText(QString("Неотвеченных запросов: %1").arg(failedCounter));
    saveCounterToFile();
    logResult("Счетчик сброшен пользователем.");
}

void MainWindow::logResult(const QString &result) {
     Logger::getInstance().log(result);
}

void MainWindow::saveCounterToFile() {
    QFile file("counter.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << requestCounter << ":" << posCounter << ":" << negCounter << ":" << failedCounter;
        file.close();
    }
}

void MainWindow::loadCounterFromFile() {
    QFile file("counter.txt");
    QString result;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result = "File is opened";
        QTextStream stream(&file);

        QString fileContents = stream.readAll();
        file.close();

        QStringList counts = fileContents.split(':');

        if (counts.size() >= 1) {
            requestCounter = counts.at(0).toInt();
            result += ", total count: " + QString::number(requestCounter);
            if(counts.size() >= 2) {
                posCounter     = counts.at(1).toInt();
                result += ", green count: " + QString::number(posCounter);

                if(counts.size() >= 3) {
                    negCounter     = counts.at(2).toInt();
                    result += ", red count: " + QString::number(negCounter);

                    if(counts.size() >= 4) {
                        failedCounter  = counts.at(3).toInt();
                        result += ", failed count: " + QString::number(failedCounter);
                    } else {
                        failedCounter = 0;
                        result += ", no failed count";
                    }
                } else {
                    negCounter = 0;
                    result += ", no red count";
                }
            } else {
                posCounter = 0;
                result += ", no green count";
            }
        } else {
            requestCounter = 0;
            result += ", no total count";
        }
    } else
        result = "File is not opened.";
    logResult(result);
    qDebug()<<result;
}
