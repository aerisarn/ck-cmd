#include "hkbstatemachinestateinfo.h"
#include "src/hkxclasses/behavior/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/behavior/hkbstatemachineeventpropertyarray.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/generators/hkbmodifiergenerator.h"
#include "src/hkxclasses/behavior/generators/bsistatetagginggenerator.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h"

using namespace UI;

uint hkbStateMachineStateInfo::refCount = 0;

const QString hkbStateMachineStateInfo::classname = "hkbStateMachineStateInfo";

hkbStateMachineStateInfo::hkbStateMachineStateInfo(HkxFile *parent, hkbStateMachine *parentsm, long ref)
    : hkbGenerator(parent, ref),
      probability(1),
      enable(true),
      parentSM(parentsm)
{
    setType(HKB_STATE_MACHINE_STATE_INFO, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    parentSM ? stateId = parentsm->generateValidStateId() : stateId = refCount;
    name = "State"+QString::number(stateId);
    refCount++;
}

const QString hkbStateMachineStateInfo::getClassname(){
    return classname;
}

QString hkbStateMachineStateInfo::getStateName(int stateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    QString sname;
    parentSM ? sname = parentSM->getStateName(stateId) : NULL;
    return sname;
}

QString hkbStateMachineStateInfo::getNestedStateName(int stateId, ulong nestedStateId) const{
    std::lock_guard <std::mutex> guard(mutex);
    QString sname;
    parentSM ? sname = parentSM->getNestedStateName(stateId, nestedStateId) : NULL;
    return sname;
}

QString hkbStateMachineStateInfo::getStateNameNoLock(int stateId) const{
    QString sname;
    parentSM ? sname = parentSM->getStateName(stateId) : NULL;
    return sname;
}

QString hkbStateMachineStateInfo::getNestedStateNameNoLock(int stateId, ulong nestedStateId) const{
    QString sname;
    parentSM ? sname = parentSM->getNestedStateName(stateId, nestedStateId) : NULL;
    return sname;
}

QString hkbStateMachineStateInfo::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

hkbStateMachine * hkbStateMachineStateInfo::getParentStateMachine() const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachine *ptr = nullptr;
    parentSM ? ptr = parentSM : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": "+name+"' has no parent state machine!");
    return ptr;
}

void hkbStateMachineStateInfo::setParentSM(hkbStateMachine *value){
    std::lock_guard <std::mutex> guard(mutex);
    value ? parentSM = value: value;
}

int hkbStateMachineStateInfo::getStateId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return stateId;
}

QString hkbStateMachineStateInfo::getNameNoLock() const{
    return name;
}

int hkbStateMachineStateInfo::getStateIdNoLock() const{
    return stateId;
}

void hkbStateMachineStateInfo::updateTransitionStateId(bool lock, int newid){
    hkbStateMachineTransitionInfoArray *trans;
    auto update = [&](){
        trans = static_cast<hkbStateMachineTransitionInfoArray *>(transitions.data());
        if (trans){
            trans->updateTransitionStateId(stateId, newid);
        }
    };
    if (lock){
        std::lock_guard <std::mutex> guard(mutex);
        update();
    }else{
        update();
    }
}

void hkbStateMachineStateInfo::removeTransitions(){
    std::lock_guard <std::mutex> guard(mutex);
    transitions = HkxSharedPtr();
}

void hkbStateMachineStateInfo::removeTransitionsNoLock(){
    transitions = HkxSharedPtr();
}

void hkbStateMachineStateInfo::removeTransitionToState(int id){
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineTransitionInfoArray *trans = static_cast<hkbStateMachineTransitionInfoArray *>(transitions.data());
    trans ? trans->removeTransitionToState(id) : trans;
}

