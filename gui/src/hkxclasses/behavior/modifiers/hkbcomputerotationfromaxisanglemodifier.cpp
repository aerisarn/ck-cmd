#include "hkbcomputerotationfromaxisanglemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbComputeRotationFromAxisAngleModifier::refCount = 0;

const QString hkbComputeRotationFromAxisAngleModifier::classname = "hkbComputeRotationFromAxisAngleModifier";

hkbComputeRotationFromAxisAngleModifier::hkbComputeRotationFromAxisAngleModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      angleDegrees(0)
{
    setType(HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ComputeRotationFromAxisAngleModifier_"+QString::number(refCount);
}

const QString hkbComputeRotationFromAxisAngleModifier::getClassname(){
    return classname;
}

QString hkbComputeRotationFromAxisAngleModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbComputeRotationFromAxisAngleModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "rotationOut"){
            rotationOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "rotationOut");
        }else if (text == "axis"){
            axis = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "axis");
        }else if (text == "angleDegrees"){
            angleDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "angleDegrees");
        }
    }
    index--;
    return true;
}

bool hkbComputeRotationFromAxisAngleModifier::write(HkxXMLWriter *writer){
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
        writedatafield("rotationOut", rotationOut.getValueAsString());
        writedatafield("axis", axis.getValueAsString());
        writedatafield("angleDegrees", QString::number(angleDegrees, char('f'), 6));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

qreal hkbComputeRotationFromAxisAngleModifier::getAngleDegrees() const{
    std::lock_guard <std::mutex> guard(mutex);
    return angleDegrees;
}

void hkbComputeRotationFromAxisAngleModifier::setAngleDegrees(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != angleDegrees) ? angleDegrees = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'angleDegrees' was not set!");
}

hkQuadVariable hkbComputeRotationFromAxisAngleModifier::getAxis() const{
    std::lock_guard <std::mutex> guard(mutex);
    return axis;
}

void hkbComputeRotationFromAxisAngleModifier::setAxis(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != axis) ? axis = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'axis' was not set!");
}

hkQuadVariable hkbComputeRotationFromAxisAngleModifier::getRotationOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rotationOut;
}

void hkbComputeRotationFromAxisAngleModifier::setRotationOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rotationOut) ? rotationOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rotationOut' was not set!");
}

bool hkbComputeRotationFromAxisAngleModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbComputeRotationFromAxisAngleModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbComputeRotationFromAxisAngleModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbComputeRotationFromAxisAngleModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbComputeRotationFromAxisAngleModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbComputeRotationFromAxisAngleModifier::evaluateDataValidity(){
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

hkbComputeRotationFromAxisAngleModifier::~hkbComputeRotationFromAxisAngleModifier(){
    refCount--;
}
