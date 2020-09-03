#include "hksimplelocalframe.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/skeletonfile.h"

using namespace UI;

uint hkSimpleLocalFrame::refCount = 0;

const QString hkSimpleLocalFrame::classname = "hkSimpleLocalFrame";

hkSimpleLocalFrame::hkSimpleLocalFrame(HkxFile *parent, const QString &string, long ref)
    : HkxObject(parent, ref),
      name(string)
{
    setType(HK_SIMPLE_LOCAL_FRAME, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkSimpleLocalFrame::getClassname(){
    return classname;
}

QString hkSimpleLocalFrame::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkSimpleLocalFrame::readData(const HkxXmlReader & reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    while (index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "name"){
            name = reader.getElementValueAt(index);
            if (name == ""){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'name' data field!\nObject Reference: "+ref);
            }
        }
        index++;
    }
    index--;
    return true;
}

bool hkSimpleLocalFrame::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("transform"), "(0.000000 0.000000 0.000000)(0.000000 0.000000 0.000000)(0.000000 0.000000 0.000000)(0.000000 0.000000 0.000000)");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"children", "0"});
        writer->writeLine(writer->parameter, list1, list2, " ");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("parentFrame"), "null");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("group"), "null");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("name"), name);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        /*for (auto i = 0; i < children.size(); i++){
            if (children.at(i).data()){
                children.at(i)->write(writer);
            }
        }
        if (parentFrame.data()){
            parentFrame->write(writer);
        }
        if (group.data()){
            group->write(writer);
        }*/
    }
    return true;
}

void hkSimpleLocalFrame::setName(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    name = value;
}

QString hkSimpleLocalFrame::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    if (name == ""){
        setDataValidity(false);
        return QString(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid 'name': "+name+"!");
    }
    setDataValidity(true);
    return QString();
}

hkSimpleLocalFrame::~hkSimpleLocalFrame(){
    refCount--;
}
