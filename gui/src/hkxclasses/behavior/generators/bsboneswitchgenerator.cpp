#include "bsboneswitchgenerator.h"
#include "bsboneswitchgeneratorbonedata.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint BSBoneSwitchGenerator::refCount = 0;

const QString BSBoneSwitchGenerator::classname = "BSBoneSwitchGenerator";

BSBoneSwitchGenerator::BSBoneSwitchGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(1)
{
    setType(BS_BONE_SWITCH_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "BoneSwitchGenerator_"+QString::number(refCount);
}

const QString BSBoneSwitchGenerator::getClassname(){
    return classname;
}

QString BSBoneSwitchGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

void BSBoneSwitchGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool BSBoneSwitchGenerator::swapChildren(int index1, int index2){
    HkxObject *gen1;
    HkxObject *gen2;
    if (ChildrenA.size() > index1 && ChildrenA.size() > index2 && index1 != index2 && index1 >= 0 && index2 >= 0){
        gen1 = ChildrenA.at(index1).data();
        gen2 = ChildrenA.at(index2).data();
        ChildrenA[index1] = HkxSharedPtr(gen2);
        ChildrenA[index2] = HkxSharedPtr(gen1);
        setIsFileChanged(true);
    }else{
        return false;
    }
    return true;
}

void BSBoneSwitchGenerator::updateChildIconNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < ChildrenA.size(); i++){
        if (ChildrenA.at(i).data()){
            static_cast<DataIconManager*>(ChildrenA.at(i).data())->updateIconNames();
        }
    }
}

QString BSBoneSwitchGenerator::getDefaultGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(pDefaultGenerator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

bool BSBoneSwitchGenerator::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA){
            if (!index){
                return false;
            }else if (index >= ChildrenA.size() || index == -1){
                ChildrenA.append(HkxSharedPtr(obj));
            }else if (index == 1 || !ChildrenA.isEmpty()){
                ChildrenA.replace(index - 1, HkxSharedPtr(obj));
            }
        }else if (obj->getType() == TYPE_GENERATOR){
            if (!index){
                pDefaultGenerator = HkxSharedPtr(obj);
            }else{
                return false;
            }
        }
    }else{
        return false;
    }
    return true;
}

bool BSBoneSwitchGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data()){
        return true;
    }
    for (auto i = 0; i < ChildrenA.size(); i++){
        if (ChildrenA.at(i).data()){
            return true;
        }
    }
    return false;
}

bool BSBoneSwitchGenerator::hasGenerator() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> BSBoneSwitchGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (pDefaultGenerator.data()){
        list.append(static_cast<DataIconManager*>(pDefaultGenerator.data()));
    }
    for (auto i = 0; i < ChildrenA.size(); i++){
        if (ChildrenA.at(i).data()){
            list.append(static_cast<DataIconManager*>(ChildrenA.at(i).data()));
        }
    }
    return list;
}

int BSBoneSwitchGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data() == obj){
        return 0;
    }else{
        for (auto i = 0; i < ChildrenA.size(); i++){
            if (static_cast<BSBoneSwitchGeneratorBoneData *>(ChildrenA.at(i).data()) == obj){
                return i + 1;
            }
        }
    }
    return -1;
}

bool BSBoneSwitchGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    BSBoneSwitchGeneratorBoneData *objChild;
    if (!index){
        pDefaultGenerator = HkxSharedPtr();
    }else if (index > -1 && index <= ChildrenA.size()){
        objChild = static_cast<BSBoneSwitchGeneratorBoneData *>(ChildrenA.at(index - 1).data());
        ChildrenA.removeAt(index - 1);
    }else if (index == -1){
        pDefaultGenerator = HkxSharedPtr();
        ChildrenA.clear();
    }else{
        return false;
    }
    return true;
}

