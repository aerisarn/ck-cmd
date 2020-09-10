#ifndef HKBBONEWEIGHTARRAY_H
#define HKBBONEWEIGHTARRAY_H

#include <QVector>

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbBoneWeightArray final: public HkDynamicObject
{
    friend class BoneWeightArrayUI;
public:
    hkbBoneWeightArray(HkxFile *parent, long ref = 0, int size = 0);
    hkbBoneWeightArray& operator=(const hkbBoneWeightArray&) = delete;
    hkbBoneWeightArray(const hkbBoneWeightArray &other) = delete;
    ~hkbBoneWeightArray();
public:
    void copyBoneWeights(const hkbBoneWeightArray *other);
    bool readData(const HkxBinaryHandler& handler, const void* object);
    hkReferencedObject* write(HkxBinaryHandler& handler);
private:
    void setBoneWeightAt(int index, int value);
    qreal getBoneWeightAt(int index, bool *ok = nullptr) const;
    int getBoneWeightsSize() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    //QString evaluateDataValidity();
    static const QString getClassname();
    bool write(HkxXMLWriter *writer);
    //hkbBoneWeightArray * clone() const;   //TO DO: implement getChildrenOtherTypes()??
    QVector <HkxObject *> getChildrenOtherTypes() const;
private:
    static uint refCount;
    static const QString classname;
    QVector <qreal> boneWeights;
    mutable std::mutex mutex;
};
}
#endif // HKBBONEWEIGHTARRAY_H
