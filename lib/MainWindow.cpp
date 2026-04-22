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
#include "SpectroControler.h"
#include "CSV_file_manager.h"

#include <iostream>

MainWindow::MainWindow(AppManager* appManager, QWidget *parent)
: QWidget(parent), appManager_(appManager) {
    save_images = false;
    pause_save_image = false;
    save_spectrum = false;
    time_between_save_ms = DEFAULT_PERIODE;
    time_last_save_cam330 = std::chrono::steady_clock::now();
    time_last_save_cam310 = std::chrono::steady_clock::now();
    time_last_save_spectrum = std::chrono::steady_clock::now();
    save_folder = "";
    image_cam330_counter = 0;
    image_cam310_counter = 0;

    master_gain = 1.0;
    diff_gain = 0.0;

    csv_file = NULL;

    //// LAYOUT ////

    mainLayout = new QVBoxLayout();

    CamLayout = new QHBoxLayout();
    CamBtLayout = new QVBoxLayout();
    CamVideoLayout = new QHBoxLayout();
    
    SpectrometerLayout = new QHBoxLayout();
    SpectrometerBtnLayout = new QVBoxLayout();
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
    btn_pause_save_images = new QPushButton("Pause Rec");
    label_periode = new QLabel("Acquire Timing :");
    spin_periode = new QSpinBox();
    spin_periode->setMinimum(MIN_ACQUIRE_TIME/1000);
    spin_periode->setMaximum(1000);
    spin_periode->setSuffix(" s");
    btn_recenter_cross = new QPushButton("Recenter crosses");
    btn_align_crosses = new QPushButton("Align crosses");
    btn_reset_images = new QPushButton("Cancel alignment");
    btn_exit = new QPushButton("Exit");
    label_exposure_time = new QLabel("Time Exposure :");
    checkBoxExposure = new QCheckBox("Time Exposure :");
    slider_exposure_time = new QSlider(Qt::Horizontal);
    slider_exposure_time->setMinimum(MIN_EXPOSURE_TIME);
    slider_exposure_time->setMaximum(MAX_EXPOSURE_TIME/2);
    slider_exposure_time->setValue(DEFAULT_EXPOSURE_TIME);
    slider_exposure_time->setSingleStep(25);
    slider_exposure_time->setPageStep(25);
    slider_exposure_time->setEnabled(false);
    label_gain = new QLabel("Master Gain :");
    checkBoxMasterGain = new QCheckBox("Master Gain :");
    slider_gain = new QSlider(Qt::Horizontal);
    slider_gain->setMinimum(0);
    slider_gain->setMaximum(100);
    slider_gain->setValue(50);
    slider_gain->setSingleStep(1);
    slider_gain->setPageStep(1);
    slider_gain->setEnabled(false);
    label_diff_gain = new QLabel("diff Gain :");
    checkBoxDiffGain = new QCheckBox("Diff Gain :");
    slider_diff_gain = new QSlider(Qt::Horizontal);
    slider_diff_gain->setMinimum(0);
    slider_diff_gain->setMaximum(100);
    slider_diff_gain->setValue(50);
    slider_diff_gain->setSingleStep(1);
    slider_diff_gain->setPageStep(1);
    slider_diff_gain->setEnabled(false);
    counter_image_rec = new QLabel("Acquire counter : ");

    CamBtLayout->addWidget(btn_connect_cam330);
    CamBtLayout->addWidget(btn_connect_cam310);
    CamBtLayout->addWidget(btn_recenter_cross);
    CamBtLayout->addWidget(btn_align_crosses);
    CamBtLayout->addWidget(btn_reset_images);
    CamBtLayout->addWidget(label_periode);
    CamBtLayout->addWidget(spin_periode);
    CamBtLayout->addWidget(btn_save_images);
    CamBtLayout->addWidget(btn_pause_save_images);
    // CamBtLayout->addWidget(label_exposure_time);
    CamBtLayout->addWidget(checkBoxExposure);
    CamBtLayout->addWidget(slider_exposure_time);
    // CamBtLayout->addWidget(label_gain);
    CamBtLayout->addWidget(checkBoxMasterGain);
    CamBtLayout->addWidget(slider_gain);
    // CamBtLayout->addWidget(label_diff_gain);
    CamBtLayout->addWidget(checkBoxDiffGain);
    CamBtLayout->addWidget(slider_diff_gain);
    CamBtLayout->addStretch();
    CamBtLayout->addWidget(counter_image_rec);
    CamBtLayout->addWidget(btn_exit);
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
    SpectrometerBtnLayout->setContentsMargins(10, 10, 10, 10);
    img_spectro = new QLabel("Spectro");
    img_spectro->setAlignment(Qt::AlignCenter);
    img_spectro->setMinimumSize(600, 100);
    btn_connect_spectro = new QPushButton("Connect Spectro");
    btn_acquire_spectro = new QPushButton("Start Rec");
    checkBox_spectro_gain = new QCheckBox("IntTime : " + QString::fromStdString(std::to_string(appManager_->Get_Spectrometer()->Get_integration_time())));
    slider_spectro_gain = new QSlider(Qt::Horizontal);
    slider_spectro_gain->setMinimum(500);
    slider_spectro_gain->setMaximum(60000);
    slider_spectro_gain->setValue(500);
    slider_spectro_gain->setSingleStep(1);
    slider_spectro_gain->setPageStep(1);
    slider_spectro_gain->setEnabled(false);
    checkBox_spectro_averaging = new QCheckBox("CoAdd : " + QString::fromStdString(std::to_string(appManager_->Get_Spectrometer()->Get_scans_to_average())));
    slider_spectro_averaging = new QSlider(Qt::Horizontal);
    slider_spectro_averaging->setMinimum(1);
    slider_spectro_averaging->setMaximum(100);
    slider_spectro_averaging->setValue(1);
    slider_spectro_averaging->setSingleStep(1);
    slider_spectro_averaging->setPageStep(1);
    slider_spectro_averaging->setEnabled(false);
    series_spectro = new QLineSeries();
    series_spectro->setPen(QPen(Qt::black, 1));
    chart_spectro = new QChart();
    chart_spectro->addSeries(series_spectro);
    chart_spectro->createDefaultAxes();
    // chart_spectro->setTitle("Spectre");
    chart_spectro->legend()->hide();
    chart_spectro->setMargins(QMargins(0, 0, 0, 0));
    chart_spectro->setBackgroundRoundness(0);
    chartView = new QChartView(chart_spectro);
    chartView->setRenderHint(QPainter::Antialiasing);
    SpectrometerBtnLayout->addWidget(btn_connect_spectro);
    SpectrometerBtnLayout->addWidget(btn_acquire_spectro);
    SpectrometerBtnLayout->addWidget(checkBox_spectro_gain);
    SpectrometerBtnLayout->addWidget(slider_spectro_gain);
    SpectrometerBtnLayout->addWidget(checkBox_spectro_averaging);
    SpectrometerBtnLayout->addWidget(slider_spectro_averaging);
    SpectrometerLayout->addLayout(SpectrometerBtnLayout, 1);
    SpectrometerLayout->addWidget(chartView, 5);
    // SpectrometerLayout->addWidget(img_spectro);
        
    mainLayout->addLayout(CamLayout, CAMLAYOUT_STRENGTH);

    group_spectrometer->setLayout(SpectrometerLayout);
    mainLayout->addWidget(group_spectrometer, SPECTROMETERLAYOUT_STRENGTH);

    mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(mainLayout);

    //// BT IMPLEMENTATION ////

    // Quit cross
    QObject::connect(qApp, &QApplication::lastWindowClosed, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("exit"));
        if(csv_file != NULL){
            if(!csv_file->is_saved()){
                csv_file->save();
            }
        }
        qApp->quit();
    });

    QObject::connect(btn_exit, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("exit"));
        if(csv_file != NULL){
            if(!csv_file->is_saved()){
                csv_file->save();
            }
        }
        qApp->quit();
    });

    QObject::connect(btn_connect_cam330, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("connect cam330"));
    });

    QObject::connect(btn_connect_cam310, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("connect cam310"));
    });

    QObject::connect(btn_connect_spectro, &QPushButton::clicked, [&]() {
        appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string("connect spectro"));
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

    QObject::connect(btn_pause_save_images, &QPushButton::clicked, [&]() {
        if(save_images && !pause_save_image) {
            save_images = false;
            pause_save_image = true;
            btn_pause_save_images->setText("Resume Rec");
            img_cam330->Set_is_reccorded(false);
            img_cam310->Set_is_reccorded(false);
        }
        else if(!save_images && pause_save_image) {
            save_images = true;
            pause_save_image = false;
            btn_pause_save_images->setText("Pause Rec");
            img_cam330->Set_is_reccorded(true);
            img_cam310->Set_is_reccorded(true);
        }
    });

    QObject::connect(spin_periode, &QSpinBox::valueChanged, [&](long value) {
        Set_Time_between_save(value * 1000);
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

    QObject::connect(checkBoxExposure, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            slider_exposure_time->setEnabled(true);
        } else {
            slider_exposure_time->setEnabled(false);
        }
    });

    QObject::connect(checkBoxMasterGain, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            slider_gain->setEnabled(true);
        } else {
            slider_gain->setEnabled(false);
        }
    });

    QObject::connect(checkBoxDiffGain, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            slider_diff_gain->setEnabled(true);
        } else {
            slider_diff_gain->setEnabled(false);
        }
    });

    QObject::connect(checkBox_spectro_gain, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            slider_spectro_gain->setEnabled(true);
        } else {
            slider_spectro_gain->setEnabled(false);
        }
    });

    QObject::connect(slider_spectro_gain, &QSlider::valueChanged, [&](int value) {
        if(appManager_->Get_Spectrometer()->Is_Connected()){
            appManager_->Get_Spectrometer()->Set_integration_time(value);
        }
        checkBox_spectro_gain->setText("IntTime : " + QString::fromStdString(std::to_string(appManager_->Get_Spectrometer()->Get_integration_time())));
    });

    QObject::connect(checkBox_spectro_averaging, &QCheckBox::toggled, [&](bool checked) {
        if (checked) {
            slider_spectro_averaging->setEnabled(true);
        } else {
            slider_spectro_averaging->setEnabled(false);
        }
    });
    
    QObject::connect(slider_spectro_averaging, &QSlider::valueChanged, [&](int value) {
        if(appManager_->Get_Spectrometer()->Is_Connected()){
            appManager_->Get_Spectrometer()->Set_scans_to_average(value);
        }
        checkBox_spectro_averaging->setText("CoAdd : " + QString::fromStdString(std::to_string(appManager_->Get_Spectrometer()->Get_scans_to_average())));
    });

    QObject::connect(btn_acquire_spectro, &QPushButton::clicked, [&]() {
        if(save_spectrum) {
            save_spectrum = false;
            btn_acquire_spectro->setText("Start Rec");
            csv_file->save();
            return;
        }

        if(!save_images) {
            QString folder = QFileDialog::getExistingDirectory(this, "Select a folder", "C:/", QFileDialog::ShowDirsOnly);
            if (!folder.isEmpty()) {
                save_folder = folder + "/";
            }
            else {
                return;
            }
        }

        save_spectrum = true;
        btn_acquire_spectro->setText("Stop Rec");
        // QString csv_name = "Spetrum_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") + ".csv";
        // csv_file = new CSV_file_manager(csv_name.toStdString(), save_folder.toStdString());
    });
}

