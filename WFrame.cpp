#include "WFrame.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------

WFrame::WFrame(MainController *pC, Data *pR) {
    c = pC;
    r = pR;
    r->setController(c);
    
    onCreate();
    manageConnections();
   
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::onCreate() {
    setWindowState(Qt::WindowMaximized);
    setWindowIcon(QIcon(":files/02.png"));
    setAcceptDrops(true);
    
    mapLoaded = false;
    
    wStart = new WStart(c,r);
    setCentralWidget(wStart);
    centralWidget()->show();
    
    statusBar()->show();
    
    statusProg = new QProgressBar();
    statusProg->setMaximum(100);
    statusProg->setMinimum(0);
    statusProg->setValue(0);
    statusProg->setVisible(false);
    statusBar()->addWidget(statusProg);
    
    
    createActions();
    createMenuBar();
    
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::createActions() {
    open = new QAction("Datei öffnen",this);
    open->setShortcut(QKeySequence::Open);
    open->setIcon(QIcon(":/files/04.png"));
    
    closeFile = new QAction("Zurücksetzen",this);
    closeFile->setIcon(QIcon(":/files/01.png"));
    
    exit = new QAction("Beenden",this);
    exit->setShortcut(QKeySequence::Close);
    exit->setIcon(QIcon(":/files/03.png"));
    
    about = new QAction("Über",this);
    about->setIcon(QIcon(":files/05.png"));
    
    links = new QAction("Links",this);
    links->setIcon(QIcon(":files/06.png"));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::createMenuBar() {
    file = menuBar()->addMenu("Datei");
    file->addAction(open);
    file->addAction(closeFile);
    file->addAction(exit);
   
    help = menuBar()->addMenu("Hilfe");
    help->addAction(about);
    help->addAction(links);
    
    menuBar()->show();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::manageConnections() {
    
    QObject::connect(c,SIGNAL(exitProgram()),this,SLOT(onExit()));
    QObject::connect(c,SIGNAL(FileLoadedSignal(int)),this,SLOT(FileLoaded(int)));
    QObject::connect(c,SIGNAL(closeMap()),this,SLOT(onCloseMap()));
    QObject::connect(this,SIGNAL(closeMapButtonClicked()),c,SLOT(closeMapClicked()));
    QObject::connect(c,SIGNAL(loadMapSignal()),this,SLOT(loadMap()));
    QObject::connect(c,SIGNAL(noNodesSignal()),this,SLOT(errorNodes()));
    QObject::connect(c,SIGNAL(noOSMSignal()),this,SLOT(errorOSM()));
    QObject::connect(c,SIGNAL(multipleDropSignal()),this,SLOT(multipleDrop()));
    
    //ACTION CONNECTS
    QObject::connect(open,SIGNAL(triggered()),this,SLOT(getFile()));
    QObject::connect(exit,SIGNAL(triggered()),c,SLOT(exitClicked()));
    QObject::connect(closeFile,SIGNAL(triggered()),this,SLOT(checkMap()));
    QObject::connect(about,SIGNAL(triggered()),this,SLOT(showAbout()));
    QObject::connect(links,SIGNAL(triggered()),this,SLOT(showLinks()));
    
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::dragEnterEvent(QDragEnterEvent* e) {
        e->acceptProposedAction();

}



//PRIVATE SLOTS--------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::checkMap() {
    if (this->centralWidget() == wMap)
        emit closeMapButtonClicked();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::showAbout() {
    QMessageBox msg;
    msg.setWindowTitle("Über dieses Programm");
    msg.setText("<b><p style='color:#FF5722'>OpenStreetMap</p></b> <br><br> "
                         "Dieses Programm entstand im Rahmen einer Projektarbeit <br> "
                          "des 3. Semesters 'Intelligent Systems Design' der <br> "
                           "<a href='http://www.hshl.de/'>Hochschule Hamm-Lippstadt</a>. <br><br> "
                           "Alle Icons wurden von <a href='http://www.freepik.com/'>FreePik</a> von <a href='http://www.flaticon.com/'>www.flaticon.com</a> erstellt. <br><br>"
                           "Autor: <a href='mailto:christian.bause@stud.hshl.de'>Christian Bause<a>");
    msg.addButton(QMessageBox::Ok);
    msg.setIconPixmap(QPixmap(":files/05.png"));
    msg.setWindowIcon(QIcon(":files/02.png"));
    msg.exec();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::showLinks() {
    QMessageBox msg;
    msg.setWindowTitle("Weiterführende Links");
    msg.setText("<a href='http://www.openstreetmap.de/karte.html'>Die Karte: openstreetmap.de</a><br><br>"
    "<a href='http://wiki.openstreetmap.org/wiki/Main_Page'>OSM-Wiki: wiki.openstreetmap.org</a><br><br>"
    "<a href='http://www.openstreetmap.org/export#map=3/29.99/47.99'>Karten Download/Export</a><br><br>"
    "<a href='https://de.wikipedia.org/wiki/OpenStreetMap'>OSM auf Wikipedia</a>");
    msg.addButton(QMessageBox::Ok);
    msg.setIconPixmap(QPixmap(":files/06.png"));
    msg.setWindowIcon(QIcon(":files/02.png"));
    msg.exec();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::loadMap() {
         QString s = r->getFilePath() + " Erfolgreich geladen";
         statusBar()->showMessage(s, 5000);
        
         wMap = new WMap(c,r);
         mapLoaded = true;
         setCentralWidget(wMap);
         centralWidget()->show();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::errorNodes() {
    QMessageBox::warning(this,"Fehler","Die Datei besitzt keine Knotenpunkte (Leer?)",QMessageBox::Ok);
    mapLoaded = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::errorOSM() {
    QMessageBox::warning(this,"Fehler","Die Datei ist beschädigt oder leer (Kein OSM-Tag).",QMessageBox::Ok);
    mapLoaded = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::multipleDrop() {
    QMessageBox::warning(this,"Fehler","Es können nicht mehrere Dateien gleichzeitig geladen werden",QMessageBox::Ok);
    mapLoaded = false;
}



//PUBLIC SLOTS---------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::getFile() {
    if (mapLoaded) {    
            delete wMap;
            wStart = new WStart(c,r);
            setCentralWidget(wStart);
    }
    
//-------------------------------------------------------------------------------------------------------------------------------------------------
    
    QString path = QFileDialog::getOpenFileName(this,"Datei öffnen",QDir::currentPath(),"*.osm, *.OSM");
    if (path != "" ) {
        r->setOSM(path);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::FileLoaded(int pPercentage) {
    if (pPercentage != 100) {
        statusProg->setVisible(true);
        statusProg->setValue(pPercentage);
    } else {
        statusProg->setValue(100);
        statusProg->setVisible(false);
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::onCloseMap() {
    delete wMap;
    mapLoaded = false;
    wStart = new WStart(c,r);
    setCentralWidget(wStart);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WFrame::onExit() {
    close();
}




