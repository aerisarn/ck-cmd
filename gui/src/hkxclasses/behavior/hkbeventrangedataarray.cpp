#include "hkbeventrangedataarray.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbEventRangeDataArray::refCount = 0;

const QString hkbEventRangeDataArray::classname = "hkbEventRangeDataArray";

const QStringList hkbEventRangeDataArray::hkbEventRangeData::EventRangeMode = {"EVENT_MODE_SEND_ON_ENTER_RANGE", "EVENT_MODE_SEND_WHEN_IN_RANGE"};

hkbEventRangeDataArray::hkbEventRangeDataArray(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_EVENT_RANGE_DATA_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbEventRangeDataArray::getClassname(){
    return classname;
}

void hkbEventRangeDataArray::addEventRange(const hkbEventRangeData & data){
    std::lock_guard <std::mutex> guard(mutex);
    eventData.append(data);
}

void hkbEventRangeDataArray::setEventRangeIdAt(int index, int id){
    std::lock_guard <std::mutex> guard(mutex);
    (eventData.size() > index) ? eventData[index].event.id = id : NULL;
}

int hkbEventRangeDataArray::getEventRangeIdAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    int id;
    (eventData.size() > index) ? id = eventData.at(index).event.id : id = -1;
    return id;
}

void hkbEventRangeDataArray::removeEventRange(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (eventData.size() > index) ? eventData.removeAt(index) : NULL;
}

int hkbEventRangeDataArray::getLastEventDataIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventData.size() - 1;
}

bool hkbEventRangeDataArray::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numranges;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "eventData"){
            numranges = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "eventData");
            (numranges > 0) ? index++ : NULL;
            for (auto j = 0; j < numranges; j++, index++){
                eventData.append(hkbEventRangeData());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "localTime"){
                        eventData.last().upperBound = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "eventData.at("+QString::number(j)+").upperBound");
                    }else if (text == "id"){
                        eventData.last().event.id = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "eventData.at("+QString::number(j)+").id");
                    }else if (text == "payload"){
                        checkvalue(eventData.last().event.payload.readShdPtrReference(index, reader), "eventData.at("+QString::number(j)+").payload");
                    }else if (text == "eventMode"){
                        eventData.last().eventMode = reader.getElementValueAt(index);
                        checkvalue(eventData.last().EventRangeMode.contains(eventData.last().eventMode), "eventData.at("+QString::number(j)+").eventMode");
                        break;
                    }
                }
            }
            (numranges > 0) ? index-- : NULL;
        }
    }
    index--;
    return true;
}

bool hkbEventRangeDataArray::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QString refString;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"eventData", QString::number(eventData.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < eventData.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("upperBound", QString::number(eventData.at(i).upperBound, char('f'), 6));
            writedatafield("event", "");
            writer->writeLine(writer->object, true);
            writedatafield("id", QString::number(eventData.at(i).event.id));
            if (eventData.at(i).event.payload.data()){
                refString = eventData.at(i).event.payload->getReferenceString();
            }else{
                refString = "null";
            }
            writedatafield("payload", refString);
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writedatafield("eventMode", eventData.at(i).eventMode);
            writer->writeLine(writer->object, false);
        }
        if (eventData.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < eventData.size(); i++){
            if (eventData.at(i).event.payload.data() && !eventData.at(i).event.payload->write(writer)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'payload' at"+QString::number(i)+"!!!");
            }
        }
    }
    return true;
}

int hkbEventRangeDataArray::getNumberOfRanges() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventData.size();
}

bool hkbEventRangeDataArray::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < eventData.size(); i++){
        if (eventData.at(i).event.id == eventindex){
            return true;
        }
    }
    return false;
}

void hkbEventRangeDataArray::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < eventData.size(); i++){
        (eventData.at(i).event.id > eventindex) ? eventData[i].event.id-- : NULL;
    }
}

void hkbEventRangeDataArray::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < eventData.size(); i++){
        (eventData.at(i).event.id == oldindex) ? eventData[i].event.id = newindex : NULL;
    }
}

void hkbEventRangeDataArray::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            for (auto i = 0; i < eventData.size(); i++){
                thiseventname = recdata->getEventNameAt(eventData.at(i).event.id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                eventData[i].event.id = eventindex;
            }
            setIsMerged(true);
        }
    }
}

void hkbEventRangeDataArray::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    for (auto i = 0; i < eventData.size(); i++){
        (eventData.at(i).event.payload.data()) ? eventData[i].event.payload->updateReferences(++ref) : NULL;
    }
}

QVector<HkxObject *> hkbEventRangeDataArray::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    for (auto i = 0; i < eventData.size(); i++){
        (eventData.at(i).event.payload.data()) ? list.append(eventData.at(i).event.payload.data()) : NULL;
    }
    return list;
}

bool hkbEventRangeDataArray::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    for (auto i = 0; i < eventData.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventData.at(i).event.payload.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                return false;
            }
            eventData[i].event.payload = *ptr;
        }
    }
    return true;
}

QString hkbEventRangeDataArray::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (eventData.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": eventData is empty!");
    }else{
        for (auto i = 0; i < eventData.size(); i++){
            if (eventData.at(i).event.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": id in eventData at "+QString::number(i)+" out of range! Setting to max index in range!");
                eventData[i].event.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
            }
            if (eventData.at(i).event.payload.data() && eventData.at(i).event.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid payload type! Signature: "+QString::number(eventData.at(i).event.payload->getSignature(), 16)+" Setting null value!");
                eventData[i].event.payload = HkxSharedPtr();
            }
            if (!hkbEventRangeData::EventRangeMode.contains(eventData.at(i).eventMode)){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid eventMode! Setting default value!");
                eventData[i].eventMode = hkbEventRangeData::EventRangeMode.first();
            }
        }
    }
    setDataValidity(isvalid);
    return errors;
}

hkbEventRangeDataArray::~hkbEventRangeDataArray(){
    refCount--;
}