void MainWindow::onNewSpectrum(std::vector<double> spectrum, std::vector<double> wavelengths) {
    if (spectrum.empty())
        return;

    QVector<QPointF> points;
    points.reserve(spectrum.size());

    for (int i = 0; i < spectrum.size(); ++i)
    {
        points.append(QPointF(wavelengths[i], spectrum[i]));
    }

    series_spectro->replace(points);

    chart_spectro->axes(Qt::Horizontal).first()->setRange(wavelengths[0], wavelengths[spectrum.size()-1]);

    auto axisY = chart_spectro->axes(Qt::Vertical).first();

    double minY = *std::min_element(spectrum.begin(), spectrum.end());
    // double maxY = *std::max_element(spectrum.begin(), spectrum.end());

    double maxY = 150.0;
    // double maxY = 150.0;
    for(double value : spectrum) {
        if(value > maxY) maxY = value;
    }
    
    axisY->setRange(minY, maxY);

    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(now - time_last_save_spectrum).count();

    if (save_spectrum && (elapsed >= time_between_save_ms)) {
        saving_spectrum(spectrum, wavelengths);
        // if(csv_file->Is_empty()) {
        //     std::vector<std::string> data;
        //     data.push_back("+++++++++++++++++++++++++");
        //     data.push_back("DATE : " + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss").toStdString());
        //     data.push_back("Spectrometer serial number : " + appManager_->Get_Spectrometer()->Get_serial_number());
        //     data.push_back("Integration Time : " + appManager_->Get_Spectrometer()->Get_integration_time());
        //     data.push_back("Spectral averaged : " + appManager_->Get_Spectrometer()->Get_scans_to_average());
        //     data.push_back("Number of pixel in file : 2048");
        //     data.push_back("begin : " + std::to_string(wavelengths[0]) + " nm");
        //     data.push_back("end : " + std::to_string(wavelengths[wavelengths.size()-1]) + " nm");
        //     data.push_back("Temperature : Unknow");
        //     data.push_back("+++++++++++++++++++++++++");
        //     int header_lentgth = data.size();
        //     data.push_back("nm\\time");
        //     for(double value : wavelengths) {
        //         data.push_back(std::to_string(value));
        //     }
        //     csv_file->Set_lines_header(data, header_lentgth);
        // }
        
        // std::vector<std::string> data;
        // data.push_back(QDateTime::currentDateTime().toString("hh/mm/ss").toStdString());
        // for(double value : spectrum) {
        //     data.push_back(std::to_string(value));
        // }
        // csv_file->push_colum_without_saving(data);
        // time_last_save_spectrum = std::chrono::steady_clock::now();
    }
}

