#include "bspassbytargettriggermodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint BSPassByTargetTriggerModifier::refCount = 0;

const QString BSPassByTargetTriggerModifier::classname = "BSPassByTargetTriggerModifier";

BSPassByTargetTriggerModifier::BSPassByTargetTriggerModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      radius(0)
{
    setType(BS_PASS_BY_TARGET_TRIGGER_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "PassByTargetTriggerModifier_"+QString::number(refCount);
}

const QString BSPassByTargetTriggerModifier::getClassname(){
    return classname;
}

QString BSPassByTargetTriggerModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSPassByTargetTriggerModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "targetPosition"){
            targetPosition = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "targetPosition");
        }else if (text == "radius"){
            radius = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "radius");
        }else if (text == "movementDirection"){
            movementDirection = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "movementDirection");
        }else if (text == "id"){
            triggerEvent.id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "triggerEvent.id");
        }else if (text == "payload"){
            checkvalue(triggerEvent.payload.readShdPtrReference(index, reader), "payload");
        }
    }
    index--;
    return true;
}

bool BSPassByTargetTriggerModifier::write(HkxXMLWriter *writer){
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
        writedatafield("targetPosition", targetPosition.getValueAsString());
        writedatafield("radius", QString::number(radius, char('f'), 6));
        writedatafield("movementDirection", movementDirection.getValueAsString());
        writedatafield("triggerEvent", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(triggerEvent.id));
        writeref(triggerEvent.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(triggerEvent.payload, "triggerEvent.payload");
    }
    return true;
}

bool BSPassByTargetTriggerModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggerEvent.id == eventindex){
        return true;
    }
    return false;
}

void BSPassByTargetTriggerModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (triggerEvent.id > eventindex) ? triggerEvent.id--: NULL;
}

void BSPassByTargetTriggerModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (triggerEvent.id == oldindex) ? triggerEvent.id = newindex: NULL;
}

void BSPassByTargetTriggerModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(triggerEvent.id);
            setIsMerged(true);
        }
    }
}

void BSPassByTargetTriggerModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (triggerEvent.payload.data()) ? triggerEvent.payload->updateReferences(++ref): NULL;
}

QVector<HkxObject *> BSPassByTargetTriggerModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (triggerEvent.payload.data()) ? list.append(triggerEvent.payload.data()): NULL;
    return list;
}

bool BSPassByTargetTriggerModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSPassByTargetTriggerModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_PASS_BY_TARGET_TRIGGER_MODIFIER){
        recobj = static_cast<BSPassByTargetTriggerModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!triggerEvent.payload.data() && recobj->triggerEvent.payload.data()){
            getParentFile()->addObjectToFile(recobj->triggerEvent.payload.data(), -1);
        }
        return true;
    }
    return false;
}

void BSPassByTargetTriggerModifier::setTriggerEventID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != triggerEvent.id && triggerEvent.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? triggerEvent.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'triggerEvent.id' was not set!");
}

void BSPassByTargetTriggerModifier::setTriggerEventPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(triggerEvent.payload.data())) ? triggerEvent.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'triggerEvent.payload' was not set!");
}

int BSPassByTargetTriggerModifier::getTriggerEventID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return triggerEvent.id;
}

hkbStringEventPayload *BSPassByTargetTriggerModifier::getTriggerEventPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(triggerEvent.payload.data());
}

hkQuadVariable BSPassByTargetTriggerModifier::getMovementDirection() const{
    std::lock_guard <std::mutex> guard(mutex);
    return movementDirection;
}

void BSPassByTargetTriggerModifier::setMovementDirection(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != movementDirection) ? movementDirection = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'movementDirection' was not set!");
}

qreal BSPassByTargetTriggerModifier::getRadius() const{
    std::lock_guard <std::mutex> guard(mutex);
    return radius;
}

void BSPassByTargetTriggerModifier::setRadius(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != radius) ? radius = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'radius' was not set!");
}

hkQuadVariable BSPassByTargetTriggerModifier::getTargetPosition() const{
    std::lock_guard <std::mutex> guard(mutex);
    return targetPosition;
}

void BSPassByTargetTriggerModifier::setTargetPosition(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != targetPosition) ? targetPosition = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'targetPosition' was not set!");
}

bool BSPassByTargetTriggerModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSPassByTargetTriggerModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSPassByTargetTriggerModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSPassByTargetTriggerModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(triggerEvent.payload.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'payload' is not a HKB_STRING_EVENT_PAYLOAD!");
            setDataValidity(false);
        }
        triggerEvent.payload = *ptr;
    }
    return true;
}

void BSPassByTargetTriggerModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    triggerEvent.payload = HkxSharedPtr();
}

QString BSPassByTargetTriggerModifier::evaluateDataValidity(){
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
    if (triggerEvent.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": triggerEvent event id out of range! Setting to max index in range!");
        triggerEvent.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (triggerEvent.payload.data() && triggerEvent.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid triggerEvent.payload type! Signature: "+QString::number(triggerEvent.payload->getSignature(), 16)+" Setting null value!");
        triggerEvent.payload = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSPassByTargetTriggerModifier::~BSPassByTargetTriggerModifier(){
    refCount--;
}
