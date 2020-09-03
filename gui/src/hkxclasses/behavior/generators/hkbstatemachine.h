#ifndef HKBSTATEMACHINE_H
#define HKBSTATEMACHINE_H

#include "hkbgenerator.h"
namespace UI {
class hkbStateMachine final: public hkbGenerator
{
    friend class StateMachineUI;
public:
    hkbStateMachine(HkxFile *parent, long ref = 0);
    hkbStateMachine& operator=(const hkbStateMachine&) = delete;
    hkbStateMachine(const hkbStateMachine &) = delete;
    ~hkbStateMachine();
public:
    QString getName() const;
    static const QString getClassname();
    QString getStateName(ulong stateId) const;
    QString getStateNameNoLock(ulong stateId) const;
    int getStateId(const QString & statename) const;
    int getNestedStateId(const QString & statename, int stateId) const;
    QStringList getStateNames() const;
    int getNumberOfStates() const;
    QString getNestedStateName(ulong stateId, ulong nestedStateId) const;
    QString getNestedStateNameNoLock(ulong stateId, ulong nestedStateId) const;
    QStringList getNestedStateNames(ulong stateId) const;
    int getNumberOfNestedStates(ulong stateId) const;
    int generateValidStateId();
    void updateTransitionStateId(int oldid, int newid);
    QVector <DataIconManager *> getChildren() const;
    void removeWildcardTransitions();
    void removeWildcardTransitionsNoLock();
private:
    bool swapChildren(int index1, int index2);
    int getEventToSendWhenStateOrTransitionChangesID() const;
    void setName(const QString &newname);
    int getStartStateId() const;
    void setStartStateId(int value);
    int getReturnToPreviousStateEventId() const;
    void setReturnToPreviousStateEventId(int value);
    int getRandomTransitionEventId() const;
    void setRandomTransitionEventId(int value);
    int getTransitionToNextHigherStateEventId() const;
    void setTransitionToNextHigherStateEventId(int value);
    int getTransitionToNextLowerStateEventId() const;
    void setTransitionToNextLowerStateEventId(int value);
    int getSyncVariableIndex() const;
    void setSyncVariableIndex(int value);
    bool getWrapAroundStateId() const;
    void setWrapAroundStateId(bool value);
    ushort getMaxSimultaneousTransitions() const;
    void setMaxSimultaneousTransitions(const ushort &value);
    QString getStartStateMode() const;
    void setStartStateMode(int index);
    QString getSelfTransitionMode() const;
    void setSelfTransitionMode(int index);
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
    bool merge(HkxObject *recessiveObject);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    int getIndexOfObj(DataIconManager *obj) const;
    bool insertObjectAt(int index, DataIconManager *obj);
    bool removeObjectAt(int index);
    hkbStateMachine * getNestedStateMachine(ulong stateId) const;
    hkbStateMachine * getNestedStateMachineNoLock(ulong stateId) const;
private:
    static uint refCount;
    static const QStringList StartStateMode;
    static const QStringList SelfTransitionMode;
    static const QString classname;
    ulong userData;
    QString name;
    //startStateChooser
    hkEventPayload eventToSendWhenStateOrTransitionChanges;
    int startStateId;
    int returnToPreviousStateEventId;
    int randomTransitionEventId;
    int transitionToNextHigherStateEventId;
    int transitionToNextLowerStateEventId;
    int syncVariableIndex;
    bool wrapAroundStateId;
    ushort maxSimultaneousTransitions;
    QString startStateMode;
    QString selfTransitionMode;
    QVector <HkxSharedPtr> states;
    HkxSharedPtr wildcardTransitions;
    mutable std::mutex mutex;
};
}
#endif // HKBSTATEMACHINE_H
