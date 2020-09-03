#ifndef HKBPOWEREDRAGDOLLCONTROLSMODIFIER_H
#define HKBPOWEREDRAGDOLLCONTROLSMODIFIER_H

#include "hkbmodifier.h"

namespace UI {
class hkbBoneWeightArray;
class hkbBoneIndexArray;

class hkbPoweredRagdollControlsModifier: public hkbModifier
{
    friend class PoweredRagdollControlsModifierUI;
public:
    hkbPoweredRagdollControlsModifier(HkxFile *parent, long ref = 0);
    hkbPoweredRagdollControlsModifier& operator=(const hkbPoweredRagdollControlsModifier&) = delete;
    hkbPoweredRagdollControlsModifier(const hkbPoweredRagdollControlsModifier &) = delete;
    ~hkbPoweredRagdollControlsModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getMaxForce() const;
    void setMaxForce(const qreal &value);
    qreal getTau() const;
    void setTau(const qreal &value);
    qreal getDamping() const;
    void setDamping(const qreal &value);
    qreal getProportionalRecoveryVelocity() const;
    void setProportionalRecoveryVelocity(const qreal &value);
    qreal getConstantRecoveryVelocity() const;
    void setConstantRecoveryVelocity(const qreal &value);
    int getPoseMatchingBone0() const;
    void setPoseMatchingBone0(int value);
    int getPoseMatchingBone1() const;
    void setPoseMatchingBone1(int value);
    int getPoseMatchingBone2() const;
    void setPoseMatchingBone2(int value);
    QString getMode() const;
    void setMode(int index);
    hkbBoneWeightArray * getBoneWeights() const;
    hkbBoneIndexArray * getBones() const;
    void setBoneWeights(hkbBoneWeightArray *value);
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
    static const QStringList Mode;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    qreal maxForce;
    qreal tau;
    qreal damping;
    qreal proportionalRecoveryVelocity;
    qreal constantRecoveryVelocity;
    HkxSharedPtr bones;
    int poseMatchingBone0;
    int poseMatchingBone1;
    int poseMatchingBone2;
    QString mode;
    HkxSharedPtr boneWeights;
    mutable std::mutex mutex;
};
}
#endif // HKBPOWEREDRAGDOLLCONTROLSMODIFIER_H
