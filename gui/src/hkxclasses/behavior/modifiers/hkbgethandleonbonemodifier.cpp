#include "hkbgethandleonbonemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbGetHandleOnBoneModifier::refCount = 0;

const QString hkbGetHandleOnBoneModifier::classname = "hkbGetHandleOnBoneModifier";

hkbGetHandleOnBoneModifier::hkbGetHandleOnBoneModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      ragdollBoneIndex(-1),
      animationBoneIndex(-1)
{
    setType(HKB_GET_HANDLE_ON_BONE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "GetHandleOnBoneModifier_"+QString::number(refCount);
}

const QString hkbGetHandleOnBoneModifier::getClassname(){
    return classname;
}

QString hkbGetHandleOnBoneModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbGetHandleOnBoneModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "handleOut"){
            checkvalue(handleOut.readShdPtrReference(index, reader), "handleOut");
        }else if (text == "localFrameName"){
            localFrameName = reader.getElementValueAt(index);
            checkvalue((name != ""), "enable");
        }else if (text == "ragdollBoneIndex"){
            ragdollBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "ragdollBoneIndex");
        }else if (text == "animationBoneIndex"){
            animationBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "animationBoneIndex");
        }
    }
    index--;
    return true;
}

bool hkbGetHandleOnBoneModifier::write(HkxXMLWriter *writer){
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
        writeref(handleOut, "handleOut");
        writedatafield("localFrameName", localFrameName, true);
        writedatafield("ragdollBoneIndex", QString::number(ragdollBoneIndex), false);
        writedatafield("animationBoneIndex", QString::number(animationBoneIndex), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
    }
    return true;
}

int hkbGetHandleOnBoneModifier::getAnimationBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return animationBoneIndex;
}

void hkbGetHandleOnBoneModifier::setAnimationBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != animationBoneIndex && animationBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? animationBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'animationBoneIndex' was not set!");
}

int hkbGetHandleOnBoneModifier::getRagdollBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return ragdollBoneIndex;
}

void hkbGetHandleOnBoneModifier::setRagdollBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != ragdollBoneIndex && ragdollBoneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)) ? ragdollBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'ragdollBoneIndex' was not set!");
}

QString hkbGetHandleOnBoneModifier::getLocalFrameName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return localFrameName;
}

void hkbGetHandleOnBoneModifier::setLocalFrameName(int index){
    std::lock_guard <std::mutex> guard(mutex);
    auto localframes = static_cast<BehaviorFile *>(getParentFile())->getLocalFrameNames();
    (index >= 0 && index < localframes.size() && localframes.at(index) != localFrameName) ? localFrameName = localframes.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'localFrameName' was not set!");
}

bool hkbGetHandleOnBoneModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbGetHandleOnBoneModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbGetHandleOnBoneModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbGetHandleOnBoneModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbGetHandleOnBoneModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbGetHandleOnBoneModifier::evaluateDataValidity(){
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
    if (localFrameName == ""){  //TO DO: check local frames...
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid localFrameName!");
    }
    if (animationBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": animationBoneIndex out of range! Setting to last bone index!");
        animationBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (ragdollBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": ragdollBoneIndex out of range! Setting to last bone index!");
        ragdollBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true) - 1;
    }
    if (animationBoneIndex > -1 && ragdollBoneIndex > -1){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": animationBoneIndex and ragdollBoneIndex are both in use at the same time! This will crash the game! Setting ragdollBoneIndex to -1!");
        ragdollBoneIndex = -1;
    }
    if (animationBoneIndex < 0 && ragdollBoneIndex < 0 ){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Neither animationBoneIndex and ragdollBoneIndex are in use! Setting animationBoneIndex to 0!");
        animationBoneIndex = 0;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbGetHandleOnBoneModifier::~hkbGetHandleOnBoneModifier(){
    refCount--;
}
