#ifndef BSGETTIMESTEPMODIFIER_H
#define BSGETTIMESTEPMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class BSGetTimeStepModifier final: public hkbModifier
{
    friend class BSGetTimeStepModifierUI;
public:
    BSGetTimeStepModifier(HkxFile *parent, long ref = 0);
    BSGetTimeStepModifier& operator=(const BSGetTimeStepModifier&) = delete;
    BSGetTimeStepModifier(const BSGetTimeStepModifier &) = delete;
    ~BSGetTimeStepModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getTimeStep() const;
    void setTimeStep(const qreal &value);
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
    qreal timeStep;
    mutable std::mutex mutex;
};
}
#endif // BSGETTIMESTEPMODIFIER_H
