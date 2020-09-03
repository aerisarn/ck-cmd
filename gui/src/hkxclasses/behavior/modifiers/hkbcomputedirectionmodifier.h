#ifndef HKBCOMPUTEDIRECTIONMODIFIER_H
#define HKBCOMPUTEDIRECTIONMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbComputeDirectionModifier final: public hkbModifier
{
    friend class ComputeDirectionModifierUI;
public:
    hkbComputeDirectionModifier(HkxFile *parent, long ref = 0);
    hkbComputeDirectionModifier& operator=(const hkbComputeDirectionModifier&) = delete;
    hkbComputeDirectionModifier(const hkbComputeDirectionModifier &) = delete;
    ~hkbComputeDirectionModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getPointIn() const;
    void setPointIn(const hkQuadVariable &value);
    hkQuadVariable getPointOut() const;
    void setPointOut(const hkQuadVariable &value);
    qreal getGroundAngleOut() const;
    void setGroundAngleOut(const qreal &value);
    qreal getUpAngleOut() const;
    void setUpAngleOut(const qreal &value);
    qreal getVerticalOffset() const;
    void setVerticalOffset(const qreal &value);
    bool getReverseGroundAngle() const;
    void setReverseGroundAngle(bool value);
    bool getReverseUpAngle() const;
    void setReverseUpAngle(bool value);
    bool getProjectPoint() const;
    void setProjectPoint(bool value);
    bool getNormalizePoint() const;
    void setNormalizePoint(bool value);
    bool getComputeOnlyOnce() const;
    void setComputeOnlyOnce(bool value);
    bool getComputedOutput() const;
    void setComputedOutput(bool value);
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
    hkQuadVariable pointIn;
    hkQuadVariable pointOut;
    qreal groundAngleOut;
    qreal upAngleOut;
    qreal verticalOffset;
    bool reverseGroundAngle;
    bool reverseUpAngle;
    bool projectPoint;
    bool normalizePoint;
    bool computeOnlyOnce;
    bool computedOutput;
    mutable std::mutex mutex;
};
}
#endif // HKBCOMPUTEDIRECTIONMODIFIER_H
