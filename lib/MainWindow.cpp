#include "MainWindow.h"
#include "AppManager.h"
#include "InputHandler.h"
#include <QApplication> 
#include <QDateTime>
#include <QMouseEvent>
#include <QPainter>
#include <thread>
#include "CamView.h"
#include "CameraControler.h"

#include <iostream>

MainWindow::MainWindow(AppManager* appManager, QWidget *parent)
: QWidget(parent), appManager_(appManager) {
    save_images = false;
    time_between_save_ms = DEFAULT_PERIODE;
    time_last_save_cam330 = std::chrono::steady_clock::now();
    time_last_save_cam310 = std::chrono::steady_clock::now();
    save_folder = "";
    image_cam330_counter = 0;
    image_cam310_counter = 0;

    master_gain = 1.0;
    diff_gain = 0.0;

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

    CamLayout->setContentsMargins(0, 0, 0, 0);
    btn_connect_cam330 = new QPushButton("Connect cam330");
    btn_connect_cam310 = new QPushButton("Connect cam310");
    btn_select_save_folder = new QPushButton("Select save folder");
    btn_save_images = new QPushButton("Start Rec");
    label_periode = new QLabel("Acquire Timing :");
    spin_periode = new QSpinBox();
    spin_periode->setMinimum(MIN_ACQUIRE_TIME);
    spin_periode->setMaximum(1000000);
    spin_periode->setSuffix(" ms");
    btn_recenter_cross = new QPushButton("Recenter crosses");
    btn_align_crosses = new QPushButton("Align crosses");
    btn_reset_images = new QPushButton("Cancel alignment");
    btn_exit = new QPushButton("Exit");
    label_exposure_time = new QLabel("Time Exposure :");
    slider_exposure_time = new QSlider(Qt::Horizontal);
    slider_exposure_time->setMinimum(MIN_EXPOSURE_TIME);
    slider_exposure_time->setMaximum(MAX_EXPOSURE_TIME/2);
    slider_exposure_time->setValue(DEFAULT_EXPOSURE_TIME);
    slider_exposure_time->setSingleStep(25);
    slider_exposure_time->setPageStep(25);
    label_gain = new QLabel("Master Gain :");
    slider_gain = new QSlider(Qt::Horizontal);
    slider_gain->setMinimum(0);
    slider_gain->setMaximum(100);
    slider_gain->setValue(50);
    slider_gain->setSingleStep(1);
    slider_gain->setPageStep(1);
    label_diff_gain = new QLabel("diff Gain :");
    slider_diff_gain = new QSlider(Qt::Horizontal);
    slider_diff_gain->setMinimum(0);
    slider_diff_gain->setMaximum(100);
    slider_diff_gain->setValue(50);
    slider_diff_gain->setSingleStep(1);
    slider_diff_gain->setPageStep(1);
    counter_image_rec = new QLabel("Acquire counter : ");

    CamBtLayout->addWidget(btn_connect_cam330);
    CamBtLayout->addWidget(btn_connect_cam310);
    CamBtLayout->addWidget(btn_recenter_cross);
    CamBtLayout->addWidget(btn_align_crosses);
    CamBtLayout->addWidget(btn_reset_images);
    CamBtLayout->addWidget(label_periode);
    CamBtLayout->addWidget(spin_periode);
    CamBtLayout->addWidget(btn_save_images);
    CamBtLayout->addWidget(label_exposure_time);
    CamBtLayout->addWidget(slider_exposure_time);
    CamBtLayout->addWidget(label_gain);
    CamBtLayout->addWidget(slider_gain);
    CamBtLayout->addWidget(label_diff_gain);
    CamBtLayout->addWidget(slider_diff_gain);
    CamBtLayout->addWidget(btn_exit);
    CamBtLayout->addStretch();
    CamBtLayout->addWidget(counter_image_rec);
    group_cameras_control->setLayout(CamBtLayout);
    CamLayout->addWidget(group_cameras_control, CAMBTLAYOUT_STRENGTH);

    CamVideoLayout->setContentsMargins(0, 0, 0, 0);
    img_cam330 = new CamView();
    img_cam310 = new CamView();
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
    
    QObject::connect(btn_recenter_cross, &QPushButton::clicked, [&]() {
        img_cam330->RecenterCross();
        img_cam310->RecenterCross();
    });
    
    QObject::connect(btn_align_crosses, &QPushButton::clicked, [&]() {
        align_crosses();
    });
    
    QObject::connect(btn_reset_images, &QPushButton::clicked, [&]() {
        img_cam330->Set_Offset(0,0,0,0);
        img_cam310->Set_Offset(0,0,0,0);
        img_cam330->RecenterCross();
        img_cam310->RecenterCross();
    });

    QObject::connect(btn_save_images, &QPushButton::clicked, [&]() {
        Save_images_activation();
    });

    QObject::connect(spin_periode, &QSpinBox::valueChanged, [&](long value) {
        Set_Time_between_save(value);
    });

    QObject::connect(slider_exposure_time, &QSlider::valueChanged, [&](int value) {
        std::vector<CameraControler*>* cameras = appManager_->Get_Cameras();

        cameras->at(0)->Set_Exposure_Time(value);
        cameras->at(1)->Set_Exposure_Time(value);
    });

    QObject::connect(slider_gain, &QSlider::valueChanged, [&](int value) {
        master_gain = (value)/10.0;
        update_gain();
    });

    QObject::connect(slider_diff_gain, &QSlider::valueChanged, [&](int value) {
        diff_gain = ((value-50)/10.0);
        update_gain();
    });
}


