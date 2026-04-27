
#include <semaphore>
#include "QtUI.h"
#include "InputHandler.h"
#include <QApplication>

void QtUi::Update_Output() {
    int argc = 0;
    QApplication Ui(argc, nullptr);
    
    window = new MainWindow(appManager_);
    window->setWindowTitle("Camera controler interface");
    window->resize(800, 600);
    window->show();
    Ui.exec();
}


void QtUi::Update_Input() {
    
    // Not used

    std::cout << "Input Thread closing ..." << std::endl;
}

void QtUi::Push_Frame(std::string* source_Name, uint8_t *data, uint32_t width, uint32_t height) {
    QImage img(data, width, height, width, QImage::Format_Grayscale8);
    QImage imgCopy = img.copy();

    QString source = QString::fromStdString(
        *(source_Name));

    QMetaObject::invokeMethod(
        window,
        "onNewFrame",
        Qt::QueuedConnection,
        Q_ARG(QString, source),
        Q_ARG(QImage, imgCopy)
    );
}

void QtUi::Push_Spectrum(std::vector <double> spectrum, std::vector <double> wavelengths) {
    QMetaObject::invokeMethod(
        window,
        "onNewSpectrum",
        Qt::QueuedConnection,
        Q_ARG(std::vector <double>, spectrum),
        Q_ARG(std::vector <double>, wavelengths)
    );
}

void QtUi::Ui_Print(std::string msg) {
    QMetaObject::invokeMethod(
        window,
        "printQt",
        Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(msg))
    );
}