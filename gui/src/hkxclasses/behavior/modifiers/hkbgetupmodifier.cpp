#include "hkbgetupmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbGetUpModifier::refCount = 0;

const QString hkbGetUpModifier::classname = "hkbGetUpModifier";

hkbGetUpModifier::hkbGetUpModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      duration(0),
      alignWithGroundDuration(0),
      rootBoneIndex(-1),
      otherBoneIndex(-1),
      anotherBoneIndex(-1)
{
    setType(HKB_GET_UP_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "GetUpModifier_"+QString::number(refCount);
}

const QString hkbGetUpModifier::getClassname(){
    return classname;
}

QString hkbGetUpModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbGetUpModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "groundNormal"){
            groundNormal = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "groundNormal");
        }else if (text == "duration"){
            duration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "duration");
        }else if (text == "alignWithGroundDuration"){
            alignWithGroundDuration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "alignWithGroundDuration");
        }else if (text == "rootBoneIndex"){
            rootBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "rootBoneIndex");
        }else if (text == "otherBoneIndex"){
            otherBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "otherBoneIndex");
        }else if (text == "anotherBoneIndex"){
            anotherBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "anotherBoneIndex");
        }
    }
    index--;
    return true;
}

bool hkbGetUpModifier::write(HkxXMLWriter *writer){
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
        writedatafield("groundNormal", groundNormal.getValueAsString());
        writedatafield("duration", QString::number(duration, char('f'), 6));
        writedatafield("alignWithGroundDuration", QString::number(alignWithGroundDuration, char('f'), 6));
        writedatafield("rootBoneIndex", QString::number(rootBoneIndex));
        writedatafield("otherBoneIndex", QString::number(otherBoneIndex));
        writedatafield("anotherBoneIndex", QString::number(anotherBoneIndex));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

int hkbGetUpModifier::getAnotherBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return anotherBoneIndex;
}

void hkbGetUpModifier::setAnotherBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != anotherBoneIndex && anotherBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? anotherBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'anotherBoneIndex' was not set!");
}

int hkbGetUpModifier::getOtherBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return otherBoneIndex;
}

void hkbGetUpModifier::setOtherBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != otherBoneIndex && otherBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? otherBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'otherBoneIndex' was not set!");
}

int hkbGetUpModifier::getRootBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rootBoneIndex;
}

void hkbGetUpModifier::setRootBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rootBoneIndex && rootBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? rootBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rootBoneIndex' was not set!");
}

qreal hkbGetUpModifier::getAlignWithGroundDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return alignWithGroundDuration;
}

void hkbGetUpModifier::setAlignWithGroundDuration(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != alignWithGroundDuration) ? alignWithGroundDuration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'alignWithGroundDuration' was not set!");
}

qreal hkbGetUpModifier::getDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return duration;
}

void hkbGetUpModifier::setDuration(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != duration) ? duration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'duration' was not set!");
}

hkQuadVariable hkbGetUpModifier::getGroundNormal() const{
    std::lock_guard <std::mutex> guard(mutex);
    return groundNormal;
}

void hkbGetUpModifier::setGroundNormal(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != groundNormal) ? groundNormal = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'groundNormal' was not set!");
}

bool hkbGetUpModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbGetUpModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbGetUpModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbGetUpModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbGetUpModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbGetUpModifier::evaluateDataValidity(){
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
    if (rootBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": rootBoneIndex out of range! Setting to last bone index!");
        rootBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (otherBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": otherBoneIndex out of range! Setting to last bone index!");
        otherBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (anotherBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": anotherBoneIndex out of range! Setting to last bone index!");
        anotherBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbGetUpModifier::~hkbGetUpModifier(){
    refCount--;
}
