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
#include <QLockFile>
#include <QDir>
#include <QMessageBox>

extern "C"{
    #include "service.h"
}

int main(int argc, char *argv[]){

    
    QApplication app(argc,argv);
    app.setQuitOnLastWindowClosed(false);


    // Makes sure that the app is not launched twice at the same time cuz that would break the nodes
    QLockFile lockFile(QDir::temp().absoluteFilePath("audionoder.lock"));
    if(!lockFile.tryLock(100)){

        QMessageBox::warning(nullptr, "Already Running", "Audio Noder is already running in the system tray.");
        return 1;
    }

    MainWindow window;

    if(argc > 1 && QString(argv[1]) == "--show"){
        window.show();
    }
    

    return app.exec();
};