bool hkbStateMachineStateInfo::setStateId(int id){
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineStateInfo *state = nullptr;
    if (parentSM){
        auto states = parentSM->getChildren();
        for (auto i = 0; i < states.size(); i++){
            state = static_cast<hkbStateMachineStateInfo *>(states.at(i));
            if (state && state->getStateId() == id){
                return false;
            }
        }
        for (auto i = 0; i < states.size(); i++){
            state = static_cast<hkbStateMachineStateInfo *>(states.at(i));
            if (state != this){
                state->updateTransitionStateId(true, id);
            }else{
                state->updateTransitionStateId(false, id);
            }
        }
        parentSM->updateTransitionStateId(stateId, id);
    }
    stateId = id;
    return true;
}

bool hkbStateMachineStateInfo::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (generator.data()){
        return true;
    }
    return false;
}

bool hkbStateMachineStateInfo::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    bool refed = false;
    auto isrefed = [&](const HkxSharedPtr & shdptr){
        if (shdptr.constData() && shdptr.constData()->isEventReferenced(eventindex))
            refed = true;
    };
    isrefed(enterNotifyEvents);
    isrefed(exitNotifyEvents);
    isrefed(transitions);
    return refed;
}

void hkbStateMachineStateInfo::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updatref = [&](const HkxSharedPtr & shdptr){
        if (shdptr.data())
            shdptr->updateEventIndices(eventindex);
    };
    updatref(enterNotifyEvents);
    updatref(exitNotifyEvents);
    updatref(transitions);
}

void hkbStateMachineStateInfo::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto mergeindex = [&](const HkxSharedPtr & shdptr){
        if (shdptr.data())
            shdptr->mergeEventIndex(oldindex, newindex);
    };
    mergeindex(enterNotifyEvents);
    mergeindex(exitNotifyEvents);
    mergeindex(transitions);
}

void hkbStateMachineStateInfo::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    auto fixmergedind = [&](const HkxSharedPtr & shdptr){
        if (shdptr.data())
            shdptr->fixMergedEventIndices(dominantfile);
    };
    fixmergedind(enterNotifyEvents);
    fixmergedind(exitNotifyEvents);
    fixmergedind(transitions);
}

bool hkbStateMachineStateInfo::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineStateInfo *obj = nullptr;
    auto merge = [&](HkxObject *ptr, HkxSharedPtr & shdptr, HkxObject *recptr, const HkxSharedPtr & recshdptr){
        if (ptr){
            if (recptr){
                ptr->merge(recptr);
            }
        }else if (recptr){
            shdptr = recshdptr;
            getParentFile()->addObjectToFile(recptr, 0);
            recptr->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
        }
    };
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_STATE_MACHINE_STATE_INFO){
        obj = static_cast<hkbStateMachineStateInfo *>(recessiveObject);
        injectWhileMerging((obj));
        merge(enterNotifyEvents.data(), enterNotifyEvents, obj->getEnterNotifyEventsData(), obj->getEnterNotifyEvents());
        merge(exitNotifyEvents.data(), exitNotifyEvents, obj->getExitNotifyEventsData(), obj->getExitNotifyEvents());
        merge(transitions.data(), transitions, obj->getTransitionsData(), obj->getTransitions());
        return true;
    }
    return false;
}

void hkbStateMachineStateInfo::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateref = [&](const HkxSharedPtr & shdptr){
        if (shdptr.data())
            shdptr->updateReferences(++ref);
    };
    setReference(ref);
    setBindingReference(++ref);
    updateref(enterNotifyEvents);
    updateref(exitNotifyEvents);
    updateref(transitions);
}

QVector<HkxObject *> hkbStateMachineStateInfo::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto getchildren = [&](const HkxSharedPtr & shdptr){
        if (shdptr.data()){
            list.append(shdptr.data());
        }
    };
    getchildren(enterNotifyEvents);
    getchildren(exitNotifyEvents);
    getchildren(transitions);
    return list;
}

QVector<DataIconManager *> hkbStateMachineStateInfo::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (generator.data()){
        list.append(static_cast<DataIconManager*>(generator.data()));
    }
    return list;
}

