#ifndef HKBTRANSFORMVECTORMODIFIER_H
#define HKBTRANSFORMVECTORMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbTransformVectorModifier final: public hkbModifier
{
    friend class TransformVectorModifierUI;
public:
    hkbTransformVectorModifier(HkxFile *parent, long ref = 0);
    hkbTransformVectorModifier& operator=(const hkbTransformVectorModifier&) = delete;
    hkbTransformVectorModifier(const hkbTransformVectorModifier &) = delete;
    ~hkbTransformVectorModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getRotation() const;
    void setRotation(const hkQuadVariable &value);
    hkQuadVariable getTranslation() const;
    void setTranslation(const hkQuadVariable &value);
    hkQuadVariable getVectorIn() const;
    void setVectorIn(const hkQuadVariable &value);
    hkQuadVariable getVectorOut() const;
    void setVectorOut(const hkQuadVariable &value);
    bool getRotateOnly() const;
    void setRotateOnly(bool value);
    bool getInverse() const;
    void setInverse(bool value);
    bool getComputeOnActivate() const;
    void setComputeOnActivate(bool value);
    bool getComputeOnModify() const;
    void setComputeOnModify(bool value);
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
    hkQuadVariable rotation;
    hkQuadVariable translation;
    hkQuadVariable vectorIn;
    hkQuadVariable vectorOut;
    bool rotateOnly;
    bool inverse;
    bool computeOnActivate;
    bool computeOnModify;
    mutable std::mutex mutex;
};
}
#endif // HKBTRANSFORMVECTORMODIFIER_H
