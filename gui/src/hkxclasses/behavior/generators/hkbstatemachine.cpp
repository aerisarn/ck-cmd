#include "hkbstatemachine.h"
#include "hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/behavior/generators/hkbmodifiergenerator.h"
#include "src/hkxclasses/behavior/generators/bsistatetagginggenerator.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h"
#include "src/hkxclasses/behavior/modifiers/hkbmodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbStateMachine::refCount = 0;

const QString hkbStateMachine::classname = "hkbStateMachine";

const QStringList hkbStateMachine::StartStateMode = {"START_STATE_MODE_DEFAULT", "START_STATE_MODE_SYNC", "START_STATE_MODE_RANDOM", "START_STATE_MODE_CHOOSER"};

const QStringList hkbStateMachine::SelfTransitionMode = {"SELF_TRANSITION_MODE_NO_TRANSITION", "SELF_TRANSITION_MODE_TRANSITION_TO_START_STATE", "SELF_TRANSITION_MODE_FORCE_TRANSITION_TO_START_STATE"};

hkbStateMachine::hkbStateMachine(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0),
      startStateId(-1),
      returnToPreviousStateEventId(-1),
      randomTransitionEventId(-1),
      transitionToNextHigherStateEventId(-1),
      transitionToNextLowerStateEventId(-1),
      syncVariableIndex(-1),
      wrapAroundStateId(false),
      maxSimultaneousTransitions(32), //Max 32, min 0.
      startStateMode(StartStateMode.first()),
      selfTransitionMode(SelfTransitionMode.first())
{
    setType(HKB_STATE_MACHINE, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "StateMachine_"+QString::number(refCount);
}

const QString hkbStateMachine::getClassname(){
    return classname;
}

QString hkbStateMachine::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

int hkbStateMachine::generateValidStateId(){
    std::lock_guard <std::mutex> guard(mutex);
    ulong stateId = 0;
    ulong prev = 0;
    hkbStateMachineStateInfo *ptr = nullptr;
    for (auto i = 0; i < states.size(); i++){
        ptr = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        stateId = ptr->getStateId();
        if (stateId <= prev){
            prev = prev + 1;
        }else if (stateId > prev){
            prev = stateId + 1;
        }
    }
    return prev;
}

int hkbStateMachine::getNumberOfStates() const{
    std::lock_guard <std::mutex> guard(mutex);
    return states.size();
}

QString hkbStateMachine::getStateName(ulong stateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineStateInfo *state;
    QString sname;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        if (state && state->getStateId() == stateId){
            sname = state->getName();
        }
    }
    return sname;
}

QString hkbStateMachine::getStateNameNoLock(ulong stateId) const{
    hkbStateMachineStateInfo *state;
    QString sname;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        if (state && state->getStateIdNoLock() == stateId){
            sname = state->getNameNoLock();
        }
    }
    return sname;
}

int hkbStateMachine::getStateId(const QString &statename) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineStateInfo *state;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        if (state && state->getName() == statename){
            return state->getStateId();
        }
    }
    return -1;
}

int hkbStateMachine::getNestedStateId(const QString &statename, int stateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *ptr = getNestedStateMachineNoLock(stateId);
    if (ptr){
        return ptr->getStateId(statename);
    }
    return -1;
}

QStringList hkbStateMachine::getStateNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    hkbStateMachineStateInfo *state;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        list.append(state->getName());
    }
    return list;
}

QString hkbStateMachine::getNestedStateName(ulong stateId, ulong nestedStateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *ptr = getNestedStateMachineNoLock(stateId);
    if (ptr){
        return ptr->getStateName(nestedStateId);
    }
    return "";
}

QString hkbStateMachine::getNestedStateNameNoLock(ulong stateId, ulong nestedStateId) const{
    hkbStateMachine *ptr = getNestedStateMachineNoLock(stateId);
    if (ptr){
        return ptr->getStateName(nestedStateId);
    }
    return "";
}

