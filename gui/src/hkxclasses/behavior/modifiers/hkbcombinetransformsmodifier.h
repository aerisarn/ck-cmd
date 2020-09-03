#ifndef HKBCOMBINETRANSFORMSMODIFIER_H
#define HKBCOMBINETRANSFORMSMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbCombineTransformsModifier final: public hkbModifier
{
    friend class CombineTransformsModifierUI;
public:
    hkbCombineTransformsModifier(HkxFile *parent, long ref = 0);
    hkbCombineTransformsModifier& operator=(const hkbCombineTransformsModifier&) = delete;
    hkbCombineTransformsModifier(const hkbCombineTransformsModifier &) = delete;
    ~hkbCombineTransformsModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getTranslationOut() const;
    void setTranslationOut(const hkQuadVariable &value);
    hkQuadVariable getRotationOut() const;
    void setRotationOut(const hkQuadVariable &value);
    hkQuadVariable getLeftTranslation() const;
    void setLeftTranslation(const hkQuadVariable &value);
    hkQuadVariable getLeftRotation() const;
    void setLeftRotation(const hkQuadVariable &value);
    hkQuadVariable getRightTranslation() const;
    void setRightTranslation(const hkQuadVariable &value);
    hkQuadVariable getRightRotation() const;
    void setRightRotation(const hkQuadVariable &value);
    bool getInvertLeftTransform() const;
    void setInvertLeftTransform(bool value);
    bool getInvertRightTransform() const;
    void setInvertRightTransform(bool value);
    bool getInvertResult() const;
    void setInvertResult(bool value);
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
    hkQuadVariable translationOut;
    hkQuadVariable rotationOut;
    hkQuadVariable leftTranslation;
    hkQuadVariable leftRotation;
    hkQuadVariable rightTranslation;
    hkQuadVariable rightRotation;
    bool invertLeftTransform;
    bool invertRightTransform;
    bool invertResult;
    mutable std::mutex mutex;
};
}
#endif // HKBCOMBINETRANSFORMSMODIFIER_H
