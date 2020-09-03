#include "hkbdampingmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbDampingModifier::refCount = 0;

const QString hkbDampingModifier::classname = "hkbDampingModifier";

hkbDampingModifier::hkbDampingModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(1),
      enable(true),
      kP(0),
      kI(0),
      kD(0),
      enableScalarDamping(true),
      enableVectorDamping(false),
      rawValue(0),
      dampedValue(0),
      errorSum(0),
      previousError(0)
{
    setType(HKB_DAMPING_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "DampingModifier_"+QString::number(refCount);
}

const QString hkbDampingModifier::getClassname(){
    return classname;
}

QString hkbDampingModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbDampingModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "kP"){
            kP = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "kP");
        }else if (text == "kI"){
            kI = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "kI");
        }else if (text == "kD"){
            kD = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "kD");
        }else if (text == "enableScalarDamping"){
            enableScalarDamping = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "enableScalarDamping");
        }else if (text == "enableVectorDamping"){
            enableVectorDamping = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "enableVectorDamping");
        }else if (text == "rawValue"){
            rawValue = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "rawValue");
        }else if (text == "dampedValue"){
            dampedValue = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "dampedValue");
        }else if (text == "rawVector"){
            rawVector = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "rawVector");
        }else if (text == "dampedVector"){
            dampedVector = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "dampedVector");
        }else if (text == "vecErrorSum"){
            vecErrorSum = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "vecErrorSum");
        }else if (text == "vecPreviousError"){
            vecPreviousError = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "vecPreviousError");
        }else if (text == "errorSum"){
            errorSum = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "errorSum");
        }else if (text == "previousError"){
            previousError = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "previousError");
        }
    }
    index--;
    return true;
}

bool hkbDampingModifier::write(HkxXMLWriter *writer){
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
        writedatafield("kP", QString::number(kP, char('f'), 6));
        writedatafield("kI", QString::number(kI, char('f'), 6));
        writedatafield("kD", QString::number(kD, char('f'), 6));
        writedatafield("enableScalarDamping", getBoolAsString(enableScalarDamping));
        writedatafield("enableVectorDamping", getBoolAsString(enableVectorDamping));
        writedatafield("rawValue", QString::number(rawValue, char('f'), 6));
        writedatafield("dampedValue", QString::number(dampedValue, char('f'), 6));
        writedatafield("rawVector", rawVector.getValueAsString());
        writedatafield("dampedVector", dampedVector.getValueAsString());
        writedatafield("vecErrorSum", vecErrorSum.getValueAsString());
        writedatafield("vecPreviousError", vecPreviousError.getValueAsString());
        writedatafield("errorSum", QString::number(errorSum, char('f'), 6));
        writedatafield("previousError", QString::number(previousError, char('f'), 6));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

qreal hkbDampingModifier::getPreviousError() const{
    std::lock_guard <std::mutex> guard(mutex);
    return previousError;
}

void hkbDampingModifier::setPreviousError(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != previousError) ? previousError = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'previousError' was not set!");
}

qreal hkbDampingModifier::getErrorSum() const{
    std::lock_guard <std::mutex> guard(mutex);
    return errorSum;
}

void hkbDampingModifier::setErrorSum(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != errorSum) ? errorSum = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'errorSum' was not set!");
}

hkQuadVariable hkbDampingModifier::getVecPreviousError() const{
    std::lock_guard <std::mutex> guard(mutex);
    return vecPreviousError;
}

void hkbDampingModifier::setVecPreviousError(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != vecPreviousError) ? vecPreviousError = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'vecPreviousError' was not set!");
}

hkQuadVariable hkbDampingModifier::getVecErrorSum() const{
    std::lock_guard <std::mutex> guard(mutex);
    return vecErrorSum;
}

void hkbDampingModifier::setVecErrorSum(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != vecErrorSum) ? vecErrorSum = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'vecErrorSum' was not set!");
}

hkQuadVariable hkbDampingModifier::getDampedVector() const{
    std::lock_guard <std::mutex> guard(mutex);
    return dampedVector;
}

void hkbDampingModifier::setDampedVector(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != dampedVector) ? dampedVector = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'dampedVector' was not set!");
}

hkQuadVariable hkbDampingModifier::getRawVector() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rawVector;
}

void hkbDampingModifier::setRawVector(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rawVector) ? rawVector = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rawVector' was not set!");
}

qreal hkbDampingModifier::getDampedValue() const{
    std::lock_guard <std::mutex> guard(mutex);
    return dampedValue;
}

void hkbDampingModifier::setDampedValue(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != dampedValue) ? dampedValue = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'dampedValue' was not set!");
}

qreal hkbDampingModifier::getRawValue() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rawValue;
}

void hkbDampingModifier::setRawValue(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rawValue) ? rawValue = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rawValue' was not set!");
}

bool hkbDampingModifier::getEnableVectorDamping() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enableVectorDamping;
}

void hkbDampingModifier::setEnableVectorDamping(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enableVectorDamping) ? enableVectorDamping = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enableVectorDamping' was not set!");
}

bool hkbDampingModifier::getEnableScalarDamping() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enableScalarDamping;
}

void hkbDampingModifier::setEnableScalarDamping(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enableScalarDamping) ? enableScalarDamping = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enableScalarDamping' was not set!");
}

qreal hkbDampingModifier::getKD() const{
    std::lock_guard <std::mutex> guard(mutex);
    return kD;
}

void hkbDampingModifier::setKD(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != kD) ? kD = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'kD' was not set!");
}

qreal hkbDampingModifier::getKI() const{
    std::lock_guard <std::mutex> guard(mutex);
    return kI;
}

void hkbDampingModifier::setKI(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != kI) ? kI = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'kI' was not set!");
}

qreal hkbDampingModifier::getKP() const{
    std::lock_guard <std::mutex> guard(mutex);
    return kP;
}

void hkbDampingModifier::setKP(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != kP) ? kP = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'kP' was not set!");
}

bool hkbDampingModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbDampingModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbDampingModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbDampingModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbDampingModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbDampingModifier::evaluateDataValidity(){
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

hkbDampingModifier::~hkbDampingModifier(){
    refCount--;
}
