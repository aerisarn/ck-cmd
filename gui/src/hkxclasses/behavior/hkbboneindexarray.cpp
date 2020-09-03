#include "hkbboneindexarray.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint hkbBoneIndexArray::refCount = 0;

const QString hkbBoneIndexArray::classname = "hkbBoneIndexArray";

hkbBoneIndexArray::hkbBoneIndexArray(HkxFile *parent, long ref)
    : HkDynamicObject(parent, ref)
{
    setType(HKB_BONE_INDEX_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbBoneIndexArray::getClassname(){
    return classname;
}

bool hkbBoneIndexArray::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numElems;
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
        }else if (text == "boneIndices"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "boneIndices");
            checkvalue((numElems > 0 && !readIntegers(reader.getElementValueAt(index), boneIndices)), "boneIndices");
        }
    }
    index--;
    return true;
}

bool hkbBoneIndexArray::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QString bones;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("variableBindingSet"), refString);
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"boneIndices", QString::number(boneIndices.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < boneIndices.size(); i++, j++){
            bones.append(QString::number(boneIndices.at(i)));
            (!(j % 16)) ? bones.append("\n") : bones.append(" ");
        }
        if (boneIndices.size() > 0){
            if (bones.endsWith(" \0")){
                bones.remove(bones.lastIndexOf(" "), 1);
            }
            writer->writeLine(bones);
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

QVector<HkxObject *> hkbBoneIndexArray::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector <HkxObject *> list;
    (getVariableBindingSetData()) ? list.append(getVariableBindingSetData()) : NULL;
    return list;
}

int hkbBoneIndexArray::getNumberOfBoneIndices() const{
    std::lock_guard <std::mutex> guard(mutex);
    return boneIndices.size();
}

int hkbBoneIndexArray::getBoneIndexAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < boneIndices.size()){
        return boneIndices.at(index);
    }
    return -1;
}

void hkbBoneIndexArray::setBoneIndexAt(int index, int value){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < boneIndices.size() && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones(true)) ? boneIndices[index] = value, getParentFile()->setIsChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set boneIndices!");
}

void hkbBoneIndexArray::addBoneIndex(int indexvalue){
    std::lock_guard <std::mutex> guard(mutex);
    boneIndices.append(indexvalue), getParentFile()->setIsChanged(true);
}

void hkbBoneIndexArray::removeBoneIndexAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < boneIndices.size()) ? boneIndices.removeAt(index), getParentFile()->setIsChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to remove boneIndices!");
}

bool hkbBoneIndexArray::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!");
    }
    return true;
}

hkbBoneIndexArray::~hkbBoneIndexArray(){
    refCount--;
}
