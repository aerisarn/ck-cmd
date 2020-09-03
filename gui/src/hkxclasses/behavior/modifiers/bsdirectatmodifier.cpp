#include "bsdirectatmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSDirectAtModifier::refCount = 0;

const QString BSDirectAtModifier::classname = "BSDirectAtModifier";

BSDirectAtModifier::BSDirectAtModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(2),
      enable(true),
      directAtTarget(true),
      sourceBoneIndex(-1),
      startBoneIndex(-1),
      endBoneIndex(-1),
      limitHeadingDegrees(0),
      limitPitchDegrees(0),
      offsetHeadingDegrees(0),
      offsetPitchDegrees(0),
      onGain(0),
      offGain(0),
      userInfo(0),
      directAtCamera(false),
      directAtCameraX(0),
      directAtCameraY(0),
      directAtCameraZ(0),
      active(false),
      currentHeadingOffset(0),
      currentPitchOffset(0)
{
    setType(BS_DIRECT_AT_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "DirectAtModifier_"+QString::number(refCount);
}

const QString BSDirectAtModifier::getClassname(){
    return classname;
}

QString BSDirectAtModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSDirectAtModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "directAtTarget"){
            directAtTarget = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "directAtTarget");
        }else if (text == "sourceBoneIndex"){
            sourceBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "sourceBoneIndex");
        }else if (text == "startBoneIndex"){
            startBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "startBoneIndex");
        }else if (text == "endBoneIndex"){
            endBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "endBoneIndex");
        }else if (text == "limitHeadingDegrees"){
            limitHeadingDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "limitHeadingDegrees");
        }else if (text == "limitPitchDegrees"){
            limitPitchDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "limitPitchDegrees");
        }else if (text == "offsetHeadingDegrees"){
            offsetHeadingDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "offsetHeadingDegrees");
        }else if (text == "offsetPitchDegrees"){
            offsetPitchDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "offsetPitchDegrees");
        }else if (text == "onGain"){
            onGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "onGain");
        }else if (text == "offGain"){
            offGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "offGain");
        }else if (text == "targetLocation"){
            targetLocation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "targetLocation");
        }else if (text == "userInfo"){
            userInfo = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "userInfo");
        }else if (text == "directAtCamera"){
            directAtCamera = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "directAtCamera");
        }else if (text == "directAtCameraX"){
            directAtCameraX = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "directAtCameraX");
        }else if (text == "directAtCameraY"){
            directAtCameraY = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "directAtCameraY");
        }else if (text == "directAtCameraZ"){
            directAtCameraZ = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "directAtCameraZ");
        }else if (text == "active"){
            active = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "active");
        }else if (text == "currentHeadingOffset"){
            currentHeadingOffset = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "currentHeadingOffset");
        }else if (text == "currentPitchOffset"){
            currentPitchOffset = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "currentPitchOffset");
        }
    }
    index--;
    return true;
}

bool BSDirectAtModifier::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writedatafield("variableBindingSet", refString, false);
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writedatafield("enable", getBoolAsString(enable), false);
        writedatafield("directAtTarget", getBoolAsString(directAtTarget), false);
        writedatafield("sourceBoneIndex", QString::number(sourceBoneIndex), false);
        writedatafield("startBoneIndex", QString::number(startBoneIndex), false);
        writedatafield("endBoneIndex", QString::number(endBoneIndex), false);
        writedatafield("limitHeadingDegrees", QString::number(limitHeadingDegrees, char('f'), 6), false);
        writedatafield("limitPitchDegrees", QString::number(limitPitchDegrees, char('f'), 6), false);
        writedatafield("offsetHeadingDegrees", QString::number(offsetHeadingDegrees, char('f'), 6), false);
        writedatafield("offsetPitchDegrees", QString::number(offsetPitchDegrees, char('f'), 6), false);
        writedatafield("onGain", QString::number(onGain, char('f'), 6), false);
        writedatafield("offGain", QString::number(offGain, char('f'), 6), false);
        writedatafield("targetLocation", targetLocation.getValueAsString(), false);
        writedatafield("userInfo", QString::number(userInfo), false);
        writedatafield("directAtCamera", getBoolAsString(directAtCamera), false);
        writedatafield("directAtCameraX", QString::number(directAtCameraX, char('f'), 6), false);
        writedatafield("directAtCameraY", QString::number(directAtCameraY, char('f'), 6), false);
        writedatafield("directAtCameraZ", QString::number(directAtCameraZ, char('f'), 6), false);
        writedatafield("active", getBoolAsString(active), false);
        writedatafield("currentHeadingOffset", QString::number(currentHeadingOffset, char('f'), 6), false);
        writedatafield("currentPitchOffset", QString::number(currentPitchOffset, char('f'), 6), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

qreal BSDirectAtModifier::getCurrentPitchOffset() const{
    std::lock_guard <std::mutex> guard(mutex);
    return currentPitchOffset;
}

void BSDirectAtModifier::setCurrentPitchOffset(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != currentPitchOffset) ? currentPitchOffset = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'currentPitchOffset' was not set!");
}

qreal BSDirectAtModifier::getCurrentHeadingOffset() const{
    std::lock_guard <std::mutex> guard(mutex);
    return currentHeadingOffset;
}

