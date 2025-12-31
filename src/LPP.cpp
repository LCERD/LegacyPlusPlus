//
// Created by zach on 9/1/25.
//
#include "LPP.h"

#include <iostream>
#include <ranges>
#include <QApplication>

#include "LCE/libLCEExports.h"
#include "GUI/Screens/Edit/EditScreen.h"

LPP::LegacyPlusPlus LPP::LegacyPlusPlus::sInstance = LegacyPlusPlus();

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LPP::LegacyPlusPlus::sInstance.run();
    return 0;
}

void LPP::LegacyPlusPlus::run() {
    std::cout << lce::lce_get_library_string() << std::endl;
    std::cout << "Starting LegacyPlusPlus v" << LPP::LegacyPlusPlus::VERSION << "\n";

    this->mainWindow = new GUI::Windows::MainWindow();
    this->mainWindow->show();

    QApplication::instance()->exec();
}

void LPP::LegacyPlusPlus::shutdown() {
    // do nothing for now
}
