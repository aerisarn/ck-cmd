#ifndef HKBPOSEMATCHINGGENERATOR_H
#define HKBPOSEMATCHINGGENERATOR_H

#include "hkbgenerator.h"
namespace UI {
class hkbBlenderGeneratorChild;

class hkbPoseMatchingGenerator final: public hkbGenerator
{
    friend class PoseMatchingGeneratorUI;
public:
    hkbPoseMatchingGenerator(HkxFile *parent, long ref = 0);
    hkbPoseMatchingGenerator& operator=(const hkbPoseMatchingGenerator&) = delete;
    hkbPoseMatchingGenerator(const hkbPoseMatchingGenerator &) = delete;
    ~hkbPoseMatchingGenerator();
public:
    QString getName() const;
    static const QString getClassname();
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
private:
    hkQuadVariable getWorldFromModelRotation() const;
    void setWorldFromModelRotation(const hkQuadVariable &value);
    qreal getBlendSpeed() const;
    void setBlendSpeed(const qreal &value);
    qreal getMinSpeedToSwitch() const;
    void setMinSpeedToSwitch(const qreal &value);
    qreal getMinSwitchTimeNoError() const;
    void setMinSwitchTimeNoError(const qreal &value);
    qreal getMinSwitchTimeFullError() const;
    void setMinSwitchTimeFullError(const qreal &value);
    int getStartPlayingEventId() const;
    void setStartPlayingEventId(int value);
    int getStartMatchingEventId() const;
    void setStartMatchingEventId(int value);
    int getRootBoneIndex() const;
    void setRootBoneIndex(int value);
    int getOtherBoneIndex() const;
    void setOtherBoneIndex(int value);
    int getAnotherBoneIndex() const;
    void setAnotherBoneIndex(int value);
    int getPelvisIndex() const;
    void setPelvisIndex(int value);
    QString getMode() const;
    void setMode(int value);
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
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    bool merge(HkxObject *recessiveObject);
    int getIndexOfChild(hkbBlenderGeneratorChild *child) const;
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QStringList Mode;
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
    hkQuadVariable worldFromModelRotation;
    qreal blendSpeed;
    qreal minSpeedToSwitch;
    qreal minSwitchTimeNoError;
    qreal minSwitchTimeFullError;
    int startPlayingEventId;
    int startMatchingEventId;
    int rootBoneIndex;
    int otherBoneIndex;
    int anotherBoneIndex;
    int pelvisIndex;
    QString mode;
    mutable std::mutex mutex;
};
}
Q_DECLARE_OPERATORS_FOR_FLAGS(UI::hkbPoseMatchingGenerator::BlenderFlags)

#endif // HKBPOSEMATCHINGGENERATOR_H
