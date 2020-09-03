#include "hkbsensehandlemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbSenseHandleModifier::refCount = 0;

const QString hkbSenseHandleModifier::classname = "hkbSenseHandleModifier";

const QStringList hkbSenseHandleModifier::SensingMode = {"SENSE_IN_NEARBY_RIGID_BODIES", "SENSE_IN_RIGID_BODIES_OUTSIDE_THIS_CHARACTER", "SENSE_IN_OTHER_CHARACTER_RIGID_BODIES", "SENSE_IN_THIS_CHARACTER_RIGID_BODIES", "SENSE_IN_GIVEN_CHARACTER_RIGID_BODIES", "SENSE_IN_GIVEN_RIGID_BODY", "SENSE_IN_OTHER_CHARACTER_SKELETON", "SENSE_IN_THIS_CHARACTER_SKELETON", "SENSE_IN_GIVEN_CHARACTER_SKELETON", "SENSE_IN_GIVEN_LOCAL_FRAME_GROUP"};

hkbSenseHandleModifier::hkbSenseHandleModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      minDistance(0),
      maxDistance(0),
      distanceOut(0),
      collisionFilterInfo(-1),
      sensorRagdollBoneIndex(-1),
      sensorAnimationBoneIndex(-1),
      extrapolateSensorPosition(false),
      keepFirstSensedHandle(false),
      foundHandleOut(false),
      sensingMode(SensingMode.first())
{
    setType(HKB_SENSE_HANDLE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "SenseHandleModifier_"+QString::number(refCount);
}

const QString hkbSenseHandleModifier::getClassname(){
    return classname;
}

QString hkbSenseHandleModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbSenseHandleModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numranges;
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
        }else if (text == "sensorLocalOffset"){
            sensorLocalOffset = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "sensorLocalOffset");
        }else if (text == "ranges"){
            numranges = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "ranges");
            (numranges > 0) ? index++ : NULL;
            for (auto j = 0; j < numranges; j++, index++){
                ranges.append(hkRanges());
                for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "id"){
                        ranges.last().event.id = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "ranges.at("+QString::number(j)+").event.id");
                    }else if (text == "payload"){
                        checkvalue(ranges.last().event.payload.readShdPtrReference(index, reader), "ranges.at("+QString::number(j)+").event.payload");
                    }else if (text == "minDistance"){
                        ranges.last().minDistance = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "ranges.at("+QString::number(j)+").minDistance");
                    }else if (text == "maxDistance"){
                        ranges.last().maxDistance = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "ranges.at("+QString::number(j)+").maxDistance");
                    }else if (text == "ignoreHandle"){
                        ranges.last().ignoreHandle = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "ranges.at("+QString::number(j)+").ignoreHandle");
                        break;
                    }
                }
            }
            (numranges > 0) ? index-- : NULL;
        }else if (text == "handleOut"){
            checkvalue(handleOut.readShdPtrReference(index, reader), "handleOut");
        }else if (text == "handleIn"){
            checkvalue(handleIn.readShdPtrReference(index, reader), "handleIn");
        }else if (text == "localFrameName"){
            localFrameName = reader.getElementValueAt(index);
            checkvalue(ok, "localFrameName");
        }else if (text == "sensorLocalFrameName"){
            sensorLocalFrameName = reader.getElementValueAt(index);
            checkvalue(ok, "sensorLocalFrameName");
        }else if (text == "minDistance"){
            minDistance = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "minDistance");
        }else if (text == "maxDistance"){
            maxDistance = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxDistance");
        }else if (text == "distanceOut"){
            distanceOut = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "distanceOut");
        }else if (text == "collisionFilterInfo"){
            collisionFilterInfo = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "collisionFilterInfo");
        }else if (text == "sensorRagdollBoneIndex"){
            sensorRagdollBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "sensorRagdollBoneIndex");
        }else if (text == "sensorAnimationBoneIndex"){
            sensorAnimationBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "sensorAnimationBoneIndex");
        }else if (text == "sensingMode"){
            sensingMode = reader.getElementValueAt(index);
            checkvalue(SensingMode.contains(sensingMode), "sensingMode");
        }else if (text == "extrapolateSensorPosition"){
            extrapolateSensorPosition = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "extrapolateSensorPosition");
        }else if (text == "keepFirstSensedHandle"){
            keepFirstSensedHandle = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "keepFirstSensedHandle");
        }else if (text == "foundHandleOut"){
            foundHandleOut = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "foundHandleOut");
        }
    }
    index--;
    return true;
}

