#ifndef DATA_HPP
#define DATA_HPP

#include <QFile>
#include <QFileInfo>
#include <QDIr>
#include <QString>
#include <QHash>
#include <QList>

#include <QDebug>
#include <QMessageBox>
#include <QXmlStreamReader>

#include "Node.hpp"
#include "Way.hpp"
#include "Relation.hpp"
#include "Member.hpp"

#include "MainController.hpp"

class Data : public QObject {
    Q_OBJECT
    
public:
    Data();
    
    
    void setOSM (const QString &pPath);
    void readOSM();
    QString getFilePath();
    QString operator<(int pI);
    
    QHash<QString,Node*>* getNodes();
    QHash<QString,Way*>* getWays();
    QHash<QString,Relation*>* getRelations();
    int getMinLat();
    int getMinLon();
    int getMaxLat();
    int getMaxLon();
    void setController(MainController *pC);

private:
    MainController *c;
    QXmlStreamReader *reader;
    QFile *osmFile;
    QHash<QString,Node*> *nodes;
    QHash<QString,Way*> *ways;
    QHash <QString,Relation*> *relations;
    int minLat;
    int minLon;
    int maxLat;
    int maxLon;
    
    int toInt(QString *pS);
    int correctInt(int i, QString s);
    
    private slots:
        void onCloseMap();
        void loadFile(QString pFileName);
    
    signals:
        void fileLoaded(int pPercentage);
        void loadMap();
        void noOSM();
        void noNodes();
        void closeMap();
        
};

#endif	/* DATA_HPP */