void BSDirectAtModifier::setCurrentHeadingOffset(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != currentHeadingOffset) ? currentHeadingOffset = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'currentHeadingOffset' was not set!");
}

bool BSDirectAtModifier::getActive() const{
    std::lock_guard <std::mutex> guard(mutex);
    return active;
}

void BSDirectAtModifier::setActive(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != active) ? active = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'active' was not set!");
}

qreal BSDirectAtModifier::getDirectAtCameraZ() const{
    std::lock_guard <std::mutex> guard(mutex);
    return directAtCameraZ;
}

void BSDirectAtModifier::setDirectAtCameraZ(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != directAtCameraZ) ? directAtCameraZ = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'directAtCameraZ' was not set!");
}

qreal BSDirectAtModifier::getDirectAtCameraY() const{
    std::lock_guard <std::mutex> guard(mutex);
    return directAtCameraY;
}

void BSDirectAtModifier::setDirectAtCameraY(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != directAtCameraY) ? directAtCameraY = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'directAtCameraY' was not set!");
}

qreal BSDirectAtModifier::getDirectAtCameraX() const{
    std::lock_guard <std::mutex> guard(mutex);
    return directAtCameraX;
}

void BSDirectAtModifier::setDirectAtCameraX(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != directAtCameraX) ? directAtCameraX = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'directAtCameraX' was not set!");
}

bool BSDirectAtModifier::getDirectAtCamera() const{
    std::lock_guard <std::mutex> guard(mutex);
    return directAtCamera;
}

void BSDirectAtModifier::setDirectAtCamera(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != directAtCamera) ? directAtCamera = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'directAtCamera' was not set!");
}

int BSDirectAtModifier::getUserInfo() const{
    std::lock_guard <std::mutex> guard(mutex);
    return userInfo;
}

void BSDirectAtModifier::setUserInfo(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != userInfo) ? userInfo = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'userInfo' was not set!");
}

hkQuadVariable BSDirectAtModifier::getTargetLocation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return targetLocation;
}

void BSDirectAtModifier::setTargetLocation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != targetLocation) ? targetLocation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'targetLocation' was not set!");
}

qreal BSDirectAtModifier::getOffGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return offGain;
}

void BSDirectAtModifier::setOffGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != offGain) ? offGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'offGain' was not set!");
}

qreal BSDirectAtModifier::getOnGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return onGain;
}

void BSDirectAtModifier::setOnGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != onGain) ? onGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'onGain' was not set!");
}

qreal BSDirectAtModifier::getOffsetPitchDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return offsetPitchDegrees;
}

void BSDirectAtModifier::setOffsetPitchDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != offsetPitchDegrees) ? offsetPitchDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'offsetPitchDegrees' was not set!");
}

qreal BSDirectAtModifier::getOffsetHeadingDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return offsetHeadingDegrees;
}

void BSDirectAtModifier::setOffsetHeadingDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != offsetHeadingDegrees) ? offsetHeadingDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'offsetHeadingDegrees' was not set!");
}

qreal BSDirectAtModifier::getLimitPitchDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return limitPitchDegrees;
}

void BSDirectAtModifier::setLimitPitchDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != limitPitchDegrees) ? limitPitchDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'limitPitchDegrees' was not set!");
}

qreal BSDirectAtModifier::getLimitHeadingDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return limitHeadingDegrees;
}

void BSDirectAtModifier::setLimitHeadingDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != limitHeadingDegrees) ? limitHeadingDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'limitHeadingDegrees' was not set!");
}

int BSDirectAtModifier::getEndBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return endBoneIndex;
}

void BSDirectAtModifier::setEndBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != endBoneIndex && endBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? endBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'endBoneIndex' was not set!");
}

int BSDirectAtModifier::getStartBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startBoneIndex;
}

void BSDirectAtModifier::setStartBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != startBoneIndex && startBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? startBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startBoneIndex' was not set!");
}

int BSDirectAtModifier::getSourceBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sourceBoneIndex;
}

void BSDirectAtModifier::setSourceBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != sourceBoneIndex && sourceBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? sourceBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sourceBoneIndex' was not set!");
}

bool BSDirectAtModifier::getDirectAtTarget() const{
    std::lock_guard <std::mutex> guard(mutex);
    return directAtTarget;
}

void BSDirectAtModifier::setDirectAtTarget(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != directAtTarget) ? directAtTarget = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'directAtTarget' was not set!");
}

bool BSDirectAtModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSDirectAtModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSDirectAtModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSDirectAtModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void BSDirectAtModifier::unlink(){
    HkDynamicObject::unlink();
}

QString BSDirectAtModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto evaluateindices = [&](int & bone, const QString & fieldname){
        if (bone >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+fieldname+" out of range! Setting to last bone index!");
            bone = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
        }
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    (temp != "") ? errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!\n"): NULL;
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    evaluateindices(sourceBoneIndex, "sourceBoneIndex");
    evaluateindices(startBoneIndex, "startBoneIndex");
    evaluateindices(endBoneIndex, "endBoneIndex");
    setDataValidity(isvalid);
    return errors;
}

BSDirectAtModifier::~BSDirectAtModifier(){
    refCount--;
}
