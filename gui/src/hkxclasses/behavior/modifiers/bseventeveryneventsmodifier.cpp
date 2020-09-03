#include "bseventeveryneventsmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint BSEventEveryNEventsModifier::refCount = 0;

const QString BSEventEveryNEventsModifier::classname = "BSEventEveryNEventsModifier";

BSEventEveryNEventsModifier::BSEventEveryNEventsModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      numberOfEventsBeforeSend(0),
      minimumNumberOfEventsBeforeSend(0),
      randomizeNumberOfEvents(false)
{
    setType(BS_EVENT_EVERY_N_EVENTS_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "EventEveryNEventsModifier_"+QString::number(refCount);
}

const QString BSEventEveryNEventsModifier::getClassname(){
    return classname;
}

QString BSEventEveryNEventsModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSEventEveryNEventsModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "enable"){
            enable = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "enable");
        }else if (text == "eventToCheckFor"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToCheckFor.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToCheckFor.id");
                }else if (text == "payload"){
                    checkvalue(eventToCheckFor.payload.readShdPtrReference(index, reader), "eventToCheckFor.payload");
                    break;
                }
            }
        }else if (text == "eventToSend"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToSend.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToSend.id");
                }else if (text == "payload"){
                    checkvalue(eventToSend.payload.readShdPtrReference(index, reader), "eventToSend.payload");
                    break;
                }
            }
        }else if (text == "numberOfEventsBeforeSend"){
            numberOfEventsBeforeSend = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "numberOfEventsBeforeSend");
        }else if (text == "minimumNumberOfEventsBeforeSend"){
            minimumNumberOfEventsBeforeSend = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "minimumNumberOfEventsBeforeSend");
        }else if (text == "randomizeNumberOfEvents"){
            randomizeNumberOfEvents = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "randomizeNumberOfEvents");
        }
    }
    index--;
    return true;
}

bool BSEventEveryNEventsModifier::write(HkxXMLWriter *writer){
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
        writedatafield("eventToCheckFor", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToCheckFor.id), false);
        writeref(eventToCheckFor.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("eventToSend", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToSend.id), false);
        writeref(eventToSend.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("numberOfEventsBeforeSend", QString::number(numberOfEventsBeforeSend), false);
        writedatafield("minimumNumberOfEventsBeforeSend", QString::number(minimumNumberOfEventsBeforeSend), false);
        writedatafield("randomizeNumberOfEvents", getBoolAsString(randomizeNumberOfEvents), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(eventToCheckFor.payload, "eventToCheckFor.payload");
        writechild(eventToSend.payload, "eventToSend.payload");
    }
    return true;
}

bool BSEventEveryNEventsModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventToCheckFor.id == eventindex || eventToSend.id == eventindex){
        return true;
    }
    return false;
}

void BSEventEveryNEventsModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateind = [&](int & index){
        (index > eventindex) ? index-- : NULL;
    };
    updateind(eventToCheckFor.id);
    updateind(eventToSend.id);
}

void BSEventEveryNEventsModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto mergeind = [&](int & index){
        (index == oldindex) ? index = newindex : NULL;
    };
    mergeind(eventToCheckFor.id);
    mergeind(eventToSend.id);
}

void BSEventEveryNEventsModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(eventToCheckFor.id);
            fixIndex(eventToSend.id);
            setIsMerged(true);
        }
    }
}

void BSEventEveryNEventsModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateref = [&](HkxSharedPtr & shdptr){
        (shdptr.data()) ? shdptr->updateReferences(++ref) : NULL;
    };
    setReference(ref);
    setBindingReference(++ref);
    updateref(eventToCheckFor.payload);
    updateref(eventToSend.payload);
}

QVector<HkxObject *> BSEventEveryNEventsModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto getchildren = [&](const HkxSharedPtr & shdptr){
        (shdptr.data()) ? list.append(shdptr.data()) : NULL;
    };
    getchildren(eventToCheckFor.payload);
    getchildren(eventToSend.payload);
    return list;
}

