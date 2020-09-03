#include "hkbcharacterdata.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/characterfile.h"
#include "src/filetypes/projectfile.h"
#include "src/hkxclasses/behavior/hkbcharacterstringdata.h"
#include "src/hkxclasses/behavior/hkbvariablevalueset.h"
#include "src/hkxclasses/behavior/hkbmirroredskeletoninfo.h"
#include "src/hkxclasses/behavior/hkbfootikdriverinfo.h"
#include "src/hkxclasses/behavior/hkbhandikdriverinfo.h"

using namespace UI;

uint hkbCharacterData::refCount = 0;

const QString hkbCharacterData::classname = "hkbCharacterData";

const QStringList hkbCharacterData::Type = {
    "VARIABLE_TYPE_BOOL",
    "VARIABLE_TYPE_INT8",
    "VARIABLE_TYPE_INT16",
    "VARIABLE_TYPE_INT32",
    "VARIABLE_TYPE_REAL",
    "VARIABLE_TYPE_POINTER",
    "VARIABLE_TYPE_VECTOR4",
    "VARIABLE_TYPE_QUATERNION"
};

hkbCharacterData::hkbCharacterData(HkxFile *parent, long ref, hkbCharacterStringData *strings, hkbVariableValueSet *values, hkbMirroredSkeletonInfo *mirrorskelinfo)
    : HkxObject(parent, ref),
      modelUpMS(0, 0, 1, 0),
      modelForwardMS(0, 1, 0, 0),
      modelRightMS(1, 0, 0, 0),
      scale(1),
      stringData(strings),
      characterPropertyValues(values),
      mirroredSkeletonInfo(mirrorskelinfo)
{
    setType(HKB_CHARACTER_DATA, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbCharacterData::getClassname(){
    return classname;
}

int hkbCharacterData::getWordVariableValueAt(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbVariableValueSet *variableValues = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    if (variableValues){
        return variableValues->getWordVariableAt(index);
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": characterPropertyValues is nullptr!");
    return -1;
}

HkxObject * hkbCharacterData::getVariantVariable(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkbVariableValueSet *variableValues = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    if (variableValues){
        if (characterPropertyInfos.size() > index){
            auto count = -1;
            for (auto i = 0; i <= index; i++){
                (characterPropertyInfos.at(i).type == "VARIABLE_TYPE_POINTER") ? count++ : NULL;
            }
            if (count != -1){
                return variableValues->getVariantVariableValueAt(count);
            }
        }else{
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": getVariantVariable(): Index out of range!");
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": characterPropertyValues is nullptr!");
    }
    return nullptr;
}

void hkbCharacterData::addVariable(hkVariableType type, const QString & name){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    hkbVariableValueSet *varData = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    auto index = -1;
    auto varAdded = false;
    if (strData && varData){
        if (name != "" /* && (isProperty || !strData->variableNames.contains(name))*/){
            hkVariableInfo varInfo;
            varInfo.type = Type.at(type);
            index = strData->addCharacterPropertyName(name, &varAdded);
            if (varAdded){
                characterPropertyInfos.append(varInfo);
                varData->addWordVariableValue(0);
                if (type > VARIABLE_TYPE_POINTER){
                    varData->addQuadVariableValue(hkQuadVariable());
                }else if (type == VARIABLE_TYPE_POINTER){
                    //varData->variantVariableValues.append(HkxSharedPtr());
                }
            }
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData and/or characterPropertyValues are nullptr!");
    }
}

void hkbCharacterData::addVariable(hkVariableType type){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    hkbVariableValueSet *varData = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    hkVariableInfo varInfo;
    varInfo.type = Type.at(type);
    if (strData && varData){
        strData->generateAppendCharacterPropertyName(varInfo.type);
        varData->addWordVariableValue(0);
        characterPropertyInfos.append(varInfo);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData and/or variableInitialValues are nullptr!");
    }
}

void hkbCharacterData::removeVariable(int index){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    hkbVariableValueSet *varData = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    auto count = -1;
    if (strData && varData){
        if (index < characterPropertyInfos.size() && index > -1){
            if (characterPropertyInfos.at(index).type == "VARIABLE_TYPE_POINTER"){
                for (auto i = 0; i <= index; i++){
                    (characterPropertyInfos.at(i).type == "VARIABLE_TYPE_POINTER") ? count++ : NULL;
                }
                varData->removeVariantVariableValueAt(count);
            }else if (characterPropertyInfos.at(index).type == "VARIABLE_TYPE_VECTOR4" || characterPropertyInfos.at(index).type == "VARIABLE_TYPE_QUATERNION"){
                for (auto i = 0; i <= index; i++){
                    (characterPropertyInfos.at(i).type == "VARIABLE_TYPE_VECTOR4" || characterPropertyInfos.at(i).type == "VARIABLE_TYPE_QUATERNION") ? count++ : NULL;
                }
                varData->removeQuadVariableValueAt(count);
            }
            strData->removeCharacterPropertyNameAt(index);
            varData->removeWordVariableValueAt(index);
            characterPropertyInfos.removeAt(index);
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData and/or variableInitialValues are nullptr!");
    }
}

hkVariableType hkbCharacterData::getVariableTypeAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    int type;
    if (characterPropertyInfos.size() > index && index > -1){
        type = Type.indexOf(characterPropertyInfos.at(index).type);
    }
    (type == -1) ? type = VARIABLE_TYPE_INT8 : NULL;
    return (hkVariableType)type;
}

void hkbCharacterData::setCharacterPropertyNameAt(int index, const QString & name){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    if (strData){
        strData->setCharacterPropertyNameAt(index, name);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

void hkbCharacterData::setWordVariableValueAt(int index, int value){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbVariableValueSet *varData = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    if (varData){
        varData->setWordVariableAt(index, value);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": characterPropertyValues is nullptr!");
    }
}

void hkbCharacterData::setQuadVariableValueAt(int index, hkQuadVariable value){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbVariableValueSet *varData = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    auto count = -1;
    if (varData){
        if (index < characterPropertyInfos.size() && index >= 0){
            if (characterPropertyInfos.at(index).type == "VARIABLE_TYPE_VECTOR4" || characterPropertyInfos.at(index).type == "VARIABLE_TYPE_QUATERNION"){
                for (auto i = 0; i <= index; i++){
                    (characterPropertyInfos.at(i).type == "VARIABLE_TYPE_VECTOR4" || characterPropertyInfos.at(i).type == "VARIABLE_TYPE_QUATERNION") ? count++ : NULL;
                }
                if (count != -1){
                    varData->setQuadVariableValueAt(count, value);
                }
            }
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": varData is nullptr!");
    }
}

void hkbCharacterData::setFootIkDriverInfo(hkbFootIkDriverInfo *fooik){
    //std::lock_guard <std::mutex> guard(mutex);
    footIkDriverInfo = HkxSharedPtr(fooik);
}

hkQuadVariable hkbCharacterData::getQuadVariable(int index, bool *ok) const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkbVariableValueSet *variableValues = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
    if (variableValues){
        if (characterPropertyInfos.size() > index){
            auto count = -1;
            for (auto i = 0; i <= index; i++){
                (characterPropertyInfos.at(i).type == "VARIABLE_TYPE_VECTOR4" || characterPropertyInfos.at(i).type == "VARIABLE_TYPE_QUATERNION") ? count++ : NULL;
            }
            if (count != -1){
                return variableValues->getQuadVariableValueAt(count, ok);
            }
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": variableInitialValues is nullptr!");
    }
    (ok) ? ok = false : NULL;
    return hkQuadVariable();
}


bool hkbCharacterData::readData(const HkxXmlReader &reader, long & index){
    //std::lock_guard <std::mutex> guard(mutex);
    int numElems;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "capsuleHeight"){
            characterControllerInfo.capsuleHeight = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "userData");
        }else if (text == "capsuleHeight"){
            characterControllerInfo.capsuleRadius = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "capsuleRadius");
        }else if (text == "collisionFilterInfo"){
            characterControllerInfo.collisionFilterInfo = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "collisionFilterInfo");
        }else if (text == "characterControllerCinfo"){
            checkvalue(characterControllerInfo.characterControllerCinfo.readShdPtrReference(index, reader), "characterControllerInfo.characterControllerCinfo");
        }else if (text == "modelUpMS"){
            modelUpMS = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "modelUpMS");
        }else if (text == "modelForwardMS"){
            modelForwardMS = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "modelForwardMS");
        }else if (text == "modelRightMS"){
            modelRightMS = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "modelRightMS");
        }else if (text == "characterPropertyInfos"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "characterPropertyInfos");
            for (auto i = 0; i < numElems; i++){
                index += 4;
                checkvalue((index < reader.getNumElements()), "characterPropertyInfos");
                hkVariableInfo temp;
                temp.role.role = reader.getElementValueAt(index);
                index++;
                checkvalue((index < reader.getNumElements()), "characterPropertyInfos");
                temp.role.flags = reader.getElementValueAt(index);
                index++;
                checkvalue((index < reader.getNumElements()), "characterPropertyInfos");
                temp.type = reader.getElementValueAt(index);
                characterPropertyInfos.append(temp);
                checkvalue(Type.contains(temp.type), "type");
            }
        }else if (text == "numBonesPerLod"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "numBonesPerLod");
            (numElems > 0) ? index++ : NULL;
            checkvalue((index < reader.getNumElements()), "numBonesPerLod");
            for (auto i = 0; i < numElems; i++, index++){
                checkvalue((index < reader.getNumElements()), "numBonesPerLod");
                numBonesPerLod.append(reader.getElementValueAt(index).toInt(&ok));
                checkvalue(ok, "numBonesPerLod");
            }
        }else if (text == "characterPropertyValues"){
            checkvalue(characterPropertyValues.readShdPtrReference(index, reader), "characterPropertyValues");
        }else if (text == "footIkDriverInfo"){
            checkvalue(footIkDriverInfo.readShdPtrReference(index, reader), "footIkDriverInfo");
        }else if (text == "handIkDriverInfo"){
            checkvalue(handIkDriverInfo.readShdPtrReference(index, reader), "handIkDriverInfo");
        }else if (text == "stringData"){
            checkvalue(stringData.readShdPtrReference(index, reader), "stringData");
        }else if (text == "mirroredSkeletonInfo"){
            checkvalue(mirroredSkeletonInfo.readShdPtrReference(index, reader), "mirroredSkeletonInfo");
        }
    }
    index--;
    return true;
}

