#ifndef WMAP_HPP
#define WMAP_HPP

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPen>
#include <QBrush>
#include <QRectF>
#include <QPointF>
#include <QPoint>
#include <QPolygonF>
#include <QLineF>
#include <QColor>
#include <QTransform>
#include <QPixmap>
#include <QKeyEvent>
#include <QKeySequence>
#include <QFileDialog>

#include <QString>
#include <QHash>
#include <QList>

#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QDebug>

#include "Data.hpp"
#include "MainController.hpp"
#include "Node.hpp"
#include "Way.hpp"
#include "Relation.hpp"
#include "Member.hpp"


class WMap : public QGraphicsView {
    Q_OBJECT
    
public:
    WMap(MainController *pC,Data *pR);
    
    void drawOuterRect();
    void drawAreas();
    void drawWays();
    void drawRelations();
    
private:
    MainController *c;
    Data *r;
    
    QPen *pen;
    QBrush *brush;
    
    QTransform *transformation;
    
    bool *tunnel;
    
    QGraphicsScene *scene;
    
    QPolygonF *currentPoly;
    Node *currentFirstNode;
    Node *currentLastNode;
    
    QRectF *boundingRect;
    int *borderWidth;
    
    bool *moveMouse;
    QPointF *mousePos;
    int *stepCounter;
    double *scaleControl;
    QTimer *timer;
    
    void setBoundingRect();
    bool changePen(QHash<QString,QString>* pTags);
    bool changeBrush(QHash<QString,QString>* pTags);
    bool outOfBounds(QPointF pPoint);
    
    protected:
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseReleaseEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);
        virtual void wheelEvent(QWheelEvent *event);
   
    
    private slots:
        void stopTimer();
        void zoom();
        void takeScreenshot();
        
        signals:
        void zoomFinished();
};

#endif	/* WMAP_HPP */

