#ifndef HKBBLENDERGENERATOR_H
#define HKBBLENDERGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class hkbBlenderGeneratorChild;

class hkbBlenderGenerator final: public hkbGenerator
{
    friend class BlenderGeneratorUI;
public:
    enum BlenderFlag{
        FLAG_NONE = 0,
        FLAG_SYNC = 1,
        FLAG_SMOOTH_GENERATOR_WEIGHTS = 4,
        FLAG_DONT_DEACTIVATE_CHILDREN_WITH_ZERO_WEIGHTS = 8,
        FLAG_PARAMETRIC_BLEND = 16,
        FLAG_IS_PARAMETRIC_BLEND_CYCLIC = 32,
        FLAG_FORCE_DENSE_POSE = 64,
        INVALID_FLAG = 128
    };
    Q_DECLARE_FLAGS(BlenderFlags, BlenderFlag)
public:
    hkbBlenderGenerator(HkxFile *parent, long ref = 0);
    hkbBlenderGenerator& operator=(const hkbBlenderGenerator&) = delete;
    hkbBlenderGenerator(const hkbBlenderGenerator &) = delete;
    ~hkbBlenderGenerator();
public:
    QString getName() const;
    static const QString getClassname();
    bool isParametricBlend() const;
private:
    bool swapChildren(int index1, int index2);
    void setName(const QString &newname);
    void updateChildIconNames() const;
    QString getFlags() const;
    void setFlags(const QString &value);
    hkbBlenderGeneratorChild * getChildDataAt(int index) const;
    bool getSubtractLastChild() const;
    void setSubtractLastChild(bool value);
    qreal getReferencePoseWeightThreshold() const;
    void setReferencePoseWeightThreshold(const qreal &value);
    qreal getBlendParameter() const;
    void setBlendParameter(const qreal &value);
    qreal getMinCyclicBlendParameter() const;
    void setMinCyclicBlendParameter(const qreal &value);
    qreal getMaxCyclicBlendParameter() const;
    void setMaxCyclicBlendParameter(const qreal &value);
    int getIndexOfSyncMasterChild() const;
    void setIndexOfSyncMasterChild(int value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    int getNumberOfChildren() const;
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    int getIndexOfChild(hkbBlenderGeneratorChild *child) const;
    bool merge(HkxObject *recessiveObject);
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    ulong userData;
    QString name;
    qreal referencePoseWeightThreshold;
    qreal blendParameter;
    qreal minCyclicBlendParameter;
    qreal maxCyclicBlendParameter;
    int indexOfSyncMasterChild;
    QString flags;
    bool subtractLastChild;
    QVector <HkxSharedPtr> children;
    mutable std::mutex mutex;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(hkbBlenderGenerator::BlenderFlags)
}
#endif // HKBBLENDERGENERATOR_H
