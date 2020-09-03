#include "hkbvariablebindingset.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "hkbbehaviorgraphdata.h"
#include "hkbbehaviorgraphstringdata.h"
#include "hkbvariablevalueset.h"

using namespace UI;

uint hkbVariableBindingSet::refCount = 0;

const QString hkbVariableBindingSet::classname = "hkbVariableBindingSet";

hkbVariableBindingSet::hkbVariableBindingSet(HkxFile *parent, long ref)
    : HkxObject(parent, ref),
      indexOfBindingToEnable(-1)
{
    setType(HKB_VARIABLE_BINDING_SET, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbVariableBindingSet::getClassname(){
    return classname;
}

int hkbVariableBindingSet::getNumberOfBindings() const{
    std::lock_guard <std::mutex> guard(mutex);
    return bindings.size();
}

bool hkbVariableBindingSet::addBinding(const QString & path, int varIndex, hkBinding::BindingType type){
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    auto exists = false;
    for (auto i = 0; i < bindings.size(); i++){ //Do this for below but remove the binding if the paths are the same...
        if (bindings.at(i).memberPath == path){
            if (type == hkBinding::BINDING_TYPE_VARIABLE){
                bindings[i].variableIndex = varIndex;
            }else if (type == hkBinding::BINDING_TYPE_CHARACTER_PROPERTY){
                index = static_cast<BehaviorFile *>(getParentFile())->addCharacterProperty(varIndex);
                if (index > -1){
                    bindings[i].variableIndex = index;
                }else{
                    return false;
                }
            }
            bindings[i].bindingType = type;
            if (path == "enable"){
                indexOfBindingToEnable = i;
            }
            exists = true;
        }
    }
    if (!exists){
        if (type == hkBinding::BINDING_TYPE_VARIABLE){
            bindings.append(hkBinding(path, varIndex, -1, type));
        }else if (type == hkBinding::BINDING_TYPE_CHARACTER_PROPERTY){
            index = static_cast<BehaviorFile *>(getParentFile())->addCharacterProperty(varIndex);
            if (index > -1){
                bindings.append(hkBinding(path, index, -1, type));
            }else{
                return false;
            }
        }
        if (path == "enable"){
            indexOfBindingToEnable = bindings.size() - 1;
        }
    }
    setIsFileChanged(true);
    return true;
}

void hkbVariableBindingSet::removeBinding(const QString & path){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        (bindings.at(i).memberPath == path) ? bindings.removeAt(i), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+"removeBinding(): Binding was not removed!!");
    }
}

void hkbVariableBindingSet::removeBinding(int varIndex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        (bindings.at(i).variableIndex == varIndex) ? bindings.removeAt(i), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+"removeBinding(): Binding was not removed!!");
    }
}

QString hkbVariableBindingSet::getPathOfBindingAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    QString path;
    (index < bindings.size() && index >= 0) ? path = bindings.at(index).memberPath : LogFile::writeToLog(getClassname()+"getPathOfBindingAt(): Invalid binding index!!");
    return path;
}

int hkbVariableBindingSet::getVariableIndexOfBinding(const QString & path) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        if (bindings.at(i).memberPath == path){
            /*if (bindings.at(i).bindingType == hkBinding::BINDING_TYPE_CHARACTER_PROPERTY){
                //return static_cast<BehaviorFile *>(getParentFile())->findCharacterPropertyIndexFromCharacter(bindings.at(i).variableIndex);
                return static_cast<BehaviorFile *>(getParentFile())->getCharacterPropertyIndex(static_cast<BehaviorFile *>(getParentFile())->getCharacterPropertyNameAt(bindings.at(i).variableIndex, true));
            }else{
                return bindings.at(i).variableIndex;
            }*/
            return bindings.at(i).variableIndex;
        }
    }
    return -1;
}

hkbVariableBindingSet::hkBinding::BindingType hkbVariableBindingSet::getBindingType(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index < bindings.size() && index >= 0){
        return bindings.at(index).bindingType;
    }
    return hkbVariableBindingSet::hkBinding::BINDING_TYPE_VARIABLE;
}

hkbVariableBindingSet::hkBinding::BindingType hkbVariableBindingSet::getBindingType(const QString & path) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        if (bindings.at(i).memberPath == path){
            return bindings.at(i).bindingType;
        }
    }
    return hkbVariableBindingSet::hkBinding::BINDING_TYPE_VARIABLE;
}

bool hkbVariableBindingSet::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numbinds;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "bindings"){
            numbinds = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "bindings");
            (numbinds > 0) ? index++ : NULL;
            for (auto j = 0; j < numbinds; j++, index++){
                bindings.append(hkBinding());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "memberPath"){
                        bindings.last().memberPath = reader.getElementValueAt(index);
                        checkvalue((bindings.last().memberPath != ""), "bindings.at("+QString::number(j)+").memberPath");
                    }else if (text == "variableIndex"){
                        bindings.last().variableIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "bindings.at("+QString::number(j)+").variableIndex");
                    }else if (text == "bitIndex"){
                        bindings.last().bitIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "bindings.at("+QString::number(j)+").bitIndex");
                    }else if (text == "bindingType"){
                        text = reader.getElementValueAt(index);
                        if (text == "BINDING_TYPE_VARIABLE"){
                            bindings.last().bindingType = hkBinding::BINDING_TYPE_VARIABLE;
                        }else if (text == "BINDING_TYPE_CHARACTER_PROPERTY"){
                            bindings.last().bindingType = hkBinding::BINDING_TYPE_CHARACTER_PROPERTY;
                        }else{
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'bindingType' data field contains an invalid string!\nObject Reference: "+ref);
                        }
                        break;
                    }
                }
            }
            (numbinds > 0) ? index-- : NULL;
        }else if (text == "indexOfBindingToEnable"){
            indexOfBindingToEnable = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "indexOfBindingToEnable");
        }
    }
    index--;
    return true;
}

