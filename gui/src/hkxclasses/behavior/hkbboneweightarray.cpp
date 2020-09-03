#include "hkbboneweightarray.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint hkbBoneWeightArray::refCount = 0;

const QString hkbBoneWeightArray::classname = "hkbBoneWeightArray";

hkbBoneWeightArray::hkbBoneWeightArray(HkxFile *parent, long ref, int size)
    : HkDynamicObject(parent, ref)
{
    setType(HKB_BONE_WEIGHT_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    //boneWeights.reserve(size);
    for (auto i = 0; i < size; i++){
        boneWeights.append(0);
    }
    refCount++;
}

void hkbBoneWeightArray::setBoneWeightAt(int index, int value){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < boneWeights.size() && value < static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()) ? boneWeights[index] = value, getParentFile()->setIsChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set boneWeights!");
}

void hkbBoneWeightArray::copyBoneWeights(const hkbBoneWeightArray *other){
    std::lock_guard <std::mutex> guard(mutex);
    boneWeights.resize(other->getBoneWeightsSize());
    for (auto i = 0; i < boneWeights.size(); i++){
        boneWeights[i] = other->boneWeights.at(i);
    }
}

qreal hkbBoneWeightArray::getBoneWeightAt(int index, bool * ok) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < boneWeights.size()){
        (ok) ? *ok = true : NULL;
        return boneWeights.at(index);
    }
    (ok) ? *ok = false : NULL;
    return -1;
}

int hkbBoneWeightArray::getBoneWeightsSize() const{
    std::lock_guard <std::mutex> guard(mutex);
    return boneWeights.size();
}

const QString hkbBoneWeightArray::getClassname(){
    return classname;
}

bool hkbBoneWeightArray::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "boneWeights"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "boneWeights");
            checkvalue((numElems > 0 && !readDoubles(reader.getElementValueAt(index), boneWeights)), "boneWeights");
        }
    }
    index--;
    return true;
}

bool hkbBoneWeightArray::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
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
        list2 = QStringList({"boneWeights", QString::number(boneWeights.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < boneWeights.size(); i++, j++){
            bones.append(QString::number(boneWeights.at(i), char('f'), 6));
            (!(j % 16)) ? bones.append("\n") : bones.append(" ");
        }
        if (boneWeights.size() > 0){
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

QVector<HkxObject *> hkbBoneWeightArray::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector <HkxObject *> list;
    (getVariableBindingSetData()) ? list.append(getVariableBindingSetData()) : NULL;
    return list;
}

bool hkbBoneWeightArray::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!");
    }
    return true;
}

hkbBoneWeightArray::~hkbBoneWeightArray(){
    refCount--;
}
