#include "hkaskeleton.h"
#include "src/hkxclasses/hksimplelocalframe.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/skeletonfile.h"

using namespace UI;

uint hkaSkeleton::refCount = 0;

const QString hkaSkeleton::classname = "hkaSkeleton";

hkaSkeleton::hkaSkeleton(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKA_SKELETON, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkaSkeleton::getClassname(){
    return classname;
}

QStringList hkaSkeleton::getBoneNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    QStringList list;
    for (auto i = 0; i < bones.size(); i++){
        list.append(bones.at(i).name);
    }
    return list;
}

QString hkaSkeleton::getLocalFrameName(int boneIndex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        if (boneIndex == localFrames.at(i).boneIndex && localFrames.at(i).localFrame.data() && localFrames.at(i).localFrame->getSignature() == HK_SIMPLE_LOCAL_FRAME){
            return static_cast<hkSimpleLocalFrame *>(localFrames.at(i).localFrame.data())->getName();
        }
    }
    return "";
}

bool hkaSkeleton::addLocalFrame(const QString & name){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        if (localFrames.at(i).localFrame.data() && localFrames.at(i).localFrame->getSignature() == HK_SIMPLE_LOCAL_FRAME){
            if (static_cast<hkSimpleLocalFrame *>(localFrames[i].localFrame.data())->getName() == name){
                return false;
            }
        }
    }
    localFrames.append(hkLocalFrame());
    localFrames.last().localFrame = HkxSharedPtr(new hkSimpleLocalFrame(getParentFile(), name));
    return true;
}

void hkaSkeleton::setLocalFrameName(int boneIndex, const QString & name){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        if (boneIndex == localFrames.at(i).boneIndex && localFrames.at(i).localFrame.data() && localFrames.at(i).localFrame->getSignature() == HK_SIMPLE_LOCAL_FRAME){
            if (name == ""){
                dynamic_cast<SkeletonFile *>(getParentFile())->localFrames.removeAll(localFrames.at(i).localFrame);
                localFrames.removeAt(i);
            }else{
                static_cast<hkSimpleLocalFrame *>(localFrames[i].localFrame.data())->setName(name);
            }
            return;
        }
    }
}

QString hkaSkeleton::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkaSkeleton::removeLocalFrame(int boneIndex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        if (boneIndex == localFrames.at(i).boneIndex){
            static_cast<SkeletonFile *>(getParentFile())->localFrames.removeAll(localFrames.at(i).localFrame);
            localFrames.removeAt(i);
            return true;
        }
    }
    return false;
}