QStringList hkbStateMachine::getNestedStateNames(ulong stateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *ptr = getNestedStateMachineNoLock(stateId);
    if (ptr){
        return ptr->getStateNames();
    }
    return QStringList();
}

int hkbStateMachine::getNumberOfNestedStates(ulong stateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *ptr = getNestedStateMachineNoLock(stateId);
    if (ptr){
        return ptr->getNumberOfStates();
    }
    return -1;
}

void hkbStateMachine::updateTransitionStateId(int oldid, int newid){
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineTransitionInfoArray *trans = static_cast<hkbStateMachineTransitionInfoArray *>(wildcardTransitions.data());
    if (trans){
        trans->updateTransitionStateId(oldid, newid);
    }
}

bool hkbStateMachine::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj && obj->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
        if (index >= states.size() || index == -1){
            states.append(HkxSharedPtr(obj));
        }else if (!index || !states.isEmpty()){
            states.replace(index, HkxSharedPtr(obj));
        }
        return true;
    }
    return false;
}

bool hkbStateMachine::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineStateInfo *state = nullptr;
    hkbStateMachineTransitionInfoArray *trans = nullptr;
    ulong stateId = 0;
    if (index > -1 && index < states.size()){
        trans = static_cast<hkbStateMachineTransitionInfoArray *>(wildcardTransitions.data());
        state = static_cast<hkbStateMachineStateInfo *>(states.at(index).data());
        stateId = state->getStateId();
        if (stateId == startStateId){
            startStateId = -1;
        }
        if (trans){
            trans->removeTransitionToState(stateId);
        }
        state->unlink();
        for (auto i = 0; i < states.size(); i++){
            state->removeTransitionToState(stateId);
        }
        states.removeAt(index);
    }else if (index == -1){
        for (auto i = 0; i < states.size(); i++){
            state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
            stateId = state->getStateId();
            if (stateId == startStateId){
                startStateId = -1;
            }
            state->removeTransitionToState(stateId);
            trans = static_cast<hkbStateMachineTransitionInfoArray *>(wildcardTransitions.data());
            if (trans){
                trans->removeTransitionToState(stateId);
            }
            state->unlink();
        }
        states.clear();
    }else{
        return false;
    }
    return true;
}

bool hkbStateMachine::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < states.size(); i++){
        if (states.at(i).data()){
            return true;
        }
    }
    return false;
}

bool hkbStateMachine::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventToSendWhenStateOrTransitionChanges.id == eventindex ||
            returnToPreviousStateEventId == eventindex ||
            randomTransitionEventId == eventindex ||
            transitionToNextHigherStateEventId == eventindex ||
            transitionToNextLowerStateEventId == eventindex ||
            (wildcardTransitions.constData() && wildcardTransitions.constData()->isEventReferenced(eventindex)))
    {
        return true;
    }
    return false;
}

void hkbStateMachine::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateindices = [&](int & index){
        (index > eventindex) ? index-- : index;
    };
    updateindices(eventToSendWhenStateOrTransitionChanges.id);
    updateindices(returnToPreviousStateEventId);
    updateindices(randomTransitionEventId);
    updateindices(transitionToNextHigherStateEventId);
    updateindices(transitionToNextLowerStateEventId);
    if (wildcardTransitions.data()){
        wildcardTransitions->updateEventIndices(eventindex);
    }
}

void hkbStateMachine::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateindices = [&](int & index){
        (index == oldindex) ? index = newindex : index;
    };
    updateindices(eventToSendWhenStateOrTransitionChanges.id);
    updateindices(returnToPreviousStateEventId);
    updateindices(randomTransitionEventId);
    updateindices(transitionToNextHigherStateEventId);
    updateindices(transitionToNextLowerStateEventId);
    if (wildcardTransitions.data()){
        wildcardTransitions->mergeEventIndex(oldindex, newindex);
    }
}

