# Etna_UV_Camera
Projet de fin d'etude de mon diplome d'ingénieur de l'INSA de Lyon au sein de L'INGV. Ce projet consiste a la mise en place d'une camera UV qui observera l'Etna en continue. Les données seront transmise en temps réel à l'observatoir de l'INGV par ethernet.


Compilation : 
Use Cmake with compiler x86_x64 Cross Tools Command Prompt for VS.

Driver needed :

EBus : (take 6.5.4 64 bits if possible) https://www.jai.com/support-software/older-versions/

QT : (install the 6.10.2 for msvc2022_64) https://www.qt.io/development/download-qt-installer

Once drivers install compile in a build folder with x86_x64 commande line : "cmake -- build ."
