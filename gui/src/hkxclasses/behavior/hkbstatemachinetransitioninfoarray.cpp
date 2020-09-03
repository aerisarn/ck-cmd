#include "hkbstatemachinetransitioninfoarray.h"
#include "generators/hkbstatemachine.h"
#include "generators/hkbstatemachinestateinfo.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbStateMachineTransitionInfoArray::refCount = 0;

const QString hkbStateMachineTransitionInfoArray::classname = "hkbStateMachineTransitionInfoArray";

const QStringList hkbStateMachineTransitionInfoArray::transitionFlags = {
    "0",
    "FLAG_USE_TRIGGER_INTERVAL",
    "FLAG_USE_INITIATE_INTERVAL",
    "FLAG_UNINTERRUPTIBLE_WHILE_PLAYING",
    "FLAG_UNINTERRUPTIBLE_WHILE_DELAYED",
    "FLAG_DELAY_STATE_CHANGE",
    "FLAG_DISABLED",
    "FLAG_DISALLOW_RETURN_TO_PREVIOUS_STATE",
    "FLAG_DISALLOW_RANDOM_TRANSITION",
    "FLAG_DISABLE_CONDITION",
    "FLAG_ALLOW_SELF_TRANSITION_BY_TRANSITION_FROM_ANY_STATE",
    "FLAG_IS_GLOBAL_WILDCARD",
    "FLAG_IS_LOCAL_WILDCARD",
    "FLAG_FROM_NESTED_STATE_ID_IS_VALID",
    "FLAG_TO_NESTED_STATE_ID_IS_VALID",
    "FLAG_ABUT_AT_END_OF_FROM_GENERATOR",
};

hkbStateMachineTransitionInfoArray::hkbStateMachineTransitionInfoArray(HkxFile *parent, hkbGenerator *parentSM, long ref)
    : HkxObject(parent, ref),
      parent(parentSM)
{
    setType(HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

bool hkbStateMachineTransitionInfoArray::HkTransition::operator ==(const HkTransition & other) const{
    if (
            triggerInterval != other.triggerInterval ||
            initiateInterval != other.initiateInterval ||
            eventId != other.eventId ||
            toStateId != other.toStateId ||
            fromNestedStateId != other.fromNestedStateId ||
            toNestedStateId != other.toNestedStateId ||
            priority != other.priority
            )
    {
        return false;
    }
    QStringList domflags = flags.split("|");
    QStringList recflags = other.flags.split("|");
    //TO DO...
    hkbExpressionCondition *exp = static_cast<hkbExpressionCondition *>(condition.data());
    hkbExpressionCondition *otherexp = static_cast<hkbExpressionCondition *>(other.condition.data());
    hkbBlendingTransitionEffect *effect = static_cast<hkbBlendingTransitionEffect *>(transition.data());
    hkbBlendingTransitionEffect *othereffect = static_cast<hkbBlendingTransitionEffect *>(other.transition.data());
    if ((((exp && otherexp) && *exp == *otherexp) || (!exp && !otherexp)) && (((effect && othereffect) && *effect == *othereffect) || (!effect && !othereffect))){
        return true;
    }
    return false;
}

const QString hkbStateMachineTransitionInfoArray::getClassname(){
    return classname;
}

int hkbStateMachineTransitionInfoArray::getNumTransitions() const{
    std::lock_guard <std::mutex> guard(mutex);
    return transitions.size();
}

void hkbStateMachineTransitionInfoArray::addTransition(){
    std::lock_guard <std::mutex> guard(mutex);
    transitions.append(HkTransition()), setIsFileChanged(true);
}

void hkbStateMachineTransitionInfoArray::removeTransition(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (transitions.size() > index && index > -1) ? transitions.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": Transition was not removed!");
}

void hkbStateMachineTransitionInfoArray::removeTransitionToState(ulong stateId){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = transitions.size() - 1; i >= 0; i--){
        (transitions.at(i).toStateId == stateId) ? transitions.removeAt(i), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": Transition was not removed!");
    }
}

