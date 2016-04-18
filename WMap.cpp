#include "WMap.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------

WMap::WMap(MainController* pC, Data* pR) {
    c = pC;
    r = pR;
    
    timer = new QTimer;
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(zoom()));
    QObject::connect(this,SIGNAL(zoomFinished()),this,SLOT(stopTimer()));
    
    tunnel = new bool;
    *tunnel = false;
    scaleControl = new double;
    *scaleControl = 1.0;
    stepCounter = new int;
    *stepCounter = 0;
    moveMouse = new bool;
    *moveMouse = false;
    mousePos = new QPointF();
    mousePos->setX(0);
    mousePos->setY(0);
       
    pen = new QPen;
    pen->setCapStyle(Qt::RoundCap);
    brush = new QBrush;
    brush->setStyle(Qt::SolidPattern);
    brush->setColor(QColor::fromRgb(128,255,128,255));
    
    scene = new QGraphicsScene;
    setBoundingRect();
    scene->setSceneRect(*boundingRect);
    setScene(scene);
    setBackgroundBrush(*brush);

    transformation = new QTransform();
    transformation->rotate(180,Qt::XAxis);
    this->setTransform(*transformation);
    

         drawOuterRect();
         drawRelations();
         drawAreas();
         drawWays();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::setBoundingRect() {
   double x,y,w,h;
   borderWidth = new int;
   *borderWidth = 1;
   
   x= (r->getMinLon() / 1E2) - *borderWidth;
   y = (r->getMinLat() / 1E2) - *borderWidth;
   w = (r->getMaxLon() / 1E2) - (r->getMinLon() / 1E2) + 2* (*borderWidth);
   h = (r->getMaxLat() / 1E2) - (r->getMinLat() / 1E2) + 2* (*borderWidth);
   
   boundingRect = new QRectF(x,y,w,h);
       

}



