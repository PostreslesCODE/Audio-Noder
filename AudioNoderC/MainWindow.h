#include <pthread.h>
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPushButton>
#include <QCloseEvent>

class MainWindow : public QWidget {

    Q_OBJECT

    public:

        
        explicit MainWindow(QWidget *parent = nullptr); // Parent being a adress for a qwidget, meaning that main window has no parent as it is one
        ~MainWindow(); // Destructor for the main window object


    private:
        
        bool serviceRunning = false;

        pthread_t thread1;
        QPushButton    *startButton;
        QPushButton    *stopButton;
        QSystemTrayIcon *tray;
        QMenu          *trayMenu;

        void setupUI();
        void setupTray();
        void setupConnections();
        
    protected:
        void closeEvent(QCloseEvent *event) override;
};