#ifndef HKBSTATEMACHINETRANSITIONINFOARRAY_H
#define HKBSTATEMACHINETRANSITIONINFOARRAY_H

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbblendingtransitioneffect.h"
#include "src/hkxclasses/behavior/hkbexpressioncondition.h"
namespace UI {
class hkbStateMachine;

class hkbStateMachineTransitionInfoArray final: public HkxObject
{
public:
    struct HkTransition
    {
        HkTransition()
            : eventId(-1),
              toStateId(0),
              fromNestedStateId(0),
              toNestedStateId(0),
              priority(0),
              flags("0")
        {
            //
        }

        bool operator==(const HkTransition & other) const;

        bool usingTriggerInterval() const{
            if (
                    triggerInterval.enterEventId > -1 ||
                    triggerInterval.exitEventId > -1 ||
                    triggerInterval.enterTime > 0.000000 ||
                    triggerInterval.exitTime > 0.000000
                    )
            {
                return true;
            }
            return false;
        }

        bool usingInitiateInterval() const{
            if (
                    initiateInterval.enterEventId > -1 ||
                    initiateInterval.exitEventId > -1 ||
                    initiateInterval.enterTime > 0.000000 ||
                    initiateInterval.exitTime > 0.000000
                    )
            {
                return true;
            }
            return false;
        }

        struct HkInterval
        {
            HkInterval()
                : enterEventId(-1),
                  exitEventId(-1),
                  enterTime(0),
                  exitTime(0)
            {
                //
            }
            bool operator!=(const HkInterval & other) const{
                if (
                        enterEventId == other.enterEventId ||
                        exitEventId == other.exitEventId ||
                        enterTime == other.enterTime ||
                        exitTime == other.exitTime
                   )
                {
                    return false;
                }
                return true;
            }
            int enterEventId;
            int exitEventId;
            qreal enterTime;
            qreal exitTime;
        };

        HkInterval triggerInterval;
        HkInterval initiateInterval;
        HkxSharedPtr transition;
        HkxSharedPtr condition;
        int eventId;
        int toStateId;
        int fromNestedStateId;
        int toNestedStateId;
        int priority;
        QString flags;
    };
public:
    hkbStateMachineTransitionInfoArray(HkxFile *parent, hkbGenerator *parentSM, long ref = -1);
    hkbStateMachineTransitionInfoArray& operator=(const hkbStateMachineTransitionInfoArray&) = delete;
    hkbStateMachineTransitionInfoArray(const hkbStateMachineTransitionInfoArray &) = delete;
    ~hkbStateMachineTransitionInfoArray();
public:
    static const QString getClassname();
    void updateTransitionStateId(int oldid, int newid);
    void removeTransitionToState(ulong stateId);
    void setParentSM(hkbGenerator *parS);
    int getNumTransitions() const;
    QString getTransitionNameAt(int index) const;
    hkbStateMachineTransitionInfoArray::HkTransition * getTransitionAt(int index);
    void addTransition();
    void removeTransition(int index);
private:
    enum Interval_Type {
        TRIGGER,
        INITIATE
    };
private:
    bool readData(const HkxXmlReader & reader, long & index);
    bool link();
    bool write(HkxXMLWriter *writer);
    bool isEventReferenced(int eventindex) const;
    void updateEventIndices(int eventindex);
    void mergeEventIndex(int oldindex, int newindex);
    void fixMergedEventIndices(BehaviorFile *dominantfile);
    bool merge(HkxObject *recessiveObject);
    void updateReferences(long &ref);
    QVector <HkxObject *> getChildrenOtherTypes() const;
    QString evaluateDataValidity();
private:
    static uint refCount;
    static const QString classname;
    static const QStringList transitionFlags;
    QVector <HkTransition> transitions;
    hkbGenerator *parent;
    mutable std::mutex mutex;
};
}
#endif // HKBSTATEMACHINETRANSITIONINFOARRAY_H
