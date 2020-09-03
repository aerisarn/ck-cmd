#ifndef HKBEVALUATEHANDLEMODIFIER_H
#define HKBEVALUATEHANDLEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbEvaluateHandleModifier final: public hkbModifier
{
    friend class EvaluateHandleModifierUI;
public:
    hkbEvaluateHandleModifier(HkxFile *parent, long ref = 0);
    hkbEvaluateHandleModifier& operator=(const hkbEvaluateHandleModifier&) = delete;
    hkbEvaluateHandleModifier(const hkbEvaluateHandleModifier &) = delete;
    ~hkbEvaluateHandleModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getHandlePositionOut() const;
    void setHandlePositionOut(const hkQuadVariable &value);
    hkQuadVariable getHandleRotationOut() const;
    void setHandleRotationOut(const hkQuadVariable &value);
    bool getIsValidOut() const;
    void setIsValidOut(bool value);
    qreal getExtrapolationTimeStep() const;
    void setExtrapolationTimeStep(const qreal &value);
    qreal getHandleChangeSpeed() const;
    void setHandleChangeSpeed(const qreal &value);
    QString getHandleChangeMode() const;
    void setHandleChangeMode(int index);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList HandleChangeMode;
    long userData;
    QString name;
    bool enable;
    HkxSharedPtr handle;
    hkQuadVariable handlePositionOut;
    hkQuadVariable handleRotationOut;
    bool isValidOut;
    qreal extrapolationTimeStep;
    qreal handleChangeSpeed;
    QString handleChangeMode;
    mutable std::mutex mutex;
};
}
#endif // HKBEVALUATEHANDLEMODIFIER_H
