#include "hkbbehaviorreferencegenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbBehaviorReferenceGenerator::refCount = 0;

const QString hkbBehaviorReferenceGenerator::classname = "hkbBehaviorReferenceGenerator";

hkbBehaviorReferenceGenerator::hkbBehaviorReferenceGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0)
{
    setType(HKB_BEHAVIOR_REFERENCE_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "BehaviorReferenceGenerator_"+QString::number(refCount);
}

const QString hkbBehaviorReferenceGenerator::getClassname(){
    return classname;
}

bool hkbBehaviorReferenceGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "behaviorName"){
            behaviorName = reader.getElementValueAt(index);
            checkvalue(static_cast<BehaviorFile *>(getParentFile())->getAllBehaviorFileNames().contains(behaviorName, Qt::CaseInsensitive), "behaviorName");
        }
    }
    index--;
    return true;
}

bool hkbBehaviorReferenceGenerator::write(HkxXMLWriter *writer){
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
        writedatafield("behaviorName", QString(behaviorName).replace("/", "\\"), true);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

void hkbBehaviorReferenceGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

void hkbBehaviorReferenceGenerator::setBehaviorName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != behaviorName && newname != "") ? behaviorName = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'behaviorName' was not set!");
}

QString hkbBehaviorReferenceGenerator::getBehaviorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (behaviorName.contains("\\")){
        return behaviorName.section("\\", -1, -1);
    }else if (behaviorName.contains("/")){
        return behaviorName.section("/", -1, -1);
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": getBehaviorName()!\nInvalid behaviorName!!!");
    return behaviorName;
}

bool hkbBehaviorReferenceGenerator::link(){
    return true;
}

QString hkbBehaviorReferenceGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

QString hkbBehaviorReferenceGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString behaviorname(behaviorName);
    behaviorname.replace("\\", "/");
    QString errors;
    auto isvalid = true;
    auto appenderror = [&](bool value, const QString & fieldname){
        if (!value){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid '"+fieldname+"'!");
        }
    };
    appenderror((HkDynamicObject::evaluateDataValidity() == ""), "hkbVariableBindingSet");
    appenderror((name != ""), "name");
    appenderror(static_cast<BehaviorFile *>(getParentFile())->doesBehaviorExist(behaviorname), "name");
    setDataValidity(isvalid);
    return errors;
}

hkbBehaviorReferenceGenerator::~hkbBehaviorReferenceGenerator(){
    refCount--;
}
