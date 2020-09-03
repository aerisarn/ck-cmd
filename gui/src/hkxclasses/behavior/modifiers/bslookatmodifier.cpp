#include "bslookatmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint BSLookAtModifier::refCount = 0;

const QString BSLookAtModifier::classname = "BSLookAtModifier";

BSLookAtModifier::BSLookAtModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      lookAtTarget(false),
      limitAngleDegrees(0),
      limitAngleThresholdDegrees(0),
      continueLookOutsideOfLimit(false),
      onGain(0),
      offGain(0),
      useBoneGains(false),
      targetOutsideLimits(false),
      id(-1),
      lookAtCamera(false),
      lookAtCameraX(0),
      lookAtCameraY(0),
      lookAtCameraZ(0)
{
    setType(BS_LOOK_AT_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "LookAtModifier_"+QString::number(refCount);
}

const QString BSLookAtModifier::getClassname(){
    return classname;
}

QString BSLookAtModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

void BSLookAtModifier::removeBone(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < bones.size()) ? bones.removeAt(index) : LogFile::writeToLog(getClassname()+": Unable to remove bone!");
}

void BSLookAtModifier::removeEyeBone(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < eyeBones.size()) ? eyeBones.removeAt(index) : LogFile::writeToLog(getClassname()+": Unable to remove eyeBone!");
}

void BSLookAtModifier::addBone(const BSLookAtModifier::BsBone &bone){
    std::lock_guard <std::mutex> guard(mutex);
    bones.append(bone);
}

void BSLookAtModifier::addEyeBone(const BSLookAtModifier::BsBone &bone){
    std::lock_guard <std::mutex> guard(mutex);
    eyeBones.append(bone);
}

bool BSLookAtModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    int numbones;
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
        }else if (text == "lookAtTarget"){
            lookAtTarget = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "lookAtTarget");
        }else if (text == "bones"){
            numbones = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "bones");
            for (auto j = 0; j < numbones; j++, index++){
                bones.append(BsBone());
                for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "index"){
                        bones.last().index = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "bones.at("+QString::number(j)+").index");
                    }else if (text == "fwdAxisLS"){
                        bones.last().fwdAxisLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "bones.at("+QString::number(j)+").fwdAxisLS");
                    }else if (text == "limitAngleDegrees"){
                        bones.last().limitAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "bones.at("+QString::number(j)+").limitAngleDegrees");
                    }else if (text == "onGain"){
                        bones.last().onGain = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "bones.at("+QString::number(j)+").onGain");
                    }else if (text == "offGain"){
                        bones.last().offGain = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "bones.at("+QString::number(j)+").offGain");
                    }else if (text == "enabled"){
                        bones.last().enabled = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "bones.at("+QString::number(j)+").enabled");
                        break;
                    }
                }
            }
            (numbones > 0) ? index-- : NULL;
        }else if (text == "eyeBones"){
            numbones = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "eyeBones");
            for (auto j = 0; j < numbones; j++, index++){
                eyeBones.append(BsBone());
                for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "index"){
                        eyeBones.last().index = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "eyeBones.at("+QString::number(j)+").index");
                    }else if (text == "fwdAxisLS"){
                        eyeBones.last().fwdAxisLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "eyeBones.at("+QString::number(j)+").fwdAxisLS");
                    }else if (text == "limitAngleDegrees"){
                        eyeBones.last().limitAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "eyeBones.at("+QString::number(j)+").limitAngleDegrees");
                    }else if (text == "onGain"){
                        eyeBones.last().onGain = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "eyeBones.at("+QString::number(j)+").onGain");
                    }else if (text == "offGain"){
                        eyeBones.last().offGain = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "eyeBones.at("+QString::number(j)+").offGain");
                    }else if (text == "enabled"){
                        eyeBones.last().enabled = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "eyeBones.at("+QString::number(j)+").enabled");
                        break;
                    }
                }
            }
            (numbones > 0) ? index-- : NULL;
        }else if (text == "limitAngleDegrees"){
            limitAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "limitAngleDegrees");
        }else if (text == "limitAngleThresholdDegrees"){
            limitAngleThresholdDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "limitAngleThresholdDegrees");
        }else if (text == "continueLookOutsideOfLimit"){
            continueLookOutsideOfLimit = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "continueLookOutsideOfLimit");
        }else if (text == "onGain"){
            onGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "onGain");
        }else if (text == "offGain"){
            offGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "offGain");
        }else if (text == "useBoneGains"){
            useBoneGains = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "useBoneGains");
        }else if (text == "targetLocation"){
            targetLocation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "targetLocation");
        }else if (text == "targetOutsideLimits"){
            targetOutsideLimits = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "targetOutsideLimits");
        }else if (text == "id"){
            id = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "id");
        }else if (text == "payload"){
            checkvalue(payload.readShdPtrReference(index, reader), "payload");
        }else if (text == "lookAtCamera"){
            lookAtCamera = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "lookAtCamera");
        }else if (text == "lookAtCameraX"){
            lookAtCameraX = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "lookAtCameraX");
        }else if (text == "lookAtCameraY"){
            lookAtCameraY = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "lookAtCameraY");
        }else if (text == "lookAtCameraZ"){
            lookAtCameraZ = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "lookAtCameraZ");
        }
    }
    index--;
    return true;
}

