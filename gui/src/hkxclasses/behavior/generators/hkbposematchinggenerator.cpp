#include "hkbposematchinggenerator.h"
#include "hkbblendergeneratorchild.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint hkbPoseMatchingGenerator::refCount = 0;

const QString hkbPoseMatchingGenerator::classname = "hkbPoseMatchingGenerator";

const QStringList hkbPoseMatchingGenerator::Mode = {"MODE_MATCH", "MODE_PLAY"};

hkbPoseMatchingGenerator::hkbPoseMatchingGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
    userData(0),
    referencePoseWeightThreshold(0),
    blendParameter(0),
    minCyclicBlendParameter(0),
    maxCyclicBlendParameter(0),
    indexOfSyncMasterChild(-1),
    flags("0"),
    subtractLastChild(false),
    blendSpeed(0),
    minSpeedToSwitch(0),
    minSwitchTimeNoError(0),
    minSwitchTimeFullError(0),
    startPlayingEventId(-1),
    startMatchingEventId(-1),
    rootBoneIndex(-1),
    otherBoneIndex(-1),
    anotherBoneIndex(-1),
    pelvisIndex(-1),
    mode(Mode.first())
{
    setType(HKB_POSE_MATCHING_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "PoseMatchingGenerator_"+QString::number(refCount);
}

const QString hkbPoseMatchingGenerator::getClassname(){
    return classname;
}

QString hkbPoseMatchingGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

int hkbPoseMatchingGenerator::getNumberOfChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    return children.size();
}

void hkbPoseMatchingGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

QString hkbPoseMatchingGenerator::getFlags() const{
    std::lock_guard <std::mutex> guard(mutex);
    return flags;
}

void hkbPoseMatchingGenerator::setFlags(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != flags) ? flags = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'flags' was not set!");
}

void hkbPoseMatchingGenerator::setSubtractLastChild(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != subtractLastChild) ? subtractLastChild = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'subtractLastChild' was not set!");
}

bool hkbPoseMatchingGenerator::getSubtractLastChild() const{
    std::lock_guard <std::mutex> guard(mutex);
    return subtractLastChild;
}

int hkbPoseMatchingGenerator::getIndexOfSyncMasterChild() const{
    std::lock_guard <std::mutex> guard(mutex);
    return indexOfSyncMasterChild;
}

void hkbPoseMatchingGenerator::setIndexOfSyncMasterChild(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != indexOfSyncMasterChild) ? indexOfSyncMasterChild = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'indexOfSyncMasterChild' was not set!");
}

qreal hkbPoseMatchingGenerator::getMaxCyclicBlendParameter() const{
    std::lock_guard <std::mutex> guard(mutex);
    return maxCyclicBlendParameter;
}

void hkbPoseMatchingGenerator::setMaxCyclicBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != maxCyclicBlendParameter) ? maxCyclicBlendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'maxCyclicBlendParameter' was not set!");
}

qreal hkbPoseMatchingGenerator::getMinCyclicBlendParameter() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minCyclicBlendParameter;
}

void hkbPoseMatchingGenerator::setMinCyclicBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minCyclicBlendParameter) ? minCyclicBlendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minCyclicBlendParameter' was not set!");
}

qreal hkbPoseMatchingGenerator::getBlendParameter() const{
    std::lock_guard <std::mutex> guard(mutex);
    return blendParameter;
}

void hkbPoseMatchingGenerator::setBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != blendParameter) ? blendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'blendParameter' was not set!");
}

qreal hkbPoseMatchingGenerator::getReferencePoseWeightThreshold() const{
    std::lock_guard <std::mutex> guard(mutex);
    return referencePoseWeightThreshold;
}

void hkbPoseMatchingGenerator::setReferencePoseWeightThreshold(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != referencePoseWeightThreshold) ? referencePoseWeightThreshold = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'referencePoseWeightThreshold' was not set!");
}

bool hkbPoseMatchingGenerator::swapChildren(int index1, int index2){
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

bool hkbPoseMatchingGenerator::insertObjectAt(int index, DataIconManager *obj){
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

bool hkbPoseMatchingGenerator::removeObjectAt(int index){
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

void hkbPoseMatchingGenerator::updateChildIconNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            static_cast<DataIconManager*>(children.at(i).data())->updateIconNames();
        }
    }
}

QString hkbPoseMatchingGenerator::getMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return mode;
}

void hkbPoseMatchingGenerator::setMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < Mode.size() && mode != Mode.at(index)) ? mode = Mode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'mode' was not set!");
}