void hkbStateMachine::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString thiseventname;
    int eventindex;
    if (!getIsMerged() && dominantfile){
        //TO DO: Support character properties...
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(dominantfile->getBehaviorGraphData());
        if (recdata && domdata){
            auto fixIndex = [&](int & id){ if (id < 0){return;}  //TO DO: Check if event id is -1?
                thiseventname = recdata->getEventNameAt(id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                id = eventindex;
            };
            fixIndex(eventToSendWhenStateOrTransitionChanges.id);
            fixIndex(returnToPreviousStateEventId);
            fixIndex(randomTransitionEventId);
            fixIndex(transitionToNextHigherStateEventId);
            fixIndex(transitionToNextLowerStateEventId);
            if (wildcardTransitions.data()){
                wildcardTransitions->fixMergedEventIndices(dominantfile);
            }
            setIsMerged(true);
        }
    }
}

bool hkbStateMachine::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *recobj = nullptr;
    hkbStateMachineStateInfo *thisobjstate = nullptr;
    hkbStateMachineStateInfo *otherobjstate = nullptr;
    ulong thisstateid = 0;
    ulong otherstateid = 0;
    ulong freeid = 0;
    bool add;
    QVector <DataIconManager *> objects;
    QVector <DataIconManager *> children;
    DataIconManager * obj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_STATE_MACHINE){
        recobj = static_cast<hkbStateMachine *>(recessiveObject);
        recobj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
        if (getVariableBindingSetData()){
            getVariableBindingSet()->merge(recobj->getVariableBindingSetData());
        }else if (recobj->getVariableBindingSetData()){
            getVariableBindingSet() = HkxSharedPtr(recobj->getVariableBindingSetData());
            recobj->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->getVariableBindingSetData(), -1);
        }
        for (auto i = 0; i < recobj->states.size(); i++){
            add = true;
            otherobjstate = static_cast<hkbStateMachineStateInfo *>(recobj->states.at(i).data());
            otherstateid = otherobjstate->getStateId();
            for (auto j = 0; j < states.size(); j++){
                thisobjstate = static_cast<hkbStateMachineStateInfo *>(states.at(j).data());
                thisstateid = thisobjstate->getStateId();
                if (thisstateid >= freeid){
                    freeid = thisstateid + 1;
                }
                if (otherstateid == thisstateid){
                    if (thisobjstate->getName() != otherobjstate->getName()){
                        for (auto k = j + 1; k < states.size(); k++){
                            thisobjstate = static_cast<hkbStateMachineStateInfo *>(states.at(k).data());
                            thisstateid = thisobjstate->getStateId();
                            if (thisstateid >= freeid){
                                freeid = thisstateid + 1;
                            }
                        }
                        otherobjstate->setParentSM(this);
                        otherobjstate->setStateId(freeid);
                        i = recobj->states.size();
                        j = states.size();
                    }else{
                        add = false;
                    }
                }
            }
            if (add && (/* For FNIS*/otherobjstate->getName().contains("TKDodge") || static_cast<BehaviorFile *>(getParentFile())->isNameUniqueInProject(otherobjstate))){
                insertObjectAt(i, otherobjstate);
                if (!static_cast<BehaviorFile *>(getParentFile())->existsInBehavior(otherobjstate)){
                    otherobjstate->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
                    otherobjstate->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
                    getParentFile()->addObjectToFile(otherobjstate, -1);
                    getParentFile()->addObjectToFile(otherobjstate->getVariableBindingSetData(), -1);
                }
                objects = otherobjstate->getChildren();
                while (!objects.isEmpty()){
                    obj = objects.last();
                    if (!static_cast<BehaviorFile *>(getParentFile())->existsInBehavior(obj)){
                        obj->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
                        obj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
                        getParentFile()->addObjectToFile(obj, -1);
                        getParentFile()->addObjectToFile(obj->getVariableBindingSetData(), -1);
                        children = obj->getChildren();
                    }
                    objects.removeLast();
                    objects = objects + children;
                    children.clear();
                }
            }
        }
        if (wildcardTransitions.data()){
            if (recobj->wildcardTransitions.data()){
                wildcardTransitions->merge(recobj->wildcardTransitions.data());
            }
        }else if (recobj->wildcardTransitions.data()){
            wildcardTransitions = recobj->wildcardTransitions;
            getParentFile()->addObjectToFile(recobj->wildcardTransitions.data(), -1);
            //obj->wildcardTransitions->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
        }
        setIsMerged(true);
        return true;
    }else{
        return false;
    }
}

