#include "hkbblendergeneratorchild.h"
#include "hkbblendergenerator.h"
#include "hkbposematchinggenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"

using namespace UI;

uint hkbBlenderGeneratorChild::refCount = 0;

const QString hkbBlenderGeneratorChild::classname = "hkbBlenderGeneratorChild";

hkbBlenderGeneratorChild::hkbBlenderGeneratorChild(HkxFile *parent, hkbGenerator *parentBG, long ref)
    : hkbGenerator(parent, ref),
      weight(1),
      worldFromModelWeight(1),
      parentBG(parentBG)
{
    setType(HKB_BLENDER_GENERATOR_CHILD, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    if (parentBG && (parentBG->getSignature() != HKB_BLENDER_GENERATOR && parentBG->getSignature() != HKB_POSE_MATCHING_GENERATOR)){
        LogFile::writeToLog("hkbBlenderGeneratorChild::hkbBlenderGeneratorChild: parentBG is incorrect type!!!");
    }
}

const QString hkbBlenderGeneratorChild::getClassname(){
    return classname;
}

hkbGenerator * hkbBlenderGeneratorChild::getParentGenerator() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (parentBG && (parentBG->getSignature() == HKB_BLENDER_GENERATOR || parentBG->getSignature() == HKB_POSE_MATCHING_GENERATOR)){
        return parentBG;
    }
    return nullptr;
}

HkxSharedPtr hkbBlenderGeneratorChild::getGenerator() const{
    std::lock_guard <std::mutex> guard(mutex);
    return generator;
}

void hkbBlenderGeneratorChild::setBoneWeights(const HkxSharedPtr &value){
    std::lock_guard <std::mutex> guard(mutex);
    boneWeights = value;
}

qreal hkbBlenderGeneratorChild::getWorldFromModelWeight() const{
    std::lock_guard <std::mutex> guard(mutex);
    return worldFromModelWeight;
}

void hkbBlenderGeneratorChild::setWorldFromModelWeight(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != worldFromModelWeight) ? worldFromModelWeight = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'worldFromModelWeight' was not set!");
}

qreal hkbBlenderGeneratorChild::getWeight() const{
    std::lock_guard <std::mutex> guard(mutex);
    return weight;
}

void hkbBlenderGeneratorChild::setWeight(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != weight) ? weight = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'weight' was not set!");
}

bool hkbBlenderGeneratorChild::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "generator"){
            checkvalue(generator.readShdPtrReference(index, reader), "generator");
        }else if (text == "boneWeights"){
            checkvalue(boneWeights.readShdPtrReference(index, reader), "boneWeights");
        }else if (text == "weight"){
            weight = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "weight");
        }else if (text == "worldFromModelWeight"){
            worldFromModelWeight = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "worldFromModelWeight");
        }
    }
    index--;
    return true;
}

bool hkbBlenderGeneratorChild::write(HkxXMLWriter *writer){
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
        writeref(generator, "generator");
        writeref(boneWeights, "boneWeights");
        writedatafield("weight", QString::number(weight, char('f'), 6), false);
        writedatafield("worldFromModelWeight", QString::number(worldFromModelWeight, char('f'), 6), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(generator, "generator");
        writechild(boneWeights, "boneWeights");
    }
    return true;
}

bool hkbBlenderGeneratorChild::isParametricBlend() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (parentBG && parentBG->getSignature() == HKB_BLENDER_GENERATOR){
        return static_cast<hkbBlenderGenerator *>(parentBG)->isParametricBlend();
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": isParametricBlend()!\nNo parent blender generator'!!!");
    }
    return false;
}

bool hkbBlenderGeneratorChild::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (generator.data()){
        return true;
    }
    return false;
}

QString hkbBlenderGeneratorChild::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString name;
    if (parentBG){
        if (parentBG->getSignature() == HKB_BLENDER_GENERATOR){
            const hkbBlenderGenerator *par = static_cast<const hkbBlenderGenerator *>(parentBG);
            name = par->getName()+"_CHILD"+QString::number(parentBG->getIndexOfObj((DataIconManager *)(this)));
        }else if (parentBG->getSignature() == HKB_POSE_MATCHING_GENERATOR){
            const hkbPoseMatchingGenerator *par = static_cast<const hkbPoseMatchingGenerator *>(parentBG);
            name = par->getName()+"_CHILD"+QString::number(parentBG->getIndexOfObj((DataIconManager *)(this)));
        }
        return name;
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": getName()!\nNo parent blender generator'!!!");
    }
    return "";
}

