#include "hkbexpressiondataarray.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbExpressionDataArray::refCount = 0;

const QString hkbExpressionDataArray::classname = "hkbExpressionDataArray";

const QStringList hkbExpressionDataArray::EventMode = {
    "EVENT_MODE_SEND_ONCE",
    "EVENT_MODE_SEND_ON_TRUE",
    "EVENT_MODE_SEND_ON_FALSE_TO_TRUE",
    "EVENT_MODE_SEND_EVERY_FRAME_ONCE_TRUE"
};

hkbExpressionDataArray::hkbExpressionDataArray(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_EXPRESSION_DATA_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

QString hkbExpressionDataArray::getClassname(){
    return classname;
}

void hkbExpressionDataArray::addExpression(const QString & exp, ExpressionEventMode mode){
    std::lock_guard <std::mutex> guard(mutex);
    expressionsData.append(hkExpression(exp, mode));
}

void hkbExpressionDataArray::removeExpression(const QString & exp){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < expressionsData.size(); i++){
        (expressionsData.at(i).expression == exp) ? expressionsData.removeAt(i) : NULL;
    }
}

void hkbExpressionDataArray::removeExpression(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index < expressionsData.size() && index > -1) ? expressionsData.removeAt(index) : NULL;
}

bool hkbExpressionDataArray::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numexpressionsData;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "expressionsData"){
            numexpressionsData = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "expressionsData");
            (numexpressionsData > 0) ? index++ : NULL;
            for (auto j = 0; j < numexpressionsData; j++, index++){
                expressionsData.append(hkExpression());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "expression"){
                        expressionsData.last().expression = reader.getElementValueAt(index);
                        checkvalue((expressionsData.last().expression != ""), "expressionsData.at("+QString::number(j)+").expression");
                    }else if (text == "assignmentVariableIndex"){
                        expressionsData.last().assignmentVariableIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "expressionsData.at("+QString::number(j)+").assignmentVariableIndex");
                    }else if (text == "assignmentEventIndex"){
                        expressionsData.last().assignmentEventIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "expressionsData.at("+QString::number(j)+").assignmentEventIndex");
                    }else if (text == "eventMode"){
                        expressionsData.last().eventMode = reader.getElementValueAt(index);
                        checkvalue(EventMode.contains(expressionsData.last().eventMode), "expressionsData.at("+QString::number(j)+").eventMode");
                        break;
                    }
                }
            }
            (numexpressionsData > 0) ? index-- : NULL;
        }
    }
    index--;
    return true;
}

bool hkbExpressionDataArray::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"expressionsData", QString::number(expressionsData.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < expressionsData.size(); i++){
            writer->writeLine(writer->object, true);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("expression"), expressionsData.at(i).expression, true);
            writedatafield("assignmentVariableIndex", QString::number(expressionsData.at(i).assignmentVariableIndex));
            writedatafield("assignmentEventIndex", QString::number(expressionsData.at(i).assignmentEventIndex));
            writedatafield("eventMode", expressionsData.at(i).eventMode);
            writer->writeLine(writer->object, false);
        }
        if (expressionsData.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool hkbExpressionDataArray::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < expressionsData.size(); i++){
        if (expressionsData.at(i).assignmentEventIndex == eventindex){
            return true;
        }
    }
    return false;
}

void hkbExpressionDataArray::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString thiseventname;
    int eventindex;
    if (!getIsMerged() && dominantfile){
        //TO DO: Support character properties...
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(dominantfile->getBehaviorGraphData());
        if (recdata && domdata){
            for (auto i = 0; i < expressionsData.size(); i++){
                thiseventname = recdata->getEventNameAt(expressionsData.at(i).assignmentEventIndex);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                expressionsData[i].assignmentEventIndex = eventindex;
            }
            setIsMerged(true);
        }
    }
}
bool hkbExpressionDataArray::hkExpression::operator==(const hkExpression & other) const{
    if (expression != other.expression || eventMode != other.eventMode){
        return false;
    }
    return true;
}

bool hkbExpressionDataArray::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    bool found;
    hkbExpressionDataArray *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_EXPRESSION_DATA_ARRAY){
        obj = static_cast<hkbExpressionDataArray *>(recessiveObject);
        for (auto i = 0; i < obj->expressionsData.size(); i++){
            found = false;
            for (auto j = 0; j < expressionsData.size(); j++){
                found = false;
                if (expressionsData.at(j) == obj->expressionsData.at(i)){
                    found = true;
                    break;
                }
            }
            if (!found){
                expressionsData.append(obj->expressionsData.at(i));
            }
        }
        setIsMerged(true);
        return true;
    }else{
        return false;
    }
}

void hkbExpressionDataArray::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < expressionsData.size(); i++){
        (expressionsData.at(i).assignmentEventIndex > eventindex) ? expressionsData[i].assignmentEventIndex-- : NULL;
    }
}

bool hkbExpressionDataArray::link(){
    return true;
}

QString hkbExpressionDataArray::getExpressionAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    QString exp;
    (index < expressionsData.size() && index >= 0) ? exp = expressionsData.at(index).expression : NULL;
    return exp;
}

QString hkbExpressionDataArray::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (expressionsData.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": expression is empty!");
    }else{
        for (auto i = 0; i < expressionsData.size(); i++){
            if (expressionsData.at(i).expression == ""){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": expression is null string!");
            }
            if (expressionsData.at(i).assignmentVariableIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfVariables()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": assignmentVariableIndex at "+QString::number(i)+" out of range!");
            }
            if (expressionsData.at(i).assignmentEventIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": assignmentEventIndex at "+QString::number(i)+" out of range!");
            }
            if (!EventMode.contains(expressionsData.at(i).eventMode)){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid eventMode!");
            }
        }
    }
    setDataValidity(isvalid);
    return errors;
}

hkbExpressionDataArray::~hkbExpressionDataArray(){
    refCount--;
}