hkbStateMachine *hkbStateMachineStateInfo::getNestedStateMachine() const{
    std::lock_guard <std::mutex> guard(mutex);
    HkxSignature sig;
    QString behaviorname;
    auto gen = static_cast<hkbGenerator *>(generator.data());
    while (gen){
        sig = gen->getSignature();
        switch (sig){
        case HKB_STATE_MACHINE:
            return static_cast<hkbStateMachine *>(gen);
        case HKB_MODIFIER_GENERATOR:
            gen = static_cast<hkbGenerator *>(static_cast<hkbModifierGenerator *>(gen)->getChildren().last());
            break;
        case BS_I_STATE_TAGGING_GENERATOR:
            gen = static_cast<hkbGenerator *>(static_cast<BSiStateTaggingGenerator *>(gen)->getChildren().first());
            break;
        case HKB_BEHAVIOR_REFERENCE_GENERATOR:
            behaviorname = reinterpret_cast<hkbBehaviorReferenceGenerator *>(gen)->getBehaviorName();
            return static_cast<BehaviorFile *>(getParentFile())->findRootStateMachineFromBehavior(behaviorname);
        default:
            return nullptr;
        }
    }
    return nullptr;
}

void hkbStateMachineStateInfo::setTransitionsParentSM(hkbStateMachine *parSM){
    std::lock_guard <std::mutex> guard(mutex);
    hkbStateMachineTransitionInfoArray *trans = static_cast<hkbStateMachineTransitionInfoArray *>(transitions.data());
    trans ? trans->setParentSM(parSM) : NULL;
}

int hkbStateMachineStateInfo::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (generator.data() == obj){
        return 0;
    }
    return -1;
}

bool hkbStateMachineStateInfo::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj && obj->getType() == TYPE_GENERATOR){
        generator = HkxSharedPtr(obj);
        return true;
    }
    return false;
}

bool hkbStateMachineStateInfo::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        generator = HkxSharedPtr();
        return true;
    }
    return false;
}

QString hkbStateMachineStateInfo::getGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(generator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

bool hkbStateMachineStateInfo::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbStateMachineStateInfo::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

qreal hkbStateMachineStateInfo::getProbability() const{
    std::lock_guard <std::mutex> guard(mutex);
    return probability;
}

void hkbStateMachineStateInfo::setProbability(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != probability && value <= 1 && value >= 0) ? probability = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'probability' was not set!");
}

void hkbStateMachineStateInfo::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

HkxObject *hkbStateMachineStateInfo::getEnterNotifyEventsData() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enterNotifyEvents.data();
}

HkxObject *hkbStateMachineStateInfo::getExitNotifyEventsData() const{
    std::lock_guard <std::mutex> guard(mutex);
    return exitNotifyEvents.data();
}

HkxObject *hkbStateMachineStateInfo::getTransitionsData() const{
    std::lock_guard <std::mutex> guard(mutex);
    return transitions.data();
}

HkxSharedPtr hkbStateMachineStateInfo::getTransitions() const{
    std::lock_guard <std::mutex> guard(mutex);
    return transitions;
}

HkxSharedPtr hkbStateMachineStateInfo::getExitNotifyEvents() const{
    std::lock_guard <std::mutex> guard(mutex);
    return exitNotifyEvents;
}

HkxSharedPtr hkbStateMachineStateInfo::getEnterNotifyEvents() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enterNotifyEvents;
}

bool hkbStateMachineStateInfo::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "enterNotifyEvents"){
            checkvalue(enterNotifyEvents.readShdPtrReference(index, reader), "enterNotifyEvents");
        }else if (text == "exitNotifyEvents"){
            checkvalue(exitNotifyEvents.readShdPtrReference(index, reader), "exitNotifyEvents");
        }else if (text == "transitions"){
            checkvalue(transitions.readShdPtrReference(index, reader), "transitions");
        }else if (text == "generator"){
            checkvalue(generator.readShdPtrReference(index, reader), "generator");
        }else if (text == "name"){
            name = reader.getElementValueAt(index);
            checkvalue((name != ""), "name");
        }else if (text == "stateId"){
            stateId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "stateId");
        }else if (text == "probability"){
            probability = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "probability");
        }else if (text == "enable"){
            enable = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "enable");
        }
    }
    index--;
    return true;
}

