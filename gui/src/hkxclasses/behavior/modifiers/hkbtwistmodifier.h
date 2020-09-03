#ifndef HKBTWISTMODIFIER_H
#define HKBTWISTMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbTwistModifier final: public hkbModifier
{
    friend class TwistModifierUI;
public:
    hkbTwistModifier(HkxFile *parent, long ref = 0);
    hkbTwistModifier& operator=(const hkbTwistModifier&) = delete;
    hkbTwistModifier(const hkbTwistModifier &) = delete;
    ~hkbTwistModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getAxisOfRotation() const;
    void setAxisOfRotation(const hkQuadVariable &value);
    qreal getTwistAngle() const;
    void setTwistAngle(const qreal &value);
    int getStartBoneIndex() const;
    void setStartBoneIndex(int value);
    int getEndBoneIndex() const;
    void setEndBoneIndex(int value);
    QString getSetAngleMethod() const;
    void setSetAngleMethod(int index);
    QString getRotationAxisCoordinates() const;
    void setRotationAxisCoordinates(int index);
    bool getIsAdditive() const;
    void setIsAdditive(bool value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList SetAngleMethod;
    static const QStringList RotationAxisCoordinates;
    long userData;
    QString name;
    bool enable;
    hkQuadVariable axisOfRotation;
    qreal twistAngle;
    int startBoneIndex;
    int endBoneIndex;
    QString setAngleMethod;
    QString rotationAxisCoordinates;
    bool isAdditive;
    mutable std::mutex mutex;
};
}
#endif // HKBTWISTMODIFIER_H
