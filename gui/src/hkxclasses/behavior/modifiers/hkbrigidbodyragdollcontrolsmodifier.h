#ifndef HKBRIGIDBODYRAGDOLLCONTROLSMODIFIER_H
#define HKBRIGIDBODYRAGDOLLCONTROLSMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbBoneIndexArray;

class hkbRigidBodyRagdollControlsModifier final: public hkbModifier
{
    friend class RigidBodyRagdollControlsModifierUI;
public:
    hkbRigidBodyRagdollControlsModifier(HkxFile *parent, long ref = 0);
    hkbRigidBodyRagdollControlsModifier& operator=(const hkbRigidBodyRagdollControlsModifier&) = delete;
    hkbRigidBodyRagdollControlsModifier(const hkbRigidBodyRagdollControlsModifier &) = delete;
    ~hkbRigidBodyRagdollControlsModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getHierarchyGain() const;
    void setHierarchyGain(const qreal &value);
    qreal getVelocityDamping() const;
    void setVelocityDamping(const qreal &value);
    qreal getAccelerationGain() const;
    void setAccelerationGain(const qreal &value);
    qreal getVelocityGain() const;
    void setVelocityGain(const qreal &value);
    qreal getPositionGain() const;
    void setPositionGain(const qreal &value);
    qreal getPositionMaxLinearVelocity() const;
    void setPositionMaxLinearVelocity(const qreal &value);
    qreal getPositionMaxAngularVelocity() const;
    void setPositionMaxAngularVelocity(const qreal &value);
    qreal getSnapGain() const;
    void setSnapGain(const qreal &value);
    qreal getSnapMaxLinearVelocity() const;
    void setSnapMaxLinearVelocity(const qreal &value);
    qreal getSnapMaxAngularVelocity() const;
    void setSnapMaxAngularVelocity(const qreal &value);
    qreal getSnapMaxLinearDistance() const;
    void setSnapMaxLinearDistance(const qreal &value);
    qreal getSnapMaxAngularDistance() const;
    void setSnapMaxAngularDistance(const qreal &value);
    qreal getDurationToBlend() const;
    void setDurationToBlend(const qreal &value);
    hkbBoneIndexArray * getBones() const;
    void setBones(hkbBoneIndexArray *value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    qreal hierarchyGain;
    qreal velocityDamping;
    qreal accelerationGain;
    qreal velocityGain;
    qreal positionGain;
    qreal positionMaxLinearVelocity;
    qreal positionMaxAngularVelocity;
    qreal snapGain;
    qreal snapMaxLinearVelocity;
    qreal snapMaxAngularVelocity;
    qreal snapMaxLinearDistance;
    qreal snapMaxAngularDistance;
    qreal durationToBlend;
    HkxSharedPtr bones;
    mutable std::mutex mutex;
};
}
#endif // HKBRIGIDBODYRAGDOLLCONTROLSMODIFIER_H
