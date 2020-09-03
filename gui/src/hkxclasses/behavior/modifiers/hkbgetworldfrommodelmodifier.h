#ifndef HKBGETWORLDFROMMODELMODIFIER_H
#define HKBGETWORLDFROMMODELMODIFIER_H

#include "hkbmodifier.h"

namespace UI {

class hkbGetWorldFromModelModifier final: public hkbModifier
{
    friend class GetWorldFromModelModifierUI;
public:
    hkbGetWorldFromModelModifier(HkxFile *parent, long ref = 0);
    hkbGetWorldFromModelModifier& operator=(const hkbGetWorldFromModelModifier&) = delete;
    hkbGetWorldFromModelModifier(const hkbGetWorldFromModelModifier &) = delete;
    ~hkbGetWorldFromModelModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkQuadVariable getTranslationOut() const;
    void setTranslationOut(const hkQuadVariable &value);
    hkQuadVariable getRotationOut() const;
    void setRotationOut(const hkQuadVariable &value);
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
    hkQuadVariable translationOut;
    hkQuadVariable rotationOut;
    mutable std::mutex mutex;
};
}
#endif // HKBGETWORLDFROMMODELMODIFIER_H
