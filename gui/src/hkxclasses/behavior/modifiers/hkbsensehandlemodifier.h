#ifndef HKBSENSEHANDLEMODIFIER_H
#define HKBSENSEHANDLEMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbSenseHandleModifier final: public hkbModifier
{
    friend class SenseHandleModifierUI;
    friend class RangesUI;
public:
    hkbSenseHandleModifier(HkxFile *parent, long ref = 0);
    hkbSenseHandleModifier& operator=(const hkbSenseHandleModifier&) = delete;
    hkbSenseHandleModifier(const hkbSenseHandleModifier &) = delete;
    ~hkbSenseHandleModifier();
public:
    QString getName() const;
    static const QString getClassname();
public:
    struct hkRanges{
        hkRanges() : minDistance(0), maxDistance(0), ignoreHandle(false){}
        hkEventPayload event;
        qreal minDistance;
        qreal maxDistance;
        bool ignoreHandle;
    };
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getSensorLocalOffset() const;
    void setSensorLocalOffset(const hkQuadVariable &value);
    QString getLocalFrameName() const;
    void setLocalFrameName(int index);
    QString getSensorLocalFrameName() const;
    void setSensorLocalFrameName(int index);
    qreal getMinDistance() const;
    void setMinDistance(const qreal &value);
    qreal getMaxDistance() const;
    void setMaxDistance(const qreal &value);
    qreal getDistanceOut() const;
    void setDistanceOut(const qreal &value);
    int getCollisionFilterInfo() const;
    void setCollisionFilterInfo(int value);
    int getSensorRagdollBoneIndex() const;
    void setSensorRagdollBoneIndex(int value);
    int getSensorAnimationBoneIndex() const;
    void setSensorAnimationBoneIndex(int value);
    QString getSensingMode() const;
    void setSensingMode(int index);
    bool getExtrapolateSensorPosition() const;
    void setExtrapolateSensorPosition(bool value);
    bool getKeepFirstSensedHandle() const;
    void setKeepFirstSensedHandle(bool value);
    bool getFoundHandleOut() const;
    void setFoundHandleOut(bool value);
    void addRange(hkRanges range = hkRanges());
    void removeRange(int index);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfRanges() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList SensingMode;
    long userData;
    QString name;
    bool enable;
    hkQuadVariable sensorLocalOffset;
    QVector <hkRanges> ranges;
    HkxSharedPtr handleOut;
    HkxSharedPtr handleIn;
    QString localFrameName;
    QString sensorLocalFrameName;
    qreal minDistance;
    qreal maxDistance;
    qreal distanceOut;
    int collisionFilterInfo;
    int sensorRagdollBoneIndex;
    int sensorAnimationBoneIndex;
    QString sensingMode;
    bool extrapolateSensorPosition;
    bool keepFirstSensedHandle;
    bool foundHandleOut;
    mutable std::mutex mutex;
};
}

#endif // HKBSENSEHANDLEMODIFIER_H
