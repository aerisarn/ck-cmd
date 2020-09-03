#ifndef HKBDETECTCLOSETOGROUNDMODIFIER_H
#define HKBDETECTCLOSETOGROUNDMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbStringEventPayload;

class hkbDetectCloseToGroundModifier final: public hkbModifier
{
    friend class DetectCloseToGroundModifierUI;
public:
    hkbDetectCloseToGroundModifier(HkxFile *parent, long ref = 0);
    hkbDetectCloseToGroundModifier& operator=(const hkbDetectCloseToGroundModifier&) = delete;
    hkbDetectCloseToGroundModifier(const hkbDetectCloseToGroundModifier &) = delete;
    ~hkbDetectCloseToGroundModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getCloseToGroundHeight() const;
    void setCloseToGroundHeight(const qreal &value);
    qreal getRaycastDistanceDown() const;
    void setRaycastDistanceDown(const qreal &value);
    int getCollisionFilterInfo() const;
    void setCollisionFilterInfo(int value);
    int getBoneIndex() const;
    void setBoneIndex(int value);
    int getAnimBoneIndex() const;
    void setAnimBoneIndex(int value);
    int getCloseToGroundEventID() const;
    void setCloseToGroundEventID(int value);
    hkbStringEventPayload * getCloseToGroundEventPayload() const;
    void setCloseToGroundEventPayload(hkbStringEventPayload *value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
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
    long userData;
    QString name;
    bool enable;
    hkEventPayload closeToGroundEvent;
    qreal closeToGroundHeight;
    qreal raycastDistanceDown;
    int collisionFilterInfo;
    int boneIndex;
    int animBoneIndex;
    mutable std::mutex mutex;
};
}
#endif // HKBDETECTCLOSETOGROUNDMODIFIER_H
