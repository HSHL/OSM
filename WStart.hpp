#ifndef WSTART_HPP
#define WSTART_HPP

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QDropEvent>
#include <QList>
#include <QUrl>
#include <QString>
#include <QDragEnterEvent>

#include "Data.hpp"
#include "MainController.hpp"

class WStart : public QWidget {
    Q_OBJECT
    
public:
    WStart(MainController *pC, Data *pR);
    
private:
    MainController *c;
    Data *r;
    QGridLayout *layout;
    QLabel *label;
    
    protected:
        virtual void dropEvent(QDropEvent *e);
        virtual void dragEnterEvent(QDragEnterEvent *e);
        
        signals:
        droppedFile(QString fileName);
        multipleDrop();
            
    

};

#endif	/* WSTART_HPP */

