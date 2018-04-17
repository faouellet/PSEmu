//#include "src/emulator/cpu/r3000a.h"
//#include "src/emulator/memory/bios.h"

#include <mainwindow.h>

#include <QApplication>

#include <iostream>

//using namespace PSEmu;

int main(int argc, char** argv)
{
    //BIOS bios;
    //bios.Init("SCPH1001.BIN");
    //
    //Interconnect interconnect{std::move(bios)};
    //Debugger debugger;
    //R3000A cpu{std::move(interconnect), std::move(debugger)};
    //
    //for(;;)
    //{
    //    cpu.Step();
    //}
    //
    //return 0;

    QApplication app{argc, argv};

    MainWindow window;
    window.show();

    return app.exec();
}