bool BSLookAtModifier::write(HkxXMLWriter *writer){
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
        writedatafield("lookAtTarget", getBoolAsString(lookAtTarget));
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"bones", QString::number(bones.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < bones.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("index", QString::number(bones.at(i).index));
            writedatafield("fwdAxisLS", bones[i].fwdAxisLS.getValueAsString());
            writedatafield("limitAngleDegrees", QString::number(bones.at(i).limitAngleDegrees, char('f'), 6));
            writedatafield("onGain", QString::number(bones.at(i).onGain, char('f'), 6));
            writedatafield("offGain", QString::number(bones.at(i).offGain, char('f'), 6));
            writedatafield("enabled", getBoolAsString(bones.at(i).enabled));
            writer->writeLine(writer->object, false);
        }
        if (bones.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list2 = QStringList({"eyeBones", QString::number(eyeBones.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < eyeBones.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("index", QString::number(eyeBones.at(i).index));
            writedatafield("fwdAxisLS", eyeBones[i].fwdAxisLS.getValueAsString());
            writedatafield("limitAngleDegrees", QString::number(eyeBones.at(i).limitAngleDegrees, char('f'), 6));
            writedatafield("onGain", QString::number(eyeBones.at(i).onGain, char('f'), 6));
            writedatafield("offGain", QString::number(eyeBones.at(i).offGain, char('f'), 6));
            writedatafield("enabled", getBoolAsString(eyeBones.at(i).enabled));
            writer->writeLine(writer->object, false);
        }
        if (eyeBones.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("limitAngleDegrees", QString::number(limitAngleDegrees));
        writedatafield("limitAngleThresholdDegrees", QString::number(limitAngleThresholdDegrees));
        writedatafield("continueLookOutsideOfLimit", getBoolAsString(continueLookOutsideOfLimit));
        writedatafield("onGain", QString::number(onGain, char('f'), 6));
        writedatafield("offGain", QString::number(offGain, char('f'), 6));
        writedatafield("useBoneGains", getBoolAsString(useBoneGains));
        writedatafield("targetLocation", targetLocation.getValueAsString());
        writedatafield("targetOutsideLimits", getBoolAsString(targetOutsideLimits));
        writedatafield("targetOutOfLimitEvent", "");
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(id));
        writeref(payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("lookAtCamera", getBoolAsString(lookAtCamera));
        writedatafield("lookAtCameraX", QString::number(lookAtCameraX, char('f'), 6));
        writedatafield("lookAtCameraY", QString::number(lookAtCameraY, char('f'), 6));
        writedatafield("lookAtCameraZ", QString::number(lookAtCameraZ, char('f'), 6));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(payload, "payload");
    }
    return true;
}

int BSLookAtModifier::getNumberOfBones() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bones.size();
}

int BSLookAtModifier::getNumberOfEyeBones() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eyeBones.size();
}

bool BSLookAtModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (id == eventindex){
        return true;
    }
    return false;
}

void BSLookAtModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (id > eventindex) ? id-- : NULL;
}

void BSLookAtModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (id == oldindex) ? id = newindex: NULL;
}

void BSLookAtModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            fixIndex(id);
            setIsMerged(true);
        }
    }
}


void BSLookAtModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (payload.data()) ? payload->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> BSLookAtModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (payload.data()) ? list.append(payload.data()) : NULL;
    return list;
}

bool BSLookAtModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSLookAtModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_LOOK_AT_MODIFIER){
        recobj = static_cast<BSLookAtModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!payload.data() && recobj->payload.data()){
            getParentFile()->addObjectToFile(recobj->payload.data(), -1);
        }
        return true;
    }
    return false;
}

QVector<BSLookAtModifier::BsBone> &BSLookAtModifier::getEyeBones(){
    std::lock_guard <std::mutex> guard(mutex);
    return eyeBones;
}

QVector<BSLookAtModifier::BsBone> &BSLookAtModifier::getBones(){
    std::lock_guard <std::mutex> guard(mutex);
    return bones;
}

qreal BSLookAtModifier::getLookAtCameraZ() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lookAtCameraZ;
}

