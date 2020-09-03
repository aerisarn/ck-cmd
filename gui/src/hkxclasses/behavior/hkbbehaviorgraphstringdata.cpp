#include "hkbbehaviorgraphstringdata.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbBehaviorGraphStringData::refCount = 0;

const QString hkbBehaviorGraphStringData::classname = "hkbBehaviorGraphStringData";

hkbBehaviorGraphStringData::hkbBehaviorGraphStringData(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_BEHAVIOR_GRAPH_STRING_DATA, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbBehaviorGraphStringData::getClassname(){
    return classname;
}

int hkbBehaviorGraphStringData::getNumberOfEvents() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventNames.size();
}

int hkbBehaviorGraphStringData::getNumberOfVariables() const{
    std::lock_guard <std::mutex> guard(mutex);
    return variableNames.size();
}

bool hkbBehaviorGraphStringData::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    int numElems = 0;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "eventNames"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventNames' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            numElems = numElems + index;
            for (; index < numElems; index++){
                if (reader.getElementNameAt(index) != "hkcstring" || index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventNames' data!\nObject Reference: "+ref);
                    return false;
                }
                eventNames.append(reader.getElementValueAt(index));
            }
            continue;
        }else if (text == "attributeNames"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attributeNames' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            numElems = numElems + index;
            for (; index < numElems; index++){
                if (reader.getElementNameAt(index) != "hkcstring" || index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attributeNames' data!\nObject Reference: "+ref);
                    return false;
                }
                attributeNames.append(reader.getElementValueAt(index));
            }
            continue;
        }else if (text == "variableNames"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableNames' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            numElems = numElems + index;
            for (; index < numElems; index++){
                if (reader.getElementNameAt(index) != "hkcstring" || index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableNames' data!\nObject Reference: "+ref);
                    return false;
                }
                variableNames.append(reader.getElementValueAt(index));
            }
            continue;
        }else if (text == "characterPropertyNames"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'characterPropertyNames' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            numElems = numElems + index;
            for (; index < numElems; index++){
                if (reader.getElementNameAt(index) != "hkcstring" || index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'characterPropertyNames' data!\nObject Reference: "+ref);
                    return false;
                }
                characterPropertyNames.append(reader.getElementValueAt(index));
            }
            continue;
        }
        index++;
    }
    index--;
    return true;
}

bool hkbBehaviorGraphStringData::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        QStringList list3 = QStringList({writer->name, writer->numelements});
        QStringList list4 = QStringList({"eventNames", QString::number(eventNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < eventNames.size(); i++){
            writer->writeLine(writer->string, QStringList(), QStringList(), eventNames.at(i));
        }
        if (eventNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"attributeNames", QString::number(attributeNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < attributeNames.size(); i++){
            writer->writeLine(writer->string, QStringList(), QStringList(), attributeNames.at(i));
        }
        if (attributeNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"variableNames", QString::number(variableNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < variableNames.size(); i++){
            writer->writeLine(writer->string, QStringList(), QStringList(), variableNames.at(i));
        }
        if (variableNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"characterPropertyNames", QString::number(characterPropertyNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < characterPropertyNames.size(); i++){
            writer->writeLine(writer->string, QStringList(), QStringList(), characterPropertyNames.at(i));
        }
        if (characterPropertyNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

QStringList hkbBehaviorGraphStringData::getEventNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventNames;
}

QStringList hkbBehaviorGraphStringData::getVariableNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    return variableNames;
}

QString hkbBehaviorGraphStringData::getVariableNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < variableNames.size()){
        return variableNames.at(index);
    }
    return "";
}

QString hkbBehaviorGraphStringData::getEventNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < eventNames.size()){
        return eventNames.at(index);
    }
    return "";
}

int hkbBehaviorGraphStringData::getIndexOfEventName(const QString &name) const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventNames.indexOf(name);
}

int hkbBehaviorGraphStringData::getIndexOfVariableName(const QString &name) const{
    std::lock_guard <std::mutex> guard(mutex);
    return variableNames.indexOf(name);
}

