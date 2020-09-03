#include "hkbcombinetransformsmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbCombineTransformsModifier::refCount = 0;

const QString hkbCombineTransformsModifier::classname = "hkbCombineTransformsModifier";

hkbCombineTransformsModifier::hkbCombineTransformsModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      invertLeftTransform(false),
      invertRightTransform(false),
      invertResult(false)
{
    setType(HKB_COMBINE_TRANSFORMS_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "CombineTransformsModifier_"+QString::number(refCount);
}

const QString hkbCombineTransformsModifier::getClassname(){
    return classname;
}

QString hkbCombineTransformsModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbCombineTransformsModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "translationOut"){
            translationOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "translationOut");
        }else if (text == "rotationOut"){
            rotationOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "rotationOut");
        }else if (text == "leftTranslation"){
            leftTranslation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "leftTranslation");
        }else if (text == "leftRotation"){
            leftRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "leftRotation");
        }else if (text == "rightTranslation"){
            rightTranslation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "rightTranslation");
        }else if (text == "rightRotation"){
            rightRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "rightRotation");
        }else if (text == "invertLeftTransform"){
            invertLeftTransform = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "invertLeftTransform");
        }else if (text == "invertRightTransform"){
            invertRightTransform = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "invertRightTransform");
        }else if (text == "invertResult"){
            invertResult = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "invertResult");
        }
    }
    index--;
    return true;
}

bool hkbCombineTransformsModifier::write(HkxXMLWriter *writer){
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
        writedatafield("translationOut", translationOut.getValueAsString());
        writedatafield("rotationOut", rotationOut.getValueAsString());
        writedatafield("leftTranslation", leftTranslation.getValueAsString());
        writedatafield("leftRotation", leftRotation.getValueAsString());
        writedatafield("rightTranslation", rightTranslation.getValueAsString());
        writedatafield("rightRotation", rightRotation.getValueAsString());
        writedatafield("invertLeftTransform", getBoolAsString(invertLeftTransform));
        writedatafield("invertRightTransform", getBoolAsString(invertRightTransform));
        writedatafield("invertResult", getBoolAsString(invertResult));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool hkbCombineTransformsModifier::getInvertResult() const{
    std::lock_guard <std::mutex> guard(mutex);
    return invertResult;
}

void hkbCombineTransformsModifier::setInvertResult(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != invertResult) ? invertResult = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'invertResult' was not set!");
}

bool hkbCombineTransformsModifier::getInvertRightTransform() const{
    std::lock_guard <std::mutex> guard(mutex);
    return invertRightTransform;
}

void hkbCombineTransformsModifier::setInvertRightTransform(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != invertRightTransform) ? invertRightTransform = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'invertRightTransform' was not set!");
}

bool hkbCombineTransformsModifier::getInvertLeftTransform() const{
    std::lock_guard <std::mutex> guard(mutex);
    return invertLeftTransform;
}

void hkbCombineTransformsModifier::setInvertLeftTransform(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != invertLeftTransform) ? invertLeftTransform = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'invertLeftTransform' was not set!");
}

hkQuadVariable hkbCombineTransformsModifier::getRightRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rightRotation;
}

void hkbCombineTransformsModifier::setRightRotation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rightRotation) ? rightRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rightRotation' was not set!");
}

hkQuadVariable hkbCombineTransformsModifier::getRightTranslation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rightTranslation;
}

void hkbCombineTransformsModifier::setRightTranslation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rightTranslation) ? rightTranslation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rightTranslation' was not set!");
}

hkQuadVariable hkbCombineTransformsModifier::getLeftRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return leftRotation;
}

void hkbCombineTransformsModifier::setLeftRotation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != leftRotation) ? leftRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'leftRotation' was not set!");
}

hkQuadVariable hkbCombineTransformsModifier::getLeftTranslation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return leftTranslation;
}

void hkbCombineTransformsModifier::setLeftTranslation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != leftTranslation) ? leftTranslation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'leftTranslation' was not set!");
}

hkQuadVariable hkbCombineTransformsModifier::getRotationOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rotationOut;
}

void hkbCombineTransformsModifier::setRotationOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rotationOut) ? rotationOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rotationOut' was not set!");
}

hkQuadVariable hkbCombineTransformsModifier::getTranslationOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return translationOut;
}

void hkbCombineTransformsModifier::setTranslationOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != translationOut) ? translationOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'translationOut' was not set!");
}

bool hkbCombineTransformsModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbCombineTransformsModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbCombineTransformsModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbCombineTransformsModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbCombineTransformsModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbCombineTransformsModifier::evaluateDataValidity(){
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
    setDataValidity(isvalid);
    return errors;
}

hkbCombineTransformsModifier::~hkbCombineTransformsModifier(){
    refCount--;
}
