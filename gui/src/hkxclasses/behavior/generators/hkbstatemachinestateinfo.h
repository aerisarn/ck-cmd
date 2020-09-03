#ifndef HKBSTATEMACHINESTATEINFO_H
#define HKBSTATEMACHINESTATEINFO_H

#include "hkbgenerator.h"
namespace UI {
class hkbStateMachine;

class hkbStateMachineStateInfo final: public hkbGenerator
{
    friend class StateUI;
public:
    hkbStateMachineStateInfo(HkxFile *parent, hkbStateMachine *parentsm, long ref = 0);
    hkbStateMachineStateInfo& operator=(const hkbStateMachineStateInfo&) = delete;
    hkbStateMachineStateInfo(const hkbStateMachineStateInfo &) = delete;
    ~hkbStateMachineStateInfo();
public:
    void removeTransitions();
    void removeTransitionsNoLock();
    void removeTransitionToState(int id);
    void setParentSM(hkbStateMachine *value);
    bool setStateId(int id);
    void unlink();
    void setTransitionsParentSM(hkbStateMachine *parSM);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    QString getName() const;
    int getStateId() const;
    QString getNameNoLock() const;
    int getStateIdNoLock() const;
    static const QString getClassname();
    QVector <DataIconManager *> getChildren() const;
    hkbStateMachine * getNestedStateMachine() const;
    hkbStateMachine * getParentStateMachine() const;
    QString getStateName(int stateId) const;
    QString getNestedStateName(int stateId, ulong nestedStateId) const;
    QString getStateNameNoLock(int stateId) const;
    QString getNestedStateNameNoLock(int stateId, ulong nestedStateId) const;
private:
    QString getGeneratorName() const;
    void setName(const QString &newname);
    qreal getProbability() const;
    void setProbability(const qreal &value);
    bool getEnable() const;
    void setEnable(bool value);
    HkxSharedPtr getEnterNotifyEvents() const;
    HkxSharedPtr getExitNotifyEvents() const;
    HkxSharedPtr getTransitions() const;
    HkxObject * getEnterNotifyEventsData() const;
    HkxObject * getExitNotifyEventsData() const;
    HkxObject * getTransitionsData() const;
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    QString evaluateDataValidity();
    bool write(HkxXMLWriter *writer);
    bool hasChildren() const;
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    bool merge(HkxObject *recessiveObject);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    void updateTransitionStateId(bool lock, int newid);
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int, DataIconManager *obj);
    bool removeObjectAt(int index);
private:
    static uint refCount;
    static const QString classname;
    hkbStateMachine *parentSM;
    HkxSharedPtr enterNotifyEvents;
    HkxSharedPtr exitNotifyEvents;
    HkxSharedPtr transitions;
    HkxSharedPtr generator;
    QString name;
    int stateId;
    qreal probability;
    bool enable;
    mutable std::mutex mutex;
};
}
#endif // HKBSTATEMACHINESTATEINFO_H
