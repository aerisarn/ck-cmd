#include "hkbdetectclosetogroundmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint hkbDetectCloseToGroundModifier::refCount = 0;

const QString hkbDetectCloseToGroundModifier::classname = "hkbDetectCloseToGroundModifier";

hkbDetectCloseToGroundModifier::hkbDetectCloseToGroundModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(1),
      enable(true),
      closeToGroundHeight(0),
      raycastDistanceDown(0),
      collisionFilterInfo(0),
      boneIndex(-1),
      animBoneIndex(-1)
{
    setType(HKB_DETECT_CLOSE_TO_GROUND_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "DetectCloseToGroundModifier_"+QString::number(refCount);
}

const QString hkbDetectCloseToGroundModifier::getClassname(){
    return classname;
}

QString hkbDetectCloseToGroundModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbDetectCloseToGroundModifier::readData(const HkxXmlReader &reader, long & index){
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
            closeToGroundEvent.id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "id");
        }else if (text == "payload"){
            checkvalue(closeToGroundEvent.payload.readShdPtrReference(index, reader), "payload");
        }else if (text == "closeToGroundHeight"){
            closeToGroundHeight = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "closeToGroundHeight");
        }else if (text == "raycastDistanceDown"){
            raycastDistanceDown = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "raycastDistanceDown");
        }else if (text == "collisionFilterInfo"){
            collisionFilterInfo = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "collisionFilterInfo");
        }else if (text == "boneIndex"){
            boneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "boneIndex");
        }else if (text == "animBoneIndex"){
            animBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "animBoneIndex");
        }
    }
    index--;
    return true;
}

bool hkbDetectCloseToGroundModifier::write(HkxXMLWriter *writer){
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
        writedatafield("closeToGroundEvent", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(closeToGroundEvent.id));
        writeref(closeToGroundEvent.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writer->writeLine(writer->object, false);
        writedatafield("closeToGroundHeight", QString::number(closeToGroundHeight, char('f'), 6));
        writedatafield("raycastDistanceDown", QString::number(raycastDistanceDown, char('f'), 6));
        writedatafield("collisionFilterInfo", QString::number(collisionFilterInfo));
        writedatafield("boneIndex", QString::number(boneIndex));
        writedatafield("animBoneIndex", QString::number(animBoneIndex));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(closeToGroundEvent.payload, "payload");
    }
    return true;
}

bool hkbDetectCloseToGroundModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (closeToGroundEvent.id == eventindex){
        return true;
    }
    return false;
}

void hkbDetectCloseToGroundModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (closeToGroundEvent.id > eventindex) ? closeToGroundEvent.id-- : NULL;
}

void hkbDetectCloseToGroundModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (closeToGroundEvent.id == oldindex) ? closeToGroundEvent.id = newindex : NULL;
}

void hkbDetectCloseToGroundModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(closeToGroundEvent.id);
            setIsMerged(true);
        }
    }
}

void hkbDetectCloseToGroundModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (closeToGroundEvent.payload.data()) ? closeToGroundEvent.payload->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> hkbDetectCloseToGroundModifier::getChildrenOtherTypes() const{
    QVector<HkxObject *> list;
    (closeToGroundEvent.payload.data()) ? list.append(closeToGroundEvent.payload.data()) : NULL;
    return list;
}

bool hkbDetectCloseToGroundModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbDetectCloseToGroundModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_DETECT_CLOSE_TO_GROUND_MODIFIER){
        recobj = static_cast<hkbDetectCloseToGroundModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!closeToGroundEvent.payload.data() && recobj->closeToGroundEvent.payload.data()){
            getParentFile()->addObjectToFile(recobj->closeToGroundEvent.payload.data(), -1);
        }
        return true;
    }
    return false;
}

hkbStringEventPayload *hkbDetectCloseToGroundModifier::getCloseToGroundEventPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(closeToGroundEvent.payload.data());
}

