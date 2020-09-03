#ifndef BSMODIFYONCEMODIFIER_H
#define BSMODIFYONCEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSModifyOnceModifier final: public hkbModifier
{
    friend class BSModifyOnceModifierUI;
public:
    BSModifyOnceModifier(HkxFile *parent, long ref = 0);
    BSModifyOnceModifier& operator=(const BSModifyOnceModifier&) = delete;
    BSModifyOnceModifier(const BSModifyOnceModifier &) = delete;
    ~BSModifyOnceModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkbModifier * getPOnActivateModifier() const;
    hkbModifier * getPOnDeactivateModifier() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    HkxSharedPtr pOnActivateModifier;
    HkxSharedPtr pOnDeactivateModifier;
    mutable std::mutex mutex;
};
}
#endif // BSMODIFYONCEMODIFIER_H
