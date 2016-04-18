
#include "WStart.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------

WStart::WStart(MainController* pC, Data* pR) {
    c = pC;
    r = pR;
    this->setStyleSheet("background-color:#B0B0B0;border:6px dashed #FF5722;");
    layout = new QGridLayout(this);
    label = new QLabel("<b>Willkommen in OpenStreetMap!</b> <br><bt><br>"
            "Mit diesem Tool können Sie Ihre OSM-Datein laden und anzeigen.<br>"
            "Entweder öffnen Sie die Datei über das Menü, oder Sie ziehen sie per Drag & Drop in dieses Feld.<br><br><br>"
            "<b>Steuerung:</b> <br>"
            "Per Linksklick kann die Karte mit der Maus gezogen werden, per Mausrad kann man zoomen.<br>Ausserdem ist es möglich, mit der Taste F12 einen Screenshot zu erstellen.");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color:#FF5722;font-size:36px");
    layout->addWidget(label);
    
    QObject::connect(this,SIGNAL(droppedFile(QString)),c,SLOT(loadFile(QString)));
    QObject::connect(this,SIGNAL(multipleDrop()),c,SLOT(multipleDropSlot()));
    setAcceptDrops(true);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WStart::dropEvent(QDropEvent* e) {
    QList<QUrl> urlList = e->mimeData()->urls();
    if (urlList.count() > 1) {
        emit multipleDrop();
    } else {
       emit droppedFile(urlList.at(0).toLocalFile());
    }
    e->acceptProposedAction();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WStart::dragEnterEvent(QDragEnterEvent* e) {
    e->acceptProposedAction();
}
//-------------------------------------------------------------------------------------------------------------------------------------------------
