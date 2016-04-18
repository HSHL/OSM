#ifndef MEMBER_HPP
#define MEMBER_HPP

#include <QString>

class Member {
public:
    Member(QString pType, QString pRef, QString pRole);
    QString getType() const;
    QString getRef() const;
    QString getRole() const;
    
private:
    const QString type;
    const QString ref;
    const QString role;
};

#endif	/* MEMBER_HPP */

