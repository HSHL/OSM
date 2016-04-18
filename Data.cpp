#include "Data.hpp"


Data::Data() {
    
    reader = new QXmlStreamReader;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Data::setController(MainController* pC) {
    c = pC;
    
    QObject::connect(this,SIGNAL(fileLoaded(int)), c,SLOT(FileLoadedSlot(int)));
    QObject::connect(c,SIGNAL(closeMap()),this,SLOT(onCloseMap()));
    QObject::connect(c,SIGNAL(droppedFile(QString)),this,SLOT(loadFile(QString)));
    QObject::connect(this,SIGNAL(loadMap()),c,SLOT(loadMapSlot()));
    QObject::connect(this,SIGNAL(noNodes()),c,SLOT(noNodesSlot()));
    QObject::connect(this,SIGNAL(noOSM()),c,SLOT(noOSMSlot()));
    QObject::connect(this,SIGNAL(closeMap()),c,SLOT(closeMapClicked()));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Data::setOSM(const QString &pPath) {
    bool b = false;
    osmFile = new QFile(pPath);
    bool a = osmFile->open(QIODevice::ReadOnly | QIODevice::Text);
    
    if(a) {
    reader->setDevice(osmFile);
     while (!reader->atEnd()) {
         if (reader->isStartElement()) {
             if ((reader->name() == "osm") || (reader->name() == "OSM")) {
                 b = true;
                 break;
             }
         }
         reader->readNextStartElement();
     }
    osmFile->close();
    }
    if(b)
        readOSM();
    else {
        emit noOSM();
        delete osmFile;
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

QString Data::getFilePath() {
    QFileInfo info(*osmFile);
    return(info.fileName());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Data::toInt(QString *pS) {
    double d = pS->toDouble();
    QString s = QString::number(d);
    QStringList sl = s.split(".");
    int dec = 7 - sl[1].length();
    
    for (int a=0;a<sl[1].length();a++) {
        d *= 10;
    }
    int result = d;
    for (int a=0;a<dec;a++) {
        result *= 10;
    }
    
    return(result);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Data::correctInt(int i, QString s) {
    QStringList sl = s.split(".");
    QString z = sl[1].at(sl[1].length()-1);
    int foo = z.toInt();
    z = QString::number(i);
    z = z.at(z.length()-1);
    int bar = z.toInt();
    int result = i;
    if (bar!=foo)
        return (result+1);
    else
        return (result);
}

// READ THE OSM-FILE------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

void Data::readOSM() {
    
    emit fileLoaded(0);
    
    osmFile->open(QIODevice::ReadOnly | QIODevice::Text);
    reader->setDevice(osmFile);
 
  
    
    nodes = new QHash<QString,Node*>;
    ways = new QHash<QString, Way*>;
    relations = new QHash<QString, Relation*>;
    
    while (!reader->atEnd()) {
    
     if (reader->isStartElement()) {
         
        if (reader->name() == "bounds") {
                QString s;
                  s = reader->attributes().value("minlat").toString();
                  minLat = toInt(&s);
                  s = reader->attributes().value("maxlat").toString();
                  maxLat = toInt(&s);
                  s = reader->attributes().value("minlon").toString();
                  minLon = toInt(&s);
                  s = reader->attributes().value("maxlon").toString();
                  maxLon = toInt(&s);
        }
        
        if (reader->name() == "node") {
               double d = 1e7;
               int lon, lat;
                QString slon, slat, sid;
                sid = reader->attributes().value("id").toString();
                slon = reader->attributes().value("lon").toString();
                slat = reader->attributes().value("lat").toString();
                lon = slon.toDouble()*d;
                lat = slat.toDouble()*d;
                lon  = correctInt(lon,slon);
                lat = correctInt(lat,slat);
                
                    //enable to see the 'outer' parts of the map
             /*   if (lat < minLat)
                    minLat = lat;
                if (lat > maxLat)
                    maxLat = lat;
                if (lon > maxLon)
                    maxLon = lon;
                if (lon < minLon)
                    minLon = lon; */
                
                Node *n = new Node(lon,lat);
                nodes->insert(sid,n);
                //qDebug() << lat << lon;
                while (reader->readNextStartElement()) {
                    if (reader->isStartElement()) {
                        if (reader->name() == "tag") {
                            n->addTag(reader->attributes().value("k").toString(),reader->attributes().value("v").toString());
                           // qDebug()  << currentID << reader->attributes().value("k").toString() <<  reader->attributes().value("v").toString() << n->getTags()->count();
                            }
                        }
                    reader->readNextStartElement();
                }
        }
        
        
        
        
        if (reader->name() == "way") {
            
            emit fileLoaded(40);
            
            Way *w = new Way;
            QString firstID, lastID;
            int counter = 0;
            QString sid = reader->attributes().value("id").toString();
            ways->insert(sid,w);
            QList<Node*> *nodeList = new QList<Node*>;
            
            while (reader->readNextStartElement()) {
                if (reader->isStartElement()) {
                    
                    if (reader->name() == "nd") {
                        QString refID = reader->attributes().value("ref").toString();
                        if (counter == 0)
                            firstID = refID;
                        lastID = refID;
                        QHash<QString,Node*>::iterator i = nodes->find(refID);
                        Node *n = *i;
                        nodeList->append(n);
                        counter++;
                    }
                    if (reader->name() == "tag") {
                        w->addTag(reader->attributes().value("k").toString(),reader->attributes().value("v").toString());
                    }
                }
            reader->readNextStartElement();
            }
            w->setNodeList(nodeList);
            if (firstID == lastID) {
                w->addTag("area","yes");
               // qDebug() << id << firstID << lastID;
            }
        }
        
        
        if(reader->name() == "relation") {
            
            emit fileLoaded(80);
            
            Relation *relation = new Relation;
            QString sID = reader->attributes().value("id").toString();
                  while (reader->readNextStartElement()) {
                        if (reader->isStartElement()) {
                    
                          if (reader->name() == "member") {
                               relation->addMember(reader->attributes().value("type").toString(),reader->attributes().value("ref").toString(),reader->attributes().value("role").toString());
                           }
                    
                        if (reader->name() == "tag") {
                                relation->addTag(reader->attributes().value("k").toString(),reader->attributes().value("v").toString());
                          }
                       }
                       reader->readNextStartElement();
            }
            relations->insert(sID,relation);
        }
        
        
  
     }
     
     reader->readNextStartElement();
    }
    emit fileLoaded(100);
    
    osmFile->close();
    
    if(this->nodes->isEmpty() || this->ways->isEmpty()) {
        emit noNodes(); 
        delete nodes;
        delete ways;
        delete relations;
        delete osmFile;
    } else {
        emit loadMap();
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------



//GETTER------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

QHash<QString,Node*>* Data::getNodes() {
    return(nodes);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

QHash<QString,Way*>* Data::getWays() {
    return(ways);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

QHash<QString,Relation*>* Data::getRelations() {
    return(relations);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Data::getMinLat() {
    return(minLat);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Data::getMinLon() {
    return(minLon);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Data::getMaxLat() {
    return(maxLat);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int Data::getMaxLon() {
    return(maxLon);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------



//SLOTS---------------------------------------------------------------------------------------------------------------------------------------

void Data::loadFile(QString pFileName) {
    setOSM(pFileName);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void Data::onCloseMap() {
    
        delete osmFile;
        delete nodes;
        delete ways;
        delete relations;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------