int hkbBlenderGeneratorChild::getThisIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (parentBG){
        return parentBG->getIndexOfObj((DataIconManager *)(this));
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": getName()!\nNo parent blender generator'!!!");
    }
    return -1;
}

void hkbBlenderGeneratorChild::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    if (boneWeights.data()){
        boneWeights->setReference(++ref);
    }
}

QVector<HkxObject *> hkbBlenderGeneratorChild::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    if (boneWeights.data()){
        list.append(boneWeights.data());
    }
    return list;
}

bool hkbBlenderGeneratorChild::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbBlenderGeneratorChild *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
        recobj = static_cast<hkbBlenderGeneratorChild *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!boneWeights.data() && recobj->boneWeights.data()){
            recobj->boneWeights->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->boneWeights.data(), -1);
        }
        return true;
    }
    return false;
}

void hkbBlenderGeneratorChild::setParentBG(hkbGenerator *blend){
    std::lock_guard <std::mutex> guard(mutex);
    blend ? parentBG = blend : NULL;
}

bool hkbBlenderGeneratorChild::operator==(const hkbBlenderGeneratorChild & other){
    std::lock_guard <std::mutex> guard(mutex);
    if (
            weight != other.weight ||
            worldFromModelWeight != other.worldFromModelWeight ||
            //!parentBG.data() ||
            //!other.parentBG.data() ||
            parentBG->getName() != other.parentBG->getName()
        )
    {
        return false;
    }
    //boneweights???
    return true;
}

QVector<DataIconManager *> hkbBlenderGeneratorChild::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (generator.data()){
        list.append(static_cast<DataIconManager*>(generator.data()));
    }
    return list;
}

hkbBoneWeightArray *hkbBlenderGeneratorChild::getBoneWeightsData() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbBoneWeightArray *>(boneWeights.data());
}

QString hkbBlenderGeneratorChild::getGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(generator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

int hkbBlenderGeneratorChild::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (generator.data() == obj){
        return 0;
    }
    return -1;
}

bool hkbBlenderGeneratorChild::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getType() == TYPE_GENERATOR){
            generator = HkxSharedPtr(obj);
            return true;
        }
    }
    return false;
}

bool hkbBlenderGeneratorChild::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        generator = HkxSharedPtr();
        return true;
    }
    return false;
}

bool hkbBlenderGeneratorChild::link(){
    std::lock_guard <std::mutex> guard(mutex);
    auto baddata = [&](const QString & fieldname){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' is linked to invalid data!");
        setDataValidity(false);
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        baddata("hkbVariableBindingSet");
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(generator.getShdPtrReference());
    if (!ptr || !ptr->data()){
        baddata("generator");
    }else if ((*ptr)->getType() != TYPE_GENERATOR || (*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
        baddata("generator");
        generator = *ptr;
    }else{
        generator = *ptr;
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(boneWeights.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_BONE_WEIGHT_ARRAY){
            baddata("boneWeights");
        }
        boneWeights = *ptr;
    }
    return true;
}

void hkbBlenderGeneratorChild::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    parentBG = nullptr;
    generator = HkxSharedPtr();
    boneWeights = HkxSharedPtr();
}

QString hkbBlenderGeneratorChild::evaluateDataValidity(){
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
    if (boneWeights.data() && boneWeights->getSignature() != HKB_BONE_WEIGHT_ARRAY){
        appenderror("spBoneWeight", "Invalid type", boneWeights->getSignature());
        boneWeights = HkxSharedPtr();
    }
    if (!generator.data()){
        appenderror("generator", "Null data", NULL_SIGNATURE);
    }else if (generator->getType() != HkxObject::TYPE_GENERATOR){
        appenderror("pGenerator", "Invalid type", generator->getSignature());
        generator = HkxSharedPtr();
    }
    if (!parentBG){
        appenderror("parentBG", "Null data", NULL_SIGNATURE);
    }else if (parentBG->getSignature() != HKB_BLENDER_GENERATOR && parentBG->getSignature() != HKB_POSE_MATCHING_GENERATOR){
        appenderror("parentBG", "Invalid type", parentBG->getSignature());
        parentBG = nullptr;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbBlenderGeneratorChild::~hkbBlenderGeneratorChild(){
    refCount--;
}