bool hkbCharacterData::write(HkxXMLWriter *writer){
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
    //std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writedatafield("characterControllerInfo", "");
        writer->writeLine(writer->object, true);
        writedatafield("capsuleHeight", QString::number(characterControllerInfo.capsuleHeight, char('f'), 6));
        writedatafield("capsuleRadius", QString::number(characterControllerInfo.capsuleRadius, char('f'), 6));
        writedatafield("collisionFilterInfo", QString::number(characterControllerInfo.collisionFilterInfo));
        writeref(characterControllerInfo.characterControllerCinfo, "characterControllerCinfo");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"characterPropertyInfos", QString::number(characterPropertyInfos.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < characterPropertyInfos.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("role", "");
            writer->writeLine(writer->object, true);
            writedatafield("role", characterPropertyInfos.at(i).role.role);
            writedatafield("flags", characterPropertyInfos.at(i).role.flags);
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writedatafield("type", characterPropertyInfos.at(i).type);
            writer->writeLine(writer->object, false);
        }
        if (characterPropertyInfos.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"numBonesPerLod", QString::number(numBonesPerLod.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < numBonesPerLod.size(); i++){
            writer->writeLine(writer->string, QStringList(), QStringList(), QString::number(numBonesPerLod.at(i)));
        }
        if (numBonesPerLod.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("scale", QString::number(scale, char('f'), 6));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writeref(characterPropertyValues, "characterPropertyValues");
        writeref(footIkDriverInfo, "footIkDriverInfo");
        writeref(handIkDriverInfo, "handIkDriverInfo");
        writeref(stringData, "stringData");
        writeref(mirroredSkeletonInfo, "mirroredSkeletonInfo");
    }
    return true;
}

bool hkbCharacterData::link(){
    //std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkptr = [&](HkxSignature sig, HkxSharedPtr & shdptr, const QString & fieldname){
        if (!(*ptr).data() || (*ptr)->getSignature() != sig){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' data field is linked to invalid child!");
            setDataValidity(false);
        }
        shdptr = *ptr;
    };
    ptr = &static_cast<CharacterFile *>(getParentFile())->getCharacterPropertyValues();
    linkptr(HKB_VARIABLE_VALUE_SET, characterPropertyValues, "characterPropertyValues");
    ptr = &static_cast<CharacterFile *>(getParentFile())->getFootIkDriverInfo();
    linkptr(HKB_FOOT_IK_DRIVER_INFO, footIkDriverInfo, "footIkDriverInfo");
    ptr = &static_cast<CharacterFile *>(getParentFile())->getHandIkDriverInfo();
    linkptr(HKB_HAND_IK_DRIVER_INFO, handIkDriverInfo, "handIkDriverInfo");
    ptr = &static_cast<CharacterFile *>(getParentFile())->getStringData();
    linkptr(HKB_CHARACTER_STRING_DATA, stringData, "stringData");
    ptr = &static_cast<CharacterFile *>(getParentFile())->getMirroredSkeletonInfo();
    linkptr(HKB_MIRRORED_SKELETON_INFO, mirroredSkeletonInfo, "mirroredSkeletonInfo");
    return true;
}

