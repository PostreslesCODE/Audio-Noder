#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "MainWindow.h"

// Qt includes
#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QMenu>

extern "C"{
    #include "service.h"
}

int main(int argc, char *argv[]){

    
    QApplication app(argc,argv);
    app.setQuitOnLastWindowClosed(false);


    MainWindow window;

    if(argc > 1 && QString(argv[1]) == "--show"){
        window.show();
    }
    

    return app.exec();
};