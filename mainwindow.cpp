#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , requestCounter(0)
{
    if (!Logger::getInstance().init("app_log.log")) {
        qWarning("Failed to initialize logger file!");
    }
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
    connect(infoButton, &QPushButton::clicked, this, &MainWindow::on_infoButton_clicked);

    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
}

MainWindow::~MainWindow()
{
    Logger::getInstance().close();
    saveCounterToFile();
}

void MainWindow::setupUiManual() {
    setWindowTitle("Менеджер запросов");
    centerWidget = new QWidget(this);

    QVBoxLayout *layout = new QVBoxLayout(centerWidget);
    layout->addStretch();

    labelCounter = new QLabel(QString("Запросов: %1").arg(requestCounter), centerWidget);
    labelCounter->setAlignment(Qt::AlignCenter);
    labelCounter->setStyleSheet("font-size: 24px; font-weight: bold;");

    pushButtonReset = new QPushButton("Сбросить счетчик", centerWidget);
    infoButton = new QPushButton("Информация", centerWidget);

    labelCounter->adjustSize();

    QFontMetrics metrics(labelCounter->font());
    QString currentText = QString("Запросов: %1").arg(requestCounter);

    int textWidth = metrics.horizontalAdvance(currentText) + 50;

    labelCounter->setMinimumWidth(textWidth);
    pushButtonReset->setMinimumWidth(textWidth);
    infoButton->setMinimumWidth(textWidth);

    layout->addWidget(labelCounter);
    layout->addWidget(pushButtonReset, 0, Qt::AlignCenter);
    layout->addWidget(infoButton, 0, Qt::AlignCenter);
    layout->addStretch();

    centerWidget->setLayout(layout);

    setCentralWidget(centerWidget);
    resize(400, 200);
}

void MainWindow::on_infoButton_clicked()
{
    QString infoText = "Приложение Менеджкер запросов (v1.0)\n\n"
                       "Оправляет http-запросы на независимый сервис на порту 8080 каждые 5 секунд по умолчанию\n"
                       "(или по настройкам из файла config.ini)\n\n"
                       "🟢 Зеленый: Сервис доступен (ответ 1)\n"
                       "🔴 Красный: Сервис доступен (ответ 0)\n"
                       "\U0001F7E0 Оранжевый: Сервис не доступен\n\n"
                       "Счетчик запросов хранится в локальном файле counter.txt, а также вы его видите на экране\n"
                       "Логи в файле app_log.txt.\n\n"
                       "Кнопка \"Cбросить счетчик\" обнуляет счетчик\n\n"
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
}

void MainWindow::sendRequest() {
    QNetworkRequest request = QNetworkRequest(QUrl(serviceUrl));
    request.setTransferTimeout(requestTimeout);
    netManager->get(request);
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
     Logger::getInstance().log(result);
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
