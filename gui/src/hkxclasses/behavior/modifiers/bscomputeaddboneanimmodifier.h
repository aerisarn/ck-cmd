#ifndef BSCOMPUTEADDBONEANIMMODIFIER_H
#define BSCOMPUTEADDBONEANIMMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSComputeAddBoneAnimModifier final: public hkbModifier
{
    friend class BSComputeAddBoneAnimModifierUI;
public:
    BSComputeAddBoneAnimModifier(HkxFile *parent, long ref = 0);
    BSComputeAddBoneAnimModifier& operator=(const BSComputeAddBoneAnimModifier&) = delete;
    BSComputeAddBoneAnimModifier(const BSComputeAddBoneAnimModifier &) = delete;
    ~BSComputeAddBoneAnimModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    int getBoneIndex() const;
    void setBoneIndex(int value);
    hkQuadVariable getTranslationLSOut() const;
    void setTranslationLSOut(const hkQuadVariable &value);
    hkQuadVariable getRotationLSOut() const;
    void setRotationLSOut(const hkQuadVariable &value);
    hkQuadVariable getScaleLSOut() const;
    void setScaleLSOut(const hkQuadVariable &value);
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
    int boneIndex;
    hkQuadVariable translationLSOut;
    hkQuadVariable rotationLSOut;
    hkQuadVariable scaleLSOut;
    mutable std::mutex mutex;
};
}
#endif // BSCOMPUTEADDBONEANIMMODIFIER_H
