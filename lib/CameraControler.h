#ifndef CAMERACONTROLER_H
#define CAMERACONTROLER_H

#include <process.h>
#include <conio.h>

#include <stdio.h>
#include <string>
// #include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <list>


class AppManager;
class OutputPackage;

class CameraControler {
public :
    CameraControler(AppManager* _appManager_);

private :
    AppManager* appManager_;
};

#endif