void BSLookAtModifier::setLookAtCameraZ(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lookAtCameraZ) ? lookAtCameraZ = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lookAtCameraZ' was not set!");
}

qreal BSLookAtModifier::getLookAtCameraY() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lookAtCameraY;
}

void BSLookAtModifier::setLookAtCameraY(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lookAtCameraY) ? lookAtCameraY = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lookAtCameraY' was not set!");
}

qreal BSLookAtModifier::getLookAtCameraX() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lookAtCameraX;
}

void BSLookAtModifier::setLookAtCameraX(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lookAtCameraX) ? lookAtCameraX = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lookAtCameraX' was not set!");
}

bool BSLookAtModifier::getLookAtCamera() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lookAtCamera;
}

void BSLookAtModifier::setLookAtCamera(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lookAtCamera) ? lookAtCamera = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lookAtCamera' was not set!");
}

hkbStringEventPayload * BSLookAtModifier::getPayload() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbStringEventPayload *>(payload.data());
}

void BSLookAtModifier::setPayload(hkbStringEventPayload *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != static_cast<hkbStringEventPayload *>(payload.data())) ? payload = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'payload' was not set!");
}

int BSLookAtModifier::getId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return id;
}

void BSLookAtModifier::setId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != id && id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? id = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'id' was not set!");
}

bool BSLookAtModifier::getTargetOutsideLimits() const{
    std::lock_guard <std::mutex> guard(mutex);
    return targetOutsideLimits;
}

void BSLookAtModifier::setTargetOutsideLimits(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != targetOutsideLimits) ? targetOutsideLimits = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'targetOutsideLimits' was not set!");
}

hkQuadVariable BSLookAtModifier::getTargetLocation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return targetLocation;
}

void BSLookAtModifier::setTargetLocation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != targetLocation) ? targetLocation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'targetLocation' was not set!");
}

bool BSLookAtModifier::getUseBoneGains() const{
    std::lock_guard <std::mutex> guard(mutex);
    return useBoneGains;
}

void BSLookAtModifier::setUseBoneGains(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != useBoneGains) ? useBoneGains = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'useBoneGains' was not set!");
}

qreal BSLookAtModifier::getOffGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return offGain;
}

void BSLookAtModifier::setOffGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != offGain) ? offGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'offGain' was not set!");
}

qreal BSLookAtModifier::getOnGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return onGain;
}

void BSLookAtModifier::setOnGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != onGain) ? onGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'onGain' was not set!");
}

bool BSLookAtModifier::getContinueLookOutsideOfLimit() const{
    std::lock_guard <std::mutex> guard(mutex);
    return continueLookOutsideOfLimit;
}

void BSLookAtModifier::setContinueLookOutsideOfLimit(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != continueLookOutsideOfLimit) ? continueLookOutsideOfLimit = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'continueLookOutsideOfLimit' was not set!");
}

qreal BSLookAtModifier::getLimitAngleThresholdDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return limitAngleThresholdDegrees;
}

void BSLookAtModifier::setLimitAngleThresholdDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != limitAngleThresholdDegrees) ? limitAngleThresholdDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'limitAngleThresholdDegrees' was not set!");
}

qreal BSLookAtModifier::getLimitAngleDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return limitAngleDegrees;
}

void BSLookAtModifier::setLimitAngleDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != limitAngleDegrees) ? limitAngleDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'limitAngleDegrees' was not set!");
}

bool BSLookAtModifier::getLookAtTarget() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lookAtTarget;
}

void BSLookAtModifier::setLookAtTarget(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lookAtTarget) ? lookAtTarget = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lookAtTarget' was not set!");
}

bool BSLookAtModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSLookAtModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSLookAtModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSLookAtModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(payload.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'payload' is not a HKB_STRING_EVENT_PAYLOAD!");
        }
        payload = *ptr;
    }
    return true;
}

void BSLookAtModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    payload = HkxSharedPtr();
}

QString BSLookAtModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (bones.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": bones is empty!");
    }else{
        for (auto i = bones.size() - 1; i >= 0; i--){
            if (bones.at(i).index >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid bones at index: "+QString::number(i)+" Removing bone!");
                bones.removeAt(i);
            }
        }
        for (auto i = eyeBones.size() - 1; i >= 0; i--){
            if (eyeBones.at(i).index >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid bones at index: "+QString::number(i)+" Removing bone!");
                eyeBones.removeAt(i);
            }
        }
    }
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": event id out of range! Setting to max index in range!");
        id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (payload.data() && payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid payload type! Signature: "+QString::number(payload->getSignature(), 16)+" Setting null value!");
        payload = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSLookAtModifier::~BSLookAtModifier(){
    refCount--;
}