bool hkbSenseHandleModifier::write(HkxXMLWriter *writer){
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
        writedatafield("sensorLocalOffset", sensorLocalOffset.getValueAsString());
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"ranges", QString::number(ranges.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < ranges.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("id", QString::number(ranges.at(i).event.id));
            writeref(ranges.at(i).event.payload, "payload");
            writedatafield("minDistance", QString::number(ranges.at(i).minDistance, char('f'), 6));
            writedatafield("maxDistance", QString::number(ranges.at(i).maxDistance, char('f'), 6));
            writedatafield("ignoreHandle", getBoolAsString(ranges.at(i).ignoreHandle));
            writer->writeLine(writer->object, false);
        }
        if (ranges.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writeref(handleOut, "handleOut");
        writeref(handleIn, "handleIn");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("localFrameName"), localFrameName, true);
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("sensorLocalFrameName"), sensorLocalFrameName, true);
        writedatafield("minDistance", QString::number(minDistance, char('f'), 6));
        writedatafield("maxDistance", QString::number(maxDistance, char('f'), 6));
        writedatafield("distanceOut", QString::number(distanceOut, char('f'), 6));
        writedatafield("collisionFilterInfo", QString::number(collisionFilterInfo));
        writedatafield("sensorRagdollBoneIndex", QString::number(sensorRagdollBoneIndex));
        writedatafield("sensorAnimationBoneIndex", QString::number(sensorAnimationBoneIndex));
        writedatafield("sensingMode", sensingMode);
        writedatafield("extrapolateSensorPosition", getBoolAsString(extrapolateSensorPosition));
        writedatafield("keepFirstSensedHandle", getBoolAsString(keepFirstSensedHandle));
        writedatafield("foundHandleOut", getBoolAsString(foundHandleOut));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        for (auto i = 0; i < ranges.size(); i++){
            writechild(ranges.at(i).event.payload, "ranges.at("+QString::number(i)+").event.payload");
        }
    }
    return true;
}

int hkbSenseHandleModifier::getNumberOfRanges() const{
    std::lock_guard <std::mutex> guard(mutex);
    return ranges.size();
}

bool hkbSenseHandleModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < ranges.size(); i++){
        if (ranges.at(i).event.id == eventindex){
            return true;
        }
    }
    return false;
}

void hkbSenseHandleModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < ranges.size(); i++){
        (ranges.at(i).event.id > eventindex) ? ranges[i].event.id-- : NULL;
    }
}

void hkbSenseHandleModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < ranges.size(); i++){
        (ranges.at(i).event.id == oldindex) ? ranges[i].event.id = newindex : NULL;
    }
}

void hkbSenseHandleModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            for (auto i = 0; i < ranges.size(); i++){
                thiseventname = recdata->getEventNameAt(ranges.at(i).event.id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                ranges[i].event.id = eventindex;
            }
            setIsMerged(true);
        }
    }
}

void hkbSenseHandleModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    for (auto i = 0; i < ranges.size(); i++){
        (ranges.at(i).event.payload.data()) ? ranges[i].event.payload->updateReferences(++ref) : NULL;
    }
}

QVector<HkxObject *> hkbSenseHandleModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    for (auto i = 0; i < ranges.size(); i++){
        (ranges.at(i).event.payload.data()) ? list.append(ranges.at(i).event.payload.data()) : NULL;
    }
    return list;
}

bool hkbSenseHandleModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbSenseHandleModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_SENSE_HANDLE_MODIFIER){
        recobj = static_cast<hkbSenseHandleModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        for (auto i = 0; i < ranges.size(); i++){
            if (ranges.at(i).event.payload.data()){
                if (!ranges.at(i).event.payload.data() && recobj->ranges.at(i).event.payload.data()){
                    getParentFile()->addObjectToFile(recobj->ranges.at(i).event.payload.data(), -1);
                }
            }
        }
        return true;
    }
    return false;
}

void hkbSenseHandleModifier::addRange(hkbSenseHandleModifier::hkRanges range){
    std::lock_guard <std::mutex> guard(mutex);
    ranges.append(range), setIsFileChanged(true);
}

void hkbSenseHandleModifier::removeRange(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < ranges.size()) ? ranges.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'ranges' was not removed!");
}

bool hkbSenseHandleModifier::getFoundHandleOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return foundHandleOut;
}

void hkbSenseHandleModifier::setFoundHandleOut(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != foundHandleOut) ? foundHandleOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'foundHandleOut' was not set!");
}

bool hkbSenseHandleModifier::getKeepFirstSensedHandle() const{
    std::lock_guard <std::mutex> guard(mutex);
    return keepFirstSensedHandle;
}

void hkbSenseHandleModifier::setKeepFirstSensedHandle(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != keepFirstSensedHandle) ? keepFirstSensedHandle = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'keepFirstSensedHandle' was not set!");
}

bool hkbSenseHandleModifier::getExtrapolateSensorPosition() const{
    std::lock_guard <std::mutex> guard(mutex);
    return extrapolateSensorPosition;
}

void hkbSenseHandleModifier::setExtrapolateSensorPosition(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != extrapolateSensorPosition) ? extrapolateSensorPosition = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'extrapolateSensorPosition' was not set!");
}

QString hkbSenseHandleModifier::getSensingMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sensingMode;
}

void hkbSenseHandleModifier::setSensingMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < SensingMode.size() && sensingMode != SensingMode.at(index)) ? sensingMode = SensingMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sensingMode' was not set!");
}

int hkbSenseHandleModifier::getSensorAnimationBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sensorAnimationBoneIndex;
}

