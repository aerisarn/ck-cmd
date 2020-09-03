#ifndef HKBSTATEMACHINEEVENTPROPERTYARRAY_H
#define HKBSTATEMACHINEEVENTPROPERTYARRAY_H

#include "src/hkxclasses/hkxobject.h"
namespace UI {
class hkbStateMachineEventPropertyArray final: public HkxObject
{
    friend class StateUI;
public:
    hkbStateMachineEventPropertyArray(HkxFile *parent, long ref = -1);
    hkbStateMachineEventPropertyArray& operator=(const hkbStateMachineEventPropertyArray&) = delete;
    hkbStateMachineEventPropertyArray(const hkbStateMachineEventPropertyArray &) = delete;
    ~hkbStateMachineEventPropertyArray();
public:
    static const QString getClassname();
    int getNumOfEvents() const;
    int getLastEventIndex() const;
private:
    bool isEventReferenced(int eventindex) const;
    QVector <HkxObject *> getChildrenOtherTypes() const;
    bool link();
    void unlink();
    void removeEvent(int index);
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    void updateReferences(long &ref);
    void setEventId(int index, int id);
    bool merge(HkxObject *recessiveObject);
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    bool readData(const HkxXmlReader & reader, long & index);
    void addEvent(const hkEventPayload & event = hkEventPayload());
private:
    static uint refCount;
    static const QString classname;
    QVector <hkEventPayload> events;
    mutable std::mutex mutex;
};
}
#endif // HKBSTATEMACHINEEVENTPROPERTYARRAY_H