void hkbStateMachine::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    auto update = [&](const HkxSharedPtr & shdptr){
        shdptr.data() ? shdptr.data()->setReference(++ref), shdptr.data()->updateReferences(++ref) : NULL;
    };
    setReference(ref);
    setBindingReference(++ref);
    update(eventToSendWhenStateOrTransitionChanges.payload);
    update(wildcardTransitions);
}

QVector<HkxObject *> hkbStateMachine::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto append = [&](const HkxSharedPtr & shdptr){
        shdptr.data() ? list.append(shdptr.data()) : NULL;
    };
    append(eventToSendWhenStateOrTransitionChanges.payload);
    append(wildcardTransitions);
    return list;
}

int hkbStateMachine::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < states.size(); i++){
        if (states.at(i).data() && obj){
            return i;
        }
    }
    return -1;
}

QVector<DataIconManager *> hkbStateMachine::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    for (auto i = 0; i < states.size(); i++){
        if (states.at(i).data()){
            list.append(static_cast<DataIconManager*>(states.at(i).data()));
        }
    }
    return list;
}

void hkbStateMachine::removeWildcardTransitions(){
    std::lock_guard <std::mutex> guard(mutex);
    wildcardTransitions = HkxSharedPtr();
}

void hkbStateMachine::removeWildcardTransitionsNoLock(){
    wildcardTransitions = HkxSharedPtr();
}

hkbStateMachine * hkbStateMachine::getNestedStateMachine(ulong stateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineStateInfo *state = nullptr;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        if (state && state->getStateId() == stateId){
            return state->getNestedStateMachine();
        }
    }
    return nullptr;
}

hkbStateMachine *hkbStateMachine::getNestedStateMachineNoLock(ulong stateId) const{
    hkbStateMachineStateInfo *state = nullptr;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        if (state && state->getStateId() == stateId){
            return state->getNestedStateMachine();
        }
    }
    return nullptr;
}

bool hkbStateMachine::swapChildren(int index1, int index2){
    std::lock_guard <std::mutex> guard(mutex);
    if (states.size() > index1 && states.size() > index2 && index1 != index2 && index1 >= 0 && index2 >= 0){
        auto gen1 = states.at(index1).data();
        auto gen2 = states.at(index2).data();
        states[index1] = HkxSharedPtr(gen2);
        states[index2] = HkxSharedPtr(gen1);
        setIsFileChanged(true);
        return true;
    }
    return false;
}

int hkbStateMachine::getEventToSendWhenStateOrTransitionChangesID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToSendWhenStateOrTransitionChanges.id;
}

void hkbStateMachine::setSelfTransitionMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < SelfTransitionMode.size() && selfTransitionMode != SelfTransitionMode.at(index)) ? selfTransitionMode = SelfTransitionMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'selfTransitionMode' was not set!");
}

QString hkbStateMachine::getSelfTransitionMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return selfTransitionMode;
}

QString hkbStateMachine::getStartStateMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startStateMode;
}

void hkbStateMachine::setStartStateMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < StartStateMode.size() && startStateMode != StartStateMode.at(index)) ? startStateMode = StartStateMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startStateMode' was not set!");
}

ushort hkbStateMachine::getMaxSimultaneousTransitions() const{
    std::lock_guard <std::mutex> guard(mutex);
    return maxSimultaneousTransitions;
}

void hkbStateMachine::setMaxSimultaneousTransitions(const ushort &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != maxSimultaneousTransitions) ? maxSimultaneousTransitions = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'maxSimultaneousTransitions' was not set!");
}

bool hkbStateMachine::getWrapAroundStateId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return wrapAroundStateId;
}

void hkbStateMachine::setWrapAroundStateId(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != wrapAroundStateId) ? wrapAroundStateId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'wrapAroundStateId' was not set!");
}