void hkbStateMachineTransitionInfoArray::updateTransitionStateId(int oldid, int newid){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto j = 0; j < transitions.size(); j++){
        (transitions.at(j).toStateId == oldid) ? transitions[j].toStateId = newid, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": Transition state id was not updated!");
    }
}

void hkbStateMachineTransitionInfoArray::setParentSM(hkbGenerator *parS){
    std::lock_guard <std::mutex> guard(mutex);
    parent = parS;
}

QString hkbStateMachineTransitionInfoArray::getTransitionNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *stateMachine;
    if (parent){
        if (index > -1 && index < transitions.size()){
            if (parent->getSignature() == HKB_STATE_MACHINE){
                stateMachine = static_cast<hkbStateMachine *>(parent);
            }else if (parent->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
                stateMachine = static_cast<hkbStateMachineStateInfo *>(parent)->getParentStateMachine();
            }
            if (stateMachine){
                if (transitions.at(index).flags.contains("FLAG_TO_NESTED_STATE_ID_IS_VALID")){
                    return "to_"+stateMachine->getNestedStateName(transitions.at(index).toStateId, transitions.at(index).toNestedStateId)+"_Via_"+stateMachine->getStateName(transitions.at(index).toStateId);
                }
                return "to_"+stateMachine->getStateName(transitions.at(index).toStateId);
            }
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": getTransitionNameAt: No parent!!! REF: "+getReferenceString());
    }
    return "";
}

bool hkbStateMachineTransitionInfoArray::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    QByteArray text;
    auto numtrans = 0;
    QStringList list;
    Interval_Type intervalType;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "transitions"){
            numtrans = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "transitions");
            (numtrans > 0) ? index++ : NULL;
            for (auto j = 0; j < numtrans; j++, index++){
                transitions.append(HkTransition());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "triggerInterval"){
                        intervalType = TRIGGER;
                    }else if (text == "initiateInterval"){
                        intervalType = INITIATE;
                    }else if (text == "enterEventId"){
                        auto temp = reader.getElementValueAt(index).toInt(&ok);
                        (intervalType == TRIGGER) ? transitions.last().triggerInterval.enterEventId = temp : transitions.last().initiateInterval.enterEventId = temp;
                        checkvalue(ok, "enterEventId");
                    }else if (text == "exitEventId"){
                        auto temp = reader.getElementValueAt(index).toInt(&ok);
                        (intervalType == TRIGGER) ? transitions.last().triggerInterval.exitEventId = temp : transitions.last().initiateInterval.exitEventId = temp;
                        checkvalue(ok, "exitEventId");
                    }else if (text == "enterTime"){
                        auto temp = reader.getElementValueAt(index).toDouble(&ok);
                        (intervalType == TRIGGER) ? transitions.last().triggerInterval.enterTime = temp : transitions.last().initiateInterval.enterTime = temp;
                        checkvalue(ok, "enterTime");
                    }else if (text== "exitTime"){
                        auto temp = reader.getElementValueAt(index).toDouble(&ok);
                        (intervalType == TRIGGER) ? transitions.last().triggerInterval.exitTime = temp : transitions.last().initiateInterval.exitTime = temp;
                        checkvalue(ok, "exitTime");
                    }else if (text == "transition"){
                        checkvalue(transitions.last().transition.readShdPtrReference(index, reader), "transition");
                    }else if (text == "condition"){
                        checkvalue(transitions.last().condition.readShdPtrReference(index, reader), "condition");
                    }else if (text == "eventId"){
                        transitions.last().eventId = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "eventId");
                    }else if (text == "toStateId"){
                        transitions.last().toStateId = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "toStateId");
                    }else if (text == "fromNestedStateId"){
                        transitions.last().fromNestedStateId = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "fromNestedStateId");
                    }else if (text == "toNestedStateId"){
                        transitions.last().toNestedStateId = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "toNestedStateId");
                    }else if (text == "priority"){
                        transitions.last().priority = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "priority");
                    }else if (text == "flags"){
                        transitions.last().flags = reader.getElementValueAt(index);
                        list = transitions.last().flags.split('|');
                        for (auto k = 0; k < list.size(); k++){
                            if (!transitionFlags.contains(list.at(k))){
                                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'flags' data field contains an invalid string!\nObject Reference: "+ref);
                            }
                        }
                        break;
                    }
                }
            }
            (numtrans > 0) ? index-- : NULL;
        }
    }
    index--;
    return true;
}

