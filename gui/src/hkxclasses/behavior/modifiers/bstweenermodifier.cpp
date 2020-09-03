#include "bstweenermodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSTweenerModifier::refCount = 0;

const QString BSTweenerModifier::classname = "BSTweenerModifier";

BSTweenerModifier::BSTweenerModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      tweenPosition(true),
      tweenRotation(true),
      useTweenDuration(true),
      tweenDuration(0)
{
    setType(BS_TWEENER_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "TweenerModifier_"+QString::number(refCount);
}

const QString BSTweenerModifier::getClassname(){
    return classname;
}

QString BSTweenerModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSTweenerModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "tweenPosition"){
            tweenPosition = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "tweenPosition");
        }else if (text == "tweenRotation"){
            tweenRotation = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "tweenRotation");
        }else if (text == "useTweenDuration"){
            useTweenDuration = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "useTweenDuration");
        }else if (text == "tweenDuration"){
            tweenDuration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "tweenDuration");
        }else if (text == "targetPosition"){
            targetPosition = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "targetPosition");
        }else if (text == "targetRotation"){
            targetRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "targetRotation");
        }
    }
    index--;
    return true;
}

bool BSTweenerModifier::write(HkxXMLWriter *writer){
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
        writedatafield("tweenPosition", getBoolAsString(tweenPosition));
        writedatafield("tweenRotation", getBoolAsString(tweenRotation));
        writedatafield("useTweenDuration", getBoolAsString(useTweenDuration));
        writedatafield("tweenDuration", QString::number(tweenDuration, char('f'), 6));
        writedatafield("targetPosition", targetPosition.getValueAsString());
        writedatafield("targetRotation", targetRotation.getValueAsString());
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

hkQuadVariable BSTweenerModifier::getTargetRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return targetRotation;
}

void BSTweenerModifier::setTargetRotation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != targetRotation) ? targetRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'targetRotation' was not set!");
}

hkQuadVariable BSTweenerModifier::getTargetPosition() const{
    std::lock_guard <std::mutex> guard(mutex);
    return targetPosition;
}

void BSTweenerModifier::setTargetPosition(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != targetPosition) ? targetPosition = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'targetPosition' was not set!");
}

qreal BSTweenerModifier::getTweenDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return tweenDuration;
}

void BSTweenerModifier::setTweenDuration(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != tweenDuration) ? tweenDuration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'tweenDuration' was not set!");
}

bool BSTweenerModifier::getUseTweenDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return useTweenDuration;
}

void BSTweenerModifier::setUseTweenDuration(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != useTweenDuration) ? useTweenDuration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'useTweenDuration' was not set!");
}

bool BSTweenerModifier::getTweenRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return tweenRotation;
}

void BSTweenerModifier::setTweenRotation(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != tweenRotation) ? tweenRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'tweenRotation' was not set!");
}

bool BSTweenerModifier::getTweenPosition() const{
    std::lock_guard <std::mutex> guard(mutex);
    return tweenPosition;
}

void BSTweenerModifier::setTweenPosition(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != tweenPosition) ? tweenPosition = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'tweenPosition' was not set!");
}

bool BSTweenerModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSTweenerModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSTweenerModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSTweenerModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void BSTweenerModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString BSTweenerModifier::evaluateDataValidity(){
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

BSTweenerModifier::~BSTweenerModifier(){
    refCount--;
}
