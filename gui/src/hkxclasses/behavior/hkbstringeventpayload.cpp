#include "hkbstringeventpayload.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbStringEventPayload::refCount = 0;

const QString hkbStringEventPayload::classname = "hkbStringEventPayload";

hkbStringEventPayload::hkbStringEventPayload(HkxFile *parent, const QString &string, long ref)
    : HkxObject(parent, ref),
      data(string)
{
    setType(HKB_STRING_EVENT_PAYLOAD, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbStringEventPayload::getClassname(){
    return classname;
}

bool hkbStringEventPayload::readData(const HkxXmlReader & reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "data"){
            data = reader.getElementValueAt(index);
            checkvalue((data != ""), "data");
        }
    }
    index--;
    return true;
}

bool hkbStringEventPayload::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("data"), data);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

void hkbStringEventPayload::setData(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (data != value) ? data = value, getParentFile()->setIsChanged(true) : LogFile::writeToLog(getClassname()+": 'data' was not set!");
}

QString hkbStringEventPayload::getData() const{
    std::lock_guard <std::mutex> guard(mutex);
    return data;
}

QString hkbStringEventPayload::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    if (data == ""){
        setDataValidity(false);
        return QString(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid 'data': "+data+"!");
    }
    setDataValidity(true);
    return QString();
}

hkbStringEventPayload::~hkbStringEventPayload(){
    refCount--;
}
