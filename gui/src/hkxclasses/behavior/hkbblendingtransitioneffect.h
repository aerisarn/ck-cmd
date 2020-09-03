#ifndef HKBBLENDINGTRANSITIONEFFECT_H
#define HKBBLENDINGTRANSITIONEFFECT_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbBlendingTransitionEffect final: public HkDynamicObject
{
    friend class BlendingTransitionEffectUI;
public:
    hkbBlendingTransitionEffect(HkxFile *parent, long ref = -1);
    hkbBlendingTransitionEffect& operator=(const hkbBlendingTransitionEffect&) = delete;
    hkbBlendingTransitionEffect(const hkbBlendingTransitionEffect &) = delete;
    ~hkbBlendingTransitionEffect();
public:
    bool operator==(const hkbBlendingTransitionEffect & other) const;
public:
    QString getName() const;
private:
    void setName(const QString &newname);
    QString getSelfTransitionMode() const;
    void setSelfTransitionMode(int index);
    QString getEventMode() const;
    void setEventMode(int index);
    qreal getDuration() const;
    void setDuration(const qreal &value);
    qreal getToGeneratorStartTimeFraction() const;
    void setToGeneratorStartTimeFraction(const qreal &value);
    QString getFlags() const;
    void setFlags(int index);
    void setFlags(const QString &value);
    QString getEndMode() const;
    void setEndMode(int index);
    QString getBlendCurve() const;
    void setBlendCurve(int index);
    bool link();
    bool readData(const HkxXmlReader & reader, long & index);
    void unlink();
    QString evaluateDataValidity();
    static QString getClassname();
    bool write(HkxXMLWriter *writer);
    QVector <HkxObject *> getChildrenOtherTypes() const;
private:
    static const QStringList SelfTransitionMode;
    static const QStringList EventMode;
    static const QStringList Flags;
    static const QStringList EndMode;
    static const QStringList BlendCurve;
    static const QString classname;
    static uint refCount;
    ulong userData;
    QString name;
    QString selfTransitionMode;
    QString eventMode;
    qreal duration;
    qreal toGeneratorStartTimeFraction;
    QString flags;
    QString endMode;
    QString blendCurve;
    mutable std::mutex mutex;
};
}
#endif // HKBBLENDINGTRANSITIONEFFECT_H
