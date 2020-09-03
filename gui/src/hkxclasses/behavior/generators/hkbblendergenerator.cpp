#include "hkbblendergenerator.h"
#include "hkbblendergeneratorchild.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint hkbBlenderGenerator::refCount = 0;

const QString hkbBlenderGenerator::classname = "hkbBlenderGenerator";

hkbBlenderGenerator::hkbBlenderGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0),
      referencePoseWeightThreshold(0),
      blendParameter(1),
      minCyclicBlendParameter(0),
      maxCyclicBlendParameter(0),
      indexOfSyncMasterChild(-1),
      flags("0"),
      subtractLastChild(false)
{
    setType(HKB_BLENDER_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "BlenderGenerator_"+QString::number(refCount);
}

const QString hkbBlenderGenerator::getClassname(){
    return classname;
}

QString hkbBlenderGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

int hkbBlenderGenerator::getNumberOfChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    return children.size();
}

bool hkbBlenderGenerator::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getSignature() == HKB_BLENDER_GENERATOR_CHILD){
            if (index >= children.size() || index == -1){
                children.append(HkxSharedPtr(obj));
            }else if (!index || !children.isEmpty()){
                children.replace(index, HkxSharedPtr(obj));
            }
            return true;
        }
    }
    return false;
}

bool hkbBlenderGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (index > -1 && index < children.size()){
        static_cast<hkbBlenderGeneratorChild *>(children.at(index).data())->unlink();
        children.removeAt(index);
    }else if (index == -1){
        for (auto i = 0; i < children.size(); i++){
            static_cast<hkbBlenderGeneratorChild *>(children.at(i).data())->unlink();
        }
        children.clear();
    }else{
        return false;
    }
    return true;
}

void hkbBlenderGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

QString hkbBlenderGenerator::getFlags() const{
    std::lock_guard <std::mutex> guard(mutex);
    return flags;
}

void hkbBlenderGenerator::setFlags(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != flags) ? flags = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'flags' was not set!");
}

void hkbBlenderGenerator::setSubtractLastChild(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    subtractLastChild = value;
}

bool hkbBlenderGenerator::getSubtractLastChild() const{
    std::lock_guard <std::mutex> guard(mutex);
    return subtractLastChild;
}

int hkbBlenderGenerator::getIndexOfSyncMasterChild() const{
    std::lock_guard <std::mutex> guard(mutex);
    return indexOfSyncMasterChild;
}

void hkbBlenderGenerator::setIndexOfSyncMasterChild(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != indexOfSyncMasterChild) ? indexOfSyncMasterChild = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'indexOfSyncMasterChild' was not set!");
}

qreal hkbBlenderGenerator::getMaxCyclicBlendParameter() const{
    std::lock_guard <std::mutex> guard(mutex);
    return maxCyclicBlendParameter;
}

void hkbBlenderGenerator::setMaxCyclicBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != maxCyclicBlendParameter) ? maxCyclicBlendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'maxCyclicBlendParameter' was not set!");
}

qreal hkbBlenderGenerator::getMinCyclicBlendParameter() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minCyclicBlendParameter;
}

void hkbBlenderGenerator::setMinCyclicBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minCyclicBlendParameter) ? minCyclicBlendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minCyclicBlendParameter' was not set!");
}

qreal hkbBlenderGenerator::getBlendParameter() const{
    std::lock_guard <std::mutex> guard(mutex);
    return blendParameter;
}

void hkbBlenderGenerator::setBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != blendParameter) ? blendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'blendParameter' was not set!");
}

qreal hkbBlenderGenerator::getReferencePoseWeightThreshold() const{
    std::lock_guard <std::mutex> guard(mutex);
    return referencePoseWeightThreshold;
}

void hkbBlenderGenerator::setReferencePoseWeightThreshold(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != referencePoseWeightThreshold) ? referencePoseWeightThreshold = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'referencePoseWeightThreshold' was not set!");
}

bool hkbBlenderGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (static_cast<hkbBlenderGeneratorChild *>(children.at(i).data())){
            return true;
        }
    }
    return false;
}

int hkbBlenderGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data() == obj){
            return i;
        }
    }
    return -1;
}

int hkbBlenderGenerator::getIndexOfChild(hkbBlenderGeneratorChild *child) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data() == child){
            return i;
        }
    }
    return -1;
}

bool hkbBlenderGenerator::isParametricBlend() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (flags.contains(FLAG_PARAMETRIC_BLEND)){
        return true;
    }
    return false;
}

bool hkbBlenderGenerator::swapChildren(int index1, int index2){
    std::lock_guard <std::mutex> guard(mutex);
    if (children.size() > index1 && children.size() > index2 && index1 != index2 && index1 >= 0 && index2 >= 0){
        auto gen1 = children.at(index1).data();
        auto gen2 = children.at(index2).data();
        children[index1] = HkxSharedPtr(gen2);
        children[index2] = HkxSharedPtr(gen1);
        setIsFileChanged(true);
        return true;
    }
    return false;
}

hkbBlenderGeneratorChild *hkbBlenderGenerator::getChildDataAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbBlenderGeneratorChild *child = nullptr;
    (index >= 0 && index < children.size()) ? child = static_cast<hkbBlenderGeneratorChild *>(children.at(index).data()) : NULL;
    return child;
}

void hkbBlenderGenerator::updateChildIconNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            static_cast<DataIconManager*>(children.at(i).data())->updateIconNames();
        }
    }
}