bool hkaSkeleton::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    int numElems = 0;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    int numtrans = 0;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "name"){
            name = reader.getElementValueAt(index);
            if (name == ""){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'name' data field!\nObject Reference: "+ref);
            }
        }else if (text == "parentIndices"){
            int numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'parentIndices' data!\nObject Reference: "+ref);
                return false;
            }
            if (numElems > 0 && !readIntegers(reader.getElementValueAt(index), parentIndices)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'parentIndices' data!\nObject Reference: "+ref);
                return false;
            }
        }else if (text == "bones"){
            numtrans = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                return false;
            }
            for (auto j = 0; j < numtrans; j++){
                bones.append(hkBone());
                while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                    if (reader.getNthAttributeValueAt(index, 0) == "name"){
                        bones.last().name = reader.getElementValueAt(index);
                        if (bones.last().name == ""){
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'name' data field!\nObject Reference: "+ref);
                        }
                    }else if (reader.getNthAttributeValueAt(index, 0) == "lockTranslation"){
                        bones.last().lockTranslation = toBool(reader.getElementValueAt(index), &ok);
                        if (!ok){
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'lockTranslation' data field!\nObject Reference: "+ref);
                        }
                        index++;
                        break;
                    }
                    index++;
                }
            }
        }else if (text == "referencePose"){
            referencePose = reader.getElementValueAt(index);
            if (referencePose == ""){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'referencePose' data field!\nObject Reference: "+ref);
            }
        }else if (text == "referenceFloats"){
            int numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'referenceFloats' data!\nObject Reference: "+ref);
                return false;
            }
            if (numElems > 0 && !readDoubles(reader.getElementValueAt(index), referenceFloats)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'referenceFloats' data!\nObject Reference: "+ref);
                return false;
            }
        }else if (text == "floatSlots"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'floatSlots' data!\nObject Reference: "+ref);
                return false;
            }
            index++;
            numElems = numElems + index;
            for (; index < numElems; index++){
                if (reader.getElementNameAt(index) != "hkcstring" || index >= reader.getNumElements()){
                    LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'eventNames' data!\nObject Reference: "+ref);
                    return false;
                }
                floatSlots.append(reader.getElementValueAt(index));
            }
            continue;
        }else if (text == "localFrames"){
            numtrans = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            if (!ok){
                return false;
            }
            for (auto j = 0; j < numtrans; j++){
                localFrames.append(hkLocalFrame());
                while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
                    if (text == "localFrame"){
                        if (!localFrames.last().localFrame.readShdPtrReference(index, reader)){
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'localFrame' reference!\nObject Reference: "+ref);
                        }
                    }else if (reader.getNthAttributeValueAt(index, 0) == "lockTranslation"){
                        localFrames.last().boneIndex = reader.getElementValueAt(index).toInt(&ok);
                        if (!ok){
                            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'boneIndex' data field!\nObject Reference: "+ref);
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

bool hkaSkeleton::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QString bonesS;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("name"), name);
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"parentIndices", QString::number(parentIndices.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < parentIndices.size(); i++, j++){
            bonesS.append(QString::number(parentIndices.at(i), char('f'), 6));
            (!(j % 16)) ? bonesS.append("\n") : bonesS.append(" ");
        }
        if (parentIndices.size() > 0){
            if (bonesS.endsWith(" \0")){
                bonesS.remove(bonesS.lastIndexOf(" "), 1);
            }
            writer->writeLine(bonesS);
            writer->writeLine(writer->parameter, false);
        }
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"bones", QString::number(bones.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < bones.size(); i++){
            writer->writeLine(writer->object, true);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("name"), bones.at(i).name);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("variableIndex"), getBoolAsString(bones.at(i).lockTranslation));
            writer->writeLine(writer->object, false);
        }
        if (bones.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        bonesS = "";
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"referenceFloats", QString::number(referenceFloats.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < referenceFloats.size(); i++, j++){
            bonesS.append(QString::number(referenceFloats.at(i), char('f'), 6));
            (!(j % 16)) ? bonesS.append("\n") : bonesS.append(" ");
        }
        if (referenceFloats.size() > 0){
            if (bonesS.endsWith(" \0")){
                bonesS.remove(bonesS.lastIndexOf(" "), 1);
            }
            writer->writeLine(bonesS);
            writer->writeLine(writer->parameter, false);
        }
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"floatSlots", QString::number(floatSlots.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < floatSlots.size(); i++){
            writer->writeLine(writer->string, QStringList(), QStringList(), floatSlots.at(i));
        }
        if (floatSlots.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        QString refString = "null";
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"localFrames", QString::number(localFrames.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < localFrames.size(); i++){
            writer->writeLine(writer->object, true);
            if (localFrames.at(i).localFrame.data()){
                refString = localFrames.at(i).localFrame->getReferenceString();
            }else{
                refString = "null";
            }
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("localFrame"), refString);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("boneIndex"), QString::number(localFrames.at(i).boneIndex));
            writer->writeLine(writer->object, false);
        }
        if (localFrames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < localFrames.size(); i++){
            if (localFrames.at(i).localFrame.data() && !localFrames.at(i).localFrame->write(writer)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'localFrame' at: "+QString::number(i)+"!!!");
            }
        }
    }
    return true;
}

bool hkaSkeleton::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    for (auto i = 0; i < localFrames.size(); i++){
        ptr = static_cast<SkeletonFile *>(getParentFile())->findLocalFrame(localFrames.at(i).localFrame.getShdPtrReference());
        if (!ptr || !ptr->data()){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'localFrames' data field!\nObject Name: "+name);
            setDataValidity(false);
        }else if (!(*ptr).data() || (*ptr)->getSignature() != HK_SIMPLE_LOCAL_FRAME){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'localFrames' data field is linked to invalid child!\nObject Name: "+name);
            setDataValidity(false);
            localFrames[i].localFrame = *ptr;
        }else{
            localFrames[i].localFrame = *ptr;
        }
    }
    return true;
}

void hkaSkeleton::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        localFrames[i].localFrame = HkxSharedPtr();
    }
}

QString hkaSkeleton::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < localFrames.size(); i++){
        if (!localFrames.at(i).localFrame.data() || localFrames.at(i).localFrame->getSignature() != HK_SIMPLE_LOCAL_FRAME){
            setDataValidity(false);
            return QString();
        }
    }
    setDataValidity(true);
    return QString();
}

hkaSkeleton::~hkaSkeleton(){
    refCount--;
}
