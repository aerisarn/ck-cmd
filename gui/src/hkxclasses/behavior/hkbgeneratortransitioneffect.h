#ifndef HKBGENERATORTRANSITIONEFFECT_H
#define HKBGENERATORTRANSITIONEFFECT_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbGeneratorTransitionEffect final: public HkDynamicObject
{
public:
    hkbGeneratorTransitionEffect(HkxFile *parent, long ref = -1);
    hkbGeneratorTransitionEffect& operator=(const hkbGeneratorTransitionEffect&) = delete;
    hkbGeneratorTransitionEffect(const hkbGeneratorTransitionEffect &) = delete;
    ~hkbGeneratorTransitionEffect();
public:
    QString getName() const;
    static const QString getClassname();
private:
    bool link();
    bool readData(const HkxXmlReader & reader, long & index);
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
private:
    static uint refCount;
    static const QString classname;
    static const QStringList EventMode;
    static const QStringList SelfTransitionMode;
    ulong userData;
    QString name;
    QString selfTransitionMode;
    QString eventMode;
    HkxSharedPtr transitionGenerator;  //This may preserve generators unexpectedly...
    qreal blendInDuration;
    qreal blendOutDuration;
    bool syncToGeneratorStartTime;
    mutable std::mutex mutex;
};
}
#endif // HKBGENERATORTRANSITIONEFFECT_H
