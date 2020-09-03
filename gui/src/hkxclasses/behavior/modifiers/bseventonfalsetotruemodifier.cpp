#include "bseventonfalsetotruemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint BSEventOnFalseToTrueModifier::refCount = 0;

const QString BSEventOnFalseToTrueModifier::classname = "BSEventOnFalseToTrueModifier";

BSEventOnFalseToTrueModifier::BSEventOnFalseToTrueModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      bEnableEvent1(false),
      bVariableToTest1(false),
      bEnableEvent2(false),
      bVariableToTest2(false),
      bEnableEvent3(false),
      bVariableToTest3(false)
{
    setType(BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "EventOnFalseToTrueModifier_"+QString::number(refCount);
}

const QString BSEventOnFalseToTrueModifier::getClassname(){
    return classname;
}

QString BSEventOnFalseToTrueModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSEventOnFalseToTrueModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "bEnableEvent1"){
            bEnableEvent1 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bEnableEvent1");
        }else if (text == "bVariableToTest1"){
            bVariableToTest1 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bVariableToTest1");
        }else if (text == "eventToSend1"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToSend1.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToSend1.id");
                }else if (text == "payload"){
                    checkvalue(eventToSend1.payload.readShdPtrReference(index, reader), "eventToSend1.payload");
                    break;
                }
            }
        }else if (text == "bEnableEvent2"){
            bEnableEvent2 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bEnableEvent2");
        }else if (text == "bVariableToTest2"){
            bVariableToTest2 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bVariableToTest2");
        }else if (text == "eventToSend2"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToSend2.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToSend2.id");
                }else if (text == "payload"){
                    checkvalue(eventToSend2.payload.readShdPtrReference(index, reader), "eventToSend2.payload");
                    break;
                }
            }
        }else if (text == "bEnableEvent3"){
            bEnableEvent3 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bEnableEvent3");
        }else if (text == "bVariableToTest3"){
            bVariableToTest3 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bVariableToTest3");
        }else if (text == "eventToSend3"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToSend3.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToSend3.id");
                }else if (text == "payload"){
                    checkvalue(eventToSend3.payload.readShdPtrReference(index, reader), "eventToSend3.payload");
                    break;
                }
            }
        }
    }
    index--;
    return true;
}

bool BSEventOnFalseToTrueModifier::write(HkxXMLWriter *writer){
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
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writedatafield("enable", getBoolAsString(enable), false);
        writedatafield("bEnableEvent1", getBoolAsString(bEnableEvent1), false);
        writedatafield("bVariableToTest1", getBoolAsString(bVariableToTest1), false);
        writedatafield("EventToSend1", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToSend1.id), false);
        writeref(eventToSend1.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("bEnableEvent2", getBoolAsString(bEnableEvent2), false);
        writedatafield("bVariableToTest2", getBoolAsString(bVariableToTest2), false);
        writedatafield("EventToSend2", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToSend2.id), false);
        writeref(eventToSend2.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("bEnableEvent3", getBoolAsString(bEnableEvent3), false);
        writedatafield("bVariableToTest3", getBoolAsString(bVariableToTest3), false);
        writedatafield("EventToSend3", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToSend3.id), false);
        writeref(eventToSend3.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(eventToSend1.payload, "eventToSend1.payload");
        writechild(eventToSend2.payload, "eventToSend2.payload");
        writechild(eventToSend3.payload, "eventToSend3.payload");
    }
    return true;
}

bool BSEventOnFalseToTrueModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventToSend1.id == eventindex || eventToSend2.id == eventindex || eventToSend3.id == eventindex){
        return true;
    }
    return false;
}

void BSEventOnFalseToTrueModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateindices = [&](int & id){
        (id > eventindex) ? id-- : NULL;
    };
    updateindices(eventToSend1.id);
    updateindices(eventToSend2.id);
    updateindices(eventToSend3.id);
}

void BSEventOnFalseToTrueModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto mergeindices = [&](int & id){
        (id == oldindex) ? id = newindex : NULL;
    };
    mergeindices(eventToSend1.id);
    mergeindices(eventToSend2.id);
    mergeindices(eventToSend3.id);
}

void BSEventOnFalseToTrueModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            auto fixIndex = [&](int & id){ if (id < 0){return;}
                thiseventname = recdata->getEventNameAt(id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                id = eventindex;
            };
            fixIndex(eventToSend1.id);
            fixIndex(eventToSend2.id);
            fixIndex(eventToSend3.id);
            setIsMerged(true);
        }
    }
}

void BSEventOnFalseToTrueModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    auto updaterefs = [&](HkxSharedPtr & shdptr){
        (shdptr.data()) ? shdptr.data()->updateReferences(++ref) : NULL;
    };
    setReference(ref);
    setBindingReference(++ref);
    updaterefs(eventToSend1.payload);
    updaterefs(eventToSend2.payload);
    updaterefs(eventToSend3.payload);
}

QVector<HkxObject *> BSEventOnFalseToTrueModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto getchildren = [&](const HkxSharedPtr & shdptr){
        (shdptr.data()) ? list.append(shdptr.data()) : NULL;
    };
    getchildren(eventToSend1.payload);
    getchildren(eventToSend2.payload);
    getchildren(eventToSend3.payload);
    return list;
}

bool BSEventOnFalseToTrueModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    auto merge = [&](HkxSharedPtr & shdptr, HkxSharedPtr & recshdptr){
        (!shdptr.data() && recshdptr.data()) ? getParentFile()->addObjectToFile(recshdptr.data(), -1) : NULL;
    };
    BSEventOnFalseToTrueModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER){
        recobj = static_cast<BSEventOnFalseToTrueModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        merge(eventToSend1.payload, recobj->eventToSend1.payload);  //Not thread safe!
        merge(eventToSend2.payload, recobj->eventToSend2.payload);
        merge(eventToSend3.payload, recobj->eventToSend3.payload);
        return true;
    }
    return false;
}

int BSEventOnFalseToTrueModifier::getEventToSend1ID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToSend1.id;
}

void BSEventOnFalseToTrueModifier::setEventToSend1ID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != eventToSend1.id && eventToSend1.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? eventToSend1.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend1.id' was not set!");
}

hkbStringEventPayload *BSEventOnFalseToTrueModifier::getEventToSend1Payload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(eventToSend1.payload.data());
}

void BSEventOnFalseToTrueModifier::setEventToSend1Payload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(eventToSend1.payload.data())) ? eventToSend1.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend1.payload' was not set!");
}

bool BSEventOnFalseToTrueModifier::getBVariableToTest1() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bVariableToTest1;
}

void BSEventOnFalseToTrueModifier::setBVariableToTest1(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bVariableToTest1) ? bVariableToTest1 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bVariableToTest1' was not set!");
}

bool BSEventOnFalseToTrueModifier::getBEnableEvent1() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bEnableEvent1;
}

void BSEventOnFalseToTrueModifier::setBEnableEvent1(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bEnableEvent1) ? bEnableEvent1 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bEnableEvent1' was not set!");
}

bool BSEventOnFalseToTrueModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSEventOnFalseToTrueModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSEventOnFalseToTrueModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

int BSEventOnFalseToTrueModifier::getEventToSend2ID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToSend2.id;
}

void BSEventOnFalseToTrueModifier::setEventToSend2ID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != eventToSend2.id && eventToSend2.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? eventToSend2.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend2.id' was not set!");
}

hkbStringEventPayload *BSEventOnFalseToTrueModifier::getEventToSend2Payload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(eventToSend2.payload.data());
}

void BSEventOnFalseToTrueModifier::setEventToSend2Payload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(eventToSend2.payload.data())) ? eventToSend2.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend2.payload' was not set!");
}

bool BSEventOnFalseToTrueModifier::getBVariableToTest2() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bVariableToTest2;
}

