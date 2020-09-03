#ifndef BSLIMBIKMODIFIER_H
#define BSLIMBIKMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSLimbIKModifier final: public hkbModifier
{
    friend class BSLimbIKModifierUI;
public:
    BSLimbIKModifier(HkxFile *parent, long ref = 0);
    BSLimbIKModifier& operator=(const BSLimbIKModifier&) = delete;
    BSLimbIKModifier(const BSLimbIKModifier &) = delete;
    ~BSLimbIKModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getLimitAngleDegrees() const;
    void setLimitAngleDegrees(const qreal &value);
    int getStartBoneIndex() const;
    void setStartBoneIndex(int value);
    int getEndBoneIndex() const;
    void setEndBoneIndex(int value);
    qreal getGain() const;
    void setGain(const qreal &value);
    qreal getBoneRadius() const;
    void setBoneRadius(const qreal &value);
    qreal getCastOffset() const;
    void setCastOffset(const qreal &value);
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
    qreal limitAngleDegrees;
    int startBoneIndex;
    int endBoneIndex;
    qreal gain;
    qreal boneRadius;
    qreal castOffset;
    mutable std::mutex mutex;
};
}
#endif // BSLIMBIKMODIFIER_H
