#include "bsdecomposevectormodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSDecomposeVectorModifier::refCount = 0;

const QString BSDecomposeVectorModifier::classname = "BSDecomposeVectorModifier";

BSDecomposeVectorModifier::BSDecomposeVectorModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      x(0),
      y(0),
      z(0),
      w(0)
{
    setType(BS_DECOMPOSE_VECTOR_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "DecomposeVectorModifier_"+QString::number(refCount);
}

const QString BSDecomposeVectorModifier::getClassname(){
    return classname;
}

QString BSDecomposeVectorModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSDecomposeVectorModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "vector"){
            vector = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "vector");
        }else if (text == "x"){
            x = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "x");
        }else if (text == "y"){
            y = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "y");
        }else if (text == "z"){
            z = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "z");
        }else if (text == "w"){
            w = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "w");
        }
    }
    index--;
    return true;
}

bool BSDecomposeVectorModifier::write(HkxXMLWriter *writer){
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
        writedatafield("vector", vector.getValueAsString(), false);
        writedatafield("x", QString::number(x, char('f'), 6), false);
        writedatafield("y", QString::number(y, char('f'), 6), false);
        writedatafield("z", QString::number(z, char('f'), 6), false);
        writedatafield("w", QString::number(w, char('f'), 6), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

qreal BSDecomposeVectorModifier::getW() const{
    std::lock_guard <std::mutex> guard(mutex);
    return w;
}

void BSDecomposeVectorModifier::setW(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != w) ? w = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'w' was not set!");
}

qreal BSDecomposeVectorModifier::getZ() const{
    std::lock_guard <std::mutex> guard(mutex);
    return z;
}

void BSDecomposeVectorModifier::setZ(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != z) ? z = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'z' was not set!");
}

qreal BSDecomposeVectorModifier::getY() const{
    std::lock_guard <std::mutex> guard(mutex);
    return y;
}

void BSDecomposeVectorModifier::setY(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != y) ? y = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'y' was not set!");
}

qreal BSDecomposeVectorModifier::getX() const{
    std::lock_guard <std::mutex> guard(mutex);
    return x;
}

void BSDecomposeVectorModifier::setX(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != x) ? x = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'x' was not set!");
}

hkQuadVariable BSDecomposeVectorModifier::getVector() const{
    std::lock_guard <std::mutex> guard(mutex);
    return vector;
}

void BSDecomposeVectorModifier::setVector(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != vector) ? vector = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'vector' was not set!");
}

bool BSDecomposeVectorModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSDecomposeVectorModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSDecomposeVectorModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSDecomposeVectorModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void BSDecomposeVectorModifier::unlink(){
    HkDynamicObject::unlink();
}

QString BSDecomposeVectorModifier::evaluateDataValidity(){
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

BSDecomposeVectorModifier::~BSDecomposeVectorModifier(){
    refCount--;
}
