#ifndef BSTIMERMODIFIER_H
#define BSTIMERMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbStringEventPayload;

class BSTimerModifier final: public hkbModifier
{
    friend class BSTimerModifierUI;
public:
    BSTimerModifier(HkxFile *parent, long ref = 0);
    BSTimerModifier& operator=(const BSTimerModifier&) = delete;
    BSTimerModifier(const BSTimerModifier &) = delete;
    ~BSTimerModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    qreal getAlarmTimeSeconds() const;
    void setAlarmTimeSeconds(const qreal &value);
    bool getResetAlarm() const;
    void setResetAlarm(bool value);
    int getAlarmEventID() const;
    void setAlarmEventID(int value);
    hkbStringEventPayload * getAlarmEventPayload() const;
    void setAlarmEventPayload(hkbStringEventPayload *value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool merge(HkxObject *recessiveObject);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    qreal alarmTimeSeconds;
    hkEventPayload alarmEvent;
    bool resetAlarm;
    mutable std::mutex mutex;
};
}
#endif // BSTIMERMODIFIER_H