int hkbStateMachine::getSyncVariableIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return syncVariableIndex;
}

void hkbStateMachine::setSyncVariableIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != syncVariableIndex) ? syncVariableIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'syncVariableIndex' was not set!");
}

int hkbStateMachine::getTransitionToNextLowerStateEventId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return transitionToNextLowerStateEventId;
}

void hkbStateMachine::setTransitionToNextLowerStateEventId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != transitionToNextLowerStateEventId) ? transitionToNextLowerStateEventId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'transitionToNextLowerStateEventId' was not set!");
}

int hkbStateMachine::getTransitionToNextHigherStateEventId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return transitionToNextHigherStateEventId;
}

void hkbStateMachine::setTransitionToNextHigherStateEventId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != transitionToNextHigherStateEventId) ? transitionToNextHigherStateEventId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'transitionToNextHigherStateEventId' was not set!");
}

int hkbStateMachine::getRandomTransitionEventId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return randomTransitionEventId;
}

void hkbStateMachine::setRandomTransitionEventId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != randomTransitionEventId) ? randomTransitionEventId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'randomTransitionEventId' was not set!");
}

int hkbStateMachine::getReturnToPreviousStateEventId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return returnToPreviousStateEventId;
}

void hkbStateMachine::setReturnToPreviousStateEventId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != returnToPreviousStateEventId) ? returnToPreviousStateEventId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'returnToPreviousStateEventId' was not set!");
}

int hkbStateMachine::getStartStateId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startStateId;
}

void hkbStateMachine::setStartStateId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != startStateId && value < states.size() && value > -1) ? startStateId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startStateId' was not set!");
}

void hkbStateMachine::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbStateMachine::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "variableBindingSet"){
            checkvalue(getVariableBindingSet().readShdPtrReference(index, reader), "variableBindingSet");
        }else if (text == "userData"){
            userData = reader.getElementValueAt(index).toULong(&ok);
            checkvalue(ok, "userData");
        }else if (text == "name"){
            name = reader.getElementValueAt(index);
            checkvalue((name != ""), "name");
        }else if (text == "id"){
            eventToSendWhenStateOrTransitionChanges.id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "eventToSendWhenStateOrTransitionChanges.id");
        }else if (text == "payload"){
            checkvalue(eventToSendWhenStateOrTransitionChanges.payload.readShdPtrReference(index, reader), "eventToSendWhenStateOrTransitionChanges.payload");
        /*}*else if (text == "startStateChooser"){
            if (!generator.readReference(index, reader)){
                return false;
            }*/
        }else if (text == "startStateId"){
            startStateId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "startStateId");
        }else if (text == "returnToPreviousStateEventId"){
            returnToPreviousStateEventId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "returnToPreviousStateEventId");
        }else if (text == "transitionToNextHigherStateEventId"){
            transitionToNextHigherStateEventId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "transitionToNextHigherStateEventId");
        }else if (text == "transitionToNextLowerStateEventId"){
            transitionToNextLowerStateEventId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "transitionToNextLowerStateEventId");
        }else if (text == "syncVariableIndex"){
            syncVariableIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "syncVariableIndex");
        }else if (text == "wrapAroundStateId"){
            wrapAroundStateId = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "wrapAroundStateId");
        }else if (text == "maxSimultaneousTransitions"){
            maxSimultaneousTransitions = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "maxSimultaneousTransitions");
        }else if (text == "startStateMode"){
            startStateMode = reader.getElementValueAt(index);
            checkvalue(StartStateMode.contains(startStateMode), "startStateMode");
        }else if (text == "selfTransitionMode"){
            selfTransitionMode = reader.getElementValueAt(index);
            checkvalue(SelfTransitionMode.contains(selfTransitionMode), "selfTransitionMode");
        }else if (text == "states"){
            checkvalue(readReferences(reader.getElementValueAt(index), states), "states");
        }else if (text == "wildcardTransitions"){
            checkvalue(wildcardTransitions.readShdPtrReference(index, reader), "wildcardTransitions");
        }
    }
    index--;
    return true;
}

