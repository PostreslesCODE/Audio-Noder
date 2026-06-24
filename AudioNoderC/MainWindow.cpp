    #include "MainWindow.h"

    extern "C"{
        #include "service.h"
    }

    //QT includes
    #include <QVBoxLayout>
    #include <QAction>
    #include <QApplication>
    #include <QIcon>
    #include <QPushButton>
    #include <QWidget>
    #include <QSystemTrayIcon>
    #include <QMenu>
    #include <QFile>
    #include <QMessageBox>
    #include <QTimer>

        MainWindow::MainWindow(QWidget *parent) : QWidget(parent){ // Constructor for Main Window
            setupUI();
            setupTray();
            setupConnections();
            if(!QFile::exists("config.json")) {
                QMessageBox::critical(this, "Error", "config.json not found");
                QTimer::singleShot(0, qApp, &QApplication::quit);
                return;
            }
            serviceRunning = true;
            pthread_create(&thread1,NULL,serviceThread,NULL);
        };

        MainWindow::~MainWindow(){ // Destructor for Main Window

        }


        void MainWindow::setupUI(){ // Setups the window, as we are now creating it with a constructor, we are referencing it with "this"

            // Main window
            setWindowTitle("Audio Noder");
            setWindowIcon(QIcon(":/icons/logo.png"));
            resize(600,400);

            //  Layout
            QVBoxLayout *layout = new QVBoxLayout(this);
            
            //Buttons
            startButton = new QPushButton("Start Service");
            stopButton = new QPushButton("Stop");

            startButton->setEnabled(false);

            layout->addWidget(startButton);
            layout->addWidget(stopButton);

        };


        void MainWindow::setupTray(){
            
            // Creation of the tray menu
            trayMenu = new QMenu(this);
            trayMenu -> addAction("Show/Hide", this, [this](){
             isVisible() ? hide() : (show(),raise(), activateWindow());   
            });

            trayMenu->addSeparator();
            trayMenu->addAction("Quit", qApp, [](){
                qDebug() << "Quit clicked";
                qApp->quit();
            });

            tray = new QSystemTrayIcon(QIcon(":/icons/logo.png"));
            tray -> setContextMenu(trayMenu);
            tray -> setToolTip("Audio Noder");
            tray -> show();

        };

        void MainWindow::closeEvent(QCloseEvent *event){
            hide();
            event->ignore();

        };


        void MainWindow::setupConnections(){ // This is where the ui elements exist and their behaviour and functionality is defined

            // [&](){} is a lambda function, instead of defining it on top separatly its defined inside
            // [&](){pthread_create(&thread1,NULL,serviceThread,NULL);}


            connect(
                startButton,
                &QPushButton::clicked,
                [this](){
                    if (!QFile::exists("config.json")) {
                        QMessageBox::critical(this, "Error", "config.json not found");
                        return;
                    }

                    startButton -> setEnabled(false);
                    stopButton -> setEnabled(true);
                    serviceRunning = true;
                    pthread_create(&thread1,NULL,serviceThread,NULL);
                }
            );


            connect(
                stopButton,
                &QPushButton::clicked,
                [this](){

                    stopButton -> setEnabled(false);
                    startButton -> setEnabled(true);
                    if(serviceRunning){
                        serviceStop();
                        pthread_join(thread1,NULL);
                        serviceRunning = false;
                    }
                    
                    
                }
            );

            connect(qApp, &QCoreApplication::aboutToQuit, 
                [this](){
                if(serviceRunning){
                    serviceStop();
                    pthread_join(thread1,NULL);
                    serviceRunning = false;
                }

                }
            );

            connect(tray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
                if (reason == QSystemTrayIcon::Trigger){
                    isVisible() ? hide() : (show(), raise(), activateWindow());
                }

            });

        };


