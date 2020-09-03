#ifndef HKBEXTRACTRAGDOLLPOSEMODIFIER_H
#define HKBEXTRACTRAGDOLLPOSEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbExtractRagdollPoseModifier final: public hkbModifier
{
    friend class ExtractRagdollPoseModifierUI;
public:
    hkbExtractRagdollPoseModifier(HkxFile *parent, long ref = 0);
    hkbExtractRagdollPoseModifier& operator=(const hkbExtractRagdollPoseModifier&) = delete;
    hkbExtractRagdollPoseModifier(const hkbExtractRagdollPoseModifier &) = delete;
    ~hkbExtractRagdollPoseModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    int getPoseMatchingBone0() const;
    void setPoseMatchingBone0(int value);
    int getPoseMatchingBone1() const;
    void setPoseMatchingBone1(int value);
    int getPoseMatchingBone2() const;
    void setPoseMatchingBone2(int value);
    bool getEnableComputeWorldFromModel() const;
    void setEnableComputeWorldFromModel(bool value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    int poseMatchingBone0;
    int poseMatchingBone1;
    int poseMatchingBone2;
    bool enableComputeWorldFromModel;
    mutable std::mutex mutex;
};
}
#endif // HKBEXTRACTRAGDOLLPOSEMODIFIER_H