void BSEventOnFalseToTrueModifier::setBVariableToTest2(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bVariableToTest2) ? bVariableToTest2 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bVariableToTest2' was not set!");
}

bool BSEventOnFalseToTrueModifier::getBEnableEvent2() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bEnableEvent2;
}

void BSEventOnFalseToTrueModifier::setBEnableEvent2(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bEnableEvent2) ? bEnableEvent2 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bEnableEvent2' was not set!");
}

int BSEventOnFalseToTrueModifier::getEventToSend3ID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToSend3.id;
}

void BSEventOnFalseToTrueModifier::setEventToSend3ID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != eventToSend3.id && eventToSend3.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? eventToSend3.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend3.id' was not set!");
}

hkbStringEventPayload *BSEventOnFalseToTrueModifier::getEventToSend3Payload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(eventToSend3.payload.data());
}

void BSEventOnFalseToTrueModifier::setEventToSend3Payload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(eventToSend3.payload.data())) ? eventToSend3.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend3.payload' was not set!");
}

bool BSEventOnFalseToTrueModifier::getBVariableToTest3() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bVariableToTest3;
}

void BSEventOnFalseToTrueModifier::setBVariableToTest3(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bVariableToTest3) ? bVariableToTest3 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bVariableToTest3' was not set!");
}

bool BSEventOnFalseToTrueModifier::getBEnableEvent3() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bEnableEvent3;
}

void BSEventOnFalseToTrueModifier::setBEnableEvent3(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bEnableEvent3) ? bEnableEvent3 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bEnableEvent3' was not set!");
}

bool BSEventOnFalseToTrueModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkpayloads = [&](HkxSharedPtr & shdptr, const QString & fieldname){
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object '"+fieldname+"' is not a HKB_STRING_EVENT_PAYLOAD!");
                setDataValidity(false);
            }
            shdptr = *ptr;
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToSend1.payload.getShdPtrReference());
    linkpayloads(eventToSend1.payload, "eventToSend1.payload");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToSend2.payload.getShdPtrReference());
    linkpayloads(eventToSend2.payload, "eventToSend2.payload");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToSend3.payload.getShdPtrReference());
    linkpayloads(eventToSend3.payload, "eventToSend3.payload");
    return true;
}

void BSEventOnFalseToTrueModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    eventToSend1.payload = HkxSharedPtr();
    eventToSend2.payload = HkxSharedPtr();
    eventToSend3.payload = HkxSharedPtr();
}

QString BSEventOnFalseToTrueModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto setinvalid = [&](const QString & message){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+message+"!");
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    (temp != "") ? errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!\n"): NULL;
    (name == "") ? setinvalid("Invalid name") : NULL;
    if (eventToSend1.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        setinvalid("eventToSend1.id out of range! Setting to max index in range!");
        eventToSend1.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (eventToSend1.payload.data() && eventToSend1.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        setinvalid("Invalid eventToSend1.payload type! Signature: "+QString::number(eventToSend1.payload->getSignature(), 16)+" Setting null value!");
        eventToSend1.payload = HkxSharedPtr();
    }
    if (eventToSend2.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        setinvalid("eventToSend2.id out of range! Setting to max index in range!");
        eventToSend2.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (eventToSend2.payload.data() && eventToSend2.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        setinvalid("Invalid eventToSend2.payload type! Signature: "+QString::number(eventToSend2.payload->getSignature(), 16)+" Setting null value!");
        eventToSend2.payload = HkxSharedPtr();
    }
    if (eventToSend3.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        setinvalid("eventToSend3.id out of range! Setting to max index in range!");
        eventToSend3.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (eventToSend3.payload.data() && eventToSend3.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        setinvalid("Invalid eventToSend3.payload type! Signature: "+QString::number(eventToSend3.payload->getSignature(), 16)+" Setting null value!");
        eventToSend3.payload = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSEventOnFalseToTrueModifier::~BSEventOnFalseToTrueModifier(){
    refCount--;
}