bool BSBoneSwitchGenerator::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSBoneSwitchGenerator *recobj;
    BSBoneSwitchGeneratorBoneData *domchild;
    BSBoneSwitchGeneratorBoneData *recchild;
    hkbGenerator *domgen;
    hkbGenerator *recgen;
    DataIconManager *temp;
    QVector <DataIconManager *> recchildren;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_BONE_SWITCH_GENERATOR){
        recobj = static_cast<BSBoneSwitchGenerator *>(recessiveObject);
        domgen = static_cast<hkbGenerator *>(pDefaultGenerator.data());
        recgen = static_cast<hkbGenerator *>(recobj->pDefaultGenerator.data());
        if (getVariableBindingSetData()){
            getVariableBindingSet()->merge(recobj->getVariableBindingSetData());
        }else if (recobj->getVariableBindingSetData()){
            getVariableBindingSet() = HkxSharedPtr(recobj->getVariableBindingSetData());
            recobj->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->getVariableBindingSetData(), -1);
        }
        if (domgen){
            if (recgen && domgen && (domgen->hasSameSignatureAndName(recgen))){
                recchildren = static_cast<DataIconManager *>(recgen)->getChildren();
                for (auto i = 0; i < recchildren.size(); i++){
                    temp = recchildren.at(i);
                    if (domgen->hasSameSignatureAndName(temp)){
                        pDefaultGenerator = HkxSharedPtr(recgen);
                        break;
                    }
                    //TO DO: add children to file?
                }
            }
        }else if (recgen){
            pDefaultGenerator = HkxSharedPtr(recgen);
        }
        for (auto i = 0; i < ChildrenA.size(); i++){
            domchild = static_cast<BSBoneSwitchGeneratorBoneData *>(ChildrenA.at(i).data());
            for (auto j = 0; j < recobj->ChildrenA.size(); j++){
                recchild = static_cast<BSBoneSwitchGeneratorBoneData *>(recobj->ChildrenA.at(j).data());
                if (*recchild == *domchild){
                    domchild->merge(recchild);
                    break;
                }
            }
        }
        setIsMerged(true);
    }
    return true;
}

bool BSBoneSwitchGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "ChildrenA"){
            checkvalue(readReferences(reader.getElementValueAt(index), ChildrenA), "ChildrenA");
        }
    }
    index--;
    return true;
}

bool BSBoneSwitchGenerator::write(HkxXMLWriter *writer){
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
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writeref(getVariableBindingSet(), "variableBindingSet");
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writeref(pDefaultGenerator, "pDefaultGenerator");
        refString = "";
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"ChildrenA", QString::number(ChildrenA.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < ChildrenA.size(); i++, j++){
            refString.append(ChildrenA.at(i)->getReferenceString());
            (!(j % 16)) ? refString.append("\n") : refString.append(" ");
        }
        if (ChildrenA.size() > 0){
            if (refString.endsWith(" \0")){
                refString.remove(refString.lastIndexOf(" "), 1);
            }
            writer->writeLine(refString);
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(pDefaultGenerator, "pDefaultGenerator");
        for (auto i = 0; i < ChildrenA.size(); i++){
            writechild(ChildrenA.at(i), "ChildrenA.at("+QString::number(i)+")");
        }
    }
    return true;
}

bool BSBoneSwitchGenerator::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkdata = [&](HkxSignature sig, HkxSharedPtr & shdptr, const QString & fieldname){
        if (ptr){
            if (!ptr->data()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link '"+fieldname+"' data field!");
                setDataValidity(false);
            }else if ((*ptr)->getType() != TYPE_GENERATOR || (sig != NULL_SIGNATURE && (*ptr)->getSignature() != sig) ||
                      ((*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD))
            {
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' data field is linked to invalid child!");
                setDataValidity(false);
            }
            shdptr = *ptr;
            if ((*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA)
                static_cast<BSBoneSwitchGeneratorBoneData *>(shdptr.data())->setParentBSG(this);
        }else{
            setDataValidity(false);
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(pDefaultGenerator.getShdPtrReference());
    linkdata(NULL_SIGNATURE, pDefaultGenerator, "pDefaultGenerator");
    for (auto i = 0; i < ChildrenA.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(ChildrenA.at(i).getShdPtrReference());
        linkdata(BS_BONE_SWITCH_GENERATOR_BONE_DATA, ChildrenA[i], "ChildrenA["+QString::number(i)+"]");
    }
    return true;
}

void BSBoneSwitchGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    pDefaultGenerator = HkxSharedPtr();
    for (auto i = 0; i < ChildrenA.size(); i++){
        if (ChildrenA.at(i).data()){
            ChildrenA[i]->unlink(); //Do here since this is not stored in the hkx file for long...
        }
        ChildrenA[i] = HkxSharedPtr();
    }
}

QString BSBoneSwitchGenerator::evaluateDataValidity(){
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
    appenderror(!(ChildrenA.isEmpty()), "ChildrenA");
    for (auto i = ChildrenA.size() - 1; i >= 0; i--){
        if (!ChildrenA.at(i).data()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": ChildrenA at index '"+QString::number(i)+"' is null!");
            ChildrenA.removeAt(i);
        }else if (ChildrenA.at(i)->getSignature() != BS_BONE_SWITCH_GENERATOR_BONE_DATA){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid child! Signature: "+QString::number(ChildrenA.at(i)->getSignature(), 16)+" Removing!");
            ChildrenA.removeAt(i);
        }
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

BSBoneSwitchGenerator::~BSBoneSwitchGenerator(){
    refCount--;
}
