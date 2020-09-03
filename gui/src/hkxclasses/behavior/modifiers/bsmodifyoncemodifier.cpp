#include "bsmodifyoncemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSModifyOnceModifier::refCount = 0;

const QString BSModifyOnceModifier::classname = "BSModifyOnceModifier";

BSModifyOnceModifier::BSModifyOnceModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true)
{
    setType(BS_MODIFY_ONCE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ModifyOnceModifier_"+QString::number(refCount);
}

const QString BSModifyOnceModifier::getClassname(){
    return classname;
}

QString BSModifyOnceModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSModifyOnceModifier::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj && obj->getType() == TYPE_MODIFIER){
        (index == 1) ? pOnDeactivateModifier = HkxSharedPtr(obj): pOnDeactivateModifier = HkxSharedPtr(obj);
        return true;
    }
    return false;
}

bool BSModifyOnceModifier::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index){
        pOnActivateModifier = HkxSharedPtr();
    }else if (index == 1){
        pOnDeactivateModifier = HkxSharedPtr();
    }else if (index == -1){
        pOnActivateModifier = HkxSharedPtr();
        pOnDeactivateModifier = HkxSharedPtr();
    }else{
        return false;
    }
    return true;
}

hkbModifier * BSModifyOnceModifier::getPOnDeactivateModifier() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbModifier *>(pOnDeactivateModifier.data());
}

hkbModifier * BSModifyOnceModifier::getPOnActivateModifier() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbModifier *>(pOnActivateModifier.data());
}

bool BSModifyOnceModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void BSModifyOnceModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void BSModifyOnceModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSModifyOnceModifier::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pOnActivateModifier.data() || pOnDeactivateModifier.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> BSModifyOnceModifier::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    auto getchildren = [&](const HkxSharedPtr & shdptr){
        (shdptr.data()) ? list.append(static_cast<DataIconManager*>(shdptr.data())) : NULL;
    };
    getchildren(pOnActivateModifier);
    getchildren(pOnDeactivateModifier);
    return list;
}

int BSModifyOnceModifier::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pOnActivateModifier.data() == obj){
        return 0;
    }else if (pOnDeactivateModifier.data() == obj){
        return 1;
    }
    return -1;
}

bool BSModifyOnceModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "pOnActivateModifier"){
            checkvalue(pOnActivateModifier.readShdPtrReference(index, reader), "pOnActivateModifier");
        }else if (text == "pOnDeactivateModifier"){
            checkvalue(pOnDeactivateModifier.readShdPtrReference(index, reader), "pOnDeactivateModifier");
        }
    }
    index--;
    return true;
}

bool BSModifyOnceModifier::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
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
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        writeref(pOnActivateModifier, "pOnActivateModifier");
        writeref(pOnDeactivateModifier, "pOnDeactivateModifier");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(pOnActivateModifier, "pOnActivateModifier");
        writechild(pOnDeactivateModifier, "pOnDeactivateModifier");
    }
    return true;
}

bool BSModifyOnceModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkmodifiers = [&](HkxSharedPtr & data, const QString & fieldname){
        if (ptr){
            if ((*ptr)->getType() != TYPE_MODIFIER){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object '"+fieldname+"' is not a modifier!");
                setDataValidity(false);
            }
            data = *ptr;
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findModifier(pOnActivateModifier.getShdPtrReference());
    linkmodifiers(pOnActivateModifier, "pOnActivateModifier");
    ptr = static_cast<BehaviorFile *>(getParentFile())->findModifier(pOnDeactivateModifier.getShdPtrReference());
    linkmodifiers(pOnDeactivateModifier, "pOnDeactivateModifier");
    return true;
}

void BSModifyOnceModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    pOnActivateModifier = HkxSharedPtr();
    pOnDeactivateModifier = HkxSharedPtr();
}

QString BSModifyOnceModifier::evaluateDataValidity(){
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
    if (!pOnActivateModifier.data()){
        if (!pOnDeactivateModifier.data()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": pOnActivateModifier and pOnDeactivateModifier are both nullptr!");
        }else if (pOnDeactivateModifier->getType() != HkxObject::TYPE_MODIFIER){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid pOnDeactivateModifier type! Signature: "+QString::number(pOnDeactivateModifier->getSignature(), 16)+" Setting null value!");
            pOnDeactivateModifier = HkxSharedPtr();
        }
    }else{
        if (pOnActivateModifier->getType() != HkxObject::TYPE_MODIFIER){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid pOnActivateModifier type! Signature: "+QString::number(pOnActivateModifier->getSignature(), 16)+" Setting null value!");
            pOnActivateModifier = HkxSharedPtr();
        }
        if (pOnDeactivateModifier.data() && pOnDeactivateModifier->getType() != HkxObject::TYPE_MODIFIER){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid pOnDeactivateModifier type! Signature: "+QString::number(pOnDeactivateModifier->getSignature(), 16)+" Setting null value!");
            pOnDeactivateModifier = HkxSharedPtr();
        }
    }
    setDataValidity(isvalid);
    return errors;
}

BSModifyOnceModifier::~BSModifyOnceModifier(){
    refCount--;
}
