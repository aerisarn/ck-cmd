#include "hkbtwistmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbTwistModifier::refCount = 0;

const QString hkbTwistModifier::classname = "hkbTwistModifier";

const QStringList hkbTwistModifier::SetAngleMethod = {"LINEAR", "RAMPED"};
const QStringList hkbTwistModifier::RotationAxisCoordinates = {"ROTATION_AXIS_IN_MODEL_COORDINATES", "ROTATION_AXIS_IN_LOCAL_COORDINATES"};

hkbTwistModifier::hkbTwistModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(1),
      enable(true),
      twistAngle(0),
      startBoneIndex(-1),
      endBoneIndex(-1),
      setAngleMethod(SetAngleMethod.first()),
      rotationAxisCoordinates(RotationAxisCoordinates.first()),
      isAdditive(true)
{
    setType(HKB_TWIST_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "TwistModifier_"+QString::number(refCount);
}

const QString hkbTwistModifier::getClassname(){
    return classname;
}

QString hkbTwistModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbTwistModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "axisOfRotation"){
            axisOfRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "axisOfRotation");
        }else if (text == "twistAngle"){
            twistAngle = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "twistAngle");
        }else if (text == "startBoneIndex"){
            startBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "startBoneIndex");
        }else if (text == "endBoneIndex"){
            endBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "endBoneIndex");
        }else if (text == "setAngleMethod"){
            setAngleMethod = reader.getElementValueAt(index);
            checkvalue(SetAngleMethod.contains(setAngleMethod), "setAngleMethod");
        }else if (text == "rotationAxisCoordinates"){
            rotationAxisCoordinates = reader.getElementValueAt(index);
            checkvalue(RotationAxisCoordinates.contains(rotationAxisCoordinates), "rotationAxisCoordinates");
        }else if (text == "isAdditive"){
            isAdditive = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "isAdditive");
        }
    }
    index--;
    return true;
}

bool hkbTwistModifier::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writedatafield("variableBindingSet", refString);
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        writedatafield("axisOfRotation", axisOfRotation.getValueAsString());
        writedatafield("twistAngle", QString::number(twistAngle, char('f'), 6));
        writedatafield("startBoneIndex", QString::number(startBoneIndex));
        writedatafield("endBoneIndex", QString::number(endBoneIndex));
        writedatafield("setAngleMethod", setAngleMethod);
        writedatafield("rotationAxisCoordinates", rotationAxisCoordinates);
        writedatafield("isAdditive", getBoolAsString(isAdditive));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool hkbTwistModifier::getIsAdditive() const{
    std::lock_guard <std::mutex> guard(mutex);
    return isAdditive;
}

void hkbTwistModifier::setIsAdditive(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != isAdditive) ? isAdditive = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'isAdditive' was not set!");
}

QString hkbTwistModifier::getRotationAxisCoordinates() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rotationAxisCoordinates;
}

void hkbTwistModifier::setRotationAxisCoordinates(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < RotationAxisCoordinates.size() && rotationAxisCoordinates != RotationAxisCoordinates.at(index)) ? rotationAxisCoordinates = RotationAxisCoordinates.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rotationAxisCoordinates' was not set!");
}

QString hkbTwistModifier::getSetAngleMethod() const{
    std::lock_guard <std::mutex> guard(mutex);
    return setAngleMethod;
}

void hkbTwistModifier::setSetAngleMethod(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < SetAngleMethod.size() && setAngleMethod != SetAngleMethod.at(index)) ? setAngleMethod = SetAngleMethod.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'setAngleMethod' was not set!");
}

int hkbTwistModifier::getEndBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return endBoneIndex;
}

void hkbTwistModifier::setEndBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != endBoneIndex && endBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? endBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'endBoneIndex' was not set!");
}

int hkbTwistModifier::getStartBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startBoneIndex;
}

void hkbTwistModifier::setStartBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != startBoneIndex && startBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? startBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startBoneIndex' was not set!");
}

qreal hkbTwistModifier::getTwistAngle() const{
    std::lock_guard <std::mutex> guard(mutex);
    return twistAngle;
}

void hkbTwistModifier::setTwistAngle(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != twistAngle) ? twistAngle = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'twistAngle' was not set!");
}

hkQuadVariable hkbTwistModifier::getAxisOfRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return axisOfRotation;
}

void hkbTwistModifier::setAxisOfRotation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != axisOfRotation) ? axisOfRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'axisOfRotation' was not set!");
}

bool hkbTwistModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbTwistModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbTwistModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbTwistModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbTwistModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbTwistModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkbones = [&](int & boneindex, const QString & fieldname){
        if (boneindex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+fieldname+" bone id out of range! Setting to max index in range!");
            boneindex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true) - 1;
        }
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    checkbones(startBoneIndex, "startBoneIndex");
    checkbones(endBoneIndex, "endBoneIndex");
    if (!SetAngleMethod.contains(setAngleMethod)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid setAngleMethod! Setting default value!");
        setAngleMethod = SetAngleMethod.first();
    }
    if (!RotationAxisCoordinates.contains(rotationAxisCoordinates)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid rotationAxisCoordinates! Setting default value!");
        rotationAxisCoordinates = RotationAxisCoordinates.first();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbTwistModifier::~hkbTwistModifier(){
    refCount--;
}
