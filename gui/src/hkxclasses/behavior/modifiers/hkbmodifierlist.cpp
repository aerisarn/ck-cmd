#include "hkbmodifierlist.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint hkbModifierList::refCount = 0;

const QString hkbModifierList::classname = "hkbModifierList";

hkbModifierList::hkbModifierList(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(1),
      enable(true)
{
    setType(HKB_MODIFIER_LIST, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ModifierList_"+QString::number(refCount);
}

const QString hkbModifierList::getClassname(){
    return classname;
}

QString hkbModifierList::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbModifierList::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj && obj->getType() == TYPE_MODIFIER){
        if (index >= modifiers.size() || index == -1){
            modifiers.append(HkxSharedPtr(obj));
        }else if (!index || !modifiers.isEmpty()){
            modifiers.replace(index, HkxSharedPtr(obj));
        }
        return true;
    }
    return false;
}

bool hkbModifierList::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (index > -1 && index < modifiers.size()){
        modifiers.removeAt(index);
    }else if (index == -1){
        modifiers.clear();
    }else{
        return false;
    }
    return true;
}

bool hkbModifierList::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbModifierList::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

hkbModifier *hkbModifierList::getModifierAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbModifier *modifier;
    (index >= 0 && index < modifiers.size()) ? modifier = static_cast<hkbModifier *>(modifiers.at(index).data()) : modifier = nullptr;
    return modifier;
}

int hkbModifierList::getNumberOfModifiers() const{
    std::lock_guard <std::mutex> guard(mutex);
    return modifiers.size();
}

void hkbModifierList::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbModifierList::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < modifiers.size(); i++){
        if (modifiers.at(i).data()){
            return true;
        }
    }
    return false;
}

bool hkbModifierList::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbModifierList *recobj = nullptr;
    bool found;
    auto size = modifiers.size();
    QVector <DataIconManager *> objects;
    QVector <DataIconManager *> children;
    QVector <HkxObject *> othertypes;
    hkbModifier *modifier = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_MODIFIER_LIST){
        recobj = static_cast<hkbModifierList *>(recessiveObject);
        if (getVariableBindingSetData()){
            getVariableBindingSet()->merge(recobj->getVariableBindingSetData());
        }else if (recobj->getVariableBindingSetData()){
            getVariableBindingSet() = HkxSharedPtr(recobj->getVariableBindingSetData());
            recobj->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->getVariableBindingSetData(), -1);
        }
        for (auto i = 0; i < recobj->modifiers.size(); i++){
            found = false;
            for (auto j = 0; j < size; j++){
                if (static_cast<DataIconManager *>(modifiers.at(j).data())->hasSameSignatureAndName(static_cast<DataIconManager *>(recobj->modifiers.at(i).data()))){
                    found = true;
                }
            }
            if (!found){
                modifiers.append(recobj->modifiers.at(i));
                getParentFile()->addObjectToFile(recobj->modifiers.at(i).data(), -1);
                othertypes = recobj->modifiers.at(i)->getChildrenOtherTypes();
                for (auto k = 0; k < othertypes.size(); k++){
                    getParentFile()->addObjectToFile(othertypes.at(k), -1);
                }
                objects = static_cast<DataIconManager *>(recobj->modifiers.at(i).data())->getChildren();
                while (!objects.isEmpty()){
                    if (objects.last()->getType() == HkxObject::TYPE_MODIFIER){
                        modifier = static_cast<hkbModifier *>(objects.last());
                        if (!static_cast<BehaviorFile *>(getParentFile())->existsInBehavior(modifier)){
                            getParentFile()->addObjectToFile(modifier, -1);
                            othertypes = modifier->getChildrenOtherTypes();
                            for (auto k = 0; k < othertypes.size(); k++){
                                getParentFile()->addObjectToFile(othertypes.at(k), -1);
                            }
                            children = modifier->getChildren();
                        }
                    }
                    objects.removeLast();
                    objects = objects + children;
                    children.clear();
                }
            }
        }
        return true;
    }
    return false;
}

QVector<DataIconManager *> hkbModifierList::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    for (auto i = 0; i < modifiers.size(); i++){
        (modifiers.at(i).data()) ? list.append(static_cast<DataIconManager*>(modifiers.at(i).data())) : NULL;
    }
    return list;
}

int hkbModifierList::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < modifiers.size(); i++){
        if (modifiers.at(i).data() == obj){
            return i;
        }
    }
    return -1;
}

bool hkbModifierList::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "modifiers"){
            checkvalue(readReferences(reader.getElementValueAt(index), modifiers), "modifiers");
        }
    }
    index--;
    return true;
}

bool hkbModifierList::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
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
        writedatafield("variableBindingSet", refString);
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        refString = "";
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"modifiers", QString::number(modifiers.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < modifiers.size(); i++, j++){
            refString.append(modifiers.at(i)->getReferenceString());
            (!(j % 16)) ? refString.append("\n") : refString.append(" ");
        }
        if (modifiers.size() > 0){
            if (refString.endsWith(" \0")){
                refString.remove(refString.lastIndexOf(" ", 1));
            }
            writer->writeLine(refString);
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        for (auto i = 0; i < modifiers.size(); i++){
            writechild(modifiers.at(i), "modifiers.at("+QString::number(i)+")");
        }
    }
    return true;
}

bool hkbModifierList::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    HkxSharedPtr *ptr;
    for (auto i = 0; i < modifiers.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findModifier(modifiers.at(i).getShdPtrReference());
        if (!ptr || !ptr->data()){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'modifiers' data field!\nObject Name: "+name);
            setDataValidity(false);
        }else if ((*ptr)->getType() != TYPE_MODIFIER){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'modifiers' data field is linked to invalid child!\nObject Name: "+name);
            setDataValidity(false);
            modifiers[i] = *ptr;
        }else{
            modifiers[i] = *ptr;
        }
    }
    return true;
}

void hkbModifierList::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    for (auto i = 0; i < modifiers.size(); i++){
        modifiers[i] = HkxSharedPtr();
    }
}

QString hkbModifierList::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (modifiers.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": modifiers is empty!");
    }else{
        for (auto i = modifiers.size() - 1; i >= 0; i--){
            if (!modifiers.at(i).data()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": modifiers at index '"+QString::number(i)+"' is null! Removing child!");
                modifiers.removeAt(i);
            }else if (modifiers.at(i)->getType() != HkxObject::TYPE_MODIFIER){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid state! Signature: "+QString::number(modifiers.at(i)->getSignature(), 16)+" Removing child!");
                modifiers.removeAt(i);
            }
        }
    }
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

hkbModifierList::~hkbModifierList(){
    refCount--;
}
