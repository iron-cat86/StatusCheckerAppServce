#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QLabel>      // <-- Добавляем
#include <QPushButton> // <-- Добавляем
#include <QVBoxLayout> // <-- Добавляем для размещения виджетов


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendRequest();
    void handleNetworkReply(QNetworkReply *reply);
    void resetCounter();

private:
    QLabel *labelCounter;
    QPushButton *pushButtonReset;
    QWidget *centerWidget; // Центральный виджет для фона
    QNetworkAccessManager *netManager;
    QTimer *timer;
    QSettings *config;
    int requestCounter;
    QString serviceUrl;
    int pollInterval;

    void loadConfig();
    void saveCounterToFile();
    void loadCounterFromFile();
    void logResult(const QString &result);
    
    void setupUiManual();
};
#endif // MAINWINDOW_H


