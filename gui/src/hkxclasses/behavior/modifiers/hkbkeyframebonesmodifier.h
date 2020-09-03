#ifndef HKBKEYFRAMEBONESMODIFIER_H
#define HKBKEYFRAMEBONESMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbBoneIndexArray;

class hkbKeyframeBonesModifier final: public hkbModifier
{
    friend class KeyframeInfoUI;
    friend class KeyframeBonesModifierUI;
public:
    hkbKeyframeBonesModifier(HkxFile *parent, long ref = 0);
    hkbKeyframeBonesModifier& operator=(const hkbKeyframeBonesModifier&) = delete;
    hkbKeyframeBonesModifier(const hkbKeyframeBonesModifier &) = delete;
    ~hkbKeyframeBonesModifier();
public:
    QString getName() const;
    static const QString getClassname();
public:
    struct hkKeyframeInfo{
        hkKeyframeInfo() : boneIndex(-1), isValid(false){}
        hkQuadVariable keyframedPosition;
        hkQuadVariable keyframedRotation;
        int boneIndex;
        bool isValid;
    };
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkbBoneIndexArray * getKeyframedBonesList() const;
    void setKeyframedBonesList(hkbBoneIndexArray *value);
    void addKeyframeInfo(hkKeyframeInfo info = hkKeyframeInfo());
    void removeKeyframeInfo(int index);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    int getNumberOfKeyframeInfos() const;
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    QVector <hkKeyframeInfo> keyframeInfo;
    HkxSharedPtr keyframedBonesList;
    mutable std::mutex mutex;
};
}
#endif // HKBKEYFRAMEBONESMODIFIER_H
