#ifndef BSLOOKATMODIFIER_H
#define BSLOOKATMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbStringEventPayload;

class BSLookAtModifier final: public hkbModifier
{
    friend class BSBoneUI;
    friend class BSLookAtModifierUI;
public:
    BSLookAtModifier(HkxFile *parent, long ref = 0);
    BSLookAtModifier& operator=(const BSLookAtModifier&) = delete;
    BSLookAtModifier(const BSLookAtModifier &) = delete;
    ~BSLookAtModifier();
public:
    static const QString getClassname();
    QString getName() const;
private:
    struct BsBone{
        BsBone(): index(-1), limitAngleDegrees(0), onGain(0), offGain(0), enabled(true){}
        int index;
        hkQuadVariable fwdAxisLS;
        qreal limitAngleDegrees;
        qreal onGain;
        qreal offGain;
        bool enabled;
    };
private:
    void removeBone(int index);
    void removeEyeBone(int index);
    void addBone(const BsBone & bone = BsBone());
    void addEyeBone(const BsBone & bone = BsBone());
    QVector<BsBone> & getBones();
    QVector<BsBone> & getEyeBones();
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    bool getLookAtTarget() const;
    void setLookAtTarget(bool value);
    qreal getLimitAngleDegrees() const;
    void setLimitAngleDegrees(const qreal &value);
    qreal getLimitAngleThresholdDegrees() const;
    void setLimitAngleThresholdDegrees(const qreal &value);
    bool getContinueLookOutsideOfLimit() const;
    void setContinueLookOutsideOfLimit(bool value);
    qreal getOnGain() const;
    void setOnGain(const qreal &value);
    qreal getOffGain() const;
    void setOffGain(const qreal &value);
    bool getUseBoneGains() const;
    void setUseBoneGains(bool value);
    hkQuadVariable getTargetLocation() const;
    void setTargetLocation(const hkQuadVariable &value);
    bool getTargetOutsideLimits() const;
    void setTargetOutsideLimits(bool value);
    int getId() const;
    void setId(int value);
    hkbStringEventPayload * getPayload() const;
    void setPayload(hkbStringEventPayload *value);
    bool getLookAtCamera() const;
    void setLookAtCamera(bool value);
    qreal getLookAtCameraX() const;
    void setLookAtCameraX(const qreal &value);
    qreal getLookAtCameraY() const;
    void setLookAtCameraY(const qreal &value);
    qreal getLookAtCameraZ() const;
    void setLookAtCameraZ(const qreal &value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfBones() const;
    int getNumberOfEyeBones() const;
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
    bool lookAtTarget;
    QVector <BsBone> bones;
    QVector <BsBone> eyeBones;
    qreal limitAngleDegrees;
    qreal limitAngleThresholdDegrees;
    bool continueLookOutsideOfLimit;
    qreal onGain;
    qreal offGain;
    bool useBoneGains;
    hkQuadVariable targetLocation;
    bool targetOutsideLimits;
    int id;
    HkxSharedPtr payload;
    bool lookAtCamera;
    qreal lookAtCameraX;
    qreal lookAtCameraY;
    qreal lookAtCameraZ;
    mutable std::mutex mutex;
};
}
#endif // BSLOOKATMODIFIER_H
