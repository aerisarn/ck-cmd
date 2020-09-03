#ifndef HKBEVENTDRIVENMODIFIER_H
#define HKBEVENTDRIVENMODIFIER_H

#include "hkbmodifier.h"
namespace UI {
class hkbEventDrivenModifier final: public hkbModifier
{
    friend class EventDrivenModifierUI;
public:
    hkbEventDrivenModifier(HkxFile *parent, long ref = 0);
    hkbEventDrivenModifier& operator=(const hkbEventDrivenModifier&) = delete;
    hkbEventDrivenModifier(const hkbEventDrivenModifier &) = delete;
    ~hkbEventDrivenModifier();
public:
    QString getName() const;
    static const QString getClassname();
private:
    void setName(const QString &newname);
    bool getEnable() const;
    void setEnable(bool value);
    hkbModifier * getModifier() const;
    int getActivateEventId() const;
    void setActivateEventId(int value);
    int getDeactivateEventId() const;
    void setDeactivateEventId(int value);
    bool getActiveByDefault() const;
    void setActiveByDefault(bool value);
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    void unlink();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    QVector <DataIconManager *> getChildren() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    long userData;
    QString name;
    bool enable;
    HkxSharedPtr modifier;
    int activateEventId;
    int deactivateEventId;
    bool activeByDefault;
    mutable std::mutex mutex;
};
}
#endif // HKBEVENTDRIVENMODIFIER_H
