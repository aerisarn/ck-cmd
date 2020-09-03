#ifndef BSSPEEDSAMPLERMODIFIER_H
#define BSSPEEDSAMPLERMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSSpeedSamplerModifier final: public hkbModifier
{
    friend class BSSpeedSamplerModifierUI;
public:
    BSSpeedSamplerModifier(HkxFile *parent, long ref = 0);
    BSSpeedSamplerModifier& operator=(const BSSpeedSamplerModifier&) = delete;
    BSSpeedSamplerModifier(const BSSpeedSamplerModifier &) = delete;
    ~BSSpeedSamplerModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    int getState() const;
    void setState(int value);
    qreal getDirection() const;
    void setDirection(const qreal &value);
    qreal getGoalSpeed() const;
    void setGoalSpeed(const qreal &value);
    qreal getSpeedOut() const;
    void setSpeedOut(const qreal &value);
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
    int state;
    qreal direction;
    qreal goalSpeed;
    qreal speedOut;
    mutable std::mutex mutex;
};
}
#endif // BSSPEEDSAMPLERMODIFIER_H
