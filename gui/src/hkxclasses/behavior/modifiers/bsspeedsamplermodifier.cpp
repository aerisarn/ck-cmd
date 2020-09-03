#include "bsspeedsamplermodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSSpeedSamplerModifier::refCount = 0;

const QString BSSpeedSamplerModifier::classname = "BSSpeedSamplerModifier";

BSSpeedSamplerModifier::BSSpeedSamplerModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      state(-1),
      direction(0),
      goalSpeed(0),
      speedOut(0)
{
    setType(BS_SPEED_SAMPLER_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "SpeedSamplerModifier_"+QString::number(refCount);
}

const QString BSSpeedSamplerModifier::getClassname(){
    return classname;
}

QString BSSpeedSamplerModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSSpeedSamplerModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "state"){
            state = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "state");
        }else if (text == "direction"){
            direction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "direction");
        }else if (text == "goalSpeed"){
            goalSpeed = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "goalSpeed");
        }else if (text == "speedOut"){
            speedOut = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "speedOut");
        }
    }
    index--;
    return true;
}

bool BSSpeedSamplerModifier::write(HkxXMLWriter *writer){
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
        writedatafield("state", QString::number(state));
        writedatafield("direction", QString::number(direction, char('f'), 6));
        writedatafield("goalSpeed", QString::number(goalSpeed, char('f'), 6));
        writedatafield("speedOut", QString::number(speedOut, char('f'), 6));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

qreal BSSpeedSamplerModifier::getSpeedOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return speedOut;
}

void BSSpeedSamplerModifier::setSpeedOut(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != speedOut) ? speedOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'speedOut' was not set!");
}

qreal BSSpeedSamplerModifier::getGoalSpeed() const{
    std::lock_guard <std::mutex> guard(mutex);
    return goalSpeed;
}

void BSSpeedSamplerModifier::setGoalSpeed(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != goalSpeed) ? goalSpeed = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'goalSpeed' was not set!");
}

qreal BSSpeedSamplerModifier::getDirection() const{
    std::lock_guard <std::mutex> guard(mutex);
    return direction;
}

void BSSpeedSamplerModifier::setDirection(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != direction) ? direction = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'direction' was not set!");
}

int BSSpeedSamplerModifier::getState() const{
    std::lock_guard <std::mutex> guard(mutex);
    return state;
}

void BSSpeedSamplerModifier::setState(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != state) ? state = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'state' was not set!");
}

bool BSSpeedSamplerModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSSpeedSamplerModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSSpeedSamplerModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSSpeedSamplerModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void BSSpeedSamplerModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString BSSpeedSamplerModifier::evaluateDataValidity(){
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

BSSpeedSamplerModifier::~BSSpeedSamplerModifier(){
    refCount--;
}
