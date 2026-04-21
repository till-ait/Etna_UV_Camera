#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <chrono>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QSlider>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QCheckBox>
#include <string>

#define DEFAULT_PERIODE 10000
#define MIN_ACQUIRE_TIME 2000    // 63

#define CAMLAYOUT_STRENGTH 6
#define SPECTROMETERLAYOUT_STRENGTH 4
#define CAMBTLAYOUT_STRENGTH 1
#define VIDEOLAYOUT_STRENGTH 5

#define VIDEO_WIDTH 50 
#define VIDEO_HEIGH 50


class AppManager;
class CamView;
class CSV_file_manager;


class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(AppManager* appManager, QWidget *parent = nullptr);

    void Set_Time_between_save(long value);

public slots:
    void onNewFrame(QString sourceName, QImage image);
    void onNewSpectrum(std::vector<double> spectrum, std::vector<double> wavelengths);
    void printQt(QString msg);

private:
    bool save_images;
    bool pause_save_image;
    bool save_spectrum;
    long time_between_save_ms;
    std::chrono::steady_clock::time_point time_last_save_cam330;
    std::chrono::steady_clock::time_point time_last_save_cam310;
    std::chrono::steady_clock::time_point time_last_save_spectrum;
    QString save_folder;
    CSV_file_manager *csv_file;

    AppManager* appManager_;

    QVBoxLayout *mainLayout;

    QHBoxLayout *CamLayout;
    QVBoxLayout *CamBtLayout;
    QHBoxLayout *CamVideoLayout;
    
    QHBoxLayout *SpectrometerLayout;
    QVBoxLayout *SpectrometerBtnLayout;
    QVBoxLayout *SpectrometerBtLayout;
    QVBoxLayout *SpectrometerVideoLayout;

    QPushButton *btn_connect_cam330;
    QPushButton *btn_connect_cam310;
    QPushButton *btn_select_save_folder;
    QPushButton *btn_save_images;
    QPushButton *btn_pause_save_images;
    QLabel *label_periode;
    QSpinBox *spin_periode;
    QPushButton *btn_recenter_cross;
    QPushButton *btn_align_crosses;
    QPushButton *btn_reset_images;
    QLabel *counter_image_rec;
    QLabel *label_exposure_time;
    QSlider *slider_exposure_time;
    QLabel *label_gain;
    QSlider *slider_gain;
    QLabel *label_diff_gain;
    QSlider *slider_diff_gain;
    QCheckBox *checkBoxExposure;
    QCheckBox *checkBoxMasterGain;
    QCheckBox *checkBoxDiffGain;
    QPushButton *btn_exit;

    QPushButton *btn_connect_spectro;
    QPushButton *btn_acquire_spectro;
    QCheckBox *checkBox_spectro_gain;
    QSlider *slider_spectro_gain;
    QCheckBox *checkBox_spectro_averaging;
    QSlider *slider_spectro_averaging;
    QLineSeries *series_spectro;
    QChart *chart_spectro;
    QChartView *chartView;

    QVBoxLayout *layout_cam330;
    QVBoxLayout *layout_cam310;
    CamView *img_cam330;
    CamView *img_cam310;

    QLabel *img_spectro;

    QGroupBox *group_cameras_control;
    QGroupBox *group_cam330;
    QGroupBox *group_cam310;
    QGroupBox *group_spectrometer;

    void Save_images_activation();
    void align_crosses();
    void update_gain();
    void saving_spectrum(std::vector<double> spectrum, std::vector<double> wavelengths);

    int image_cam330_counter;
    int image_cam310_counter;

    float master_gain;
    float diff_gain;
};

#endif