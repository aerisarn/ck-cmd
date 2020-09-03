#ifndef HKBDAMPINGMODIFIER_H
#define HKBDAMPINGMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbDampingModifier final: public hkbModifier
{
    friend class DampingModifierUI;
public:
    hkbDampingModifier(HkxFile *parent, long ref = 0);
    hkbDampingModifier& operator=(const hkbDampingModifier&) = delete;
    hkbDampingModifier(const hkbDampingModifier &) = delete;
    ~hkbDampingModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getKP() const;
    void setKP(const qreal &value);
    qreal getKI() const;
    void setKI(const qreal &value);
    qreal getKD() const;
    void setKD(const qreal &value);
    bool getEnableScalarDamping() const;
    void setEnableScalarDamping(bool value);
    bool getEnableVectorDamping() const;
    void setEnableVectorDamping(bool value);
    qreal getRawValue() const;
    void setRawValue(const qreal &value);
    qreal getDampedValue() const;
    void setDampedValue(const qreal &value);
    hkQuadVariable getRawVector() const;
    void setRawVector(const hkQuadVariable &value);
    hkQuadVariable getDampedVector() const;
    void setDampedVector(const hkQuadVariable &value);
    hkQuadVariable getVecErrorSum() const;
    void setVecErrorSum(const hkQuadVariable &value);
    hkQuadVariable getVecPreviousError() const;
    void setVecPreviousError(const hkQuadVariable &value);
    qreal getErrorSum() const;
    void setErrorSum(const qreal &value);
    qreal getPreviousError() const;
    void setPreviousError(const qreal &value);
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
    qreal kP;
    qreal kI;
    qreal kD;
    bool enableScalarDamping;
    bool enableVectorDamping;
    qreal rawValue;
    qreal dampedValue;
    hkQuadVariable rawVector;
    hkQuadVariable dampedVector;
    hkQuadVariable vecErrorSum;
    hkQuadVariable vecPreviousError;
    qreal errorSum;
    qreal previousError;
    mutable std::mutex mutex;
};
}
#endif // HKBDAMPINGMODIFIER_H
