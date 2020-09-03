#ifndef HKBMODIFIERGENERATOR_H
#define HKBMODIFIERGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
//Do not allow variables to be bound to this class in the editor...
class hkbModifierGenerator final: public hkbGenerator
{
    friend class ModifierGeneratorUI;
public:
    hkbModifierGenerator(HkxFile *parent, long ref = 0);
    hkbModifierGenerator& operator=(const hkbModifierGenerator&) = delete;
    hkbModifierGenerator(const hkbModifierGenerator &) = delete;
    ~hkbModifierGenerator();
public:
    QString getName() const;
    static const QString getClassname();
    QVector <DataIconManager *> getChildren() const;
private:
    void setName(const QString &newname);
    QString getGeneratorName() const;
    QString getModifierName() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    HkxSharedPtr modifier;
    HkxSharedPtr generator;
    mutable std::mutex mutex;
};
}

#endif // HKBMODIFIERGENERATOR_H
