#ifndef HKBSTRINGCONDITION_H
#define HKBSTRINGCONDITION_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbStringCondition final: public HkxObject
{
public:
    hkbStringCondition(BehaviorFile *parent, const QString & string = "", long ref = -1);
    hkbStringCondition& operator=(const hkbStringCondition&) = delete;
    hkbStringCondition(const hkbStringCondition &) = delete;
    ~hkbStringCondition();
public:
    static const QString getClassname();
    QString getExpression() const;
private:
    bool readData(const HkxXmlReader & reader, long & index);
    QString evaluateDataValidity();
    bool link(){return true;}
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    QString conditionString;
    mutable std::mutex mutex;
};
}
#endif // HKBSTRINGCONDITION_H