bool hkbBlenderGenerator::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbBlenderGenerator *recobj;
    hkbBlenderGeneratorChild *domchild;
    hkbBlenderGeneratorChild *recchild;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_BLENDER_GENERATOR){
        recobj = static_cast<hkbBlenderGenerator *>(recessiveObject);
        if (getVariableBindingSetData()){
            getVariableBindingSet()->merge(recobj->getVariableBindingSetData());
        }else if (recobj->getVariableBindingSetData()){
            getVariableBindingSet() = HkxSharedPtr(recobj->getVariableBindingSetData());
            recobj->fixMergedIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(recobj->getVariableBindingSetData(), -1);
        }
        for (auto i = 0; i < children.size(); i++){
            domchild = static_cast<hkbBlenderGeneratorChild *>(children.at(i).data());
            for (auto j = 0; j < recobj->children.size(); j++){
                recchild = static_cast<hkbBlenderGeneratorChild *>(recobj->children.at(j).data());
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

QVector<DataIconManager *> hkbBlenderGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            list.append(static_cast<DataIconManager*>(children.at(i).data()));
        }
    }
    return list;
}

bool hkbBlenderGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "referencePoseWeightThreshold"){
            referencePoseWeightThreshold = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "referencePoseWeightThreshold");
        }else if (text == "blendParameter"){
            blendParameter = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "blendParameter");
        }else if (text == "minCyclicBlendParameter"){
            minCyclicBlendParameter = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "minCyclicBlendParameter");
        }else if (text == "maxCyclicBlendParameter"){
            maxCyclicBlendParameter = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxCyclicBlendParameter");
        }else if (text == "indexOfSyncMasterChild"){
            indexOfSyncMasterChild = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "indexOfSyncMasterChild");
        }else if (text == "flags"){
            flags = reader.getElementValueAt(index);
            checkvalue((flags != ""), "flags");    //TO DO: fix...
        }else if (text == "subtractLastChild"){
            subtractLastChild = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "subtractLastChild");
        }else if (text == "children"){
            checkvalue(readReferences(reader.getElementValueAt(index), children), "children");
        }
    }
    index--;
    return true;
}

bool hkbBlenderGenerator::write(HkxXMLWriter *writer){
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
        writedatafield("variableBindingSet", refString, false);
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writedatafield("referencePoseWeightThreshold", QString::number(referencePoseWeightThreshold, char('f'), 6), false);
        writedatafield("blendParameter", QString::number(blendParameter, char('f'), 6), false);
        writedatafield("minCyclicBlendParameter", QString::number(minCyclicBlendParameter, char('f'), 6), false);
        writedatafield("maxCyclicBlendParameter", QString::number(maxCyclicBlendParameter, char('f'), 6), false);
        writedatafield("indexOfSyncMasterChild", QString::number(indexOfSyncMasterChild), false);
        writedatafield("flags", flags, false);
        writedatafield("subtractLastChild", getBoolAsString(subtractLastChild), false);
        refString = "";
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"children", QString::number(children.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < children.size(); i++, j++){
            refString.append(children.at(i)->getReferenceString());
            (!(j % 16)) ? refString.append("\n") : refString.append(" ");
        }
        if (children.size() > 0){
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
        for (auto i = 0; i < children.size(); i++){
            writechild(children.at(i), "children.at("+QString::number(i)+")");
        }
    }
    return true;
}

bool hkbBlenderGenerator::link(){
    std::lock_guard <std::mutex> guard(mutex);
    auto baddata = [&](const QString & fieldname){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' is linked to invalid data!");
        setDataValidity(false);
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    HkxSharedPtr *ptr;
    for (auto i = 0; i < children.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(children.at(i).getShdPtrReference());
        if (!ptr || !ptr->data()){
            baddata("children.at("+QString::number(i)+")");
        }else if ((*ptr)->getSignature() != HKB_BLENDER_GENERATOR_CHILD){
            baddata("children.at("+QString::number(i)+")");
            children[i] = *ptr;
        }else{
            children[i] = *ptr;
            static_cast<hkbBlenderGeneratorChild *>(children[i].data())->setParentBG(this);
        }
    }
    return true;
}

void hkbBlenderGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            children[i]->unlink(); //Do here since this is not stored in the hkx file for long...
        }
        children[i] = HkxSharedPtr();
    }
}

QString hkbBlenderGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto valid = true;
    auto appenderror = [&](bool value, const QString & fieldname){
        if (!value){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid '"+fieldname+"'!");
        }
    };
    appenderror((HkDynamicObject::evaluateDataValidity() == ""), "hkbVariableBindingSet");
    appenderror((name != ""), "name");
    appenderror(!(children.isEmpty()), "children");
    for (auto i = children.size() - 1; i >= 0; i--){
        if (!children.at(i).data()){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": children at index '"+QString::number(i)+"' is null! Removing child!");
            children.removeAt(i);
        }else if (children.at(i)->getSignature() != HKB_BLENDER_GENERATOR_CHILD){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid child! Signature: "+QString::number(children.at(i)->getSignature(), 16)+" Removing child!");
            children.removeAt(i);
        }
    }
    if (indexOfSyncMasterChild >= children.size()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": indexOfSyncMasterChild is out of range! Setting default value!");
        indexOfSyncMasterChild = -1;
    }
    if (flags.toUInt(&valid) >= INVALID_FLAG || !valid){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid flags!");
        //TO DO: fix flag here!
    }
    setDataValidity(isvalid);
    return errors;
}

hkbBlenderGenerator::~hkbBlenderGenerator(){
    refCount--;
}