bool hkbStateMachineTransitionInfoArray::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
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
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"transitions", QString::number(transitions.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < transitions.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("triggerInterval", "");
            writer->writeLine(writer->object, true);
            writedatafield("enterEventId", QString::number(transitions.at(i).triggerInterval.enterEventId));
            writedatafield("exitEventId", QString::number(transitions.at(i).triggerInterval.exitEventId));
            writedatafield("enterTime", QString::number(transitions.at(i).triggerInterval.enterTime, char('f'), 6));
            writedatafield("exitTime", QString::number(transitions.at(i).triggerInterval.exitTime, char('f'), 6));
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writedatafield("initiateInterval", "");
            writer->writeLine(writer->object, true);
            writedatafield("enterEventId", QString::number(transitions.at(i).initiateInterval.enterEventId));
            writedatafield("exitEventId", QString::number(transitions.at(i).initiateInterval.exitEventId));
            writedatafield("enterTime", QString::number(transitions.at(i).initiateInterval.enterTime, char('f'), 6));
            writedatafield("exitTime", QString::number(transitions.at(i).initiateInterval.exitTime, char('f'), 6));
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writeref(transitions.at(i).transition, "transition");
            writeref(transitions.at(i).condition, "condition");
            writedatafield("eventId", QString::number(transitions.at(i).eventId));
            writedatafield("toStateId", QString::number(transitions.at(i).toStateId));
            writedatafield("fromNestedStateId", QString::number(transitions.at(i).fromNestedStateId));
            writedatafield("toNestedStateId", QString::number(transitions.at(i).toNestedStateId));
            writedatafield("priority", QString::number(transitions.at(i).priority));
            writedatafield("flags", transitions.at(i).flags);
            writer->writeLine(writer->object, false);
        }
        if (transitions.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < transitions.size(); i++){
            writechild(transitions.at(i).transition, "transitions.at("+QString::number(i)+").transition");
            writechild(transitions.at(i).condition, "transitions.at("+QString::number(i)+").condition");
        }
    }
    return true;
}

bool hkbStateMachineTransitionInfoArray::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < transitions.size(); i++){
        if (transitions.at(i).triggerInterval.enterEventId == eventindex ||
                transitions.at(i).triggerInterval.exitEventId == eventindex ||
                transitions.at(i).initiateInterval.enterEventId == eventindex ||
                transitions.at(i).initiateInterval.exitEventId == eventindex ||
                transitions.at(i).eventId == eventindex)
        {
            return true;
        }
    }
    return false;
}

void hkbStateMachineTransitionInfoArray::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateindices = [&](int & index){
        (index > eventindex) ? index-- : index;
    };
    for (auto i = 0; i < transitions.size(); i++){
        updateindices(transitions[i].triggerInterval.enterEventId);
        updateindices(transitions[i].triggerInterval.exitEventId);
        updateindices(transitions[i].initiateInterval.enterEventId);
        updateindices(transitions[i].initiateInterval.exitEventId);
        updateindices(transitions[i].eventId);
    }
}

void hkbStateMachineTransitionInfoArray::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateindices = [&](int & index){
        (index == oldindex) ? index = newindex : index;
    };
    for (auto i = 0; i < transitions.size(); i++){
        updateindices(transitions[i].triggerInterval.enterEventId);
        updateindices(transitions[i].triggerInterval.exitEventId);
        updateindices(transitions[i].initiateInterval.enterEventId);
        updateindices(transitions[i].initiateInterval.exitEventId);
        updateindices(transitions[i].eventId);
    }
}

