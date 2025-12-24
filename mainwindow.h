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
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "logger.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void shown();

private slots:
    void sendRequest();
    void handleNetworkReply(QNetworkReply *reply);
    void resetCounter();
    void on_infoButton_clicked();

protected:
    void showEvent(QShowEvent *event) override;

private:
    QLabel *labelCounter;
    QPushButton *pushButtonReset;
    QPushButton *infoButton;
    QWidget *centerWidget; // Центральный виджет для фона
    QNetworkAccessManager *netManager;
    QTimer *timer;
    QSettings *config;
    int requestCounter;
    QString serviceUrl;
    QString appLog;
    int pollInterval;
    int requestTimeout;

    void loadConfig();
    void saveCounterToFile();
    void loadCounterFromFile();
    void logResult(const QString &result);
    
    void setupUiManual();
};
#endif // MAINWINDOW_H


