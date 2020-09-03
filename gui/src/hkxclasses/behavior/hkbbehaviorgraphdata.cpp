#include "hkbbehaviorgraphdata.h"
#include "hkbbehaviorgraphstringdata.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

#include "src/hkxclasses/behavior/hkbvariablevalueset.h"

using namespace UI;

uint hkbBehaviorGraphData::refCount = 0;

const QString hkbBehaviorGraphData::classname = "hkbBehaviorGraphData";

const QStringList hkbBehaviorGraphData::Type = {
    "VARIABLE_TYPE_BOOL",
    "VARIABLE_TYPE_INT8",
    "VARIABLE_TYPE_INT16",
    "VARIABLE_TYPE_INT32",
    "VARIABLE_TYPE_REAL",
    "VARIABLE_TYPE_POINTER",
    "VARIABLE_TYPE_VECTOR4",
    "VARIABLE_TYPE_QUATERNION"
};

hkbBehaviorGraphData::hkbBehaviorGraphData(HkxFile *parent, long ref, hkbBehaviorGraphStringData *strings, hkbVariableValueSet *values)
    : HkxObject(parent, ref)
{
    setType(HKB_BEHAVIOR_GRAPH_DATA, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
    stringData = HkxSharedPtr(strings);
    variableInitialValues = HkxSharedPtr(values);
}

const QString hkbBehaviorGraphData::getClassname(){
    return classname;
}

int hkbBehaviorGraphData::addVariable(hkVariableType type, const QString & name, bool isProperty){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    auto varData = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    auto index = -1;
    auto varAdded = false;
    if (strData && varData){
        if (name != "" /* && (isProperty || !strData->variableNames.contains(name))*/){
            hkVariableInfo varInfo;
            varInfo.type = Type.at(type);
            if (isProperty){
                index = strData->addCharacterPropertyName(name, &varAdded);
                (varAdded) ? characterPropertyInfos.append(varInfo) : NULL;
            }else{
                index = strData->addVariableName(name, &varAdded);
                if (varAdded){
                    variableInfos.append(varInfo);
                    varData->addWordVariableValue(0);
                }
            }
            if (varAdded){
                if (type > VARIABLE_TYPE_POINTER){
                    varData->addQuadVariableValue(hkQuadVariable());
                }else if (type == VARIABLE_TYPE_POINTER){
                    //varData->variantVariableValues.append(HkxSharedPtr());
                }
            }
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData and/or variableInitialValues are nullptr!");
    }
    return index;
}

void hkbBehaviorGraphData::addVariable(hkVariableType type){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    auto varData = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    hkVariableInfo varInfo;
    varInfo.type = Type.at(type);
    if (strData && varData){
        strData->generateAppendVariableName(varInfo.type);
        varData->addWordVariableValue(0);
        variableInfos.append(varInfo);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData and/or variableInitialValues are nullptr!");
    }
}

void hkbBehaviorGraphData::removeVariable(int index){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    auto varData = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    auto count = -1;
    if (strData && varData){
        if (index < variableInfos.size() && index > -1){
            if (variableInfos.at(index).type == "VARIABLE_TYPE_POINTER"){
                for (auto i = 0; i <= index; i++){
                    (variableInfos.at(i).type == "VARIABLE_TYPE_POINTER") ? count++ : NULL;
                }
                varData->removeVariantVariableValueAt(count);
            }else if (variableInfos.at(index).type == "VARIABLE_TYPE_VECTOR4" || variableInfos.at(index).type == "VARIABLE_TYPE_QUATERNION"){
                for (auto i = 0; i <= index; i++){
                    (variableInfos.at(i).type == "VARIABLE_TYPE_VECTOR4" || variableInfos.at(i).type == "VARIABLE_TYPE_QUATERNION") ? count++ : NULL;
                }
                varData->removeQuadVariableValueAt(count);
            }
            strData->removeVariableNameAt(index);
            varData->removeWordVariableValueAt(index);
            variableInfos.removeAt(index);
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData and/or variableInitialValues are nullptr!");
    }
}

void hkbBehaviorGraphData::addEvent(const QString &name){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    auto wasadded = false;
    if (strData){
        strData->addEventName(name, &wasadded);
        (wasadded) ? eventInfos.append("0") : NULL;
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

void hkbBehaviorGraphData::addEvent(){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        strData->generateAppendEventName();
        eventInfos.append("0");
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

void hkbBehaviorGraphData::removeEvent(int index){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        if (index < eventInfos.size() && index > -1){
            (strData->removeEventNameAt(index)) ? eventInfos.removeAt(index) : NULL;
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

void hkbBehaviorGraphData::setEventNameAt(int index, const QString & name){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        strData->setEventNameAt(index, name);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

void hkbBehaviorGraphData::setEventFlagAt(int index, bool state){
    std::lock_guard <std::mutex> guard(mutex);
    if (eventInfos.size() > index && index > -1){
        (state) ? eventInfos.replace(index, "FLAG_SYNC_POINT") : eventInfos.replace(index, "0");
    }
}

hkVariableType hkbBehaviorGraphData::getVariableTypeAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    int type;
    if (variableInfos.size() > index && index > -1){
        type = Type.indexOf(variableInfos.at(index).type);
    }
    (type == -1) ? type = VARIABLE_TYPE_INT8 : NULL;
    return (hkVariableType)type;
}

QString hkbBehaviorGraphData::getVariableNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        return strData->getVariableNameAt(index);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
    return "";
}

QString hkbBehaviorGraphData::getEventNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        return strData->getEventNameAt(index);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
    return "";
}

QString hkbBehaviorGraphData::getEventInfoAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventInfos.size() > index && index > -1){
        return eventInfos.at(index);
    }
    return "";
}

int hkbBehaviorGraphData::getWordVariableValueAt(int index) const{
    auto varData = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    if (varData){
        return varData->getWordVariableAt(index);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": variableInitialValues is nullptr!");
    }
    return -1;
}

void hkbBehaviorGraphData::setVariableNameAt(int index, const QString & name){
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        strData->setVariableNameAt(index, name);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
}

void hkbBehaviorGraphData::setWordVariableValueAt(int index, int value){
    std::lock_guard <std::mutex> guard(mutex);
    auto varData = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    if (varData){
        varData->setWordVariableAt(index, value);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": variableInitialValues is nullptr!");
    }
}

void hkbBehaviorGraphData::setQuadVariableValueAt(int index, hkQuadVariable value){
    std::lock_guard <std::mutex> guard(mutex);
    auto varData = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    auto count = -1;
    if (varData){
        if (index < variableInfos.size() && index >= 0){
            if (variableInfos.at(index).type == "VARIABLE_TYPE_VECTOR4" || variableInfos.at(index).type == "VARIABLE_TYPE_QUATERNION"){
                for (auto i = 0; i <= index; i++){
                    (variableInfos.at(i).type == "VARIABLE_TYPE_VECTOR4" || variableInfos.at(i).type == "VARIABLE_TYPE_QUATERNION") ? count++ : NULL;
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

hkQuadVariable hkbBehaviorGraphData::getQuadVariableValueAt(int index, bool *ok) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto variableValues = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    if (variableValues){
        if (variableInfos.size() > index){
            auto count = -1;
            for (auto i = 0; i <= index; i++){
                (variableInfos.at(i).type == "VARIABLE_TYPE_VECTOR4" || variableInfos.at(i).type == "VARIABLE_TYPE_QUATERNION") ? count++ : NULL;
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

HkxObject * hkbBehaviorGraphData::getVariantVariable(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto variableValues = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
    if (variableValues){
        if (variableInfos.size() > index){
            auto count = -1;
            for (auto i = 0; i <= index; i++){
                (variableInfos.at(i).type == "VARIABLE_TYPE_POINTER") ? count++ : NULL;
            }
            if (count != -1){
                return variableValues->getVariantVariableValueAt(count);
            }
        }else{
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": getVariantVariable(): Index out of range!");
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": variableInitialValues is nullptr!");
    }
    return nullptr;
}

hkVariableType hkbBehaviorGraphData::getCharacterPropertyTypeAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    int type;
    if (characterPropertyInfos.size() > index && index > -1){
        type = Type.indexOf(characterPropertyInfos.at(index).type);
    }
    (type == -1) ? type = VARIABLE_TYPE_INT8 : NULL;
    return (hkVariableType)type;
}

QString hkbBehaviorGraphData::getCharacterPropertyNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto strdata = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strdata){
        return strdata->getCharacterPropertyNameAt(index);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
    return "";
}

bool hkbBehaviorGraphData::readData(const HkxXmlReader &reader, long & index){  //TO DO: tidy up...
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    auto numElems = 0;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "variableInfos"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableInfos' data!\nObject Reference: "+ref);
                return false;
            }
            for (auto i = 0; i < numElems; i++){
                index += 4;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                hkVariableInfo temp;
                temp.role.role = reader.getElementValueAt(index);
                index++;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                temp.role.flags = reader.getElementValueAt(index);
                index++;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                temp.type = reader.getElementValueAt(index);
                variableInfos.append(temp);
                if (!Type.contains(temp.type)){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nInvalid variable type read!\nObject Reference: "+ref);
                    return false;
                }
            }
        }else if (text == "characterPropertyInfos"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'characterPropertyInfos' data!\nObject Reference: "+ref);
                return false;
            }
            for (auto i = 0; i < numElems; i++){
                index += 4;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'characterPropertyInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                hkVariableInfo temp;
                temp.role.role = reader.getElementValueAt(index);
                index++;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'characterPropertyInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                temp.role.flags = reader.getElementValueAt(index);
                index++;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'characterPropertyInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                temp.type = reader.getElementValueAt(index);
                characterPropertyInfos.append(temp);
                if (!Type.contains(temp.type)){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nInvalid variable type read!\nObject Reference: "+ref);
                    return false;
                }
            }
        }else if (text == "eventInfos"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventInfos' data!\nObject Reference: "+ref);
                return false;
            }
            /*index++;
            if (index >= reader.getNumElements()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventInfos' data!\nObject Reference: "+ref);
                return false;
            }*/
            for (auto i = 0; i < numElems; i++){
                index += 2;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventInfos' data!\nObject Reference: "+ref);
                    return false;
                }
                eventInfos.append(reader.getElementValueAt(index));
                if (eventInfos.last() == ""){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventInfos' data!\nObject Reference: "+ref);
                    return false;
                }
            }
        }else if (text == "wordMinVariableValues"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordMinVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            if (index >= reader.getNumElements()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventInfos' data!\nObject Reference: "+ref);
                return false;
            }
            for (auto i = 0; i < numElems; i++){
                index++;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordMinVariableValues' data!\nObject Reference: "+ref);
                    return false;
                }
                wordMinVariableValues.append(reader.getElementValueAt(index).toInt(&ok));
                if (!ok){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordMinVariableValues' data!\nObject Reference: "+ref);
                    return false;
                }
            }
        }else if (text == "wordMaxVariableValues"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordMaxVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            if (index >= reader.getNumElements()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventInfos' data!\nObject Reference: "+ref);
                return false;
            }
            for (auto i = 0; i < numElems; i++){
                index++;
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordMaxVariableValues' data!\nObject Reference: "+ref);
                    return false;
                }
                wordMaxVariableValues.append(reader.getElementValueAt(index).toInt(&ok));
                if (!ok){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordMaxVariableValues' data!\nObject Reference: "+ref);
                    return false;
                }
            }
        }else if (text == "variableInitialValues"){
            if (!variableInitialValues.readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableInitialValues' reference!\nObject Reference: "+ref);
            }
        }else if (text == "stringData"){
            if (!stringData.readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'stringData' reference!\nObject Reference: "+ref);
            }
        }
        index++;
    }
    index--;
    return true;
}

