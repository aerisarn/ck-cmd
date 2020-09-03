#ifndef HKBCOMPUTEROTATIONTOTARGETMODIFIER_H
#define HKBCOMPUTEROTATIONTOTARGETMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbComputeRotationToTargetModifier final: public hkbModifier
{
    friend class ComputeRotationToTargetModifierUI;
public:
    hkbComputeRotationToTargetModifier(HkxFile *parent, long ref = 0);
    hkbComputeRotationToTargetModifier& operator=(const hkbComputeRotationToTargetModifier&) = delete;
    hkbComputeRotationToTargetModifier(const hkbComputeRotationToTargetModifier &) = delete;
    ~hkbComputeRotationToTargetModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getRotationOut() const;
    void setRotationOut(const hkQuadVariable &value);
    hkQuadVariable getTargetPosition() const;
    void setTargetPosition(const hkQuadVariable &value);
    hkQuadVariable getCurrentPosition() const;
    void setCurrentPosition(const hkQuadVariable &value);
    hkQuadVariable getCurrentRotation() const;
    void setCurrentRotation(const hkQuadVariable &value);
    hkQuadVariable getLocalAxisOfRotation() const;
    void setLocalAxisOfRotation(const hkQuadVariable &value);
    hkQuadVariable getLocalFacingDirection() const;
    void setLocalFacingDirection(const hkQuadVariable &value);
    bool getResultIsDelta() const;
    void setResultIsDelta(bool value);
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
    hkQuadVariable rotationOut;
    hkQuadVariable targetPosition;
    hkQuadVariable currentPosition;
    hkQuadVariable currentRotation;
    hkQuadVariable localAxisOfRotation;
    hkQuadVariable localFacingDirection;
    bool resultIsDelta;
    mutable std::mutex mutex;
};
}
#endif // HKBCOMPUTEROTATIONTOTARGETMODIFIER_H
