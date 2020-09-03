#include "bsisactivemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSIsActiveModifier::refCount = 0;

const QString BSIsActiveModifier::classname = "BSIsActiveModifier";

BSIsActiveModifier::BSIsActiveModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(2),
      enable(true),
      bIsActive0(false),
      bInvertActive0(false),
      bIsActive1(false),
      bInvertActive1(false),
      bIsActive2(false),
      bInvertActive2(false),
      bIsActive3(false),
      bInvertActive3(false),
      bIsActive4(false),
      bInvertActive4(false)
{
    setType(BS_IS_ACTIVE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "IsActiveModifier_"+QString::number(refCount);
}

const QString BSIsActiveModifier::getClassname(){
    return classname;
}

QString BSIsActiveModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSIsActiveModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "bIsActive0"){
            bIsActive0 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bIsActive0");
        }else if (text == "bInvertActive0"){
            bInvertActive0 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bInvertActive0");
        }else if (text == "bIsActive1"){
            bIsActive1 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bIsActive1");
        }else if (text == "bInvertActive1"){
            bInvertActive1 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bInvertActive1");
        }else if (text == "bIsActive2"){
            bIsActive2 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bIsActive2");
        }else if (text == "bInvertActive2"){
            bInvertActive2 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bInvertActive2");
        }else if (text == "bIsActive3"){
            bIsActive3 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bIsActive3");
        }else if (text == "bInvertActive3"){
            bInvertActive3 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bInvertActive3");
        }else if (text == "bIsActive4"){
            bIsActive4 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bIsActive4");
        }else if (text == "bInvertActive4"){
            bInvertActive4 = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "bInvertActive4");
        }else{
            //LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nUnknown field '"+text+"' found!\nObject Reference: "+ref);
        }
    }
    index--;
    return true;
}

bool BSIsActiveModifier::write(HkxXMLWriter *writer){
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
        writedatafield("bIsActive0", getBoolAsString(bIsActive0), false);
        writedatafield("bInvertActive0", getBoolAsString(bInvertActive0), false);
        writedatafield("bIsActive1", getBoolAsString(bIsActive1), false);
        writedatafield("bInvertActive1", getBoolAsString(bInvertActive1), false);
        writedatafield("bIsActive2", getBoolAsString(bIsActive2), false);
        writedatafield("bInvertActive2", getBoolAsString(bInvertActive2), false);
        writedatafield("bIsActive3", getBoolAsString(bIsActive3), false);
        writedatafield("bInvertActive3", getBoolAsString(bInvertActive3), false);
        writedatafield("bIsActive4", getBoolAsString(bIsActive4), false);
        writedatafield("bInvertActive4", getBoolAsString(bInvertActive4), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool BSIsActiveModifier::getBInvertActive4() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bInvertActive4;
}

void BSIsActiveModifier::setBInvertActive4(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bInvertActive4) ? bInvertActive4 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bInvertActive4' was not set!");
}

bool BSIsActiveModifier::getBIsActive4() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bIsActive4;
}

void BSIsActiveModifier::setBIsActive4(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bIsActive4) ? bIsActive4 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bIsActive4' was not set!");
}

bool BSIsActiveModifier::getBInvertActive3() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bInvertActive3;
}

void BSIsActiveModifier::setBInvertActive3(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bInvertActive3) ? bInvertActive3 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bInvertActive3' was not set!");
}

bool BSIsActiveModifier::getBIsActive3() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bIsActive3;
}

void BSIsActiveModifier::setBIsActive3(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bIsActive3) ? bIsActive3 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bIsActive3' was not set!");
}

bool BSIsActiveModifier::getBInvertActive2() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bInvertActive2;
}

void BSIsActiveModifier::setBInvertActive2(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bInvertActive2) ? bInvertActive2 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bInvertActive2' was not set!");
}

bool BSIsActiveModifier::getBIsActive2() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bIsActive2;
}

void BSIsActiveModifier::setBIsActive2(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bIsActive2) ? bIsActive2 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bIsActive2' was not set!");
}

bool BSIsActiveModifier::getBInvertActive1() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bInvertActive1;
}

void BSIsActiveModifier::setBInvertActive1(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bInvertActive1) ? bInvertActive1 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bInvertActive1' was not set!");
}

bool BSIsActiveModifier::getBIsActive1() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bIsActive1;
}

void BSIsActiveModifier::setBIsActive1(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bIsActive1) ? bIsActive1 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bIsActive1' was not set!");
}

bool BSIsActiveModifier::getBInvertActive0() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bInvertActive0;
}

void BSIsActiveModifier::setBInvertActive0(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bInvertActive0) ? bInvertActive0 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bInvertActive0' was not set!");
}

bool BSIsActiveModifier::getBIsActive0() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bIsActive0;
}

void BSIsActiveModifier::setBIsActive0(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != bIsActive0) ? bIsActive0 = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'bIsActive0' was not set!");
}

bool BSIsActiveModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSIsActiveModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSIsActiveModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSIsActiveModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void BSIsActiveModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString BSIsActiveModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto setinvalid = [&](const QString & message){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": "+message+"!");
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    (temp != "") ? errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!\n"): NULL;
    (name == "") ? setinvalid("Invalid name") : NULL;
    setDataValidity(isvalid);
    return errors;
}

BSIsActiveModifier::~BSIsActiveModifier(){
    refCount--;
}
