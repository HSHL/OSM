#ifndef NODE_HPP
#define NODE_HPP

#include <QString>
#include <QHash>

class Node {
    
public:
    Node(int pLon, int pLat);
    int getLon() const;
    int getLat() const;
    void addTag(QString pK, QString pV);
    QHash<QString,QString>* getTags();
    
private:
    const int longitude;
    const int latitude;
    QHash<QString,QString> *tags;
};

#endif	/* NODE_HPP */

