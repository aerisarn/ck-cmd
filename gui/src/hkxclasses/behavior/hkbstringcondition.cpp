#include "hkbstringcondition.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbStringCondition::refCount = 0;

const QString hkbStringCondition::classname = "hkbStringCondition";

hkbStringCondition::hkbStringCondition(BehaviorFile *parent, const QString &string, long ref)
    : HkxObject(parent, ref),
      conditionString(string)
{
    setType(HKB_STRING_CONDITION, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbStringCondition::getClassname(){
    return classname;
}

QString hkbStringCondition::getExpression() const{
    std::lock_guard <std::mutex> guard(mutex);
    return conditionString;
}

bool hkbStringCondition::readData(const HkxXmlReader & reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "conditionString"){
            conditionString = reader.getElementValueAt(index);
            checkvalue((conditionString != ""), "conditionString");
        }
    }
    index--;
    return true;
}

bool hkbStringCondition::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("conditionString"), conditionString);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

QString hkbStringCondition::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    if (conditionString == ""){
        setDataValidity(false);
        return QString(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid 'conditionstring': "+conditionString+"!");
    }
    setDataValidity(true);
    return QString();
}

hkbStringCondition::~hkbStringCondition(){
    refCount--;
}
