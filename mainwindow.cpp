#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , requestCounter(0)
{
    setupUiManual(); 
    
    // 1. Инициализация сетевого менеджера
    netManager = new QNetworkAccessManager(this);
    connect(netManager, &QNetworkAccessManager::finished, 
            this, &MainWindow::handleNetworkReply);

    // 2. Загрузка конфигурации из config.ini
    loadConfig();
    
    // 3. Загрузка счетчика из файла (хранение не в ОП)
    loadCounterFromFile();
    
    // 4. Настройка и запуск таймера
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::sendRequest);
    
    if (pollInterval > 0) {
        timer->start(pollInterval);
    } else {
        logResult("Ошибка: некорректный интервал опроса в конфиге.");
    }

    // 5. Подключение кнопки сброса счетчика (предполагаем, что у вас в UI есть кнопка с именем pushButtonReset)
    connect(pushButtonReset, &QPushButton::clicked, this, &MainWindow::resetCounter);

    // Инициализируем UI счетчиком
    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
}

MainWindow::~MainWindow()
{
    // Сохраняем счетчик при закрытии приложения
    saveCounterToFile();
}

void MainWindow::setupUiManual() {
    centerWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centerWidget);

    labelCounter = new QLabel(QString("Запросов: %1").arg(requestCounter), centerWidget);
    // Делаем текст счетчика большим и заметным
    labelCounter->setAlignment(Qt::AlignCenter);
    labelCounter->setStyleSheet("font-size: 24px; font-weight: bold;");
    
    pushButtonReset = new QPushButton("Сбросить счетчик", centerWidget);

    layout->addWidget(labelCounter);
    layout->addWidget(pushButtonReset);
    
    centerWidget->setLayout(layout);
    setCentralWidget(centerWidget);
    resize(400, 200); // Задаем размер окна по умолчанию
}

// --- СЛОТЫ И МЕТОДЫ РЕАЛИЗАЦИИ ---

// Метод 1: Загрузка настроек из config.ini
void MainWindow::loadConfig() {
    // Указываем, что файл config.ini находится рядом с исполняемым файлом
    config = new QSettings("config.ini", QSettings::IniFormat, this);
    serviceUrl = config->value("Settings/ServiceUrl", "http://localhost:8080").toString();
    // Интервал в миллисекундах. Значение по умолчанию 5000 мс (5 секунд)
    pollInterval = config->value("Settings/PollIntervalMs", 5000).toInt(); 
}

// Метод 2: Отправка запроса к сервису
void MainWindow::sendRequest() {
    netManager->get(QNetworkRequest(QUrl(serviceUrl)));
    requestCounter++;
    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
    // При каждом запросе обновляем файл счетчика на диске
    saveCounterToFile(); 
}

// Метод 3: Обработка ответа от сервиса (главная логика ТЗ)
void MainWindow::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QString result = QString::fromUtf8(responseData).trimmed();

        // Заливка интерфейса (устанавливаем QWidget style sheet для главного окна)
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
        // Ошибка сети или сервиса
        this->setStyleSheet("background-color: orange;"); 
        logResult("Ошибка сети: " + reply->errorString());
    }
    reply->deleteLater();
}

// Метод 4: Сброс счетчика запросов
void MainWindow::resetCounter() {
    requestCounter = 0;
    labelCounter->setText(QString("Запросов: %1").arg(requestCounter));
    saveCounterToFile(); // Сохраняем сброшенное значение
    logResult("Счетчик сброшен пользователем.");
}

// Метод 5: Запись в лог-файл app_log.txt
void MainWindow::logResult(const QString &result) {
    QFile file("app_log.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") 
               << " - " << result << "\n";
        file.close();
    }
}

// Метод 6 & 7: Загрузка/Сохранение счетчика на диск (чтобы не хранить в ОП)
void MainWindow::saveCounterToFile() {
    // Сохраняем в файл counter.txt
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
        requestCounter = val.toInt(); // Если файл пуст или не число, будет 0
        file.close();
    }
}
