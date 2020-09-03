#include "hkbrotatecharactermodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbRotateCharacterModifier::refCount = 0;

const QString hkbRotateCharacterModifier::classname = "hkbRotateCharacterModifier";

hkbRotateCharacterModifier::hkbRotateCharacterModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      degreesPerSecond(0),
      speedMultiplier(0)
{
    setType(HKB_ROTATE_CHARACTER_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "RotateCharacterModifier_"+QString::number(refCount);
}

const QString hkbRotateCharacterModifier::getClassname(){
    return classname;
}

QString hkbRotateCharacterModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbRotateCharacterModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "degreesPerSecond"){
            degreesPerSecond = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "degreesPerSecond");
        }else if (text == "speedMultiplier"){
            speedMultiplier = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "speedMultiplier");
        }else if (text == "axisOfRotation"){
            axisOfRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "axisOfRotation");
        }
    }
    index--;
    return true;
}

bool hkbRotateCharacterModifier::write(HkxXMLWriter *writer){
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
        writedatafield("degreesPerSecond", QString::number(degreesPerSecond, char('f'), 6));
        writedatafield("speedMultiplier", QString::number(speedMultiplier, char('f'), 6));
        writedatafield("axisOfRotation", axisOfRotation.getValueAsString());
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

hkQuadVariable hkbRotateCharacterModifier::getAxisOfRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return axisOfRotation;
}

void hkbRotateCharacterModifier::setAxisOfRotation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != axisOfRotation) ? axisOfRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'axisOfRotation' was not set!");
}

qreal hkbRotateCharacterModifier::getSpeedMultiplier() const{
    std::lock_guard <std::mutex> guard(mutex);
    return speedMultiplier;
}

void hkbRotateCharacterModifier::setSpeedMultiplier(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != speedMultiplier) ? speedMultiplier = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'speedMultiplier' was not set!");
}

qreal hkbRotateCharacterModifier::getDegreesPerSecond() const{
    std::lock_guard <std::mutex> guard(mutex);
    return degreesPerSecond;
}

void hkbRotateCharacterModifier::setDegreesPerSecond(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != degreesPerSecond) ? degreesPerSecond = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'degreesPerSecond' was not set!");
}

bool hkbRotateCharacterModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbRotateCharacterModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbRotateCharacterModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbRotateCharacterModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbRotateCharacterModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbRotateCharacterModifier::evaluateDataValidity(){
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

hkbRotateCharacterModifier::~hkbRotateCharacterModifier(){
    refCount--;
}