int hkbPoseMatchingGenerator::getPelvisIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return pelvisIndex;
}

void hkbPoseMatchingGenerator::setPelvisIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != pelvisIndex && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? pelvisIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'pelvisIndex' was not set!");
}

int hkbPoseMatchingGenerator::getAnotherBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return anotherBoneIndex;
}

void hkbPoseMatchingGenerator::setAnotherBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != anotherBoneIndex && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? anotherBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'anotherBoneIndex' was not set!");
}

int hkbPoseMatchingGenerator::getOtherBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return otherBoneIndex;
}

void hkbPoseMatchingGenerator::setOtherBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != otherBoneIndex && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? otherBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'otherBoneIndex' was not set!");
}

int hkbPoseMatchingGenerator::getRootBoneIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rootBoneIndex;
}

void hkbPoseMatchingGenerator::setRootBoneIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != rootBoneIndex && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? rootBoneIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'rootBoneIndex' was not set!");
}

int hkbPoseMatchingGenerator::getStartMatchingEventId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startMatchingEventId;
}

void hkbPoseMatchingGenerator::setStartMatchingEventId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != startMatchingEventId && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? startMatchingEventId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startMatchingEventId' was not set!");
}

int hkbPoseMatchingGenerator::getStartPlayingEventId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startPlayingEventId;
}

void hkbPoseMatchingGenerator::setStartPlayingEventId(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != startPlayingEventId && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? startPlayingEventId = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startPlayingEventId' was not set!");
}

qreal hkbPoseMatchingGenerator::getMinSwitchTimeFullError() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minSwitchTimeFullError;
}

void hkbPoseMatchingGenerator::setMinSwitchTimeFullError(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minSwitchTimeFullError) ? minSwitchTimeFullError = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minSwitchTimeFullError' was not set!");
}

qreal hkbPoseMatchingGenerator::getMinSwitchTimeNoError() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minSwitchTimeNoError;
}

void hkbPoseMatchingGenerator::setMinSwitchTimeNoError(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minSwitchTimeNoError) ? minSwitchTimeNoError = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minSwitchTimeNoError' was not set!");
}

qreal hkbPoseMatchingGenerator::getMinSpeedToSwitch() const{
    std::lock_guard <std::mutex> guard(mutex);
    return minSpeedToSwitch;
}

void hkbPoseMatchingGenerator::setMinSpeedToSwitch(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != minSpeedToSwitch) ? minSpeedToSwitch = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'minSpeedToSwitch' was not set!");
}

qreal hkbPoseMatchingGenerator::getBlendSpeed() const{
    std::lock_guard <std::mutex> guard(mutex);
    return blendSpeed;
}

void hkbPoseMatchingGenerator::setBlendSpeed(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != blendSpeed) ? blendSpeed = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'blendSpeed' was not set!");
}

hkQuadVariable hkbPoseMatchingGenerator::getWorldFromModelRotation() const{
    std::lock_guard <std::mutex> guard(mutex);
    return worldFromModelRotation;
}

void hkbPoseMatchingGenerator::setWorldFromModelRotation(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != worldFromModelRotation) ? worldFromModelRotation = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'worldFromModelRotation' was not set!");
}

hkbBlenderGeneratorChild *hkbPoseMatchingGenerator::getChildDataAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    hkbBlenderGeneratorChild *child = nullptr;
    (index >= 0 && index < children.size()) ? child = static_cast<hkbBlenderGeneratorChild *>(children.at(index).data()) : NULL;
    return child;
}

bool hkbPoseMatchingGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            return true;
        }
    }
    return false;
}

bool hkbPoseMatchingGenerator::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (startPlayingEventId == eventindex || startMatchingEventId == eventindex){
        return true;
    }
    return false;
}

void hkbPoseMatchingGenerator::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    auto updateindices = [&](int & index){
        (index > eventindex) ? index-- : index;
    };
    updateindices(startPlayingEventId);
    updateindices(startMatchingEventId);
}

int hkbPoseMatchingGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data() == (HkxObject *)obj){
            return i;
        }
    }
    return -1;
}

QVector<DataIconManager *> hkbPoseMatchingGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            list.append(static_cast<DataIconManager*>(children.at(i).data()));
        }
    }
    return list;
}

