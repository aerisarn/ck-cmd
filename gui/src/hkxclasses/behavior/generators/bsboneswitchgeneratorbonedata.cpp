#include "bsboneswitchgeneratorbonedata.h"
#include "bsboneswitchgenerator.h"
#include "bsboneswitchgeneratorbonedata.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

/*
 * CLASS: BSBoneSwitchGeneratorBoneData
*/

uint BSBoneSwitchGeneratorBoneData::refCount = 0;

const QString BSBoneSwitchGeneratorBoneData::classname = "BSBoneSwitchGeneratorBoneData";

BSBoneSwitchGeneratorBoneData::BSBoneSwitchGeneratorBoneData(HkxFile *parent, BSBoneSwitchGenerator *parentBSG, long ref)
    : hkbGenerator(parent, ref),
      parentBSG(parentBSG)
{
    setType(BS_BONE_SWITCH_GENERATOR_BONE_DATA, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    if (parentBSG && (parentBSG->getSignature() != BS_BONE_SWITCH_GENERATOR)){
        CRITICAL_ERROR_MESSAGE("BSBoneSwitchGeneratorBoneData::BSBoneSwitchGeneratorBoneData: parentBSG is incorrect type!!!");
    }
}

const QString BSBoneSwitchGeneratorBoneData::getClassname(){
    return classname;
}

bool BSBoneSwitchGeneratorBoneData::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "variableBindingSet"){
            checkvalue(getVariableBindingSet().readShdPtrReference(index, reader), "variableBindingSet");
        }else if (text == "pGenerator"){
            checkvalue(pGenerator.readShdPtrReference(index, reader), "pGenerator");
        }else if (text == "spBoneWeight"){
            checkvalue(spBoneWeight.readShdPtrReference(index, reader), "spBoneWeight");
        }
    }
    index--;
    return true;
}

bool BSBoneSwitchGeneratorBoneData::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
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
        writeref(pGenerator, "pGenerator");
        writeref(spBoneWeight, "spBoneWeight");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(pGenerator, "pGenerator");
        writechild(spBoneWeight, "spBoneWeight");
    }
    return true;
}

bool BSBoneSwitchGeneratorBoneData::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pGenerator.data()){
        return true;
    }
    return false;
}

QString BSBoneSwitchGeneratorBoneData::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (parentBSG){
        return parentBSG->getName()+"_CHILD"+QString::number(parentBSG->getIndexOfObj((BSBoneSwitchGeneratorBoneData *)this));
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": getName()!\nNo parent BSBoneSwitchGenerator'!!!");
    return "";
}

int BSBoneSwitchGeneratorBoneData::getThisIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (parentBSG){
        return parentBSG->getIndexOfObj((BSBoneSwitchGeneratorBoneData *)this);
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": getName()!\nNo parent BSBoneSwitchGenerator'!!!");
    return -1;
}

void BSBoneSwitchGeneratorBoneData::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    if (spBoneWeight.data()){
        spBoneWeight->setReference(++ref);
    }
}

bool BSBoneSwitchGeneratorBoneData::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSBoneSwitchGeneratorBoneData *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA){
        recobj = static_cast<BSBoneSwitchGeneratorBoneData *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!spBoneWeight.data() && recobj->spBoneWeight.data()){
            recobj->spBoneWeight->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->spBoneWeight.data(), -1);
        }
    }else{
        return false;
    }
    return true;
}


bool BSBoneSwitchGeneratorBoneData::operator==(const BSBoneSwitchGeneratorBoneData & other){
    std::lock_guard <std::mutex> guard(mutex);
    if (static_cast<hkbGenerator *>(parentBSG)->getName() != static_cast<hkbGenerator *>(other.parentBSG)->getName()){
        return false;
    }
    //TO DO: boneweights???
    return true;
}

void BSBoneSwitchGeneratorBoneData::setParentBSG(BSBoneSwitchGenerator *par){
    std::lock_guard <std::mutex> guard(mutex);
    parentBSG = par;
}

QVector<DataIconManager *> BSBoneSwitchGeneratorBoneData::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (pGenerator.data()){
        list.append(static_cast<DataIconManager *>(pGenerator.data()));
    }
    return list;
}

int BSBoneSwitchGeneratorBoneData::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pGenerator.data() == obj){
        return 0;
    }
    return -1;
}

bool BSBoneSwitchGeneratorBoneData::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getType() == TYPE_GENERATOR){
            pGenerator = HkxSharedPtr(obj);
            return true;
        }
    }
    return false;
}

bool BSBoneSwitchGeneratorBoneData::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        pGenerator = HkxSharedPtr();
        return true;
    }
    return false;
}

QString BSBoneSwitchGeneratorBoneData::getPGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(pGenerator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

HkxSharedPtr BSBoneSwitchGeneratorBoneData::getSpBoneWeight() const{
    std::lock_guard <std::mutex> guard(mutex);
    return spBoneWeight;
}

HkxSharedPtr BSBoneSwitchGeneratorBoneData::getPGenerator() const{
    std::lock_guard <std::mutex> guard(mutex);
    return pGenerator;
}

bool BSBoneSwitchGeneratorBoneData::link(){
    std::lock_guard <std::mutex> guard(mutex);
    auto baddata = [&](const QString & fieldname){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' is linked to invalid data!");
        setDataValidity(false);
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        baddata("hkbVariableBindingSet");
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(pGenerator.getShdPtrReference());
    if (!ptr || !ptr->data()){
        baddata("pGenerator");
    }else if ((*ptr)->getType() != TYPE_GENERATOR || (*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
        baddata("pGenerator");
        pGenerator = *ptr;
    }else{
        pGenerator = *ptr;
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(spBoneWeight.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_BONE_WEIGHT_ARRAY){
            baddata("spBoneWeight");
        }
        spBoneWeight = *ptr;
    }
    return true;
}

void BSBoneSwitchGeneratorBoneData::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    parentBSG = nullptr;
    pGenerator = HkxSharedPtr();
    spBoneWeight = HkxSharedPtr();
}

QString BSBoneSwitchGeneratorBoneData::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto appenderror = [&](const QString & fieldname, const QString & errortype, HkxSignature sig){
        QString sigstring;
        if (sig != NULL_SIGNATURE)
            sigstring = " Signature of invalid type: "+QString::number(sig, 16);
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+fieldname+": "+errortype+"!"+sigstring+"\n");
    };
    if (HkDynamicObject::evaluateDataValidity() != ""){
        appenderror("variableBindingSet", "Invalid data", NULL_SIGNATURE);
    }
    if (spBoneWeight.data() && spBoneWeight->getSignature() != HKB_BONE_WEIGHT_ARRAY){
        appenderror("spBoneWeight", "Invalid type", spBoneWeight->getSignature());
        spBoneWeight = HkxSharedPtr();
    }
    if (!pGenerator.data()){
        appenderror("pGenerator", "Null data", NULL_SIGNATURE);
    }else if (pGenerator->getType() != HkxObject::TYPE_GENERATOR){
        appenderror("pGenerator", "Invalid type", pGenerator->getSignature());
        pGenerator = HkxSharedPtr();
    }
    if (!parentBSG){
        appenderror("parentBSG", "Null data", NULL_SIGNATURE);
    }else if (parentBSG->getSignature() != BS_BONE_SWITCH_GENERATOR){
        appenderror("parentBSG", "Invalid type", parentBSG->getSignature());
        parentBSG = nullptr;
    }
    setDataValidity(isvalid);
    return errors;
}

BSBoneSwitchGeneratorBoneData::~BSBoneSwitchGeneratorBoneData(){
    refCount--;
}
