#include "hkbrigidbodyragdollcontrolsmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"

using namespace UI;

uint hkbRigidBodyRagdollControlsModifier::refCount = 0;

const QString hkbRigidBodyRagdollControlsModifier::classname = "hkbRigidBodyRagdollControlsModifier";

hkbRigidBodyRagdollControlsModifier::hkbRigidBodyRagdollControlsModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      hierarchyGain(0),
      velocityDamping(0),
      accelerationGain(0),
      velocityGain(0),
      positionGain(0),
      positionMaxLinearVelocity(0),
      positionMaxAngularVelocity(0),
      snapGain(0),
      snapMaxLinearVelocity(0),
      snapMaxAngularVelocity(0),
      snapMaxLinearDistance(0),
      snapMaxAngularDistance(0),
      durationToBlend(0)
{
    setType(HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "RigidBodyRagdollControlsModifier_"+QString::number(refCount);
}

const QString hkbRigidBodyRagdollControlsModifier::getClassname(){
    return classname;
}

QString hkbRigidBodyRagdollControlsModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbRigidBodyRagdollControlsModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "hierarchyGain"){
            hierarchyGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "hierarchyGain");
        }else if (text == "velocityDamping"){
            velocityDamping = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "velocityDamping");
        }else if (text == "accelerationGain"){
            accelerationGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "accelerationGain");
        }else if (text == "velocityGain"){
            velocityGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "velocityGain");
        }else if (text == "positionGain"){
            positionGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "positionGain");
        }else if (text == "positionMaxLinearVelocity"){
            positionMaxLinearVelocity = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "positionMaxLinearVelocity");
        }else if (text == "positionMaxAngularVelocity"){
            positionMaxAngularVelocity = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "positionMaxAngularVelocity");
        }else if (text == "snapGain"){
            snapGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "snapGain");
        }else if (text == "snapMaxLinearVelocity"){
            snapMaxLinearVelocity = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "snapMaxLinearVelocity");
        }else if (text == "snapMaxAngularVelocity"){
            snapMaxAngularVelocity = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "snapMaxAngularVelocity");
        }else if (text == "snapMaxLinearDistance"){
            snapMaxLinearDistance = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "snapMaxLinearDistance");
        }else if (text == "snapMaxAngularDistance"){
            snapMaxAngularDistance = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "snapMaxAngularDistance");
        }else if (text == "durationToBlend"){
            durationToBlend = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "durationToBlend");
        }else if (text == "bones"){
            checkvalue(bones.readShdPtrReference(index, reader), "bones");
        }else{
            //LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nUnknown field '"+text+"' found!\nObject Reference: "+ref);
        }
    }
    index--;
    return true;
}

bool hkbRigidBodyRagdollControlsModifier::write(HkxXMLWriter *writer){
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
        writedatafield("controlData", "");
        writer->writeLine(writer->object, true);
        writedatafield("keyFrameHierarchyControlData", "");
        writer->writeLine(writer->object, true);
        writedatafield("hierarchyGain", QString::number(hierarchyGain, char('f'), 6));
        writedatafield("velocityDamping", QString::number(velocityDamping, char('f'), 6));
        writedatafield("accelerationGain", QString::number(accelerationGain, char('f'), 6));
        writedatafield("velocityGain", QString::number(velocityGain, char('f'), 6));
        writedatafield("positionGain", QString::number(positionGain, char('f'), 6));
        writedatafield("positionMaxLinearVelocity", QString::number(positionMaxLinearVelocity, char('f'), 6));
        writedatafield("positionMaxAngularVelocity", QString::number(positionMaxAngularVelocity, char('f'), 6));
        writedatafield("snapGain", QString::number(snapGain, char('f'), 6));
        writedatafield("snapMaxLinearVelocity", QString::number(snapMaxLinearVelocity, char('f'), 6));
        writedatafield("snapMaxAngularVelocity", QString::number(snapMaxAngularVelocity, char('f'), 6));
        writedatafield("snapMaxLinearDistance", QString::number(snapMaxLinearDistance, char('f'), 6));
        writedatafield("snapMaxAngularDistance", QString::number(snapMaxAngularDistance, char('f'), 6));
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("durationToBlend", QString::number(durationToBlend, char('f'), 6));
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writeref(bones, "bones");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(bones, "bones");
    }
    return true;
}

void hkbRigidBodyRagdollControlsModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (bones.data()) ? bones->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> hkbRigidBodyRagdollControlsModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (bones.data()) ? list.append(bones.data()) : NULL;
    return list;
}

bool hkbRigidBodyRagdollControlsModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbRigidBodyRagdollControlsModifier *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER){
        obj = static_cast<hkbRigidBodyRagdollControlsModifier *>(recessiveObject);
        if (bones.data()){
            if (obj->bones.data()){
                bones->merge(obj->bones.data());
            }
        }else if (obj->bones.data()){
            bones = obj->bones;
            getParentFile()->addObjectToFile(obj->bones.data(), 0);
        }
        injectWhileMerging(obj);
        return true;
    }
    return false;
}