bool hkbStateMachine::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    auto writeref = [&](const HkxSharedPtr & shdptr, const QString & name){
        QString refString = "null";
        (shdptr.data()) ? refString = shdptr->getReferenceString() : NULL;
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), refString);
    };
    auto writechild = [&](const HkxSharedPtr & shdptr, const QString & datafield){
        if (shdptr.data() && !shdptr->write(writer))
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write '"+datafield+"'!!!");
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writeref(getVariableBindingSet(), "variableBindingSet");
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writedatafield("eventToSendWhenStateOrTransitionChanges", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToSendWhenStateOrTransitionChanges.id), false);
        writeref(eventToSendWhenStateOrTransitionChanges.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("startStateChooser", "null", false);
        writedatafield("startStateId", QString::number(startStateId), false);
        writedatafield("returnToPreviousStateEventId", QString::number(returnToPreviousStateEventId), false);
        writedatafield("randomTransitionEventId", QString::number(randomTransitionEventId), false);
        writedatafield("transitionToNextHigherStateEventId", QString::number(transitionToNextHigherStateEventId), false);
        writedatafield("transitionToNextLowerStateEventId", QString::number(transitionToNextLowerStateEventId), false);
        writedatafield("syncVariableIndex", QString::number(syncVariableIndex), false);
        writedatafield("wrapAroundStateId", getBoolAsString(wrapAroundStateId), false);
        writedatafield("maxSimultaneousTransitions", QString::number(maxSimultaneousTransitions), false);
        writedatafield("startStateMode", startStateMode, false);
        writedatafield("selfTransitionMode", selfTransitionMode, false);
        refString = "";
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"states", QString::number(states.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < states.size(); i++, j++){
            refString.append(states.at(i)->getReferenceString());
            (!(j % 16)) ? refString.append("\n") : refString.append(" ");
        }
        if (states.size() > 0){
            if (refString.endsWith(" \0")){
                refString.remove(refString.lastIndexOf(" "), 1);
            }
            writer->writeLine(refString);
            writer->writeLine(writer->parameter, false);
        }
        writeref(wildcardTransitions, "wildcardTransitions");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(eventToSendWhenStateOrTransitionChanges.payload, "eventToSendWhenStateOrTransitionChanges.payload");
        writechild(wildcardTransitions, "wildcardTransitions");
        for (auto i = 0; i < states.size(); i++){
            writechild(states.at(i), "states.at("+QString::number(i)+")");
        }
    }
    return true;
}

bool hkbStateMachine::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToSendWhenStateOrTransitionChanges.payload.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'payload' is not a HKB_STRING_EVENT_PAYLOAD!");
        }
        eventToSendWhenStateOrTransitionChanges.payload = *ptr;
    }
    for (auto i = 0; i < states.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(states.at(i).getShdPtrReference());
        if (!ptr || !ptr->data()){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'states' data field!\nObject Name: "+name);
            setDataValidity(false);
        }else if ((*ptr)->getSignature() != HKB_STATE_MACHINE_STATE_INFO){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'generator' data field is linked to invalid child!\nObject Name: "+name);
            setDataValidity(false);
            states[i] = *ptr;
        }else{
            states[i] = *ptr;
            static_cast<hkbStateMachineStateInfo *>(states[i].data())->setParentSM(this);
            static_cast<hkbStateMachineStateInfo *>(states[i].data())->setTransitionsParentSM(this);
        }
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(wildcardTransitions.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'wildcardTransitions' is not a HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY!");
        }
        wildcardTransitions = *ptr;
        static_cast<hkbStateMachineTransitionInfoArray *>(wildcardTransitions.data())->setParentSM(this);
    }
    return true;
}

void hkbStateMachine::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    eventToSendWhenStateOrTransitionChanges.payload = HkxSharedPtr();
    for (auto i = 0; i < states.size(); i++){
        if (states.at(i).data()){
            states[i]->unlink(); //Do here since this is not stored in the hkx file for long...
        }
        states[i] = HkxSharedPtr();
    }
    wildcardTransitions = HkxSharedPtr();
}

