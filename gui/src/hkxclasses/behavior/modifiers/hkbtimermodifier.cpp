#include "hkbtimermodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint hkbTimerModifier::refCount = 0;

const QString hkbTimerModifier::classname = "hkbTimerModifier";

hkbTimerModifier::hkbTimerModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      alarmTimeSeconds(0)
{
    setType(HKB_TIMER_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "TimerModifier_"+QString::number(refCount);
}

const QString hkbTimerModifier::getClassname(){
    return classname;
}

QString hkbTimerModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

hkbStringEventPayload *hkbTimerModifier::getAlarmEventPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(alarmEvent.payload.data());
}

void hkbTimerModifier::setAlarmEventPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(alarmEvent.payload.data())) ? alarmEvent.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'alarmEvent.payload' was not set!");
}

int hkbTimerModifier::getAlarmEventID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return alarmEvent.id;
}

void hkbTimerModifier::setAlarmEventID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != alarmEvent.id && alarmEvent.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? alarmEvent.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'alarmEvent.id' was not set!");
}

qreal hkbTimerModifier::getAlarmTimeSeconds() const{
    std::lock_guard <std::mutex> guard(mutex);
    return alarmTimeSeconds;
}

void hkbTimerModifier::setAlarmTimeSeconds(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != alarmTimeSeconds) ? alarmTimeSeconds = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'alarmTimeSeconds' was not set!");
}

bool hkbTimerModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbTimerModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbTimerModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbTimerModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "alarmTimeSeconds"){
            alarmTimeSeconds = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "alarmTimeSeconds");
        }else if (text == "id"){
            alarmEvent.id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "id");
        }else if (text == "payload"){
            checkvalue(alarmEvent.payload.readShdPtrReference(index, reader), "payload");
        }
    }
    index--;
    return true;
}

bool hkbTimerModifier::write(HkxXMLWriter *writer){
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
        writeref(getVariableBindingSet(), "variableBindingSet");
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        writedatafield("alarmTimeSeconds", QString::number(alarmTimeSeconds, char('f'), 6));
        writedatafield("alarmEvent", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(alarmEvent.id));
        writeref(alarmEvent.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(alarmEvent.payload, "payload");
    }
    return true;
}

bool hkbTimerModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (alarmEvent.id == eventindex){
        return true;
    }
    return false;
}

void hkbTimerModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (alarmEvent.id > eventindex) ? alarmEvent.id-- : NULL;
}

void hkbTimerModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (alarmEvent.id == oldindex) ? alarmEvent.id = newindex : NULL;
}

void hkbTimerModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(alarmEvent.id);
            setIsMerged(true);
        }
    }
}

void hkbTimerModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (alarmEvent.payload.data()) ? alarmEvent.payload->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> hkbTimerModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (alarmEvent.payload.data()) ? list.append(alarmEvent.payload.data()) : NULL;
    return list;
}

bool hkbTimerModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbTimerModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_TIMER_MODIFIER){
        recobj = static_cast<hkbTimerModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!alarmEvent.payload.data() && recobj->alarmEvent.payload.data()){
            getParentFile()->addObjectToFile(recobj->alarmEvent.payload.data(), -1);
        }
        return true;
    }
    return false;
}

bool hkbTimerModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(alarmEvent.payload.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'payload' is not a HKB_STRING_EVENT_PAYLOAD!");
            setDataValidity(false);
        }
        alarmEvent.payload = *ptr;
    }
    return true;
}

void hkbTimerModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    alarmEvent.payload = HkxSharedPtr();
}

QString hkbTimerModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (alarmEvent.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": alarmEvent event id out of range! Setting to max index in range!");
        alarmEvent.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (alarmEvent.payload.data() && alarmEvent.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid alarmEvent.payload type! Signature: "+QString::number(alarmEvent.payload->getSignature(), 16)+" Setting null value!");
        alarmEvent.payload = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbTimerModifier::~hkbTimerModifier(){
    refCount--;
}

