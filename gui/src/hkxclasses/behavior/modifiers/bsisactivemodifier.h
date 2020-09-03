#ifndef BSISACTIVEMODIFIER_H
#define BSISACTIVEMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSIsActiveModifier final: public hkbModifier
{
    friend class BSIsActiveModifierUI;
public:
    BSIsActiveModifier(HkxFile *parent, long ref = 0);
    BSIsActiveModifier& operator=(const BSIsActiveModifier&) = delete;
    BSIsActiveModifier(const BSIsActiveModifier &) = delete;
    ~BSIsActiveModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    bool getBIsActive0() const;
    void setBIsActive0(bool value);
    bool getBInvertActive0() const;
    void setBInvertActive0(bool value);
    bool getBIsActive1() const;
    void setBIsActive1(bool value);
    bool getBInvertActive1() const;
    void setBInvertActive1(bool value);
    bool getBIsActive2() const;
    void setBIsActive2(bool value);
    bool getBInvertActive2() const;
    void setBInvertActive2(bool value);
    bool getBIsActive3() const;
    void setBIsActive3(bool value);
    bool getBInvertActive3() const;
    void setBInvertActive3(bool value);
    bool getBIsActive4() const;
    void setBIsActive4(bool value);
    bool getBInvertActive4() const;
    void setBInvertActive4(bool value);
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
    bool bIsActive0;
    bool bInvertActive0;
    bool bIsActive1;
    bool bInvertActive1;
    bool bIsActive2;
    bool bInvertActive2;
    bool bIsActive3;
    bool bInvertActive3;
    bool bIsActive4;
    bool bInvertActive4;
    mutable std::mutex mutex;
};
}
#endif // BSISACTIVEMODIFIER_H
