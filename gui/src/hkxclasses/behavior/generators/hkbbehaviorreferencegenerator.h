#ifndef HKBBEHAVIORREFERENCEGENERATOR_H
#define HKBBEHAVIORREFERENCEGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class hkbBehaviorReferenceGenerator final: public hkbGenerator
{
    friend class BehaviorReferenceGeneratorUI;
public:
    hkbBehaviorReferenceGenerator(HkxFile *parent, long ref = 0);
    hkbBehaviorReferenceGenerator& operator=(const hkbBehaviorReferenceGenerator&) = delete;
    hkbBehaviorReferenceGenerator(const hkbBehaviorReferenceGenerator &) = delete;
    ~hkbBehaviorReferenceGenerator();
public:
    QString getName() const;
    static const QString getClassname();
    QString getBehaviorName() const;
private:
    void setName(const QString &newname);
    void setBehaviorName(const QString &newname);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    ulong userData;
    QString name;
    QString behaviorName;
    mutable std::mutex mutex;
};
}
#endif // HKBBEHAVIORREFERENCEGENERATOR_H
