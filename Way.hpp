
#ifndef WAY_HPP
#define WAY_HPP

#include <QList>
#include <QHash>

#include "Node.hpp"
class Way {
    
public:
    Way();
    void setNodeList(QList<Node*> *pNodeList);
    void addTag(QString pK, QString pV);
    QList<Node*>* getNodeList();
    QHash<QString, QString>* getTags();
private:
    QList<Node*> *nodeList;
    QHash<QString, QString> *tags;
};

#endif	/* WAY_HPP */