void hkbStateMachineTransitionInfoArray::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString thiseventname;
    int eventindex;
    auto fixnmerge = [&](int & id){
        thiseventname = recdata->getEventNameAt(id);
        eventindex = domdata->getIndexOfEvent(thiseventname);
        if (eventindex == -1 && thiseventname != ""){
            domdata->addEvent(thiseventname);
            eventindex = domdata->getNumberOfEvents() - 1;
        }
        id = eventindex;
    };
    if (!getIsMerged() && dominantfile){
        //TO DO: Support character properties...
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(dominantfile->getBehaviorGraphData());
        if (recdata && domdata){
            for (auto i = 0; i < transitions.size(); i++){
                fixnmerge(transitions[i].triggerInterval.enterEventId);
                fixnmerge(transitions[i].triggerInterval.exitEventId);
                fixnmerge(transitions[i].initiateInterval.enterEventId);
                fixnmerge(transitions[i].initiateInterval.exitEventId);
                fixnmerge(transitions[i].eventId);
            }
            setIsMerged(true);
        }
    }
}

bool hkbStateMachineTransitionInfoArray::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *statemachine;
    hkbStateMachineStateInfo *state;
    hkbStateMachineTransitionInfoArray *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY){
        obj = static_cast<hkbStateMachineTransitionInfoArray *>(recessiveObject);
        obj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
        if (parent){
            if (parent->getSignature() == HKB_STATE_MACHINE){
                statemachine = static_cast<hkbStateMachine *>(parent);
                for (auto i = 0; i < obj->transitions.size(); i++){
                    if (!transitions.contains(obj->transitions.at(i)) && statemachine->getStateName(obj->transitions.at(i).toStateId) != "" &&
                          (!obj->transitions.at(i).flags.contains(transitionFlags.at(14)) || statemachine->getNestedStateName(obj->transitions.at(i).toStateId, obj->transitions.at(i).toNestedStateId) != ""))
                    {
                        transitions.append(obj->transitions.at(i));
                    }
                }
            }else if (parent->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
                state = static_cast<hkbStateMachineStateInfo *>(parent);
                for (auto i = 0; i < obj->transitions.size(); i++){
                    if (!transitions.contains(obj->transitions.at(i)) && state->getStateName(obj->transitions.at(i).toStateId) != "" &&
                          (!obj->transitions.at(i).flags.contains(transitionFlags.at(14)) || state->getNestedStateName(obj->transitions.at(i).toStateId, obj->transitions.at(i).toNestedStateId) != ""))
                    {
                        transitions.append(obj->transitions.at(i));
                    }
                }
            }
        }
        setIsMerged(true);
        return true;
    }
    return false;
}

void hkbStateMachineTransitionInfoArray::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    auto update = [&](const HkxSharedPtr & shdptr){
        shdptr.data() ? shdptr.data()->setReference(++ref), shdptr.data()->updateReferences(++ref) : NULL;
    };
    setReference(ref);
    for (auto i = 0; i < transitions.size(); i++){
        update(transitions.at(i).transition);
        update(transitions.at(i).condition);
    }
}

QVector<HkxObject *> hkbStateMachineTransitionInfoArray::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto append = [&](const HkxSharedPtr & shdptr){
        shdptr.data() ? list.append(shdptr.data()) : NULL;
    };
    for (auto i = 0; i < transitions.size(); i++){
        append(transitions.at(i).transition);
        append(transitions.at(i).condition);
    }
    return list;
}

bool hkbStateMachineTransitionInfoArray::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    for (auto i = 0; i < transitions.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(transitions.at(i).transition.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_BLENDING_TRANSITION_EFFECT && (*ptr)->getSignature() != HKB_GENERATOR_TRANSITION_EFFECT){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'transition' data field is linked to invalid child!");
            }
            transitions[i].transition = *ptr;
        }
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(transitions.at(i).condition.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_EXPRESSION_CONDITION/* && (*ptr)->getSignature() != HKB_STRING_CONDITION*/){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'condition' data field is linked to invalid child!");
            }
            transitions[i].condition = *ptr;
        }
    }
    return true;
}

