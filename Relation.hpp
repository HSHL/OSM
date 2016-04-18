#ifndef RELATION_HPP
#define RELATION_HPP

#include <QList>
#include <QString>
#include <QHash>

#include "Member.hpp"

class Relation {
public:
    Relation();
    void addMember(QString pType, QString pRef, QString pRole);
    void addTag(QString pK, QString pV);
    QList<Member*>* getMemberList();
    QHash<QString,QString>* getTags();
    
private:
    QList<Member*> *memberList;
    QHash<QString, QString> *tags;
    
};

#endif	/* RELATION_HPP */

