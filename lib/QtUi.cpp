
#include <semaphore>
#include "QtUI.h"
#include "InputHandler.h"
#include <QApplication>

void QtUi::Update_Output() {
    int argc = 0;
    QApplication Ui(argc, nullptr);
    
    window = new MainWindow(appManager_);  // ✅
    window->setWindowTitle("Camera controler interface");
    window->resize(800, 600);
    window->show();
    Ui.exec();
}

// void QtUi::Update_Output2() {
//     std::cout << "Output Thread running ..." << std::flush;

//     int argc = 0;
//     QApplication Ui(argc, nullptr);
//     // QLabel label("Hello");
//     // label.show();
//     // Ui.exec();

//     QWidget window;
//     window.setWindowTitle("Camera ");
//     window.resize(800, 600);

//     // Layout principal : une colonne
//     QHBoxLayout *mainLayout = new QHBoxLayout();

//     // Barre du haut : boutons
//     QVBoxLayout *topBar = new QVBoxLayout();
//     QPushButton *btnCam330 = new QPushButton("Connecter cam330");
//     QPushButton *btnCam310 = new QPushButton("Connecter cam310");
//     QPushButton *btnExit   = new QPushButton("Quitter");
//     btnCam330->setMinimumSize(40,40);
//     btnCam310->setMinimumSize(40,40);
//     btnExit->setMinimumSize(40,40);
//     topBar->addWidget(btnCam330);
//     topBar->addWidget(btnCam310);
//     // topBar->addStretch();   // pousse le bouton exit à droite
//     topBar->addWidget(btnExit);

//     // Zone images : deux cameras côte à côte
//     QVBoxLayout *imagesBar = new QVBoxLayout();
//     QLabel *imgCam330 = new QLabel("Cam330");
//     QLabel *imgCam310 = new QLabel("Cam310");
//     imgCam330->setAlignment(Qt::AlignCenter);
//     imgCam310->setAlignment(Qt::AlignCenter);
//     imgCam330->setMinimumSize(320, 256);
//     imgCam310->setMinimumSize(320, 256);
//     imagesBar->addWidget(imgCam330);
//     imagesBar->addWidget(imgCam310);

//     // Barre du bas : logs
//     QLabel *logLabel = new QLabel("Logs : ");
//     logLabel->setAlignment(Qt::AlignLeft);

//     // Assembler
//     mainLayout->addLayout(topBar);
//     mainLayout->addLayout(imagesBar);
//     // mainLayout->addWidget(logLabel);
//     window.setLayout(mainLayout);

//     // Connecter les boutons
//     QObject::connect(btnExit, &QPushButton::clicked, [&]() {
//         appManager_->Set_Is_Running(false);
//         Ui.quit();
//     });

//     window.show();
//     Ui.exec();
    
//     std::cout << "Output Thread closing ..." << std::endl;
// }

void QtUi::Update_Input() {
    
    // Not used


    // std::string user_input;
    
    // std::binary_semaphore& signal_Input_Handled = appManager_->Get_InputHandler()->Get_Input_Handled();

    // while(appManager_->Get_Is_Running()) {
    //     // std::getline(std::cin, user_input);
    //     user_input = "exit";

    //     appManager_->Get_InputHandler()->Get_InputQueue()->push(new std::string(user_input));

    //     signal_Input_Handled.acquire();
    // }
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