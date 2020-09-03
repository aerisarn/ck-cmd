#include "bsragdollcontactlistenermodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint BSRagdollContactListenerModifier::refCount = 0;

const QString BSRagdollContactListenerModifier::classname = "BSRagdollContactListenerModifier";

BSRagdollContactListenerModifier::BSRagdollContactListenerModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true)
{
    setType(BS_RAGDOLL_CONTACT_LISTENER_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "RagdollContactListenerModifier_"+QString::number(refCount);
}

const QString BSRagdollContactListenerModifier::getClassname(){
    return classname;
}

QString BSRagdollContactListenerModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSRagdollContactListenerModifier::readData(const HkxXmlReader &reader, long & index){
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
            contactEvent.id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "contactEvent.id");
        }else if (text == "payload"){
            checkvalue(contactEvent.payload.readShdPtrReference(index, reader), "payload");
        }else if (text == "bones"){
            checkvalue(bones.readShdPtrReference(index, reader), "bones");
        }
    }
    index--;
    return true;
}

bool BSRagdollContactListenerModifier::write(HkxXMLWriter *writer){
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
        writedatafield("event", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(contactEvent.id));
        writeref(contactEvent.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writeref(bones, "bones");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(contactEvent.payload, "contactEvent.payload");
        writechild(bones, "bones");
    }
    return true;
}

bool BSRagdollContactListenerModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (contactEvent.id == eventindex){
        return true;
    }
    return false;
}

void BSRagdollContactListenerModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (contactEvent.id > eventindex) ? contactEvent.id--: NULL;
}

void BSRagdollContactListenerModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (contactEvent.id == oldindex) ? contactEvent.id = newindex: NULL;
}

void BSRagdollContactListenerModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(contactEvent.id);
            setIsMerged(true);
        }
    }
}

void BSRagdollContactListenerModifier::updateReferences(long &ref){
    auto updateref = [&](HkxSharedPtr & shdptr){
        (shdptr.data()) ? shdptr->updateReferences(++ref) : NULL;
    };
    setReference(ref);
    setBindingReference(++ref);
    updateref(contactEvent.payload);
    updateref(bones);
}

QVector<HkxObject *> BSRagdollContactListenerModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto getchildren = [&](const HkxSharedPtr & shdptr){
        (shdptr.data()) ? list.append(shdptr.data()) : NULL;
    };
    getchildren(contactEvent.payload);
    getchildren(bones);
    return list;
}

bool BSRagdollContactListenerModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSRagdollContactListenerModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_RAGDOLL_CONTACT_LISTENER_MODIFIER){
        recobj = static_cast<BSRagdollContactListenerModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!contactEvent.payload.data() && recobj->contactEvent.payload.data()){
            getParentFile()->addObjectToFile(recobj->contactEvent.payload.data(), -1);
        }
        if (!bones.data() && recobj->bones.data()){
            recobj->bones->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->bones.data(), -1);
        }
        return true;
    }else{
        return false;
    }
}

void BSRagdollContactListenerModifier::setBones(hkbBoneIndexArray *value){
    std::lock_guard <std::mutex> guard(mutex);
    bones = HkxSharedPtr(value), setIsFileChanged(true);
}

hkbBoneIndexArray *BSRagdollContactListenerModifier::getBones() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbBoneIndexArray *>(bones.data());
}

void BSRagdollContactListenerModifier::setContactEventID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != contactEvent.id && contactEvent.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? contactEvent.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'contactEvent.id' was not set!");
}

void BSRagdollContactListenerModifier::setContactEventPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(contactEvent.payload.data())) ? contactEvent.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'contactEvent.payload' was not set!");
}

int BSRagdollContactListenerModifier::getContactEventID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return contactEvent.id;
}

hkbStringEventPayload *BSRagdollContactListenerModifier::getContactEventPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(contactEvent.payload.data());
}

bool BSRagdollContactListenerModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSRagdollContactListenerModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSRagdollContactListenerModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSRagdollContactListenerModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkstuff = [&](HkxSharedPtr & data, const QString & fieldname, HkxSignature sig){
        if (ptr){
            if ((*ptr)->getSignature() != sig){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object '"+fieldname+"' is not a HKB_STRING_EVENT_PAYLOAD!");
                setDataValidity(false);
            }
            data = *ptr;
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(contactEvent.payload.getShdPtrReference());
    linkstuff(contactEvent.payload, "contactEvent.payload", HKB_STRING_EVENT_PAYLOAD);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(bones.getShdPtrReference());
    linkstuff(bones, "bones", HKB_BONE_INDEX_ARRAY);
    return true;
}

void BSRagdollContactListenerModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    contactEvent.payload = HkxSharedPtr();
    bones = HkxSharedPtr();
}

QString BSRagdollContactListenerModifier::evaluateDataValidity(){
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
    if (contactEvent.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": contactEvent event id out of range! Setting to max index in range!");
        contactEvent.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (contactEvent.payload.data() && contactEvent.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid contactEvent.payload type! Signature: "+QString::number(contactEvent.payload->getSignature(), 16)+" Setting null value!");
        contactEvent.payload = HkxSharedPtr();
    }
    if (!bones.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null bones!");
    }else if (bones->getSignature() != HKB_BONE_INDEX_ARRAY){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid bones type! Signature: "+QString::number(bones->getSignature(), 16)+" Setting null value!");
        bones = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSRagdollContactListenerModifier::~BSRagdollContactListenerModifier(){
    refCount--;
}
