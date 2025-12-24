#include "mainwindow.h"
#include <QIcon>
#include <QSplashScreen>
#include <QTimer>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QPixmap>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString iconPath = ":/icon.png";
    QString imagePath = ":/splash.png";
    QPixmap pixmap(imagePath);

    if (pixmap.isNull()) {
        qDebug() << "ERROR: Failed to load splash image!";
    }
    QPixmap originalPixmap(iconPath);

    if (originalPixmap.isNull()) {
        qDebug() << "ERROR: Failed to load original image!";
    }

    QPixmap scaledIcon = originalPixmap.scaled(
            256, 256,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
    QIcon icon(scaledIcon);

    if(icon.availableSizes().isEmpty()) {
        qDebug()<<"ERROR: Fauld to load icon!";
    }
    a.setWindowIcon(icon);
    int desiredWidth = 800;
    int desiredHeight = 800;

    QPixmap scaledPixmap = pixmap.scaled(
            desiredWidth,
            desiredHeight,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
    QSplashScreen splash(scaledPixmap);
    splash.show();
    a.processEvents();
    MainWindow w;
    QTimer::singleShot(3000, &w, &MainWindow::show);
    QObject::connect(&w, &MainWindow::shown, &splash, &QSplashScreen::close);
    w.setWindowIcon(icon);
    return a.exec();
}