void hkbCharacterData::setVariableNameAt(int index, const QString & name){
    //std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbCharacterStringData *>(stringData.data());
    if (strData){
        strData->setVariableNameAt(index, name);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

QStringList hkbCharacterData::getCharacterPropertyNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    if (strData){
        return strData->getCharacterPropertyNames();
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    return QStringList();
}

QString hkbCharacterData::getCharacterPropertyNameAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    if (strData){
        return strData->getCharacterPropertyNameAt(index);
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    return "";
}

QString hkbCharacterData::getLastCharacterPropertyName() const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterStringData *strData = static_cast<hkbCharacterStringData *>(stringData.data());
    if (strData){
        return strData->getLastCharacterPropertyName();
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    return "";
}

QStringList hkbCharacterData::getCharacterPropertyTypenames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < characterPropertyInfos.size(); i++){
        list.append(characterPropertyInfos.at(i).type);
    }
    return list;
}

bool hkbCharacterData::merge(HkxObject *recessiveobj){
    //std::lock_guard <std::mutex> guard(mutex);
    hkbCharacterData *otherdata;
    hkbCharacterStringData *otherstrings;
    hkbVariableValueSet *othervalues;
    hkbCharacterStringData *strings;
    hkbVariableValueSet *values;
    bool found;
    int size;
    if (recessiveobj && recessiveobj->getSignature() == HKB_CHARACTER_DATA){
        otherdata = static_cast<hkbCharacterData *>(recessiveobj);
        //if (evaluateDataValidity() && recessiveobj->evaluateDataValidity()){
            strings = static_cast<hkbCharacterStringData *>(stringData.data());
            values = static_cast<hkbVariableValueSet *>(characterPropertyValues.data());
            otherstrings = static_cast<hkbCharacterStringData *>(otherdata->stringData.data());
            othervalues = static_cast<hkbVariableValueSet *>(otherdata->characterPropertyValues.data());
            size = strings->animationNames.size() - 1;
            for (auto i = otherstrings->animationNames.size() - 1; i >= 0; i--){
                found = false;
                for (auto j = size; j >= 0; j--){
                    if (strings->animationNames.at(j) == otherstrings->animationNames.at(i)){
                        found = true;
                        break;
                    }
                }
                if (!found){
                    strings->animationNames.append(otherstrings->animationNames.at(i));
                    static_cast<CharacterFile *>(getParentFile())->appendAnimation(&static_cast<CharacterFile *>(recessiveobj->getParentFile())->getAnimationMotionData(i));
                    //static_cast<BehaviorFile *>(otherdata->getParentFile())->mergeEventIndices(i, eventInfos.size() - 1);
                }
            }
            size = strings->characterPropertyNames.size() - 1;
            for (auto i = otherstrings->characterPropertyNames.size() - 1; i >= 0; i--){
                found = false;
                for (auto j = size; j >= 0; j--){
                    if (strings->characterPropertyNames.at(j) == otherstrings->characterPropertyNames.at(i)){
                        found = true;
                        break;
                    }
                }
                if (!found){    //TO DO: Support quad and pointer variables...
                    strings->characterPropertyNames.append(otherstrings->characterPropertyNames.at(i));
                    if (othervalues->wordVariableValues.size() > i){
                        values->wordVariableValues.append(othervalues->wordVariableValues.at(i));
                    }else{
                        WARNING_MESSAGE("hkbCharacterData: merge(): Mismatch in size between variableNames and wordVariableValues!!!");
                    }
                    if (otherdata->characterPropertyInfos.size() > i){
                        characterPropertyInfos.append(otherdata->characterPropertyInfos.at(i));
                    }else{
                        WARNING_MESSAGE("hkbCharacterData: merge(): Mismatch in size between variableNames and variableInfos!!!");
                    }
                    //TO DO: Support character properties...
                    //static_cast<BehaviorFile *>(otherdata->getParentFile())->mergeVariableIndices(i, variableInfos.size() - 1);
                }
            }
            return true;
        /*}else{
            WARNING_MESSAGE("hkbCharacterData: merge(): Invalid data detected!!!");
        }*/
    }else{
        WARNING_MESSAGE("hkbCharacterData: merge(): Attempting to merge invalid object type or nullptr!!!");
    }
    return false;
}

