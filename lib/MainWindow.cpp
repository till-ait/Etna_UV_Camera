#include "MainWindow.h"
#include "AppManager.h"
#include "InputHandler.h"
#include <QApplication> 
#include <QDateTime>
#include <thread>

MainWindow::MainWindow(AppManager* appManager, QWidget *parent)
: QWidget(parent), appManager_(appManager) {
    save_images = false;
    time_between_save_ms = DEFAULT_PERIODE;
    time_last_save_cam330 = std::chrono::steady_clock::now();
    time_last_save_cam310 = std::chrono::steady_clock::now();
    save_folder = "";

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
    btn_select_save_folder = new QPushButton("Select save folder.");
    btn_save_images = new QPushButton("Save Images");
    btn_exit = new QPushButton("Exit");
    CamBtLayout->addWidget(btn_connect_cam330);
    CamBtLayout->addWidget(btn_connect_cam310);
    CamBtLayout->addWidget(btn_select_save_folder);
    CamBtLayout->addWidget(btn_save_images);
    CamBtLayout->addWidget(btn_exit);
    CamBtLayout->addStretch();
    CamLayout->addLayout(CamBtLayout, CAMBTLAYOUT_STRENGTH);

    CamVideoLayout->setContentsMargins(10, 10, 10, 10);
    img_cam330 = new QLabel("Cam330");
    img_cam310 = new QLabel("Cam310");
    img_cam330->setAlignment(Qt::AlignCenter);
    img_cam310->setAlignment(Qt::AlignCenter);
    img_cam330->setMinimumSize(VIDEO_WIDTH, VIDEO_HEIGH);
    img_cam310->setMinimumSize(VIDEO_WIDTH, VIDEO_HEIGH);
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

    QObject::connect(btn_select_save_folder, &QPushButton::clicked, [&]() {
        QString folder = QFileDialog::getExistingDirectory(this, "Select a folder", "C:/", QFileDialog::ShowDirsOnly);
        if (!folder.isEmpty()) save_folder = folder;
    });


    QObject::connect(btn_save_images, &QPushButton::clicked, [&]() {
        if(save_folder == ""){
            save_images = false;
            printQt("No destination folder selected.");
            return;
        }

        if(save_images){
            save_images = false;
            btn_save_images->setText("Save Images");
        }
        else {
            save_images = true;
            btn_save_images->setText("Stop Save");
        }
    });
}


void MainWindow::onNewFrame(QString sourceName, QImage image) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    if (sourceName == "cam330") {
        img_cam330->setPixmap(QPixmap::fromImage(image).scaled(
        img_cam330->size(), Qt::KeepAspectRatio));
        
        double elapsed = std::chrono::duration<double, std::milli>(now - time_last_save_cam330).count();

        if (save_images && (elapsed >= time_between_save_ms)) {
            QString filename = save_folder + sourceName + "_" + 
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".png";
            
            QImage imageCopy = image.copy();
            QString filenameCopy = filename;
            std::thread([imageCopy, filenameCopy]() {imageCopy.save(filenameCopy);}).detach();

            time_last_save_cam330 = std::chrono::steady_clock::now();
        }
    }
    else if (sourceName == "cam310") {
        img_cam310->setPixmap(QPixmap::fromImage(image).scaled(
        img_cam310->size(), Qt::KeepAspectRatio));

        double elapsed = std::chrono::duration<double, std::milli>(now - time_last_save_cam310).count();

        if (save_images && (elapsed >= time_between_save_ms)) {
            QString filename = save_folder + sourceName + "_" + 
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".png";
            
            QImage imageCopy = image.copy();
            QString filenameCopy = filename;
            std::thread([imageCopy, filenameCopy]() {imageCopy.save(filenameCopy);}).detach();

            time_last_save_cam310 = std::chrono::steady_clock::now();
        }
    }
}

void MainWindow::printQt(QString msg){
    QMessageBox::warning(this, "Error", msg);
}