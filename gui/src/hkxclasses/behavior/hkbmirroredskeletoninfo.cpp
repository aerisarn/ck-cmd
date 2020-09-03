#include "hkbmirroredskeletoninfo.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/projectfile.h"

using namespace UI;

uint hkbMirroredSkeletonInfo::refCount = 0;

const QString hkbMirroredSkeletonInfo::classname = "hkbMirroredSkeletonInfo";

hkbMirroredSkeletonInfo::hkbMirroredSkeletonInfo(HkxFile *parent, long ref, int size)
    : HkxObject(parent, ref),
      mirrorAxis(1, 0, 0, 0)
{
    setType(HKB_MIRRORED_SKELETON_INFO, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
    //bonePairMap.reserve(size);
    for (auto i = 0; i < size; i++){
        bonePairMap.append(0);
    }
}

const QString hkbMirroredSkeletonInfo::getClassname(){
    return classname;
}

bool hkbMirroredSkeletonInfo::readData(const HkxXmlReader &reader, long & index){
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
        if (text == "mirrorAxis"){
            mirrorAxis = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "userData");
        }else if (text == "bonePairMap"){
            numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "bonePairMap");
            checkvalue((numElems > 0 && !readIntegers(reader.getElementValueAt(index), bonePairMap)), "bonePairMap");
        }
    }
    index--;
    return true;
}

bool hkbMirroredSkeletonInfo::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QString bones;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("mirrorAxis"), mirrorAxis.getValueAsString());
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"bonePairMap", QString::number(bonePairMap.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0, j = 1; i < bonePairMap.size(); i++, j++){
            bones.append(QString::number(bonePairMap.at(i)));
            (!(j % 16)) ? bones.append("\n") : bones.append(" ");
        }
        if (bonePairMap.size() > 0){
            if (bones.endsWith(" \0")){
                bones.remove(bones.lastIndexOf(" "), 1);
            }
            writer->writeLine(bones);
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool hkbMirroredSkeletonInfo::link(){
    return true;
}

QString hkbMirroredSkeletonInfo::evaluateDataValidity(){    //TO DO...
    std::lock_guard <std::mutex> guard(mutex);
    setDataValidity(true);
    return QString();
}

hkbMirroredSkeletonInfo::~hkbMirroredSkeletonInfo(){
    refCount--;
}