bool hkbVariableBindingSet::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QString string;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"bindings", QString::number(bindings.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < bindings.size(); i++){
            writer->writeLine(writer->object, true);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("memberPath"), bindings.at(i).memberPath);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("variableIndex"), QString::number(bindings.at(i).variableIndex));
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("bitIndex"), QString::number(bindings.at(i).bitIndex));
            if (bindings.at(i).bindingType == hkBinding::BINDING_TYPE_VARIABLE){
                string = "BINDING_TYPE_VARIABLE";
            }else{
                string = "BINDING_TYPE_CHARACTER_PROPERTY";
            }
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("bindingType"), string);
            writer->writeLine(writer->object, false);
        }
        if (bindings.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("indexOfBindingToEnable"), QString::number(indexOfBindingToEnable));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool hkbVariableBindingSet::isVariableRefed(int variableindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        if (bindings.at(i).variableIndex == variableindex && bindings.at(i).bindingType == hkBinding::BINDING_TYPE_VARIABLE){
            return true;
        }
    }
    return false;
}

void hkbVariableBindingSet::updateVariableIndices(int index){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        if (bindings.at(i).variableIndex > index && bindings.at(i).bindingType == hkBinding::BINDING_TYPE_VARIABLE){
            bindings[i].variableIndex--;
        }
    }
}

void hkbVariableBindingSet::mergeVariableIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < bindings.size(); i++){
        if (bindings.at(i).variableIndex == oldindex && bindings.at(i).bindingType == hkBinding::BINDING_TYPE_VARIABLE){
            bindings[i].variableIndex = newindex;
        }
    }
}

bool hkbVariableBindingSet::fixMergedIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString thisvarname;
    int varindex;
    if (!getIsMerged() && dominantfile){
        //TO DO: Support character properties...
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(dominantfile->getBehaviorGraphData());
        if (recdata && domdata){
            for (auto i = 0; i < bindings.size(); i++){
                thisvarname = recdata->getVariableNameAt(bindings.at(i).variableIndex);
                varindex = domdata->getIndexOfVariable(thisvarname);
                if (varindex == -1){
                    domdata->addVariable(recdata->getVariableTypeAt(bindings.at(i).variableIndex), thisvarname);
                    varindex = domdata->getNumberOfVariables() - 1;
                }
                bindings[i].variableIndex = varindex;
            }
            setIsMerged(true);
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}

bool hkbVariableBindingSet::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    bool found;
    hkbVariableBindingSet *obj = nullptr;
    hkbBehaviorGraphData *thisdata;
    hkbBehaviorGraphData *otherdata;
    //QString thisvarname;
    QString othervarname;
    int varindex;
    //TO DO: Support character properties...
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_VARIABLE_BINDING_SET){
        obj = static_cast<hkbVariableBindingSet *>(recessiveObject);
        thisdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        otherdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(obj->getParentFile())->getBehaviorGraphData());
        if (thisdata && otherdata){
            for (auto i = 0; i < obj->bindings.size(); i++){
                found = false;
                for (auto j = 0; j < bindings.size(); j++){
                    if (bindings.at(j).memberPath == obj->bindings.at(i).memberPath){
                        found = true;
                    }
                }
                if (!found){
                    othervarname = otherdata->getVariableNameAt(obj->bindings.at(i).variableIndex);
                    varindex = thisdata->getIndexOfVariable(othervarname);
                    if (varindex == -1){
                        thisdata->addVariable(otherdata->getVariableTypeAt(obj->bindings.at(i).variableIndex), othervarname);
                        varindex = thisdata->getNumberOfVariables() - 1;
                        obj->bindings[i].variableIndex = varindex;
                        bindings.append(obj->bindings.at(i));
                    }
                }
            }
        }else{
            return false;
        }
    }else{
        return false;
    }
    setIsMerged(true);
    return true;
}

bool hkbVariableBindingSet::link(){
    return true;
}

QString hkbVariableBindingSet::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (bindings.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": bindings is empty!");
    }else{
        for (auto i = 0; i < bindings.size(); i++){
            if (bindings.at(i).memberPath == ""){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": memberPath is null string!");
            }
            if (bindings.at(i).bindingType == hkBinding::BINDING_TYPE_VARIABLE && bindings.at(i).variableIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfVariables()){
                //isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": variableIndex at "+QString::number(i)+" out of range! Setting to last variable index!");
                bindings[i].variableIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfVariables() - 1;
            }
            /*if (bindings.at(i).bindingType == hkBinding::BINDING_TYPE_CHARACTER_PROPERTY && bindings.at(i).variableIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfCharacterProperties()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": variableIndex at "+QString::number(i)+" out of range!");
            }*/
        }
    }
    setDataValidity(isvalid);
    return errors;
}

hkbVariableBindingSet::~hkbVariableBindingSet(){
    refCount--;
}
