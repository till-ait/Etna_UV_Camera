#include "MainWindow.h"
#include "AppManager.h"
#include "InputHandler.h"
#include <QApplication> 
#include <QDateTime>
#include <thread>

#include <iostream>

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

    group_cameras_control = new QGroupBox("Cemras Controls");
    layout_cam330 = new QVBoxLayout();
    group_cam330 = new QGroupBox("Cameras 330 nm");
    layout_cam310 = new QVBoxLayout();
    group_cam310 = new QGroupBox("Cameras 310 nm");
    group_spectrometer = new QGroupBox("Spectrometer");
    
    // TODO : il faut ajouter les differents bouton et les different layout

    CamLayout->setContentsMargins(0, 0, 0, 0);
    btn_connect_cam330 = new QPushButton("Connect cam330");
    btn_connect_cam310 = new QPushButton("Connect cam310");
    btn_select_save_folder = new QPushButton("Select save folder");
    btn_save_images = new QPushButton("Save Images");
    label_periode = new QLabel("Time between save :");
    spin_periode = new QSpinBox();
    spin_periode->setMinimum(63);
    spin_periode->setMaximum(1000000);
    spin_periode->setSuffix(" ms");
    btn_exit = new QPushButton("Exit");

    CamBtLayout->addWidget(btn_connect_cam330);
    CamBtLayout->addWidget(btn_connect_cam310);
    CamBtLayout->addWidget(btn_select_save_folder);
    CamBtLayout->addWidget(label_periode);
    CamBtLayout->addWidget(spin_periode);
    CamBtLayout->addWidget(btn_save_images);
    CamBtLayout->addWidget(btn_exit);
    CamBtLayout->addStretch();
    group_cameras_control->setLayout(CamBtLayout);
    CamLayout->addWidget(group_cameras_control, CAMBTLAYOUT_STRENGTH);

    CamVideoLayout->setContentsMargins(0, 0, 0, 0);
    img_cam330 = new QLabel("Cam330");
    img_cam310 = new QLabel("Cam310");
    img_cam330->setAlignment(Qt::AlignCenter);
    img_cam310->setAlignment(Qt::AlignCenter);
    img_cam330->setMinimumSize(VIDEO_WIDTH, VIDEO_HEIGH);
    img_cam310->setMinimumSize(VIDEO_WIDTH, VIDEO_HEIGH);
    layout_cam330->addWidget(img_cam330);
    group_cam330->setLayout(layout_cam330);
    CamVideoLayout->addWidget(group_cam330);
    layout_cam310->addWidget(img_cam310);
    group_cam310->setLayout(layout_cam310);
    CamVideoLayout->addWidget(group_cam310);
    // CamVideoLayout->addWidget(img_cam310);
    // CamVideoLayout->addStretch();
    CamLayout->addLayout(CamVideoLayout, VIDEOLAYOUT_STRENGTH);
    
    SpectrometerLayout->setContentsMargins(0, 0, 0, 0);
    img_spectro = new QLabel("Spectro");
    img_spectro->setAlignment(Qt::AlignCenter);
    img_spectro->setMinimumSize(600, 100);
    SpectrometerLayout->addWidget(img_spectro);  // TODO : Implement spectro layout
        
    mainLayout->addLayout(CamLayout, CAMLAYOUT_STRENGTH);

    group_spectrometer->setLayout(SpectrometerLayout);
    mainLayout->addWidget(group_spectrometer, SPECTROMETERLAYOUT_STRENGTH);

    mainLayout->setContentsMargins(10, 10, 10, 10);

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
        if (!folder.isEmpty()) save_folder = folder + "/";
    });


    QObject::connect(btn_save_images, &QPushButton::clicked, [&]() {
        if(save_folder == ""){
            save_images = false;
            printQt("No destination folder selected.");
            QString folder = QFileDialog::getExistingDirectory(this, "Select a folder", "C:/", QFileDialog::ShowDirsOnly);
            if (!folder.isEmpty()) save_folder = folder + "/";
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

    QObject::connect(spin_periode, &QSpinBox::valueChanged, [&](long value) {
        Set_Time_between_save(value);
    });
}


void MainWindow::onNewFrame(QString sourceName, QImage image) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    
    if (sourceName == "cam330") {
        img_cam330->setPixmap(QPixmap::fromImage(image).scaled(
        img_cam330->size(), Qt::KeepAspectRatio));
        
        double elapsed = std::chrono::duration<double, std::milli>(now - time_last_save_cam330).count();

        if (save_images && (elapsed >= time_between_save_ms)) {
            QString filename = save_folder + 
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") 
                    + "_" + sourceName + ".png";
            
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
            QString filename = save_folder + 
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") 
                    + "_" + sourceName + ".png";
            
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

void MainWindow::Set_Time_between_save(long value) {
    if(value > 62) {
        time_between_save_ms = value;
    }
}