QString hkbStateMachineTransitionInfoArray::evaluateDataValidity(){ //Do not call this outside it's parent state machine or state in a multithreaded context or there will be a potential race condition...
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (!parent){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Null parent!");
    }else if (parent->getSignature() != HKB_STATE_MACHINE && parent->getSignature() != HKB_STATE_MACHINE_STATE_INFO){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid parent type! Signature: "+QString::number(parent->getSignature(), 16)+"\n");
    }else{
        if (transitions.isEmpty()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": transitions is empty!");
        }else{
            for (auto i = transitions.size() - 1; i >= 0; i--){
                if (transitions.at(i).condition.data()){
                    if (transitions.at(i).condition->getSignature() != HKB_EXPRESSION_CONDITION){
                        if (transitions.at(i).condition->getSignature() == HKB_STRING_CONDITION){
                            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid condition type! Fixing: "+QString::number(transitions.at(i).condition->getSignature(), 16)+"\n");
                            transitions[i].condition = HkxSharedPtr();
                        }else{
                            isvalid = false;
                            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid condition type! Signature: "+QString::number(transitions.at(i).condition->getSignature(), 16)+"\n");
                        }
                    }else if (transitions.at(i).condition->isDataValid() && transitions.at(i).condition->evaluateDataValidity() != ""){
                        isvalid = false;
                        //errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid condition data!");
                    }
                }
                if (transitions.at(i).transition.data()){
                    if (transitions.at(i).transition->getSignature() != HKB_BLENDING_TRANSITION_EFFECT/* || transitions.at(i).condition->getSignature() != HKB_GENERATOR_TRANSITION_EFFECT*/){
                        isvalid = false;
                        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid transition type! Signature: "+QString::number(transitions.at(i).transition->getSignature(), 16)+"\n");
                    }else if (transitions.at(i).transition->isDataValid() && transitions.at(i).transition->evaluateDataValidity() != ""){
                        isvalid = false;
                        //errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid transition data!");
                    }
                }
                if (transitions.at(i).eventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                    isvalid = false;
                    errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": eventId in transitions at "+QString::number(i)+" out of range! Removing transition!");
                    transitions.removeAt(i);
                    break;
                }
                if (parent->getSignature() == HKB_STATE_MACHINE){
                    if (static_cast<hkbStateMachine *>(parent)->getStateNameNoLock(transitions.at(i).toStateId) == ""){
                        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": toStateId in transitions at "+QString::number(i)+" does not exist! Removing transition!");
                        transitions.removeAt(i);
                        break;
                    }
                    if (transitions.at(i).flags.contains("FLAG_TO_NESTED_STATE_ID_IS_VALID") && static_cast<hkbStateMachine *>(parent)->getNestedStateNameNoLock(transitions.at(i).toStateId, transitions.at(i).toNestedStateId) == ""){
                        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": toNestedStateId in transitions at "+QString::number(i)+" does not exist!");
                    }
                }else if (parent->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
                    if (static_cast<hkbStateMachineStateInfo *>(parent)->getStateNameNoLock(transitions.at(i).toStateId) == ""){
                        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": toStateId in transitions at "+QString::number(i)+" does not exist! Removing transition!");
                        transitions.removeAt(i);
                        break;
                    }
                    if (transitions.at(i).flags.contains("FLAG_TO_NESTED_STATE_ID_IS_VALID") && static_cast<hkbStateMachineStateInfo *>(parent)->getNestedStateNameNoLock(transitions.at(i).toStateId, transitions.at(i).toNestedStateId) == ""){
                        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": toNestedStateId in transitions at "+QString::number(i)+" does not exist!");
                    }
                }
            }
        }
        if (transitions.isEmpty()){
            if (parent->getSignature() == HKB_STATE_MACHINE){
                static_cast<hkbStateMachine *>(parent)->removeWildcardTransitionsNoLock();
            }else if (parent->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
                static_cast<hkbStateMachineStateInfo *>(parent)->removeTransitionsNoLock();
            }
        }
        setDataValidity(isvalid);
    }
    return errors;
}

hkbStateMachineTransitionInfoArray::~hkbStateMachineTransitionInfoArray(){
    refCount--;
}

hkbStateMachineTransitionInfoArray::HkTransition *hkbStateMachineTransitionInfoArray::getTransitionAt(int index){
    HkTransition *ptr = nullptr;
    (index > -1 && index < transitions.size()) ? ptr = &transitions[index] : ptr;
    return ptr;
}
