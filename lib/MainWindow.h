#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>

#define CAMLAYOUT_STRENGTH 2
#define SPECTROMETERLAYOUT_STRENGTH 1
#define CAMBTLAYOUT_STRENGTH 1
#define VIDEOLAYOUT_STRENGTH 5


class AppManager;


class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(AppManager* appManager, QWidget *parent = nullptr);

public slots:
    // Appelé depuis le thread principal quand une image arrive
    void onNewFrame(QString sourceName, QImage image);

private:
    AppManager* appManager_;

    QVBoxLayout *mainLayout;

    QHBoxLayout *CamLayout;
    QVBoxLayout *CamBtLayout;
    QVBoxLayout *CamVideoLayout;
    
    QHBoxLayout *SpectrometerLayout;
    QVBoxLayout *SpectrometerBtLayout;
    QVBoxLayout *SpectrometerVideoLayout;

    QPushButton *btn_connect_cam330;
    QPushButton *btn_connect_cam310;
    QPushButton *btn_exit;

    QLabel *img_cam330;
    QLabel *img_cam310;

    QLabel *img_spectro;
};

#endif