void MainWindow::onNewFrame(QString sourceName, QImage image) {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    
    if (sourceName == "cam330") {
        img_cam330->setFrame(image);
        
        double elapsed = std::chrono::duration<double, std::milli>(now - time_last_save_cam330).count();

        if (save_images && (elapsed >= time_between_save_ms)) {
            image_cam330_counter++;
            QString filename = save_folder + 
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") 
                    + "_" + sourceName + "_"+ QString::number(image_cam330_counter) + ".png";
            
            QImage imageCopy = img_cam310->getLastImage();
            QString filenameCopy = filename;
            std::thread([imageCopy, filenameCopy]() {imageCopy.save(filenameCopy);}).detach();

            counter_image_rec->setText("Acquire counter : " + QString::number(image_cam330_counter));

            time_last_save_cam330 = std::chrono::steady_clock::now();
        }
    }
    else if (sourceName == "cam310") {
        img_cam310->setFrame(image);

        double elapsed = std::chrono::duration<double, std::milli>(now - time_last_save_cam310).count();
        if (save_images && (elapsed >= time_between_save_ms)) {
            image_cam310_counter++;
            QString filename = save_folder + 
                    QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") 
                    + "_" + sourceName + "_"+ QString::number(image_cam310_counter) + ".png";
            
            QImage imageCopy = img_cam310->getLastImage();
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


void MainWindow::Save_images_activation(){
    if(save_images){
        save_images = false;
        btn_save_images->setText("Start Rec");
        img_cam330->Set_is_reccorded(false);
        img_cam310->Set_is_reccorded(false);
    }
    else {
        QString folder = QFileDialog::getExistingDirectory(this, "Select a folder", "C:/", QFileDialog::ShowDirsOnly);
        if (!folder.isEmpty()) {
            save_folder = folder + "/";
        }
        else {
            return;
        }

        save_images = true;
        btn_save_images->setText("Stop Rec");
        
        image_cam330_counter = 0;
        image_cam310_counter = 0;

        img_cam330->Set_is_reccorded(true);
        img_cam310->Set_is_reccorded(true);
    }
}


void MainWindow::align_crosses() {
    int cam330_left_offset = 0;
        int cam330_right_offset = 0;
        int cam330_top_offset = 0;
        int cam330_bottom_offset = 0;
        int cam310_left_offset = 0;
        int cam310_right_offset = 0;
        int cam310_top_offset = 0;
        int cam310_bottom_offset = 0;

        QPointF cam330_offset = img_cam330->getOffsetInImagePixels();
        QPointF cam310_offset = img_cam310->getOffsetInImagePixels();

        if(cam330_offset.x() >= 0) {
            cam330_left_offset = cam330_offset.x()*2;
        }
        else {
            cam330_right_offset = cam330_offset.x()*(-1)*2;
        }

        if(cam330_offset.y() >= 0) {
            cam330_top_offset = cam330_offset.y()*2;
        }
        else {
            cam330_bottom_offset = cam330_offset.y()*(-1)*2;
        }

        if(cam310_offset.x() >= 0) {
            cam310_left_offset = cam310_offset.x()*2;
        }
        else {
            cam310_right_offset = cam310_offset.x()*(-1)*2;
        }

        if(cam310_offset.y() >= 0) {
            cam310_top_offset = cam310_offset.y()*2;
        }
        else {
            cam310_bottom_offset = cam310_offset.y()*(-1)*2;
        }

        int cropped_cam330_width = img_cam330->GetImgWidth() - cam330_left_offset - cam330_right_offset;
        int cropped_cam330_height = img_cam330->GetImgHeigh() - cam330_top_offset - cam330_bottom_offset;
        int cropped_cam310_width = img_cam310->GetImgWidth() - cam310_left_offset - cam310_right_offset;
        int cropped_cam310_height = img_cam310->GetImgHeigh() - cam310_top_offset - cam310_bottom_offset;
        
        if(cropped_cam330_width >= cropped_cam310_width) {
            cam330_left_offset = cam330_left_offset + ((cropped_cam330_width - cropped_cam310_width)/2);
            cam330_right_offset = cam330_right_offset + ((cropped_cam330_width - cropped_cam310_width)/2);
        }
        else {
            cam310_left_offset = cam310_left_offset + ((cropped_cam310_width - cropped_cam330_width)/2);
            cam310_right_offset = cam310_right_offset + ((cropped_cam310_width - cropped_cam330_width)/2);            
        }

        if(cropped_cam330_height >= cropped_cam310_height) {
            cam330_top_offset = cam330_top_offset + ((cropped_cam330_height - cropped_cam310_height)/2);
            cam330_bottom_offset = cam330_bottom_offset + ((cropped_cam330_height - cropped_cam310_height)/2);
        }
        else {
            cam310_top_offset = cam310_top_offset + ((cropped_cam310_height - cropped_cam330_height)/2);
            cam310_bottom_offset = cam310_bottom_offset + ((cropped_cam310_height - cropped_cam330_height)/2);            
        }

        img_cam330->Set_Offset(cam330_left_offset,
                                cam330_right_offset,
                                cam330_top_offset,
                                cam330_bottom_offset);

        img_cam310->Set_Offset(cam310_left_offset,
                                cam310_right_offset,
                                cam310_top_offset,
                                cam310_bottom_offset);

        img_cam330->RecenterCross();
        img_cam310->RecenterCross();
}

void MainWindow::update_gain() {
    std::vector<CameraControler*>* cameras = appManager_->Get_Cameras();
    
    if(diff_gain < 0) {
        cameras->at(0)->setGain(master_gain - diff_gain);
        cameras->at(1)->setGain(master_gain);
    }
    else {
        cameras->at(0)->setGain(master_gain);
        cameras->at(1)->setGain(master_gain + diff_gain);
    }

    
}