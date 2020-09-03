#ifndef HKBROTATECHARACTERMODIFIER_H
#define HKBROTATECHARACTERMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbRotateCharacterModifier final: public hkbModifier
{
    friend class RotateCharacterModifierUI;
public:
    hkbRotateCharacterModifier(HkxFile *parent, long ref = 0);
    hkbRotateCharacterModifier& operator=(const hkbRotateCharacterModifier&) = delete;
    hkbRotateCharacterModifier(const hkbRotateCharacterModifier &) = delete;
    ~hkbRotateCharacterModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getDegreesPerSecond() const;
    void setDegreesPerSecond(const qreal &value);
    qreal getSpeedMultiplier() const;
    void setSpeedMultiplier(const qreal &value);
    hkQuadVariable getAxisOfRotation() const;
    void setAxisOfRotation(const hkQuadVariable &value);
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
    qreal degreesPerSecond;
    qreal speedMultiplier;
    hkQuadVariable axisOfRotation;
    mutable std::mutex mutex;
};
}
#endif // HKBROTATECHARACTERMODIFIER_H
