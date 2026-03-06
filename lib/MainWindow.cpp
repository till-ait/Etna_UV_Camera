#include "MainWindow.h"
#include "AppManager.h"
#include "InputHandler.h"
#include <QApplication> 

MainWindow::MainWindow(AppManager* appManager, QWidget *parent)
: QWidget(parent), appManager_(appManager) {
    //// LAYOUT ////

    mainLayout = new QVBoxLayout();

    CamLayout = new QHBoxLayout();
    CamBtLayout = new QVBoxLayout();
    CamVideoLayout = new QVBoxLayout();
    
    SpectrometerLayout = new QHBoxLayout();
    SpectrometerBtLayout = new QVBoxLayout();
    SpectrometerVideoLayout = new QVBoxLayout();
    
    // TODO : il faut ajouter les differents bouton et les different layout

    CamLayout->setContentsMargins(10, 10, 10, 10);
    btn_connect_cam330 = new QPushButton("Connect cam330");
    btn_connect_cam310 = new QPushButton("Connect cam310");
    btn_exit = new QPushButton("Exit");
    CamBtLayout->addWidget(btn_connect_cam330);
    CamBtLayout->addWidget(btn_connect_cam310);
    CamBtLayout->addWidget(btn_exit);
    CamBtLayout->addStretch();
    CamLayout->addLayout(CamBtLayout, CAMBTLAYOUT_STRENGTH);

    CamVideoLayout->setContentsMargins(10, 10, 10, 10);
    img_cam330 = new QLabel("Cam330");
    img_cam310 = new QLabel("Cam310");
    img_cam330->setAlignment(Qt::AlignCenter);
    img_cam310->setAlignment(Qt::AlignCenter);
    img_cam330->setMinimumSize(180, 180);
    img_cam310->setMinimumSize(180, 180);
    CamVideoLayout->addWidget(img_cam330);
    CamVideoLayout->addWidget(img_cam310);
    // CamVideoLayout->addStretch();
    CamLayout->addLayout(CamVideoLayout, VIDEOLAYOUT_STRENGTH);

    SpectrometerLayout->setContentsMargins(10, 10, 10, 10);
    img_spectro = new QLabel("Spectro");
    img_spectro->setAlignment(Qt::AlignCenter);
    img_spectro->setMinimumSize(600, 100);
    SpectrometerLayout->addWidget(img_spectro);  // TODO : Implement spectro layout

    mainLayout->addLayout(CamLayout, CAMLAYOUT_STRENGTH);
    mainLayout->addLayout(SpectrometerLayout, SPECTROMETERLAYOUT_STRENGTH);
    // mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(mainLayout);

    //// BT IMPLEMENTATION ////

    // Quit cross
    QObject::connect(qApp, &QApplication::lastWindowClosed, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("exit"));
        qApp->quit();
    });

    QObject::connect(btn_exit, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("exit"));
        qApp->quit();
    });

    QObject::connect(btn_connect_cam330, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("connect cam330"));
    });

    QObject::connect(btn_connect_cam310, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("connect cam310"));
    });
}


void MainWindow::onNewFrame(QString sourceName, QImage image) {
    // Ici on est dans le thread principal → Qt est content
    if (sourceName == "cam330") {
        img_cam330->setPixmap(QPixmap::fromImage(image).scaled(
        img_cam330->size(), Qt::KeepAspectRatio));
    }
    else if (sourceName == "cam310") {
        img_cam310->setPixmap(QPixmap::fromImage(image).scaled(
        img_cam310->size(), Qt::KeepAspectRatio));
    }
}