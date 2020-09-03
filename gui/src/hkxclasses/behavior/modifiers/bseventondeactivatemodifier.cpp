#include "bseventondeactivatemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint BSEventOnDeactivateModifier::refCount = 0;

const QString BSEventOnDeactivateModifier::classname = "BSEventOnDeactivateModifier";

BSEventOnDeactivateModifier::BSEventOnDeactivateModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true)
{
    setType(BS_EVENT_ON_DEACTIVATE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "EventOnDeactivateModifier_"+QString::number(refCount);
}

const QString BSEventOnDeactivateModifier::getClassname(){
    return classname;
}

QString BSEventOnDeactivateModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSEventOnDeactivateModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "id"){
            event.id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "event.id");
        }else if (text == "payload"){
            checkvalue(event.payload.readShdPtrReference(index, reader), "event.payload");
        }
    }
    index--;
    return true;
}

bool BSEventOnDeactivateModifier::write(HkxXMLWriter *writer){
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
        writedatafield("event", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(event.id), false);
        writeref(event.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(event.payload, "event.payload");
    }
    return true;
}

bool BSEventOnDeactivateModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (event.id == eventindex){
        return true;
    }
    return false;
}

void BSEventOnDeactivateModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (event.id > eventindex) ? event.id-- : NULL;
}

void BSEventOnDeactivateModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (event.id == oldindex) ? event.id = newindex : NULL;
}

void BSEventOnDeactivateModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(event.id);
            setIsMerged(true);
        }
    }
}

void BSEventOnDeactivateModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (event.payload.data()) ? event.payload->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> BSEventOnDeactivateModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (event.payload.data()) ? list.append(event.payload.data()): NULL;
    return list;
}

bool BSEventOnDeactivateModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSEventOnDeactivateModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_EVENT_ON_DEACTIVATE_MODIFIER){
        recobj = static_cast<BSEventOnDeactivateModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!event.payload.data() && recobj->event.payload.data()){
            getParentFile()->addObjectToFile(recobj->event.payload.data(), -1);
        }
        return true;
    }else{
        return false;
    }
}

hkbStringEventPayload *BSEventOnDeactivateModifier::getEventPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(event.payload.data());
}

void BSEventOnDeactivateModifier::setEventPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(event.payload.data())) ? event.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'event.payload' was not set!");
}

int BSEventOnDeactivateModifier::getEventID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return event.id;
}

void BSEventOnDeactivateModifier::setEventID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != event.id && event.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? event.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'event.id' was not set!");
}

bool BSEventOnDeactivateModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSEventOnDeactivateModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSEventOnDeactivateModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSEventOnDeactivateModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(event.payload.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'payload' is not a HKB_STRING_EVENT_PAYLOAD!");
            setDataValidity(false);
        }
        event.payload = *ptr;
    }
    return true;
}

void BSEventOnDeactivateModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    event.payload = HkxSharedPtr();
}

QString BSEventOnDeactivateModifier::evaluateDataValidity(){
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
    if (event.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        setinvalid("Event id out of range! Setting to max index in range!");
        event.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (event.payload.data() && event.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        setinvalid("Invalid event.payload type! Signature: "+QString::number(event.payload->getSignature(), 16)+" Setting null value!");
        event.payload = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSEventOnDeactivateModifier::~BSEventOnDeactivateModifier(){
    refCount--;
}
