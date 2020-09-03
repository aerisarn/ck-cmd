#ifndef HKROOTLEVELCONTAINER_H
#define HKROOTLEVELCONTAINER_H

#include "src/hkxclasses/hkxobject.h"

namespace UI {

class hkRootLevelContainer final: public HkxObject
{
public:
    hkRootLevelContainer(HkxFile *parent, long ref = -1);
    hkRootLevelContainer& operator=(const hkRootLevelContainer&) = delete;
    hkRootLevelContainer(const hkRootLevelContainer &) = delete;
    ~hkRootLevelContainer();
public:
    static const QString getClassname();
    void addVariant(const QString & name, HkxObject *ptr);
private:
    bool link();
    bool readData(const HkxXmlReader & reader, long & index);
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    void setVariantAt(int index, HkxObject *ptr);
private:
    struct hkRootLevelContainerNamedVariant
    {
        bool operator==(const hkRootLevelContainerNamedVariant & other);
        hkRootLevelContainerNamedVariant(const QString & varname = "hkbBehaviorGraph", const QString & classname = "hkbBehaviorGraph", HkxObject *ptr = nullptr);
        QString name;
        QString className;
        HkxSharedPtr variant;  //This can be one of many (any?) types.
    };
private:
    static const QString classname;
    static uint refCount;
    QVector <hkRootLevelContainerNamedVariant> namedVariants;
    mutable std::mutex mutex;
};

}

#endif // HKROOTLEVELCONTAINER_H