void hkbSenseHandleModifier::setSensorAnimationBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != sensorAnimationBoneIndex && sensorAnimationBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? sensorAnimationBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sensorAnimationBoneIndex' was not set!");
}

void hkbSenseHandleModifier::setSensorRagdollBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != sensorRagdollBoneIndex && sensorRagdollBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)) ? sensorRagdollBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sensorRagdollBoneIndex' was not set!");
}

int hkbSenseHandleModifier::getSensorRagdollBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sensorRagdollBoneIndex;
}

int hkbSenseHandleModifier::getCollisionFilterInfo() const{
    std::lock_guard <std::mutex> guard(mutex);
    return collisionFilterInfo;
}

void hkbSenseHandleModifier::setCollisionFilterInfo(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != collisionFilterInfo && collisionFilterInfo >= 0 && collisionFilterInfo < 50/*TO DO*/) ? collisionFilterInfo = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'collisionFilterInfo' was not set!");
}

qreal hkbSenseHandleModifier::getDistanceOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return distanceOut;
}

void hkbSenseHandleModifier::setDistanceOut(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != distanceOut) ? distanceOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'distanceOut' was not set!");
}

qreal hkbSenseHandleModifier::getMaxDistance() const{
    std::lock_guard <std::mutex> guard(mutex);
    return maxDistance;
}

void hkbSenseHandleModifier::setMaxDistance(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != maxDistance) ? maxDistance = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'maxDistance' was not set!");
}

qreal hkbSenseHandleModifier::getMinDistance() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minDistance;
}

void hkbSenseHandleModifier::setMinDistance(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minDistance) ? minDistance = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minDistance' was not set!");
}

QString hkbSenseHandleModifier::getSensorLocalFrameName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sensorLocalFrameName;
}

void hkbSenseHandleModifier::setSensorLocalFrameName(int index){
    std::lock_guard <std::mutex> guard(mutex);
    auto localframes = static_cast<BehaviorFile *>(getParentFile())->getLocalFrameNames();
    (index >= 0 && index < localframes.size() && localframes.at(index) != sensorLocalFrameName) ? sensorLocalFrameName = localframes.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sensorLocalFrameName' was not set!");
}

QString hkbSenseHandleModifier::getLocalFrameName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return localFrameName;
}

void hkbSenseHandleModifier::setLocalFrameName(int index){
    std::lock_guard <std::mutex> guard(mutex);
    auto localframes = static_cast<BehaviorFile *>(getParentFile())->getLocalFrameNames();
    (index >= 0 && index < localframes.size() && localframes.at(index) != localFrameName) ? localFrameName = localframes.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'localFrameName' was not set!");
}

hkQuadVariable hkbSenseHandleModifier::getSensorLocalOffset() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sensorLocalOffset;
}

void hkbSenseHandleModifier::setSensorLocalOffset(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != sensorLocalOffset) ? sensorLocalOffset = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sensorLocalOffset' was not set!");
}

bool hkbSenseHandleModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbSenseHandleModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbSenseHandleModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbSenseHandleModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    HkxSharedPtr *ptr;
    for (auto i = 0; i < ranges.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(ranges.at(i).event.payload.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                return false;
            }
            ranges[i].event.payload = *ptr;
        }
    }
    return true;
}

void hkbSenseHandleModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    for (auto i = 0; i < ranges.size(); i++){
        ranges[i].event.payload = HkxSharedPtr();
    }
}

QString hkbSenseHandleModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (ranges.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": ranges is empty!");
    }else{
        for (auto i = 0; i < ranges.size(); i++){
            if (ranges.at(i).event.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": id in ranges at "+QString::number(i)+" out of range!");
            }
            if (ranges.at(i).event.payload.data() && ranges.at(i).event.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid payload type! Signature: "+QString::number(ranges.at(i).event.payload->getSignature(), 16)+"\n");
            }
        }
    }
    auto temp = HkDynamicObject::evaluateDataValidity(); if (temp != ""){errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");}
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (!SensingMode.contains(sensingMode)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid sensingMode! Setting default value!");
        sensingMode = SensingMode.first();
    }
    if (sensorRagdollBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": sensorRagdollBoneIndex out of range! Setting to last bone index!");
        sensorRagdollBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true) - 1;
    }
    if (sensorAnimationBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": sensorAnimationBoneIndex out of range! Setting to last bone index!");
        sensorAnimationBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (sensorRagdollBoneIndex > -1 && sensorAnimationBoneIndex > -1){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": sensorRagdollBoneIndex and sensorAnimationBoneIndex are both in use at the same time! This will crash the game! Setting sensorRagdollBoneIndex to -1!");
        sensorRagdollBoneIndex = -1;
    }
    if (sensorRagdollBoneIndex < 0 && sensorAnimationBoneIndex < 0 ){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Neither sensorRagdollBoneIndex and sensorAnimationBoneIndex are in use! Setting sensorAnimationBoneIndex to 0!");
        sensorAnimationBoneIndex = 0;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbSenseHandleModifier::~hkbSenseHandleModifier(){
    refCount--;
}
