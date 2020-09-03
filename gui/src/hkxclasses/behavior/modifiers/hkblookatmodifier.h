#ifndef HKBLOOKATMODIFIER_H
#define HKBLOOKATMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbLookAtModifier final: public hkbModifier
{
    friend class LookAtModifierUI;
public:
    hkbLookAtModifier(HkxFile *parent, long ref = 0);
    hkbLookAtModifier& operator=(const hkbLookAtModifier&) = delete;
    hkbLookAtModifier(const hkbLookAtModifier &) = delete;
    ~hkbLookAtModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getTargetWS() const;
    void setTargetWS(const hkQuadVariable &value);
    hkQuadVariable getHeadForwardLS() const;
    void setHeadForwardLS(const hkQuadVariable &value);
    hkQuadVariable getNeckForwardLS() const;
    void setNeckForwardLS(const hkQuadVariable &value);
    hkQuadVariable getNeckRightLS() const;
    void setNeckRightLS(const hkQuadVariable &value);
    hkQuadVariable getEyePositionHS() const;
    void setEyePositionHS(const hkQuadVariable &value);
    qreal getNewTargetGain() const;
    void setNewTargetGain(const qreal &value);
    qreal getOnGain() const;
    void setOnGain(const qreal &value);
    qreal getOffGain() const;
    void setOffGain(const qreal &value);
    qreal getLimitAngleDegrees() const;
    void setLimitAngleDegrees(const qreal &value);
    qreal getLimitAngleLeft() const;
    void setLimitAngleLeft(const qreal &value);
    qreal getLimitAngleRight() const;
    void setLimitAngleRight(const qreal &value);
    qreal getLimitAngleUp() const;
    void setLimitAngleUp(const qreal &value);
    qreal getLimitAngleDown() const;
    void setLimitAngleDown(const qreal &value);
    int getHeadIndex() const;
    void setHeadIndex(int value);
    int getNeckIndex() const;
    void setNeckIndex(int value);
    bool getIsOn() const;
    void setIsOn(bool value);
    bool getIndividualLimitsOn() const;
    void setIndividualLimitsOn(bool value);
    bool getIsTargetInsideLimitCone() const;
    void setIsTargetInsideLimitCone(bool value);
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
    hkQuadVariable targetWS;
    hkQuadVariable headForwardLS;
    hkQuadVariable neckForwardLS;
    hkQuadVariable neckRightLS;
    hkQuadVariable eyePositionHS;
    qreal newTargetGain;
    qreal onGain;
    qreal offGain;
    qreal limitAngleDegrees;
    qreal limitAngleLeft;
    qreal limitAngleRight;
    qreal limitAngleUp;
    qreal limitAngleDown;
    int headIndex;
    int neckIndex;
    bool isOn;
    bool individualLimitsOn;
    bool isTargetInsideLimitCone;
    mutable std::mutex mutex;
};
}
#endif // HKBLOOKATMODIFIER_H
