#include "hkbmanualselectorgenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbManualSelectorGenerator::refCount = 0;

const QString hkbManualSelectorGenerator::classname = "hkbManualSelectorGenerator";

hkbManualSelectorGenerator::hkbManualSelectorGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0),
      selectedGeneratorIndex(0),
      currentGeneratorIndex(0)
{
    setType(HKB_MANUAL_SELECTOR_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ManualSelectorGenerator_"+QString::number(refCount);
}

const QString hkbManualSelectorGenerator::getClassname(){
    return classname;
}

QString hkbManualSelectorGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbManualSelectorGenerator::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getType() == TYPE_GENERATOR){
            if (index >= generators.size() || index == -1){
                generators.append(HkxSharedPtr(obj));
            }else if (!index || !generators.isEmpty()){
                generators.replace(index, HkxSharedPtr(obj));
            }
            return true;
        }
    }
    return false;
}

bool hkbManualSelectorGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (index > -1 && index < generators.size()){
        generators.removeAt(index);
    }else if (index == -1){
        generators.clear();
    }else{
        return false;
    }
    return true;
}

void hkbManualSelectorGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

qint8 hkbManualSelectorGenerator::getCurrentGeneratorIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return currentGeneratorIndex;
}

void hkbManualSelectorGenerator::setCurrentGeneratorIndex(const qint8 &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != currentGeneratorIndex) ? currentGeneratorIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'currentGeneratorIndex' was not set!");
}

bool hkbManualSelectorGenerator::swapChildren(int index1, int index2){
    std::lock_guard <std::mutex> guard(mutex);
    if (generators.size() > index1 && generators.size() > index2 && index1 != index2 && index1 >= 0 && index2 >= 0){
        auto gen1 = generators.at(index1).data();
        auto gen2 = generators.at(index2).data();
        generators[index1] = HkxSharedPtr(gen2);
        generators[index2] = HkxSharedPtr(gen1);
        if (selectedGeneratorIndex == index1){
            selectedGeneratorIndex = index2;
        }else if (selectedGeneratorIndex == index2){
            selectedGeneratorIndex = index1;
        }
        if (currentGeneratorIndex == index1){
            currentGeneratorIndex = index2;
        }else if (currentGeneratorIndex == index2){
            currentGeneratorIndex = index1;
        }
        setIsFileChanged(true);
        return true;
    }
    return false;
}

qint8 hkbManualSelectorGenerator::getSelectedGeneratorIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return selectedGeneratorIndex;
}

void hkbManualSelectorGenerator::setSelectedGeneratorIndex(const qint8 &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != selectedGeneratorIndex) ? selectedGeneratorIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'selectedGeneratorIndex' was not set!");
}

bool hkbManualSelectorGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < generators.size(); i++){
        if (generators.at(i).data()){
            return true;
        }
    }
    return false;
}

bool hkbManualSelectorGenerator::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbManualSelectorGenerator *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_MANUAL_SELECTOR_GENERATOR){
        obj = static_cast<hkbManualSelectorGenerator *>(recessiveObject);
        /*for (auto i = generators.size(); i < obj->generators.size(); i++){
            generators.append(obj->generators.at(i));
            getParentFile()->addObjectToFile(obj->generators.at(i).data(), -1);
        }*/
        injectWhileMerging((obj));
        return true;
    }else{
        return false;
    }
}

QVector<DataIconManager *> hkbManualSelectorGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    for (auto i = 0; i < generators.size(); i++){
        if (generators.at(i).data()){
            list.append(static_cast<DataIconManager*>(generators.at(i).data()));
        }
    }
    return list;
}

int hkbManualSelectorGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < generators.size(); i++){
        if (generators.at(i).data() == obj){
            return i;
        }
    }
    return -1;
}

bool hkbManualSelectorGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "generators"){
            checkvalue(readReferences(reader.getElementValueAt(index), generators), "generators");
        }else if (text == "selectedGeneratorIndex"){
            selectedGeneratorIndex = reader.getElementValueAt(index).toShort(&ok);
            checkvalue(ok, "selectedGeneratorIndex");
        }else if (text == "currentGeneratorIndex"){
            currentGeneratorIndex = reader.getElementValueAt(index).toShort(&ok);
            checkvalue(ok, "currentGeneratorIndex");
        }
    }
    index--;
    return true;
}

bool hkbManualSelectorGenerator::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    auto writechild = [&](const HkxSharedPtr & shdptr, const QString & datafield){
        if (shdptr.data() && !shdptr->write(writer))
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write '"+datafield+"'!!!");
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("variableBindingSet"), refString);
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        refString = "";
        list1 = QStringList({writer->name, writer->numelements});
        for (auto i = generators.size() - 1; i >= 0; i--){
            if (!generators.at(i).data()){
                generators.removeAt(i);
            }
        }
        list2 = QStringList({"generators", QString::number(generators.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < generators.size(); i++, j++){
            refString.append(generators.at(i)->getReferenceString());
            (!(j % 16)) ? refString.append("\n") : refString.append(" ");
        }
        if (generators.size() > 0){
            if (refString.endsWith(" \0")){
                refString.remove(refString.lastIndexOf(" "), 1);
            }
            writer->writeLine(refString);
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("selectedGeneratorIndex", QString::number(selectedGeneratorIndex), false);
        writedatafield("currentGeneratorIndex", QString::number(currentGeneratorIndex), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        for (auto i = 0; i < generators.size(); i++){
            writechild(generators.at(i), "generators.at("+QString::number(i)+")");
        }
    }
    return true;
}

bool hkbManualSelectorGenerator::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    HkxSharedPtr *ptr;
    for (auto i = 0; i < generators.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(generators.at(i).getShdPtrReference());
        if (!ptr || !ptr->data()){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'generators' data field!\nObject Name: "+name);
            setDataValidity(false);
        }else if ((*ptr)->getType() != TYPE_GENERATOR || (*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'generators' data field is linked to invalid child!\nObject Name: "+name);
            setDataValidity(false);
            generators[i] = *ptr;
        }else{
            generators[i] = *ptr;
        }
    }
    return true;
}

void hkbManualSelectorGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    for (auto i = 0; i < generators.size(); i++){
        generators[i] = HkxSharedPtr();
    }
}

QString hkbManualSelectorGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
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
    appenderror(!(generators.isEmpty()), "generators");
    for (auto i = generators.size() - 1; i >= 0; i--){
        if (!generators.at(i).data()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": generators at index '"+QString::number(i)+"' is null! Removing child!");
            generators.removeAt(i);
        }else if (generators.at(i)->getType() != HkxObject::TYPE_GENERATOR){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid generator! Signature: "+QString::number(generators.at(i)->getSignature(), 16)+" Removing child!");
            generators.removeAt(i);
        }
    }
    if (selectedGeneratorIndex >= generators.size()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": selectedGeneratorIndex is out of range! Setting max value!");
        selectedGeneratorIndex = generators.size() - 1;
    }
    if (currentGeneratorIndex >= generators.size()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": currentGeneratorIndex is out of range! Setting max value!");
        currentGeneratorIndex = generators.size() - 1;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbManualSelectorGenerator::~hkbManualSelectorGenerator(){
    refCount--;
}
