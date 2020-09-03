#ifndef HKBBONEINDEXARRAY_H
#define HKBBONEINDEXARRAY_H

#include <QVector>

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbBoneIndexArray final: public HkDynamicObject
{
    friend class BoneIndexArrayUI;
public:
    hkbBoneIndexArray(HkxFile *parent, long ref = 0);
    hkbBoneIndexArray& operator=(const hkbBoneIndexArray&) = delete;
    hkbBoneIndexArray(const hkbBoneIndexArray &) = delete;
    ~hkbBoneIndexArray();
private:
    void addBoneIndex(int indexvalue = 0);
    void removeBoneIndexAt(int index);
    int getNumberOfBoneIndices() const;
    void setBoneIndexAt(int index, int value);
    int getBoneIndexAt(int index) const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    //QString evaluateDataValidity();
    static const QString getClassname();
    bool write(HkxXMLWriter *writer);   //TO DO: implement getChildrenOtherTypes()??
    QVector <HkxObject *> getChildrenOtherTypes() const;
private:
    static uint refCount;
    static const QString classname;
    QVector <int> boneIndices;
    mutable std::mutex mutex;
};
}
#endif // HKBBONEINDEXARRAY_H
