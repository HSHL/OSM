#ifndef  WFRAME_HPP
#define WFRAME_HPP

#include <QMainWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QIcon>
#include <QDragEnterEvent>

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QKeySequence>

#include <QStatusBar>
#include <QProgressBar>
#include <QToolBar>


#include <QFileDialog>
#include <QDir>
#include <QString>


#include "Data.hpp"
#include "MainController.hpp"
#include "WMap.hpp"
#include "WStart.hpp"

class WFrame :public QMainWindow {
    Q_OBJECT
    
public:
    WFrame(MainController *pC, Data *pR);
    
private:
    MainController *c;
    Data *r;
    
    WStart *wStart;
    WMap *wMap;
    bool mapLoaded;
    
    QMenu *file;
    QMenu *edit;
    QMenu *help;
    
    QAction *open;
    QAction *options;
    QAction *closeFile;
    QAction *exit;
    QAction *about;
    QAction *links;
    
    
    QProgressBar *statusProg;
    
    void onCreate();
    void manageConnections();
    void createMenuBar();
    void createActions();
    
    
    void createToolbar();
    void createStatusbar();
    
    
    protected:
        void dragEnterEvent(QDragEnterEvent *e);
    
    private slots:
         void checkMap();
         void showAbout();
         void showLinks();
         void loadMap();
         void errorNodes();
         void errorOSM();
         
    public slots:
        void onExit();
        void getFile();
        void onCloseMap();
        void FileLoaded(int pPercentage);
        void multipleDrop();
        
        
        signals:
            void closeMapButtonClicked();
        
        
};

#endif	/* WFRAME_HPP */