bool hkbBehaviorGraphData::write(HkxXMLWriter *writer){
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
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"attributeDefaults", QString::number(/*attributeDefaults.size()*/0)});
        writer->writeLine(writer->object, list1, list2, "");
        /*for (auto i = 0; i < attributeDefaults.size(); i++){
            //???
        }
        if (attributeDefaults.size() > 0){
            writer->writeLine(writer->object, false);
        }
        writer->writeLine(writer->object, false);*/
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"variableInfos", QString::number(variableInfos.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < variableInfos.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("role", "");
            writer->writeLine(writer->object, true);
            writedatafield("role", variableInfos.at(i).role.role);
            writedatafield("flags", variableInfos.at(i).role.flags);
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writedatafield("type", variableInfos.at(i).type);
            writer->writeLine(writer->object, false);
        }
        if (variableInfos.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
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
        list2 = QStringList({"eventInfos", QString::number(eventInfos.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < eventInfos.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("flags", eventInfos.at(i));
            writer->writeLine(writer->object, false);
        }
        if (eventInfos.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writeref(variableInitialValues, "variableInitialValues");
        writeref(stringData, "stringData");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(variableInitialValues, "variableInitialValues");
        writechild(stringData, "stringData");
    }
    return true;
}

QStringList hkbBehaviorGraphData::getVariableTypeNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < variableInfos.size(); i++){
        list.append(variableInfos.at(i).type);
    }
    return list;
}

