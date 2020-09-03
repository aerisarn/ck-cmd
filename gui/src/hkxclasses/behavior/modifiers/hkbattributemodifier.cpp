#include "hkbattributemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbAttributeModifier::refCount = 0;

const QString hkbAttributeModifier::classname = "hkbAttributeModifier";

hkbAttributeModifier::hkbAttributeModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true)
{
    setType(HKB_ATTRIBUTE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "AttributeModifier_"+QString::number(refCount);
}

const QString hkbAttributeModifier::getClassname(){
    return classname;
}

QString hkbAttributeModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbAttributeModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "variableBindingSet"){
            if (!getVariableBindingSet().readShdPtrReference(index, reader)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'variableBindingSet' reference!\nObject Reference: "+ref);
            }
        }else if (text == "userData"){
            userData = reader.getElementValueAt(index).toULong(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'userData' data field!\nObject Reference: "+ref);
            }
        }else if (text == "name"){
            name = reader.getElementValueAt(index);
            if (name == ""){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'name' data field!\nObject Reference: "+ref);
            }
        }else if (text == "enable"){
            enable = toBool(reader.getElementValueAt(index), &ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'enable' data field!\nObject Reference: "+ref);
            }
        }else if (text == "assignments"){
            int numlegs = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                return false;
            }
            for (auto j = 0; j < numlegs; j++){
                assignments.append(hkAttributeMod());
                while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                    if (text == "attributeIndex"){
                        assignments.last().attributeIndex = reader.getElementValueAt(index).toDouble(&ok);
                        if (!ok){
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attributeIndex' data field!\nObject Reference: "+ref);
                        }
                    }else if (text == "attributeValue"){
                        assignments.last().attributeValue = reader.getElementValueAt(index).toDouble(&ok);
                        if (!ok){
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'attributeValue' data field!\nObject Reference: "+ref);
                        }
                        index++;
                        break;
                    }
                    index++;
                }
            }
        }
        index++;
    }
    index--;
    return true;
}

bool hkbAttributeModifier::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writedatafield("variableBindingSet", refString);
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"assignments", QString::number(assignments.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < assignments.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("attributeIndex", QString::number(assignments.at(i).attributeIndex));
            writedatafield("attributeValue", QString::number(assignments.at(i).attributeValue, char('f'), 6));
            writer->writeLine(writer->object, false);
        }
        if (assignments.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool hkbAttributeModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbAttributeModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbAttributeModifier::evaluateDataValidity(){  //TO DO...
    std::lock_guard <std::mutex> guard(mutex);
    if (HkDynamicObject::evaluateDataValidity() != "" || (name == "")){
        setDataValidity(false);
        return QString();
    }else{
        setDataValidity(true);
        return QString();
    }
}

hkbAttributeModifier::~hkbAttributeModifier(){
    refCount--;
}