void hkbCharacterData::setHandIkDriverInfo(hkbHandIkDriverInfo *value){
    //std::lock_guard <std::mutex> guard(mutex);
    handIkDriverInfo = HkxSharedPtr(value);
}

hkVariableType hkbCharacterData::getCharacterPropertyTypeAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    int type;
    if (characterPropertyInfos.size() > index && index > -1){
        type = Type.indexOf(characterPropertyInfos.at(index).type);
    }
    (type == -1) ? type = VARIABLE_TYPE_INT8 : NULL;
    return (hkVariableType)type;
}

QString hkbCharacterData::evaluateDataValidity(){   //TO DO: finish....
    //std::lock_guard <std::mutex> guard(mutex);
    if ((!characterPropertyValues.data() || characterPropertyValues->getSignature() != HKB_VARIABLE_VALUE_SET) || (footIkDriverInfo.data() && footIkDriverInfo->getSignature() != HKB_FOOT_IK_DRIVER_INFO) ||
            (handIkDriverInfo.data() && handIkDriverInfo->getSignature() != HKB_HAND_IK_DRIVER_INFO) || (!stringData.data() || stringData->getSignature() != HKB_CHARACTER_STRING_DATA) ||
            (!mirroredSkeletonInfo.data() || mirroredSkeletonInfo->getSignature() != HKB_MIRRORED_SKELETON_INFO))
    {
        setDataValidity(false);
        return QString();
    }else {
        //Check other data...
        setDataValidity(true);
        return QString();
    }
}

hkbCharacterData::~hkbCharacterData(){
    refCount--;
}
