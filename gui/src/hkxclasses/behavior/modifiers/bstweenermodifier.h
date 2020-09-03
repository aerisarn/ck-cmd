#ifndef BSTWEENERMODIFIER_H
#define BSTWEENERMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSTweenerModifier final: public hkbModifier
{
    friend class BSTweenerModifierUI;
public:
    BSTweenerModifier(HkxFile *parent, long ref = 0);
    BSTweenerModifier& operator=(const BSTweenerModifier&) = delete;
    BSTweenerModifier(const BSTweenerModifier &) = delete;
    ~BSTweenerModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    bool getTweenPosition() const;
    void setTweenPosition(bool value);
    bool getTweenRotation() const;
    void setTweenRotation(bool value);
    bool getUseTweenDuration() const;
    void setUseTweenDuration(bool value);
    qreal getTweenDuration() const;
    void setTweenDuration(const qreal &value);
    hkQuadVariable getTargetPosition() const;
    void setTargetPosition(const hkQuadVariable &value);
    hkQuadVariable getTargetRotation() const;
    void setTargetRotation(const hkQuadVariable &value);
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
    bool tweenPosition;
    bool tweenRotation;
    bool useTweenDuration;
    qreal tweenDuration;
    hkQuadVariable targetPosition;
    hkQuadVariable targetRotation;
    mutable std::mutex mutex;
};
}
#endif // BSTWEENERMODIFIER_H