QString hkbStateMachine::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (states.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": states is empty!");
    }else{
        for (auto i = states.size() - 1; i >= 0; i--){
            if (!states.at(i).data()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": states at index '"+QString::number(i)+"' is null! Removing child!");
                states.removeAt(i);
            }else if (states.at(i)->getSignature() != HKB_STATE_MACHINE_STATE_INFO){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid state! Signature: "+QString::number(states.at(i)->getSignature(), 16)+" Removing child!");
                states.removeAt(i);
            }
        }
    }
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    hkbStateMachineStateInfo *state;
    QString sname;
    for (auto i = 0; i < states.size(); i++){
        state = static_cast<hkbStateMachineStateInfo *>(states.at(i).data());
        if (state && state->getStateId() == startStateId){
            sname = state->getName();
        }
    }
    if (sname == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid startStateId! Setting default value!");
        if (!states.isEmpty()){
            startStateId = static_cast<hkbStateMachineStateInfo *>(states.first().data())->getStateId();
        }else{
            startStateId = -1;
        }
    }
    if (eventToSendWhenStateOrTransitionChanges.payload.data() && eventToSendWhenStateOrTransitionChanges.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid eventToSendWhenStateOrTransitionChanges.payload type! Signature: "+QString::number(eventToSendWhenStateOrTransitionChanges.payload->getSignature(), 16)+" Setting null value!");
        eventToSendWhenStateOrTransitionChanges.payload = HkxSharedPtr();
    }
    if (returnToPreviousStateEventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": returnToPreviousStateEventId event id out of range! Setting to max index in range!");
        returnToPreviousStateEventId = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (randomTransitionEventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": randomTransitionEventId event id out of range! Setting to max index in range!");
        randomTransitionEventId = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (transitionToNextHigherStateEventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": transitionToNextHigherStateEventId event id out of range! Setting to max index in range!");
        transitionToNextHigherStateEventId = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (transitionToNextLowerStateEventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": transitionToNextLowerStateEventId event id out of range! Setting to max index in range!");
        transitionToNextLowerStateEventId = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (syncVariableIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfVariables()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": syncVariableIndex out of range! Setting to last variable index!");
        syncVariableIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfVariables() - 1;
    }
    if (startStateMode == "START_STATE_MODE_SYNC" && syncVariableIndex < 0){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Using START_STATE_MODE_SYNC but syncVariableIndex is not set! Setting to max index in range!");
        syncVariableIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfVariables() - 1;
    }
    if (!StartStateMode.contains(startStateMode)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid startStateMode! Setting default value!");
        startStateMode = StartStateMode.first();
    }
    if (!SelfTransitionMode.contains(selfTransitionMode)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid selfTransitionMode! Setting default value!");
        selfTransitionMode = SelfTransitionMode.first();
    }
    if (maxSimultaneousTransitions > 32 || maxSimultaneousTransitions < 0){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid maxSimultaneousTransitions! Setting default value!");
        maxSimultaneousTransitions = 32;
    }
    if (wildcardTransitions.data()){
        if (wildcardTransitions->getSignature() != HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid wildcardTransitions type! Signature: "+QString::number(wildcardTransitions->getSignature(), 16)+" Setting null value!");
            wildcardTransitions = HkxSharedPtr();
        }else if (static_cast<hkbStateMachineTransitionInfoArray *>(wildcardTransitions.data())->getNumTransitions() < 1){
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": wildcardTransitions has no transitions! Setting null value!");
            wildcardTransitions = HkxSharedPtr();
        }/*else if (wildcardTransitions->isDataValid() && wildcardTransitions->evaluateDataValidity() != ""){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid wildcardTransitions data!");
        }*/
    }
    setDataValidity(isvalid);
    return errors;
}

hkbStateMachine::~hkbStateMachine(){
    refCount--;
}