//DRAW MAP---------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::drawOuterRect() {
   pen->setWidth(*borderWidth);
   pen->setColor(Qt::black);
   pen->setWidth(10);
   brush->setColor(QColor::fromRgb(128,255,128,255));
   QRectF borderRect = *boundingRect;
   borderRect.setX(borderRect.x()-5);
   borderRect.setY(borderRect.y()-5);
   borderRect.setWidth(borderRect.width()+10);
   borderRect.setHeight(borderRect.height()+10);
   scene->addRect(borderRect,*pen,*brush);
   
   bool b = true;
   QRect rect;
   QPolygonF visibleScene;
   
   QPolygonF::iterator polyIt;
   while (b) {
       polyIt = visibleScene.begin();
        rect = this->viewport()->rect();
        visibleScene = this->mapToScene(rect);
        for (int i = 0; i < visibleScene.count(); i++) {
            b = false;
            if (outOfBounds(*polyIt)) {
                b = true;
                break;
            }
            polyIt++;
        }
        if (b)
            scale(1.01,1.01);
   }
   
   
   
   while (!b) {
       polyIt = visibleScene.begin();
       rect = this->viewport()->rect();
       visibleScene = this->mapToScene(rect);
        for (int i = 0; i < visibleScene.count(); i++) {
            b = false;
            if (outOfBounds(*polyIt)) {
                b = true;
                break;
            }
            polyIt++;
        }
       if (!b)
         scale(0.99,0.99);
   }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::drawAreas() {
    QHash<QString,Way*>::iterator wayIt = r->getWays()->begin();
   
    for (int i = 0; i < r->getWays()->count(); i++) {
            Way *w = *wayIt;
            QList<Node*>::iterator nodeListIt = w->getNodeList()->begin();
            
            QHash<QString,QString>::iterator tagIt = w->getTags()->begin();
            bool area = false;
            for (int j = 0; j < w->getTags()->count(); j++) {
                if(tagIt.key() == "area")
                    area = true;
                tagIt++;
            }
            
                    if((area == true) && (w->getTags()->count()>1)) {
                             QPolygonF poly;
                     
                          for (int k = 0; k < w->getNodeList()->count()-1; k++) {
                               double x,y;
                                 Node *n = *nodeListIt;
                                 x = n->getLon() / 1E2;
                                 y = n->getLat() / 1E2;
                                 
                                 QPointF point;
                                 point.setX(x);
                                 point.setY(y);
                                 
                                 poly << point;
                                 nodeListIt++;
                           }
                             
                             if (changeBrush(w->getTags())) {
                                 scene->addPolygon(poly,*pen,*brush);
                            }
                   }
            wayIt++;
        }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------


void WMap::drawWays() {
    QHash<QString,Way*>::iterator wayIt = r->getWays()->begin();
        
        for (int i = 0; i < r->getWays()->count(); i++) {
           *tunnel = false;
            Way *w = *wayIt;
            QList<Node*>::iterator nodeListIt = w->getNodeList()->begin();
            
            QHash<QString,QString>::iterator tagIt = w->getTags()->begin();
            bool area = false;
            for (int j = 0; j < w->getTags()->count(); j++) {
                if(tagIt.key() == "area")
                    area = true;
                if(tagIt.key() == "tunnel")
                    *tunnel = true;
                tagIt++;
            }
            
                 if (!area) {
                          for (int k = 0; k < w->getNodeList()->count()-1; k++) {
                                   double x1, x2, y1, y2;
                                   Node *n = *nodeListIt;
                                   y1 = n->getLat() / 1E2;
                                   x1 = n->getLon() / 1E2;
                                   nodeListIt++;
                                   n = *nodeListIt;
                                   y2 = n->getLat() / 1E2;
                                   x2 = n->getLon() / 1E2;
                                   
                                   if (changePen(w->getTags())) 
                                   scene->addLine(x1,y1,x2,y2,*pen);
                           }
                   }
            wayIt++;
        }   
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::drawRelations() {
    QHash<QString,Relation*>::Iterator relationIt = r->getRelations()->begin();
    
    for(int i = 0; i < r->getRelations()->count(); i++) {
        
        
        Relation *re = *relationIt;
        QHash<QString,QString>::iterator tagIt = re->getTags()->begin();
        QList<Member*>::iterator memberIt = re->getMemberList()->begin();
        QList<Node*>::iterator nodeIt;
        QHash<QString,Way*>::iterator wayIt;
        
                for(int j = 0; j < re->getTags()->count(); j++) {
                    
                    if (tagIt.value() == "multipolygon") {
                               currentPoly = new QPolygonF;
                               bool openPoly = false;
                               QList<QPolygonF> polyList;
                               QList<QString> roleList;
                                            for (int l=0;l<re->getMemberList()->count();l++) {
                                                Member *member = *memberIt;
                                                if (member->getType() == "way" && member->getRole() != "") {
                                                    
                                                    if (r->getWays()->contains(member->getRef()) ) {
                                                                            wayIt = r->getWays()->find(member->getRef());
                                                                            Way *w = *wayIt;
                                                                            
                                                                             
                                                                             QHash<QString,QString>::iterator wayTagIt = w->getTags()->begin();
                                                                             bool area = false;
                                                                                 for (int v = 0; v < w->getTags()->count(); v++) {
                                                                                     if(wayTagIt.key() == "area")
                                                                                         area = true;
                                                                                         wayTagIt++;
                                                                                      }
                                                                            
                                                                            nodeIt = w->getNodeList()->begin();
                                                                            if(!openPoly)
                                                                                currentFirstNode = *nodeIt;
                                                                              for(int k=0;k<w->getNodeList()->count();k++) {
                                                                                   currentLastNode = *nodeIt;
                                                                                   Node *n = *nodeIt;
                                                                                   QPointF point;
                                                                                   point.setX(n->getLon() / 1E2);
                                                                                   point.setY(n->getLat() / 1E2);
                                                                                   *currentPoly << point;
                                                                                   nodeIt++;
                                                                                }
                                                                             if( (area == true )) {
                                                                                 polyList.append(*currentPoly);
                                                                                 roleList.append(member->getRole());
                                                                                 currentPoly = new QPolygonF;
                                                                                 openPoly = false;
                                                                             } else {
                                                                                 if (!openPoly) {
                                                                                 }  else {
                                                                                     if (currentLastNode == currentFirstNode) {
                                                                                         polyList.append(*currentPoly);
                                                                                         roleList.append(member->getRole());
                                                                                         currentPoly = new QPolygonF;
                                                                                         openPoly = false;
                                                                                     }
                                                                                 }
                                                                             }
                                                     } else {
                                                          delete currentPoly;
                                                          break;
                                                     }
                                                                    
                                            }
                        }
                        memberIt++;
                       if (!polyList.isEmpty()) {                  
                               QList<QPolygonF>::iterator polyIt = polyList.begin();
                               QList<QString>::iterator roleIt = roleList.begin();
                                for (int u = 0; u < polyList.count(); u++) {
                                    re->addTag("role",*roleIt);
                                   
                                   if (changeBrush(re->getTags()))
                                    scene->addPolygon(*polyIt,*pen,*brush);
                                    
                                    re->getTags()->erase(re->getTags()->find("role"));
                                    polyIt++;
                                    roleIt++;
                                }
                            
                         }
                     } // END MULTIPOLYGON
                    
                    
                    
                    
                    if (tagIt.value() == "associatedStreet") {
                        for (int k = 0; k<re->getMemberList()->count(); k++) {
                            Member *member = *memberIt;
                            if (r->getWays()->contains( member->getRef() )) {
                                Way *w = *r->getWays()->find(member->getRef());
                               if(member->getRole() == "house") {
                                   QPolygonF poly;
                                   nodeIt = w->getNodeList()->begin();
                                    for(int f = 0; f < w->getNodeList()->count(); f++) {
                                       Node *n = *nodeIt;
                                       QPointF point;
                                       point.setX(n->getLon() / 1E2);
                                       point.setY(n->getLat() / 1E2);
                                       poly << point;
                                       nodeIt++;
                                    }
                                   if (changeBrush(w->getTags()))
                                   scene->addPolygon(poly,*pen,*brush);
                            } else {
                             nodeIt = w->getNodeList()->begin();
                             for (int f = 0; f< w->getNodeList()->count() - 1; f++) {
                                 Node *n = *nodeIt;
                                 nodeIt++;
                                 Node *m = *nodeIt;
                                 QPointF a,b;
                                 a.setX(n->getLon() / 1E2);
                                 a.setY(n->getLat() / 1E2);
                                 b.setX(m->getLon() / 1E2);
                                 b.setY(m->getLat() / 1E2);
                                 if (changePen(w->getTags()))
                                 scene->addLine(a.x(),a.y(),b.x(),b.y(),*pen);
                             } 
                           }
                            
                          }
                          memberIt++;
                         }
                    } //END ASSOCIATEDSTREET
                    
                    tagIt++;
                }
         relationIt++;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool WMap::changePen(QHash<QString,QString>* pTags) {
    QHash<QString,QString>::iterator tagIt = pTags->begin();
    bool set = false;
    for(int i = 0; i < pTags->count(); i++) {
        pen->setStyle(Qt::SolidLine);
        if(*tunnel == true)
            pen->setStyle(Qt::DashDotLine);
        
        
        if(tagIt.key() == "highway") {
                         if(tagIt.value() == "primary" || tagIt.value() == "primary_link" || tagIt.value() == "trunk" || tagIt.value() == "trunk_link") {
                            pen->setColor(QColor::fromRgb(255,160,128,255)); pen->setWidth(10); set=true; break;}
                          if(tagIt.value() == "secondary" || tagIt.value() == "secondary_link") {
                            pen->setColor(QColor::fromRgb(255,224,128,255)); pen->setWidth(8); set=true; break;}
                           if (tagIt.value() == "tertiary" || tagIt.value() == "tertiary_link") {
                            pen->setColor(QColor::fromRgb(128,128,208,255)); pen->setWidth(8); set=true; break;}
                          if(tagIt.value() == "residential" || tagIt.value() == "living_street") {
                            pen->setColor(QColor::fromRgb(64,64,64,255)); pen->setWidth(5); set=true; break;}
                          if(tagIt.value() == "service") {
                            pen->setColor(QColor::fromRgb(208,208,208,255)); pen->setWidth(4); set=true; break;}
                          if(tagIt.value() == "track") {
                            pen->setColor(QColor::fromRgb(96,32,0,255)); pen->setWidth(1); pen->setStyle(Qt::DashLine); set=true; break;}
                          if(tagIt.value() == "path") {
                            pen->setColor(QColor::fromRgb(128,64,0,255)); pen->setWidth(1); pen->setStyle(Qt::DashLine); set=true; break;}
                          if(tagIt.value() == "footway") {
                            pen->setColor(QColor::fromRgb(192,192,192,255)); pen->setWidth(2); pen->setStyle(Qt::DashLine); set=true; break;}
                          if(tagIt.value() == "road") {
                            pen->setColor(QColor::fromRgb(32,32,32,255)); pen->setWidth(5); set=true; break;}
                          if((tagIt.value() == "motorway") || (tagIt.value() == "motorway_link")) {
                            pen->setColor(QColor::fromRgb(160,80,112,255)); pen->setWidth(10); set=true; break;}
        }
        
        if(tagIt.key() == "waterway") {
                        if(tagIt.value() == "river") {
                            pen->setColor(Qt::blue); pen->setWidth(20); set=true; break;}
                        if(tagIt.value() == "stream") {
                           pen->setColor(Qt::blue); pen->setWidth(5); set=true; break;}
            }
        
        
        if(tagIt.value() == "water") {
            pen->setColor(Qt::blue); set=true; break;
        }
        
        if(tagIt.key() == "railway") {
            pen->setWidth(2);
            pen->setColor(Qt::black);
            pen->setStyle(Qt::DotLine);
            set = true; 
            break;
        }
        
         tagIt++;
        }
    
    
    return(set);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool WMap::changeBrush(QHash<QString,QString>* pTags) {
            QHash<QString,QString>::iterator tagIt = pTags->begin();
            bool set = false;
            brush->setStyle(Qt::SolidPattern);
            pen->setStyle(Qt::NoPen);
    
    for(int i = 0; i < pTags->count(); i++) {
                 if((tagIt.value() == "reservoir") || (tagIt.value() == "basin") || (tagIt.value() == "water")) {
                     brush->setColor(Qt::blue); pen->setColor(Qt::blue); set=true; break;
                 }
                 

                 if(tagIt.key() == "building" || tagIt.value() == "building") {
                     brush->setColor(QColor::fromRgb(128,128,128,255)); pen->setStyle(Qt::SolidLine); pen->setColor(QColor::fromRgb(16,16,16,255)); pen->setWidth(1); set=true; break;
                 }
                 
                 if (tagIt.key() == "landuse") {
                 
                     if (tagIt.value() == "forest") {
                         brush->setColor(QColor::fromRgb(0,160,0,125)); set=true; break;}
                     if(tagIt.value() == "industrial" || tagIt.value() == "retail" || tagIt.value() == "commercial") {
                        brush->setColor(QColor::fromRgb(160,160,255,125)); set=true; break;}
                     if(tagIt.value() == "railway") {
                        brush->setColor(QColor::fromRgb(208,208,255,125)); set=true; break;}
                     if(tagIt.value() == "residential") {
                        brush->setColor(QColor::fromRgb(220,220,220,125)); set=true; break;}
                     if(tagIt.value() == "cemetery") {
                        brush->setStyle(Qt::DiagCrossPattern); brush->setColor(QColor::fromRgb(128,255,128,125)); set=true; break;}
                     if(tagIt.value() == "recreation_ground" || tagIt.value() == "village_green" || tagIt.value() == "meadow" || tagIt.value() == "grass" || tagIt.value() == "greenfield") {
                        brush->setColor(QColor::fromRgb(80,255,80,125)); set=true; break;}
                 }
             tagIt++;
    }
             
                
                
            return(set);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool WMap::outOfBounds(QPointF pPoint) {
    if ((pPoint.x() < boundingRect->left()) || (pPoint.x() > boundingRect->right()) || 
        (pPoint.y() < boundingRect->top()) || (pPoint.y() > boundingRect->bottom()) )
        return(true);
    return(false);
}



//KEYEVENTS-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::keyPressEvent(QKeyEvent* event) {
    if(event->key() == 16777275)
        takeScreenshot();
}




//MOUSEEVENTS--------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        setCursor(Qt::ClosedHandCursor);
        mousePos->setX(event->x());
        mousePos->setY(event->y());
        *moveMouse = true;
        event->accept();
    }
    
    event->ignore();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::mouseReleaseEvent(QMouseEvent *event) {
       if(event->button() == Qt::LeftButton) {
           setCursor(Qt::ArrowCursor);
        *moveMouse = false;
        event->accept();
    }
       event->ignore();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::mouseMoveEvent(QMouseEvent* event) {
    if (*moveMouse == true) {
        
        this->horizontalScrollBar()->setValue(this->horizontalScrollBar()->value() - (event->x() - mousePos->x()));
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - (event->y() - mousePos->y()));
        
        mousePos->setX(event->x());
        mousePos->setY(event->y());
    } 
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::wheelEvent(QWheelEvent* event) {
    int degrees = event->delta() / 15;
    int steps = degrees / 8; 
    
    if(*stepCounter * steps < 0)
        *stepCounter = steps;
    else
    *stepCounter += steps;
    
    
    if (!timer->isActive()) {;
        timer->start(20);
    }  
    
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::stopTimer() {
    timer->stop();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::zoom() {
    if (*stepCounter != 0) {
        qreal viewScale = 1.0;
        qreal factor = 0.2;
        qreal max = 3;
                     if(*stepCounter < 0) {
                              if (  (this->horizontalScrollBar()->value() != 0) && (this->verticalScrollBar()->value() != 0)  ) {
                                    viewScale -= factor;
                                    *scaleControl -= factor;
                              } 
                        *stepCounter += 1;
                     } else {
                         if((*scaleControl+factor)<max) {
                             viewScale += factor;
                             *scaleControl += factor;
                         }
                     *stepCounter -= 1;
                }
         
         scale(viewScale,viewScale);
         bool b = true;
         QRect rect;
         QPolygonF poly;
         QPolygonF::iterator polyIt;
         while(b) {
             rect = this->viewport()->visibleRegion().boundingRect();
             poly = this->mapToScene(rect);
             polyIt = poly.begin();
             for (int i=0;i < poly.count(); i++) {
                  if (!this->outOfBounds(*polyIt)) {
                         b = false;
             } else {
                      b = true;
                      break;
             }
             polyIt++;
         }
             *scaleControl -= factor;
             *stepCounter = 0;
             scale(1.01,1.01);
             
        }
         
   } else {
            emit zoomFinished();
        }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void WMap::takeScreenshot() {
    QString fileName = QFileDialog::getSaveFileName(this,"Screenshot speichern", "", "(*.png)");
    QPixmap pic = QPixmap::grabWidget(this,this->viewport()->rect());
    pic.save(fileName,"" ,100);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