int hkbBehaviorGraphData::getIndexOfVariable(const QString & name) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        return strData->getIndexOfVariableName(name);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
    return -1;
}

int hkbBehaviorGraphData::getIndexOfEvent(const QString &name) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
    if (strData){
        return strData->getIndexOfEventName(name);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": stringData is nullptr!");
    }
    return -1;
}

bool hkbBehaviorGraphData::merge(HkxObject *recessiveobj){  //TO DO: not thread safe...
    std::lock_guard <std::mutex> guard(mutex);
    bool found;
    if (recessiveobj && recessiveobj->getSignature() == HKB_BEHAVIOR_GRAPH_DATA){
        auto otherdata = static_cast<hkbBehaviorGraphData *>(recessiveobj);
        auto strings = static_cast<hkbBehaviorGraphStringData *>(stringData.data());
        //auto values = static_cast<hkbVariableValueSet *>(variableInitialValues.data());
        auto otherstrings = static_cast<hkbBehaviorGraphStringData *>(otherdata->stringData.data());
        //auto othervalues = static_cast<hkbVariableValueSet *>(otherdata->variableInitialValues.data());
        auto size = strings->eventNames.size();
        for (auto i = 0; i < otherstrings->eventNames.size(); i++){
            found = false;
            for (auto j = 0; j < size; j++){
                if (strings->eventNames.at(j) == otherstrings->eventNames.at(i)){
                    if (i != j){
                        static_cast<BehaviorFile *>(otherdata->getParentFile())->mergeEventIndices(i, j);
                    }
                    found = true;
                    break;
                }
            }
            if (!found){
                strings->eventNames.append(otherstrings->eventNames.at(i));
                if (otherdata->eventInfos.size() > i){
                    eventInfos.append(otherdata->eventInfos.at(i));
                }else{
                    LogFile::writeToLog("hkbBehaviorGraphData: merge(): Mismatch in size between eventNames and eventInfos!!!");
                }
                static_cast<BehaviorFile *>(otherdata->getParentFile())->mergeEventIndices(i, eventInfos.size() - 1);
            }
        }
        size = strings->variableNames.size();
        for (auto i = 0; i < otherstrings->variableNames.size(); i++){
            found = false;
            for (auto j = 0; j < size; j++){
                if (strings->variableNames.at(j) == otherstrings->variableNames.at(i)){
                    if (i != j){
                        static_cast<BehaviorFile *>(otherdata->getParentFile())->mergeVariableIndices(i, j);
                    }
                    found = true;
                    break;
                }
            }
            if (!found){
                auto ind = Type.indexOf(otherdata->variableInfos.at(i).type);
                if (ind != -1){
                    addVariable((hkVariableType)ind, otherstrings->variableNames.at(i));
                    static_cast<BehaviorFile *>(otherdata->getParentFile())->mergeVariableIndices(i, variableInfos.size() - 1);
                }
            }
        }
        //TO DO: Support character properties...
        return true;
    }else{
        LogFile::writeToLog("hkbBehaviorGraphData: merge(): Attempting to merge invalid object type or nullptr!!!");
    }
    return false;
}

