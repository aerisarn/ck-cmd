#include "hkbvariablevalueset.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/filetypes/characterfile.h"

using namespace UI;

uint hkbVariableValueSet::refCount = 0;

const QString hkbVariableValueSet::classname = "hkbVariableValueSet";

hkbVariableValueSet::hkbVariableValueSet(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_VARIABLE_VALUE_SET, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbVariableValueSet::getClassname(){
    return classname;
}

int hkbVariableValueSet::getWordVariableAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (wordVariableValues.size() > index && index > -1){
        return wordVariableValues.at(index);
    }
    return -1;
}

HkxObject * hkbVariableValueSet::getVariantVariableValueAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (variantVariableValues.size() > index && index > -1){
        return variantVariableValues.at(index).data();
    }
    return nullptr;
}

hkQuadVariable hkbVariableValueSet::getQuadVariableValueAt(int index, bool *ok){
    std::lock_guard <std::mutex> guard(mutex);
    if (quadVariableValues.size() > index && index > -1){
        (ok) ? *ok = true : NULL;
        return quadVariableValues.at(index);
    }
    (ok) ? *ok = false : NULL;
    return hkQuadVariable();
}

void hkbVariableValueSet::setQuadVariableValueAt(int index, const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    (quadVariableValues.size() > index && index > -1) ? quadVariableValues.replace(index, value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'setQuadVariableValueAt' failed!");
}

void hkbVariableValueSet::setWordVariableAt(int index, int value){
    std::lock_guard <std::mutex> guard(mutex);
    (wordVariableValues.size() > index && index > -1) ? wordVariableValues.replace(index, value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'setWordVariableAt' failed!");
}

void hkbVariableValueSet::removeWordVariableValueAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index > -1 && index < wordVariableValues.size()) ? wordVariableValues.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'removeWordVariableValueAt' failed!");
}

void hkbVariableValueSet::removeQuadVariableValueAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index > -1 && index < quadVariableValues.size()) ? quadVariableValues.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'removeQuadVariableValueAt' failed!");
}

void hkbVariableValueSet::removeVariantVariableValueAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index > -1 && index < variantVariableValues.size()) ? variantVariableValues.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'removeVariantVariableValueAt' failed!");
}

void hkbVariableValueSet::addQuadVariableValue(const hkQuadVariable &value){
    std::lock_guard <std::mutex> guard(mutex);
    quadVariableValues.append(value), setIsFileChanged(true);
}

void hkbVariableValueSet::addWordVariableValue(int value){
    std::lock_guard <std::mutex> guard(mutex);
    wordVariableValues.append(value), setIsFileChanged(true);
}

bool hkbVariableValueSet::readData(const HkxXmlReader &reader, long & index){
    bool ok;
    auto numElems = 0;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "wordVariableValues"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
            index += 2;
            numElems = numElems*2 + index;
            for (; index < numElems; index += 2){
                if (index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordVariableValues' data!\nObject Reference: "+ref);
                    return false;
                }
                wordVariableValues.append(reader.getElementValueAt(index).toInt(&ok));
                if (!ok){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'wordVariableValues' data!\nObject Reference: "+ref);
                    return false;
                }
            }
            index--;
            continue;
        }else if (text == "quadVariableValues"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'quadVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
            if (numElems > 0 && !readMultipleVector4(reader.getElementValueAt(index), quadVariableValues)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'quadVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
        }else if (text == "variantVariableValues"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variantVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
            if (numElems > 0 && !readReferences(reader.getElementValueAt(index), variantVariableValues)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variantVariableValues' data!\nObject Reference: "+ref);
                return false;
            }
        }
        index++;
    }
    index--;
    return true;
}

bool hkbVariableValueSet::write(HkxXMLWriter *writer){
    if (!writer){
        return false;
    }
    if (!getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"wordVariableValues", QString::number(wordVariableValues.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < wordVariableValues.size(); i++){
            writer->writeLine(writer->object, true);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("value"), QString::number(wordVariableValues.at(i)));
            writer->writeLine(writer->object, false);
        }
        if (wordVariableValues.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"quadVariableValues", QString::number(quadVariableValues.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < quadVariableValues.size(); i++){
            writer->writeLine(quadVariableValues[i].getValueAsString());
        }
        if (quadVariableValues.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"variantVariableValues", QString::number(variantVariableValues.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        QString refs;
        for (auto i = 0, j = 1; i < variantVariableValues.size(); i++, j++){
            refs.append(variantVariableValues.at(i)->getReferenceString());
            (!(j % 16)) ? refs.append("\n") : refs.append(" ");
        }
        if (variantVariableValues.size() > 0){
            if (refs.endsWith(" \0")){
                refs.remove(refs.lastIndexOf(" "), 1);
            }
            writer->writeLine(refs);
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < variantVariableValues.size(); i++){
            if (variantVariableValues.at(i).data()){
                variantVariableValues.at(i)->write(writer);
            }
        }
    }
    return true;
}

bool hkbVariableValueSet::merge(HkxObject *recessiveobj){
    if (recessiveobj){
        if (getSignature() == recessiveobj->getSignature()){
            //TO DO???
        }else{
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": merge()!\n'recessiveobj' is not the correct type!");
        }
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": merge()!\n'recessiveobj' is nullptr!");
    }
    return true;
}

bool hkbVariableValueSet::link(){
    HkxFile *file = nullptr;
    for (auto i = 0; i < variantVariableValues.size(); i++){
        file = dynamic_cast<BehaviorFile *>(getParentFile());
        if (file){
            auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(variantVariableValues.at(i).getShdPtrReference());
            if (!ptr || !ptr->data()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variantVariableValues' data field!");
                setDataValidity(false);
            }else if ((*ptr)->getSignature() != HKB_BONE_WEIGHT_ARRAY){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'variantVariableValues' data field is linked to invalid child!");
                setDataValidity(false);
                variantVariableValues[i] = *ptr;
            }else{
                variantVariableValues[i] = *ptr;
            }
        }else{
            file = dynamic_cast<CharacterFile *>(getParentFile());
            if (file){
                auto ptr = static_cast<CharacterFile *>(getParentFile())->findCharacterPropertyValues(variantVariableValues.at(i).getShdPtrReference());
                if (!ptr || !ptr->data()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variantVariableValues' data field!");
                    setDataValidity(false);
                }else if ((*ptr)->getSignature() != HKB_BONE_WEIGHT_ARRAY){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'variantVariableValues' data field is linked to invalid child!");
                    setDataValidity(false);
                    variantVariableValues[i] = *ptr;
                }else{
                    variantVariableValues[i] = *ptr;
                }
            }
        }
    }
    return true;
}

QString hkbVariableValueSet::evaluateDataValidity(){
    for (auto i = 0; i < variantVariableValues.size(); i++){
        if (!variantVariableValues.at(i).data() || variantVariableValues.at(i)->getSignature() != HKB_BONE_WEIGHT_ARRAY){
            setDataValidity(false);
            return QString();
        }
    }
    setDataValidity(true);
    return QString();
}

hkbVariableValueSet::~hkbVariableValueSet(){
    refCount--;
}