void hkbDetectCloseToGroundModifier::setCloseToGroundEventPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(closeToGroundEvent.payload.data())) ? closeToGroundEvent.payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'closeToGroundEvent.payload' was not set!");
}

int hkbDetectCloseToGroundModifier::getCloseToGroundEventID() const{
    std::lock_guard <std::mutex> guard(mutex);
    return closeToGroundEvent.id;
}

void hkbDetectCloseToGroundModifier::setCloseToGroundEventID(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != closeToGroundEvent.id && closeToGroundEvent.id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? closeToGroundEvent.id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'closeToGroundEvent.id' was not set!");
}

int hkbDetectCloseToGroundModifier::getAnimBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return animBoneIndex;
}

void hkbDetectCloseToGroundModifier::setAnimBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (boneIndex < 0 && value != animBoneIndex && animBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? animBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'animBoneIndex' was not set!");
}

int hkbDetectCloseToGroundModifier::getBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return boneIndex;
}

void hkbDetectCloseToGroundModifier::setBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (animBoneIndex < 0 && value != boneIndex && boneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? boneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'boneIndex' was not set!");
}

int hkbDetectCloseToGroundModifier::getCollisionFilterInfo() const{
    std::lock_guard <std::mutex> guard(mutex);
    return collisionFilterInfo;
}

void hkbDetectCloseToGroundModifier::setCollisionFilterInfo(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != collisionFilterInfo && collisionFilterInfo >= 0 && collisionFilterInfo < 50/*TO DO*/) ? collisionFilterInfo = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'collisionFilterInfo' was not set!");
}

qreal hkbDetectCloseToGroundModifier::getRaycastDistanceDown() const{
    std::lock_guard <std::mutex> guard(mutex);
    return raycastDistanceDown;
}

void hkbDetectCloseToGroundModifier::setRaycastDistanceDown(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != raycastDistanceDown) ? raycastDistanceDown = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'raycastDistanceDown' was not set!");
}

qreal hkbDetectCloseToGroundModifier::getCloseToGroundHeight() const{
    std::lock_guard <std::mutex> guard(mutex);
    return closeToGroundHeight;
}

void hkbDetectCloseToGroundModifier::setCloseToGroundHeight(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != closeToGroundHeight) ? closeToGroundHeight = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'closeToGroundHeight' was not set!");
}

bool hkbDetectCloseToGroundModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbDetectCloseToGroundModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbDetectCloseToGroundModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbDetectCloseToGroundModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(closeToGroundEvent.payload.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'payload' is not a HKB_STRING_EVENT_PAYLOAD!");
            setDataValidity(false);
        }
        closeToGroundEvent.payload = *ptr;
    }
    return true;
}

void hkbDetectCloseToGroundModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    closeToGroundEvent.payload = HkxSharedPtr();
}

QString hkbDetectCloseToGroundModifier::evaluateDataValidity(){
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
    if (closeToGroundEvent.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": closeToGroundEvent event id out of range! Setting to max index in range!");
        closeToGroundEvent.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (closeToGroundEvent.payload.data() && closeToGroundEvent.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid closeToGroundEvent.payload type! Signature: "+QString::number(closeToGroundEvent.payload->getSignature(), 16)+" Setting null value!");
        closeToGroundEvent.payload = HkxSharedPtr();
    }
    if (boneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": boneIndex out of range! Setting to last bone index!");
        boneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true) - 1;
    }
    if (animBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": animBoneIndex out of range! Setting to last bone index!");
        animBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (boneIndex > -1 && animBoneIndex > -1){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": boneIndex and animBoneIndex are both in use at the same time! This will crash the game! Setting boneIndex to -1!");
        boneIndex = -1;
    }
    if (boneIndex < 0 && animBoneIndex < 0 ){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Neither boneIndex and animBoneIndex are in use! Setting animBoneIndex to 0!");
        animBoneIndex = 0;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbDetectCloseToGroundModifier::~hkbDetectCloseToGroundModifier(){
    refCount--;
}