hkbBoneIndexArray * hkbRigidBodyRagdollControlsModifier::getBones() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbBoneIndexArray *>(bones.data());
}

void hkbRigidBodyRagdollControlsModifier::setBones(hkbBoneIndexArray *value){
    std::lock_guard <std::mutex> guard(mutex);
    bones = HkxSharedPtr(value), setIsFileChanged(true);
}

qreal hkbRigidBodyRagdollControlsModifier::getDurationToBlend() const{
    std::lock_guard <std::mutex> guard(mutex);
    return durationToBlend;
}

void hkbRigidBodyRagdollControlsModifier::setDurationToBlend(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != durationToBlend) ? durationToBlend = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'durationToBlend' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getSnapMaxAngularDistance() const{
    std::lock_guard <std::mutex> guard(mutex);
    return snapMaxAngularDistance;
}

void hkbRigidBodyRagdollControlsModifier::setSnapMaxAngularDistance(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != snapMaxAngularDistance) ? snapMaxAngularDistance = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'snapMaxAngularDistance' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getSnapMaxLinearDistance() const{
    std::lock_guard <std::mutex> guard(mutex);
    return snapMaxLinearDistance;
}

void hkbRigidBodyRagdollControlsModifier::setSnapMaxLinearDistance(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != snapMaxLinearDistance) ? snapMaxLinearDistance = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'snapMaxLinearDistance' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getSnapMaxAngularVelocity() const{
    std::lock_guard <std::mutex> guard(mutex);
    return snapMaxAngularVelocity;
}

void hkbRigidBodyRagdollControlsModifier::setSnapMaxAngularVelocity(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != snapMaxAngularVelocity) ? snapMaxAngularVelocity = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'snapMaxAngularVelocity' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getSnapMaxLinearVelocity() const{
    std::lock_guard <std::mutex> guard(mutex);
    return snapMaxLinearVelocity;
}

void hkbRigidBodyRagdollControlsModifier::setSnapMaxLinearVelocity(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != snapMaxLinearVelocity) ? snapMaxLinearVelocity = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'snapMaxLinearVelocity' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getSnapGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return snapGain;
}

void hkbRigidBodyRagdollControlsModifier::setSnapGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != snapGain) ? snapGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'snapGain' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getPositionMaxAngularVelocity() const{
    std::lock_guard <std::mutex> guard(mutex);
    return positionMaxAngularVelocity;
}

void hkbRigidBodyRagdollControlsModifier::setPositionMaxAngularVelocity(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != positionMaxAngularVelocity) ? positionMaxAngularVelocity = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'positionMaxAngularVelocity' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getPositionMaxLinearVelocity() const{
    std::lock_guard <std::mutex> guard(mutex);
    return positionMaxLinearVelocity;
}

void hkbRigidBodyRagdollControlsModifier::setPositionMaxLinearVelocity(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != positionMaxLinearVelocity) ? positionMaxLinearVelocity = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'positionMaxLinearVelocity' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getPositionGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return positionGain;
}

void hkbRigidBodyRagdollControlsModifier::setPositionGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != positionGain) ? positionGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'positionGain' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getVelocityGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return velocityGain;
}

void hkbRigidBodyRagdollControlsModifier::setVelocityGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != velocityGain) ? velocityGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'velocityGain' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getAccelerationGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return accelerationGain;
}

void hkbRigidBodyRagdollControlsModifier::setAccelerationGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != accelerationGain) ? accelerationGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'accelerationGain' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getVelocityDamping() const{
    std::lock_guard <std::mutex> guard(mutex);
    return velocityDamping;
}

void hkbRigidBodyRagdollControlsModifier::setVelocityDamping(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != velocityDamping) ? velocityDamping = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'velocityDamping' was not set!");
}

qreal hkbRigidBodyRagdollControlsModifier::getHierarchyGain() const{
    std::lock_guard <std::mutex> guard(mutex);
    return hierarchyGain;
}

void hkbRigidBodyRagdollControlsModifier::setHierarchyGain(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != hierarchyGain) ? hierarchyGain = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'hierarchyGain' was not set!");
}

bool hkbRigidBodyRagdollControlsModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbRigidBodyRagdollControlsModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbRigidBodyRagdollControlsModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbRigidBodyRagdollControlsModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(bones.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_BONE_INDEX_ARRAY){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'bones' is not a HKB_BONE_INDEX_ARRAY!");
            setDataValidity(false);
        }
        bones = *ptr;
    }
    return true;
}

void hkbRigidBodyRagdollControlsModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    bones = HkxSharedPtr();
}

QString hkbRigidBodyRagdollControlsModifier::evaluateDataValidity(){
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
    if (!bones.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null bones!");
    }else if (bones->getSignature() != HKB_BONE_INDEX_ARRAY){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid bones type! Signature: "+QString::number(bones->getSignature(), 16)+" Setting null value!");
        bones = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbRigidBodyRagdollControlsModifier::~hkbRigidBodyRagdollControlsModifier(){
    refCount--;
}