bool hkbPoseMatchingGenerator::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbPoseMatchingGenerator *recobj;
    hkbBlenderGeneratorChild *domchild;
    hkbBlenderGeneratorChild *recchild;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_POSE_MATCHING_GENERATOR){
        recobj = static_cast<hkbPoseMatchingGenerator *>(recessiveObject);
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

int hkbPoseMatchingGenerator::getIndexOfChild(hkbBlenderGeneratorChild *child) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data() == child){
            return i;
        }
    }
    return -1;
}

bool hkbPoseMatchingGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "worldFromModelRotation"){
            worldFromModelRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "worldFromModelRotation");
        }else if (text == "blendSpeed"){
            blendSpeed = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "blendSpeed");
        }else if (text == "minSpeedToSwitch"){
            minSpeedToSwitch = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "minSpeedToSwitch");
        }else if (text == "minSwitchTimeNoError"){
            minSwitchTimeNoError = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "minSwitchTimeNoError");
        }else if (text == "minSwitchTimeFullError"){
            minSwitchTimeFullError = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "minSwitchTimeFullError");
        }else if (text == "startPlayingEventId"){
            startPlayingEventId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "startPlayingEventId");
        }else if (text == "startMatchingEventId"){
            startMatchingEventId = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "startMatchingEventId");
        }else if (text == "rootBoneIndex"){
            rootBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "rootBoneIndex");
        }else if (text == "otherBoneIndex"){
            otherBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "otherBoneIndex");
        }else if (text == "anotherBoneIndex"){
            anotherBoneIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "anotherBoneIndex");
        }else if (text == "pelvisIndex"){
            pelvisIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "pelvisIndex");
        }else if (text == "mode"){
            mode = reader.getElementValueAt(index);
            checkvalue(Mode.contains(mode), "mode");
        }
    }
    index--;
    return true;
}

bool hkbPoseMatchingGenerator::write(HkxXMLWriter *writer){
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
        writedatafield("worldFromModelRotation", worldFromModelRotation.getValueAsString(), false);
        writedatafield("blendSpeed", QString::number(blendSpeed, char('f'), 6), false);
        writedatafield("minSpeedToSwitch", QString::number(minSpeedToSwitch, char('f'), 6), false);
        writedatafield("minSwitchTimeNoError", QString::number(minSwitchTimeNoError, char('f'), 6), false);
        writedatafield("minSwitchTimeFullError", QString::number(minSwitchTimeFullError, char('f'), 6), false);
        writedatafield("startPlayingEventId", QString::number(startPlayingEventId), false);
        writedatafield("startMatchingEventId", QString::number(startMatchingEventId), false);
        writedatafield("rootBoneIndex", QString::number(rootBoneIndex), false);
        writedatafield("otherBoneIndex", QString::number(otherBoneIndex), false);
        writedatafield("anotherBoneIndex", QString::number(anotherBoneIndex), false);
        writedatafield("pelvisIndex", QString::number(pelvisIndex), false);
        writedatafield("mode", mode, false);
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

bool hkbPoseMatchingGenerator::link(){
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

void hkbPoseMatchingGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    for (auto i = 0; i < children.size(); i++){
        if (children.at(i).data()){
            children[i]->unlink(); //Do here since this is not stored in the hkx file for long...
        }
        children[i] = HkxSharedPtr();
    }
}

QString hkbPoseMatchingGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto valid = true;
    if (children.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": children is empty!");
    }else{
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
    }
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (flags.toUInt(&valid) >= INVALID_FLAG || !valid){    //TO DO: fix
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid flags!");
    }
    if (indexOfSyncMasterChild >= children.size()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": indexOfSyncMasterChild is out of range! Setting default value!");
        indexOfSyncMasterChild = -1;
    }
    if (!Mode.contains(mode)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid mode! Setting default value!");
        mode = Mode.first();
    }
    if (startPlayingEventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": startPlayingEventId event id out of range! Setting to last event index!");
        startPlayingEventId = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (startMatchingEventId >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": startMatchingEventId event id out of range! Setting to last event index!");
        startMatchingEventId = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (rootBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": rootBoneIndex out of range! Setting to last bone index!");
        rootBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (otherBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": otherBoneIndex out of range! Setting to last bone index!");
        otherBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (anotherBoneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": anotherBoneIndex out of range! Setting to last bone index!");
        anotherBoneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    if (pelvisIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": pelvisIndex out of range! Setting to last bone index!");
        pelvisIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
    }
    setDataValidity(isvalid);
    return errors;
}

hkbPoseMatchingGenerator::~hkbPoseMatchingGenerator(){
    refCount--;
}