bool hkbBehaviorGraphData::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto link = [&](HkxSharedPtr & shdptr, const QString & fieldname, HkxSignature sig){
        if (!ptr || !ptr->data()){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link '"+fieldname+"' data field!");
            setDataValidity(false);
        }else if ((*ptr)->getSignature() != sig){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' data field is linked to invalid child!");
            setDataValidity(false);
            shdptr = *ptr;
        }else{
            shdptr = *ptr;
        }
    };
    ptr = &static_cast<BehaviorFile *>(getParentFile())->getVariableValues();
    link(variableInitialValues, "variableInitialValues", HKB_VARIABLE_VALUE_SET);
    ptr = &static_cast<BehaviorFile *>(getParentFile())->getStringData();
    link(stringData, "stringData", HKB_BEHAVIOR_GRAPH_STRING_DATA);
    return true;
}

QStringList hkbBehaviorGraphData::getVariableNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data()){
        return static_cast<hkbBehaviorGraphStringData *>(stringData.data())->getVariableNames();
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+":getVariableNames(): stringData is null!!!");
    return QStringList();
}

QStringList hkbBehaviorGraphData::getEventNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data()){
        return static_cast<hkbBehaviorGraphStringData *>(stringData.data())->getEventNames();
    }
    LogFile::writeToLog(getParentFilename()+": "+getClassname()+":getEventNames(): stringData is null!!!");
    return QStringList();
}

int hkbBehaviorGraphData::getNumberOfEvents() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventInfos.size();
}

int hkbBehaviorGraphData::getNumberOfVariables() const{
    std::lock_guard <std::mutex> guard(mutex);
    return variableInfos.size();
}

QString hkbBehaviorGraphData::evaluateDataValidity(){   //TO DO...
    std::lock_guard <std::mutex> guard(mutex);
    if (!variableInitialValues.data() || variableInitialValues->getSignature() != HKB_VARIABLE_VALUE_SET || !stringData.data() || stringData->getSignature() != HKB_BEHAVIOR_GRAPH_STRING_DATA){
        setDataValidity(false);
        return QString();
    }else{
        //Check other data...
        setDataValidity(true);
        return QString();
    }
}

hkbBehaviorGraphData::~hkbBehaviorGraphData(){
    refCount--;
}
