#include "hkbcomputedirectionmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbComputeDirectionModifier::refCount = 0;

const QString hkbComputeDirectionModifier::classname = "hkbComputeDirectionModifier";

hkbComputeDirectionModifier::hkbComputeDirectionModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      groundAngleOut(0),
      upAngleOut(0),
      verticalOffset(0),
      reverseGroundAngle(false),
      reverseUpAngle(false),
      projectPoint(false),
      normalizePoint(false),
      computeOnlyOnce(false),
      computedOutput(false)
{
    setType(HKB_COMPUTE_DIRECTION_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ComputeDirectionModifier_"+QString::number(refCount);
}

const QString hkbComputeDirectionModifier::getClassname(){
    return classname;
}

QString hkbComputeDirectionModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbComputeDirectionModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "pointIn"){
            pointIn = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "pointIn");
        }else if (text == "pointOut"){
            pointOut = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "pointOut");
        }else if (text == "groundAngleOut"){
            groundAngleOut = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "groundAngleOut");
        }else if (text == "upAngleOut"){
            upAngleOut = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "upAngleOut");
        }else if (text == "verticalOffset"){
            verticalOffset = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "verticalOffset");
        }else if (text == "reverseGroundAngle"){
            reverseGroundAngle = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "reverseGroundAngle");
        }else if (text == "reverseUpAngle"){
            reverseUpAngle = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "reverseUpAngle");
        }else if (text == "projectPoint"){
            projectPoint = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "projectPoint");
        }else if (text == "normalizePoint"){
            normalizePoint = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "normalizePoint");
        }else if (text == "computeOnlyOnce"){
            computeOnlyOnce = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "computeOnlyOnce");
        }else if (text == "computedOutput"){
            computedOutput = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "computedOutput");
        }
    }
    index--;
    return true;
}

bool hkbComputeDirectionModifier::write(HkxXMLWriter *writer){
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
        writedatafield("pointIn", pointIn.getValueAsString());
        writedatafield("pointOut", pointOut.getValueAsString());
        writedatafield("groundAngleOut", QString::number(groundAngleOut, char('f'), 6));
        writedatafield("upAngleOut", QString::number(upAngleOut, char('f'), 6));
        writedatafield("verticalOffset", QString::number(verticalOffset, char('f'), 6));
        writedatafield("reverseGroundAngle", getBoolAsString(reverseGroundAngle));
        writedatafield("reverseUpAngle", getBoolAsString(reverseUpAngle));
        writedatafield("projectPoint", getBoolAsString(projectPoint));
        writedatafield("normalizePoint", getBoolAsString(normalizePoint));
        writedatafield("computeOnlyOnce", getBoolAsString(computeOnlyOnce));
        writedatafield("computedOutput", getBoolAsString(computedOutput));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool hkbComputeDirectionModifier::getComputedOutput() const{
    std::lock_guard <std::mutex> guard(mutex);
    return computedOutput;
}

void hkbComputeDirectionModifier::setComputedOutput(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != computedOutput) ? computedOutput = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'computedOutput' was not set!");
}

bool hkbComputeDirectionModifier::getComputeOnlyOnce() const{
    std::lock_guard <std::mutex> guard(mutex);
    return computeOnlyOnce;
}

void hkbComputeDirectionModifier::setComputeOnlyOnce(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != computeOnlyOnce) ? computeOnlyOnce = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'computeOnlyOnce' was not set!");
}

bool hkbComputeDirectionModifier::getNormalizePoint() const{
    std::lock_guard <std::mutex> guard(mutex);
    return normalizePoint;
}

void hkbComputeDirectionModifier::setNormalizePoint(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != normalizePoint) ? normalizePoint = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'normalizePoint' was not set!");
}

bool hkbComputeDirectionModifier::getProjectPoint() const{
    std::lock_guard <std::mutex> guard(mutex);
    return projectPoint;
}

void hkbComputeDirectionModifier::setProjectPoint(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != projectPoint) ? projectPoint = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'projectPoint' was not set!");
}

bool hkbComputeDirectionModifier::getReverseUpAngle() const{
    std::lock_guard <std::mutex> guard(mutex);
    return reverseUpAngle;
}

void hkbComputeDirectionModifier::setReverseUpAngle(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != reverseUpAngle) ? reverseUpAngle = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'reverseUpAngle' was not set!");
}

bool hkbComputeDirectionModifier::getReverseGroundAngle() const{
    std::lock_guard <std::mutex> guard(mutex);
    return reverseGroundAngle;
}

void hkbComputeDirectionModifier::setReverseGroundAngle(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != reverseGroundAngle) ? reverseGroundAngle = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'reverseGroundAngle' was not set!");
}

qreal hkbComputeDirectionModifier::getVerticalOffset() const{
    std::lock_guard <std::mutex> guard(mutex);
    return verticalOffset;
}

void hkbComputeDirectionModifier::setVerticalOffset(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != verticalOffset) ? verticalOffset = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'verticalOffset' was not set!");
}

qreal hkbComputeDirectionModifier::getUpAngleOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return upAngleOut;
}

void hkbComputeDirectionModifier::setUpAngleOut(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != upAngleOut) ? upAngleOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'upAngleOut' was not set!");
}

qreal hkbComputeDirectionModifier::getGroundAngleOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return groundAngleOut;
}

void hkbComputeDirectionModifier::setGroundAngleOut(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != groundAngleOut) ? groundAngleOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'groundAngleOut' was not set!");
}

hkQuadVariable hkbComputeDirectionModifier::getPointOut() const{
    std::lock_guard <std::mutex> guard(mutex);
    return pointOut;
}

void hkbComputeDirectionModifier::setPointOut(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != pointOut) ? pointOut = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'pointOut' was not set!");
}

hkQuadVariable hkbComputeDirectionModifier::getPointIn() const{
    std::lock_guard <std::mutex> guard(mutex);
    return pointIn;
}

void hkbComputeDirectionModifier::setPointIn(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != pointIn) ? pointIn = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'pointIn' was not set!");
}

bool hkbComputeDirectionModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbComputeDirectionModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbComputeDirectionModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbComputeDirectionModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbComputeDirectionModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbComputeDirectionModifier::evaluateDataValidity(){
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

hkbComputeDirectionModifier::~hkbComputeDirectionModifier(){
    refCount--;
}
