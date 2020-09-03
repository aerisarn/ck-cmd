#include "bscomputeaddboneanimmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSComputeAddBoneAnimModifier::refCount = 0;

const QString BSComputeAddBoneAnimModifier::classname = "BSComputeAddBoneAnimModifier";

BSComputeAddBoneAnimModifier::BSComputeAddBoneAnimModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      boneIndex(-1)
{
    setType(BS_COMPUTE_ADD_BONE_ANIM_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ComputeAddBoneAnimModifier_"+QString::number(refCount);
}

const QString BSComputeAddBoneAnimModifier::getClassname(){
    return classname;
}

QString BSComputeAddBoneAnimModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSComputeAddBoneAnimModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "boneIndex"){
            boneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "boneIndex");
        }else if (text == "translationLSOut"){
            translationLSOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "translationLSOut");
        }else if (text == "rotationLSOut"){
            rotationLSOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "rotationLSOut");
        }else if (text == "scaleLSOut"){
            scaleLSOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "scaleLSOut");
        }
    }
    index--;
    return true;
}

bool BSComputeAddBoneAnimModifier::write(HkxXMLWriter *writer){
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
        writedatafield("boneIndex", QString::number(boneIndex), false);
        writedatafield("translationLSOut", translationLSOut.getValueAsString(), false);
        writedatafield("rotationLSOut", rotationLSOut.getValueAsString(), false);
        writedatafield("scaleLSOut", scaleLSOut.getValueAsString(), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

hkQuadVariable BSComputeAddBoneAnimModifier::getScaleLSOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return scaleLSOut;
}

void BSComputeAddBoneAnimModifier::setScaleLSOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != scaleLSOut) ? scaleLSOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'scaleLSOut' was not set!");
}

hkQuadVariable BSComputeAddBoneAnimModifier::getRotationLSOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rotationLSOut;
}

void BSComputeAddBoneAnimModifier::setRotationLSOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rotationLSOut) ? rotationLSOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rotationLSOut' was not set!");
}

hkQuadVariable BSComputeAddBoneAnimModifier::getTranslationLSOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return translationLSOut;
}

void BSComputeAddBoneAnimModifier::setTranslationLSOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != translationLSOut) ? translationLSOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'translationLSOut' was not set!");
}

int BSComputeAddBoneAnimModifier::getBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return boneIndex;
}

void BSComputeAddBoneAnimModifier::setBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != boneIndex && boneIndex < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? boneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'boneIndex' was not set!");
}

bool BSComputeAddBoneAnimModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSComputeAddBoneAnimModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSComputeAddBoneAnimModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSComputeAddBoneAnimModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void BSComputeAddBoneAnimModifier::unlink(){
    HkDynamicObject::unlink();
}

QString BSComputeAddBoneAnimModifier::evaluateDataValidity(){
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
    if (boneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": boneIndex out of range! Setting to last bone index!");
        boneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    setDataValidity(isvalid);
    return errors;
}

BSComputeAddBoneAnimModifier::~BSComputeAddBoneAnimModifier(){
    refCount--;
}