bool hkbStateMachineStateInfo::write(HkxXMLWriter *writer){
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
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writeref(getVariableBindingSet(), "variableBindingSet");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"listeners", "0"});
        writer->writeLine(writer->parameter, list1, list2, "");
        writeref(enterNotifyEvents, "enterNotifyEvents");
        writeref(exitNotifyEvents, "exitNotifyEvents");
        writeref(transitions, "transitions");
        writeref(generator, "generator");
        writedatafield("name", name, false);
        writedatafield("stateId", QString::number(stateId), false);
        writedatafield("probability", QString::number(probability, char('f'), 6), false);
        writedatafield("enable", getBoolAsString(enable), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(enterNotifyEvents, "enterNotifyEvents");
        writechild(exitNotifyEvents, "exitNotifyEvents");
        writechild(transitions, "transitions");
        writechild(generator, "generator");
    }
    return true;
}

bool hkbStateMachineStateInfo::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkdata = [&](HkxType type, HkxSignature sig, HkxSharedPtr & shdptr, const QString & fieldname, bool nullallowed){
        if (ptr){
            if (!ptr->data()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link '"+fieldname+"' data field!");
                setDataValidity(false);
            }else if ((*ptr)->getType() != type || (sig != NULL_SIGNATURE && (*ptr)->getSignature() != sig) ||
                      ((*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD))
            {
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' data field is linked to invalid child!");
                setDataValidity(false);
            }
            shdptr = *ptr;
        }else if (!nullallowed){
            setDataValidity(false);
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(enterNotifyEvents.getShdPtrReference());
    linkdata(TYPE_OTHER, HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY, enterNotifyEvents, "enterNotifyEvents", false);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(exitNotifyEvents.getShdPtrReference());
    linkdata(TYPE_OTHER, HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY, exitNotifyEvents, "exitNotifyEvents", false);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(transitions.getShdPtrReference());
    linkdata(TYPE_OTHER, HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY, transitions, "transitions", false);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(generator.getShdPtrReference());
    linkdata(TYPE_GENERATOR, NULL_SIGNATURE, generator, "generator", false);
    return true;
}

void hkbStateMachineStateInfo::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    parentSM = nullptr;
    enterNotifyEvents = HkxSharedPtr();
    exitNotifyEvents = HkxSharedPtr();
    transitions = HkxSharedPtr();
    generator = HkxSharedPtr();
}

QString hkbStateMachineStateInfo::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    QString temp;
    auto checknfix = [&](HkxSharedPtr & shdptr, const QString & datafield, HkxSignature sig){
        if (shdptr.data()){
            if (shdptr->getSignature() != sig){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid "+datafield+" type! Signature: "+QString::number(shdptr->getSignature(), 16)+" Setting null value!");
                shdptr = HkxSharedPtr();
            }else if ((sig == HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY && static_cast<hkbStateMachineEventPropertyArray *>(shdptr.data())->getNumOfEvents() < 1) ||
                      (sig == HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY && static_cast<hkbStateMachineTransitionInfoArray *>(shdptr.data())->getNumTransitions() < 1))
            {
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+datafield+" is empty! Setting null value!");
                shdptr = HkxSharedPtr();
            }/*else if (shdptr->isDataValid() && shdptr->evaluateDataValidity() != ""){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid "+datafield+" data!");
            }*/
        }
    };
    temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    checknfix(enterNotifyEvents, "enterNotifyEvents", HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY);
    checknfix(exitNotifyEvents, "exitNotifyEvents", HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY);
    checknfix(transitions, "transitions", HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY);
    if (!generator.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null generator!");
    }else if (generator->getType() != HkxObject::TYPE_GENERATOR){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid generator type! Signature: "+QString::number(generator->getSignature(), 16)+" Setting null value!");
        generator = HkxSharedPtr();
    }
    if (!parentSM){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null parentSM!");
    }
    setDataValidity(isvalid);
    return errors;
}

hkbStateMachineStateInfo::~hkbStateMachineStateInfo(){
    refCount--;
}
