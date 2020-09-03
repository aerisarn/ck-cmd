#include "bsistatetagginggenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSiStateTaggingGenerator::refCount = 0;

const QString BSiStateTaggingGenerator::classname = "BSiStateTaggingGenerator";

BSiStateTaggingGenerator::BSiStateTaggingGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(1),
      iStateToSetAs(-1),
      iPriority(0)
{
    setType(BS_I_STATE_TAGGING_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "iStateTaggingGenerator_"+QString::number(refCount);
}

const QString BSiStateTaggingGenerator::getClassname(){
    return classname;
}

QString BSiStateTaggingGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSiStateTaggingGenerator::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj && obj->getType() == TYPE_GENERATOR){
        pDefaultGenerator = HkxSharedPtr(obj);
        return true;
    }
    return false;
}

bool BSiStateTaggingGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        pDefaultGenerator = HkxSharedPtr();
        return true;
    }
    return false;
}

void BSiStateTaggingGenerator::setIPriority(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != iPriority) ? iPriority = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'iPriority' was not set!");
}

void BSiStateTaggingGenerator::setIStateToSetAs(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != iStateToSetAs) ? iStateToSetAs = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'iStateToSetAs' was not set!");
}

void BSiStateTaggingGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

int BSiStateTaggingGenerator::getIPriority() const{
    std::lock_guard <std::mutex> guard(mutex);
    return iPriority;
}

int BSiStateTaggingGenerator::getIStateToSetAs() const{
    std::lock_guard <std::mutex> guard(mutex);
    return iStateToSetAs;
}

QString BSiStateTaggingGenerator::getDefaultGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(pDefaultGenerator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

bool BSiStateTaggingGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> BSiStateTaggingGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (pDefaultGenerator.data()){
        list.append(static_cast<DataIconManager*>(pDefaultGenerator.data()));
    }
    return list;
}

int BSiStateTaggingGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data() == obj){
        return 0;
    }
    return -1;
}

bool BSiStateTaggingGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "pDefaultGenerator"){
            checkvalue(pDefaultGenerator.readShdPtrReference(index, reader), "pDefaultGenerator");
        }else if (text == "iStateToSetAs"){
            iStateToSetAs = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "iStateToSetAs");
        }else if (text == "iPriority"){
            iPriority = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "iPriority");
        }
    }
    index--;
    return true;
}

bool BSiStateTaggingGenerator::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    auto writeref = [&](const HkxSharedPtr & shdptr, const QString & name){
        QString refString = "null";
        (shdptr.data()) ? refString = shdptr->getReferenceString() : NULL;
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), refString);
    };
    auto writechild = [&](const HkxSharedPtr & shdptr, const QString & datafield){
        if (shdptr.data() && !shdptr->write(writer))
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write '"+datafield+"'!!!");
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writeref(getVariableBindingSet(), "variableBindingSet");
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writeref(pDefaultGenerator, "pDefaultGenerator");
        writedatafield("iStateToSetAs", QString::number(iStateToSetAs), false);
        writedatafield("iPriority", QString::number(iPriority), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(pDefaultGenerator, "pDefaultGenerator");
    }
    return true;
}

bool BSiStateTaggingGenerator::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(pDefaultGenerator.getShdPtrReference());
    if (!ptr || !ptr->data()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'pDefaultGenerator' data field!\nObject Name: "+name);
        setDataValidity(false);
    }else if ((*ptr)->getType() != TYPE_GENERATOR || (*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'pDefaultGenerator' data field is linked to invalid child!\nObject Name: "+name);
        setDataValidity(false);
        pDefaultGenerator = *ptr;
    }else{
        pDefaultGenerator = *ptr;
    }
    return true;
}

void BSiStateTaggingGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    pDefaultGenerator = HkxSharedPtr();
}

QString BSiStateTaggingGenerator::evaluateDataValidity(){
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
    if (!pDefaultGenerator.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null pDefaultGenerator!");
    }else if (pDefaultGenerator->getType() != HkxObject::TYPE_GENERATOR){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid pDefaultGenerator type! Signature: "+QString::number(pDefaultGenerator->getSignature(), 16)+" Setting null value!");
        pDefaultGenerator = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSiStateTaggingGenerator::~BSiStateTaggingGenerator(){
    refCount--;
}
