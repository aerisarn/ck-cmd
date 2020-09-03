#include "hkbexpressioncondition.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbExpressionCondition::refCount = 0;

const QString hkbExpressionCondition::classname = "hkbExpressionCondition";

hkbExpressionCondition::hkbExpressionCondition(BehaviorFile *parent, const QString &string, long ref)
    : HkxObject(parent, ref),
      expression(string)
{
    setType(HKB_EXPRESSION_CONDITION, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbExpressionCondition::getClassname(){
    return classname;
}

QString hkbExpressionCondition::getExpression() const{
    std::lock_guard <std::mutex> guard(mutex);
    return expression;
}

bool hkbExpressionCondition::readData(const HkxXmlReader & reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "expression"){
            expression = reader.getElementValueAt(index);
            if (expression == ""){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\nFailed to properly read 'expression' data field!\nObject Reference: "+ref);
            }
        }
    }
    index--;
    return true;
}

bool hkbExpressionCondition::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("expression"), expression);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

void hkbExpressionCondition::setExpression(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    expression = value;
}

QString hkbExpressionCondition::evaluateDataValidity(){ //TO DO...
    std::lock_guard <std::mutex> guard(mutex);
    if (expression == ""){
        setDataValidity(false);
        return QString(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid "+expression+"!");
    }else{
        setDataValidity(true);
        return QString();
    }
}

hkbExpressionCondition::~hkbExpressionCondition(){
    refCount--;
}
