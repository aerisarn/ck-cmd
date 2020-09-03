#ifndef HKBBLENDERGENERATORCHILD_H
#define HKBBLENDERGENERATORCHILD_H

#include "hkbgenerator.h"
namespace UI {
class hkbBlenderGenerator;
class hkbBoneWeightArray;

class hkbBlenderGeneratorChild final: public hkbGenerator
{
    friend class BlenderGeneratorChildUI;
public:
    hkbBlenderGeneratorChild(HkxFile *parent, hkbGenerator *parentBG, long ref = 0);
    hkbBlenderGeneratorChild& operator=(const hkbBlenderGeneratorChild&) = delete;
    hkbBlenderGeneratorChild(const hkbBlenderGeneratorChild &) = delete;
    ~hkbBlenderGeneratorChild();
public:
    bool operator==(const hkbBlenderGeneratorChild & other);
public:
    QString getName() const;
    int getThisIndex() const;
    static const QString getClassname();
    void unlink();
    bool merge(HkxObject *recessiveObject);
    void setParentBG(hkbGenerator *blend);
    QVector <DataIconManager *> getChildren() const;
    HkxSharedPtr getGenerator() const;
private:
    void setBoneWeights(const HkxSharedPtr &value);
    hkbBoneWeightArray * getBoneWeightsData() const;
    QString getGeneratorName() const;
    qreal getWeight() const;
    void setWeight(const qreal &value);
    qreal getWorldFromModelWeight() const;
    void setWorldFromModelWeight(const qreal &value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool isParametricBlend() const;
    bool hasChildren() const;
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
    hkbGenerator *getParentGenerator() const;
private:
    static uint refCount;
    static const QString classname;
    hkbGenerator *parentBG;
    HkxSharedPtr generator;
    HkxSharedPtr boneWeights;
    qreal weight;
    qreal worldFromModelWeight;
    mutable std::mutex mutex;
};
}
#endif // HKBBLENDERGENERATORCHILD_H
