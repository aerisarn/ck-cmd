#ifndef HKBGETUPMODIFIER_H
#define HKBGETUPMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbGetUpModifier final: public hkbModifier
{
    friend class GetUpModifierUI;
public:
    hkbGetUpModifier(HkxFile *parent, long ref = 0);
    hkbGetUpModifier& operator=(const hkbGetUpModifier&) = delete;
    hkbGetUpModifier(const hkbGetUpModifier &) = delete;
    ~hkbGetUpModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getGroundNormal() const;
    void setGroundNormal(const hkQuadVariable &value);
    qreal getDuration() const;
    void setDuration(const qreal &value);
    qreal getAlignWithGroundDuration() const;
    void setAlignWithGroundDuration(const qreal &value);
    int getRootBoneIndex() const;
    void setRootBoneIndex(int value);
    int getOtherBoneIndex() const;
    void setOtherBoneIndex(int value);
    int getAnotherBoneIndex() const;
    void setAnotherBoneIndex(int value);
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
    hkQuadVariable groundNormal;
    qreal duration;
    qreal alignWithGroundDuration;
    int rootBoneIndex;
    int otherBoneIndex;
    int anotherBoneIndex;
    mutable std::mutex mutex;
};
}
#endif // HKBGETUPMODIFIER_H
