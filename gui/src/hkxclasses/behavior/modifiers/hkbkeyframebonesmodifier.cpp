#include "hkbkeyframebonesmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbboneindexarray.h"

using namespace UI;

uint hkbKeyframeBonesModifier::refCount = 0;

const QString hkbKeyframeBonesModifier::classname = "hkbKeyframeBonesModifier";

hkbKeyframeBonesModifier::hkbKeyframeBonesModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true)
{
    setType(HKB_KEY_FRAME_BONES_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "KeyframeBonesModifier_"+QString::number(refCount);
}

const QString hkbKeyframeBonesModifier::getClassname(){
    return classname;
}

QString hkbKeyframeBonesModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbKeyframeBonesModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numexp;
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
        }else if (text == "keyframeInfo"){
            numexp = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "keyframeInfo");
            (numexp > 0) ? index++ : NULL;
            for (auto j = 0; j < numexp; j++, index++){
                keyframeInfo.append(hkKeyframeInfo());
                for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "keyframedPosition"){
                        keyframeInfo.last().keyframedPosition = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").keyframedPosition");
                    }else if (text == "keyframedRotation"){
                        keyframeInfo.last().keyframedRotation = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").keyframedRotation");
                    }else if (text == "boneIndex"){
                        keyframeInfo.last().boneIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").boneIndex");
                    }else if (text == "isValid"){
                        keyframeInfo.last().isValid = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").isValid");
                        break;
                    }
                }
            }
            (numexp > 0) ? index-- : NULL;
        }else if (text == "keyframedBonesList"){
            checkvalue(keyframedBonesList.readShdPtrReference(index, reader), "keyframedBonesList");
        }
    }
    index--;
    return true;
}

bool hkbKeyframeBonesModifier::write(HkxXMLWriter *writer){
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
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"keyframeInfo", QString::number(keyframeInfo.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < keyframeInfo.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("keyframedPosition", keyframeInfo[i].keyframedPosition.getValueAsString());
            writedatafield("keyframedRotation", keyframeInfo[i].keyframedRotation.getValueAsString());
            writedatafield("boneIndex", QString::number(keyframeInfo.at(i).boneIndex));
            writedatafield("isValid", getBoolAsString(keyframeInfo.at(i).isValid));
            writer->writeLine(writer->object, false);
        }
        if (keyframeInfo.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writeref(keyframedBonesList, "keyframedBonesList");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(keyframedBonesList, "keyframedBonesList");
    }
    return true;
}

int hkbKeyframeBonesModifier::getNumberOfKeyframeInfos() const{
    std::lock_guard <std::mutex> guard(mutex);
    return keyframeInfo.size();
}

void hkbKeyframeBonesModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (keyframedBonesList.data()) ? keyframedBonesList->updateReferences(ref) : NULL;
}

QVector<HkxObject *> hkbKeyframeBonesModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (keyframedBonesList.data()) ? list.append(keyframedBonesList.data()) : NULL;
    return list;
}

bool hkbKeyframeBonesModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbKeyframeBonesModifier *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_KEY_FRAME_BONES_MODIFIER){
        obj = static_cast<hkbKeyframeBonesModifier *>(recessiveObject);
        if (keyframedBonesList.data()){
            if (obj->keyframedBonesList.data()){
                keyframedBonesList->merge(obj->keyframedBonesList.data());
            }
        }else if (obj->keyframedBonesList.data()){
            keyframedBonesList = obj->keyframedBonesList;
            getParentFile()->addObjectToFile(obj->keyframedBonesList.data(), 0);
        }
        injectWhileMerging(obj);
        return true;
    }
    return false;
}

hkbBoneIndexArray * hkbKeyframeBonesModifier::getKeyframedBonesList() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbBoneIndexArray *>(keyframedBonesList.data());
}

void hkbKeyframeBonesModifier::setKeyframedBonesList(hkbBoneIndexArray *value){
    std::lock_guard <std::mutex> guard(mutex);
    keyframedBonesList = value;
}

void hkbKeyframeBonesModifier::addKeyframeInfo(hkbKeyframeBonesModifier::hkKeyframeInfo info){
    std::lock_guard <std::mutex> guard(mutex);
    keyframeInfo.append(info);
}

void hkbKeyframeBonesModifier::removeKeyframeInfo(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < keyframeInfo.size()) ? keyframeInfo.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'keyframeInfo' was not removed!");
}

bool hkbKeyframeBonesModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbKeyframeBonesModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbKeyframeBonesModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbKeyframeBonesModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(keyframedBonesList.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_BONE_INDEX_ARRAY){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'keyframedBonesList' is not a HKB_BONE_INDEX_ARRAY!");
            setDataValidity(false);
        }
        keyframedBonesList = *ptr;
    }
    return true;
}

void hkbKeyframeBonesModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    keyframedBonesList = HkxSharedPtr();
}

QString hkbKeyframeBonesModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (keyframeInfo.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": keyframeInfo is empty!");
    }else{
        for (auto i = 0; i < keyframeInfo.size(); i++){
            if (keyframeInfo.at(i).boneIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": boneIndex at "+QString::number(i)+" out of range! Setting to last bone index!");
                keyframeInfo[i].boneIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
            }
        }
    }
    auto temp = HkDynamicObject::evaluateDataValidity(); if (temp != ""){errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");}
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (keyframedBonesList.data()){
        if (keyframedBonesList->getSignature() != HKB_BONE_INDEX_ARRAY){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid keyframedBonesList type! Signature: "+QString::number(keyframedBonesList->getSignature(), 16)+" Setting null value!");
            keyframedBonesList = HkxSharedPtr();    //TO DO: add here?
        }else if (keyframedBonesList->isDataValid() && keyframedBonesList->evaluateDataValidity() != ""){
            isvalid = false;
            //errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid keyframedBonesList data!");
        }
    }else if (!keyframedBonesList.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null keyframedBonesList!");
    }
    setDataValidity(isvalid);
    return errors;
}

hkbKeyframeBonesModifier::~hkbKeyframeBonesModifier(){
    refCount--;
}