QString hkbBehaviorGraphStringData::getCharacterPropertyNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < characterPropertyNames.size()){
        return characterPropertyNames.at(index);
    }
    return "";
}

void hkbBehaviorGraphStringData::setVariableNameAt(int index, const QString &name){
    std::lock_guard <std::mutex> guard(mutex);
    (variableNames.size() > index && index > -1) ? variableNames.replace(index, name), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'setVariableNameAt' failed!");
}

void hkbBehaviorGraphStringData::setEventNameAt(int index, const QString &name){
    std::lock_guard <std::mutex> guard(mutex);
    (eventNames.size() > index && index > -1) ? eventNames.replace(index, name), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'setEventNameAt' failed!");
}

bool hkbBehaviorGraphStringData::removeEventNameAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    bool wasremoved = false;
    if (index >= 0 && index < eventNames.size()){
        eventNames.removeAt(index);
        setIsFileChanged(true);
        wasremoved = true;
    }
    return wasremoved;
}

void hkbBehaviorGraphStringData::removeVariableNameAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < variableNames.size()) ? variableNames.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'removeVariableNameAt' failed!");
}

int hkbBehaviorGraphStringData::getCharacterPropertyIndex(const QString &name) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    for (auto i = 0; i < characterPropertyNames.size(); i++){
        if (name == characterPropertyNames.at(i)){
            index = i;
            break;
        }
    }
    return index;
}

void hkbBehaviorGraphStringData::generateAppendVariableName(const QString &type){
    std::lock_guard <std::mutex> guard(mutex);
    QString newname = "NEW_"+type+"_";
    generateAppendStringToList(variableNames, newname, QChar('_'));
}

void hkbBehaviorGraphStringData::generateAppendEventName(){
    std::lock_guard <std::mutex> guard(mutex);
    QString newname = "NEW_Event_";
    generateAppendStringToList(eventNames, newname, QChar('_'));
}

int hkbBehaviorGraphStringData::addCharacterPropertyName(const QString &name, bool * wasadded){
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    if (characterPropertyNames.contains(name)){
        index = characterPropertyNames.indexOf(name);
        (wasadded) ? *wasadded = false : NULL;
    }else{
        characterPropertyNames.append(name);
        index = characterPropertyNames.size() - 1;
        (wasadded) ? *wasadded = false : setIsFileChanged(true);
    }
    return index;
}

int hkbBehaviorGraphStringData::addEventName(const QString &name, bool * wasadded){
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    if (eventNames.contains(name)){
        index = eventNames.indexOf(name);
        (wasadded) ? *wasadded = false : NULL;
    }else{
        eventNames.append(name);
        index = eventNames.size() - 1;
        (wasadded) ? *wasadded = false : setIsFileChanged(true);
    }
    return index;
}

bool hkbBehaviorGraphStringData::addVariableName(const QString &name, bool * wasadded){
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    if (variableNames.contains(name)){
        index = variableNames.indexOf(name);
        (wasadded) ? *wasadded = false : NULL;
    }else{
        variableNames.append(name);
        index = variableNames.size() - 1;
        (wasadded) ? *wasadded = false : setIsFileChanged(true);
    }
    return index;
}

bool hkbBehaviorGraphStringData::link(){
    return true;
}

QString hkbBehaviorGraphStringData::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkstring = [&](const QStringList & list, const QString & fieldname){
        for (auto i = 0; i < list.size(); i++){
            if (list.at(i) == ""){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid "+fieldname+" at "+QString::number(i)+"!");
            }
        }
    };
    checkstring(eventNames, "eventNames");
    checkstring(attributeNames, "attributeNames");
    checkstring(variableNames, "variableNames");
    checkstring(characterPropertyNames, "characterPropertyNames");
    setDataValidity(isvalid);
    return errors;
}

hkbBehaviorGraphStringData::~hkbBehaviorGraphStringData(){
    refCount--;
}