void MainWindow::saving_spectrum(std::vector<double> spectrum, std::vector<double> wavelengths) {
    QString csv_name = "Spetrum_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") + ".csv";
    csv_file = new CSV_file_manager(csv_name.toStdString(), save_folder.toStdString());

    if(csv_file->Is_empty()) {
        std::vector<std::string> data;
        data.push_back("+++++++++++++++++++++++++");
        data.push_back("DATE : " + QDateTime::currentDateTime().toString("yyyy/MM/dd_hh:mm:ss").toStdString());
        data.push_back("Spectrometer serial number : " + appManager_->Get_Spectrometer()->Get_serial_number());
        data.push_back("Integration Time : " + std::to_string(appManager_->Get_Spectrometer()->Get_integration_time()));
        data.push_back("Spectral averaged : " + std::to_string(appManager_->Get_Spectrometer()->Get_scans_to_average()));
        // data.push_back("Number of pixel in file : 2048");
        // data.push_back("begin : " + std::to_string(wavelengths[0]) + " nm");
        // data.push_back("end : " + std::to_string(wavelengths[wavelengths.size()-1]) + " nm");
        // data.push_back("Temperature : Unknow");
        data.push_back("+++++++++++++++++++++++++");
        int header_lentgth = data.size();
        data.push_back("nm\\time");
        for(double value : wavelengths) {
            data.push_back(std::to_string(value));
        }
        csv_file->Set_lines_header(data, header_lentgth);
    }
    
    std::vector<std::string> data;
    data.push_back(QDateTime::currentDateTime().toString("hh/mm/ss").toStdString());
    for(double value : spectrum) {
        data.push_back(std::to_string(value));
    }
    csv_file->push_colum_without_saving(data);
    time_last_save_spectrum = std::chrono::steady_clock::now();
    csv_file->save();
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
    if(save_images || pause_save_image){
        save_images = false;
        pause_save_image = false;
        btn_save_images->setText("Start Rec");
        btn_pause_save_images->setText("Pause Rec");
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
        pause_save_image = false;
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