bool BSEventEveryNEventsModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSEventEveryNEventsModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_EVENT_EVERY_N_EVENTS_MODIFIER){
        recobj = static_cast<BSEventEveryNEventsModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!eventToCheckFor.payload.data() && recobj->eventToCheckFor.payload.data()){
            getParentFile()->addObjectToFile(recobj->eventToCheckFor.payload.data(), -1);
        }
        if (!eventToSend.payload.data() && recobj->eventToSend.payload.data()){
            getParentFile()->addObjectToFile(recobj->eventToSend.payload.data(), -1);
        }
        return true;
    }
    return false;
}

bool BSEventEveryNEventsModifier::getRandomizeNumberOfEvents() const{
    std::lock_guard <std::mutex> guard(mutex);
    return randomizeNumberOfEvents;
}

void BSEventEveryNEventsModifier::setRandomizeNumberOfEvents(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != randomizeNumberOfEvents) ? randomizeNumberOfEvents = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'randomizeNumberOfEvents' was not set!");
}

int BSEventEveryNEventsModifier::getMinimumNumberOfEventsBeforeSend() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minimumNumberOfEventsBeforeSend;
}

void BSEventEveryNEventsModifier::setMinimumNumberOfEventsBeforeSend(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minimumNumberOfEventsBeforeSend) ? minimumNumberOfEventsBeforeSend = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minimumNumberOfEventsBeforeSend' was not set!");
}

int BSEventEveryNEventsModifier::getNumberOfEventsBeforeSend() const{
    std::lock_guard <std::mutex> guard(mutex);
    return numberOfEventsBeforeSend;
}

void BSEventEveryNEventsModifier::setNumberOfEventsBeforeSend(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != numberOfEventsBeforeSend) ? numberOfEventsBeforeSend = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'numberOfEventsBeforeSend' was not set!");
}

int BSEventEveryNEventsModifier::getEventToSendID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToSend.id;
}

void BSEventEveryNEventsModifier::setEventToSendID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != eventToSend.id && eventToSend.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? eventToSend.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend.id' was not set!");
}

void BSEventEveryNEventsModifier::setEventToSendPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(eventToSend.payload.data())) ? eventToSend.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToSend.payload' was not set!");
}

hkbStringEventPayload *BSEventEveryNEventsModifier::getEventToSendPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(eventToSend.payload.data());
}

void BSEventEveryNEventsModifier::setEventToCheckForPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(eventToCheckFor.payload.data())) ? eventToCheckFor.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToCheckFor.payload' was not set!");
}

int BSEventEveryNEventsModifier::getEventToCheckForID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToCheckFor.id;
}

void BSEventEveryNEventsModifier::setEventToCheckForID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != eventToCheckFor.id && eventToCheckFor.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? eventToCheckFor.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventToCheckFor.id' was not set!");
}

hkbStringEventPayload *BSEventEveryNEventsModifier::getEventToCheckForPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(eventToCheckFor.payload.data());
}

bool BSEventEveryNEventsModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSEventEveryNEventsModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSEventEveryNEventsModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}


bool BSEventEveryNEventsModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkpayloads = [&](HkxSharedPtr & data, const QString & fieldname){
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object '"+fieldname+"' is not a HKB_STRING_EVENT_PAYLOAD!");
                setDataValidity(false);
            }
            data = *ptr;
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToCheckFor.payload.getShdPtrReference());
    linkpayloads(eventToCheckFor.payload, "eventToCheckFor.payload");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToSend.payload.getShdPtrReference());
    linkpayloads(eventToSend.payload, "eventToSend.payload");
    return true;
}

void BSEventEveryNEventsModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    eventToCheckFor.payload = HkxSharedPtr();
    eventToSend.payload = HkxSharedPtr();
}

QString BSEventEveryNEventsModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkevents = [&](int & id, HkxSharedPtr & payload, const QString & fieldname){
        if (id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+fieldname+" event id out of range! Setting to max index in range!");
            id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
        }
        if (payload.data() && payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid "+fieldname+" type! Signature: "+QString::number(payload->getSignature(), 16)+" Setting null value!");
            payload = HkxSharedPtr();
        }
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    (temp != "") ? errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!\n"): NULL;
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    checkevents(eventToCheckFor.id, eventToCheckFor.payload, "eventToCheckFor");
    checkevents(eventToSend.id, eventToSend.payload, "eventToSend");
    setDataValidity(isvalid);
    return errors;
}

BSEventEveryNEventsModifier::~BSEventEveryNEventsModifier(){
    refCount--;
}
