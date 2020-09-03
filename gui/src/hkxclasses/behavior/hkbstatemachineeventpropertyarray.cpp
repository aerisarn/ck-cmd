#include "hkbstatemachineeventpropertyarray.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbStateMachineEventPropertyArray::refCount = 0;

const QString hkbStateMachineEventPropertyArray::classname = "hkbStateMachineEventPropertyArray";

hkbStateMachineEventPropertyArray::hkbStateMachineEventPropertyArray(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbStateMachineEventPropertyArray::getClassname(){
    return classname;
}

void hkbStateMachineEventPropertyArray::addEvent(const hkEventPayload &event){
    std::lock_guard <std::mutex> guard(mutex);
    events.append(event), setIsFileChanged(true);
}

void hkbStateMachineEventPropertyArray::setEventId(int index, int id){
    std::lock_guard <std::mutex> guard(mutex);
    (events.size() > index) ? events[index].id = id, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": Unable to add event!");
}

void hkbStateMachineEventPropertyArray::removeEvent(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (events.size() > index) ? events.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": Unable to remove event!");
}

int hkbStateMachineEventPropertyArray::getLastEventIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return events.size() - 1;
}

bool hkbStateMachineEventPropertyArray::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numEvents;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "events"){
            numEvents = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "events");
            (numEvents > 0) ? index++ : NULL;
            for (auto j = 0; j < numEvents; j++, index++){
                events.append(hkEventPayload());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "id"){
                        events.last().id = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "events.at("+QString::number(j)+").id");
                    }else if (text == "payload"){
                        checkvalue(events.last().payload.readShdPtrReference(index, reader), "events.at("+QString::number(j)+").payload");
                        break;
                    }
                }
            }
            (numEvents > 0) ? index-- : NULL;
        }
    }
    index--;
    return true;
}

bool hkbStateMachineEventPropertyArray::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    if (writer && !getIsWritten()){
        QString refString;
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"events", QString::number(events.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < events.size(); i++){
            writer->writeLine(writer->object, true);
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("id"), QString::number(events.at(i).id));
            if (events.at(i).payload.data()){
                refString = events.at(i).payload->getReferenceString();
            }else{
                refString = "null";
            }
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("payload"), refString);
            writer->writeLine(writer->object, false);
        }
        if (events.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < events.size(); i++){
            if (events.at(i).payload.data() && !events.at(i).payload->write(writer)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'payload' at"+QString::number(i)+"!!!");
            }
        }
    }
    return true;
}

bool hkbStateMachineEventPropertyArray::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < events.size(); i++){
        if (events.at(i).id == eventindex){
            return true;
        }
    }
    return false;
}

void hkbStateMachineEventPropertyArray::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < events.size(); i++){
        (events.at(i).id > eventindex) ? events[i].id-- : NULL;
    }
}

void hkbStateMachineEventPropertyArray::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < events.size(); i++){
        (events.at(i).id == oldindex) ? events[i].id = newindex : NULL;
    }
}

void hkbStateMachineEventPropertyArray::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            for (auto i = 0; i < events.size(); i++){
                thiseventname = recdata->getEventNameAt(events.at(i).id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                events[i].id = eventindex;
            }
            setIsMerged(true);
        }
    }
}

bool hkbStateMachineEventPropertyArray::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString othereventname;
    int eventindex;
    bool found;
    hkbStateMachineEventPropertyArray *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY){
        obj = static_cast<hkbStateMachineEventPropertyArray *>(recessiveObject);
        //obj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(obj->getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        for (auto i = 0; i < obj->events.size(); i++){
            found = false;
            for (auto j = 0; j < events.size(); j++){
                if (events.at(j).id == obj->events.at(i).id){
                    found = true;
                }
            }
            if (!found){
                othereventname = recdata->getEventNameAt(obj->events.at(i).id);
                eventindex = domdata->getIndexOfEvent(othereventname);
                if (eventindex == -1 && othereventname != ""){
                    domdata->addEvent(othereventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                    obj->events[i].id = eventindex;
                    events.append(obj->events.at(i));
                }
            }
        }
        setIsMerged(true);
        return true;
    }
    return false;
}

void hkbStateMachineEventPropertyArray::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    for (auto i = 0; i < events.size(); i++){
        (events.at(i).payload.data()) ? events[i].payload->updateReferences(++ref) : NULL;
    }
}

QVector<HkxObject *> hkbStateMachineEventPropertyArray::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    for (auto i = 0; i < events.size(); i++){
        (events.at(i).payload.data()) ? list.append(events.at(i).payload.data()) : NULL;
    }
    return list;
}

int hkbStateMachineEventPropertyArray::getNumOfEvents() const{
    std::lock_guard <std::mutex> guard(mutex);
    return events.size();
}

bool hkbStateMachineEventPropertyArray::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    for (auto i = 0; i < events.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(events.at(i).payload.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                return false;
            }
            events[i].payload = *ptr;
        }
    }
    return true;
}

void hkbStateMachineEventPropertyArray::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < events.size(); i++){
        events[i].payload = HkxSharedPtr();
    }
}

QString hkbStateMachineEventPropertyArray::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (events.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": events is empty!");
    }else{
        for (auto i = 0; i < events.size(); i++){
            if (events.at(i).id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": id in events at "+QString::number(i)+" out of range! Setting to last event index!");
                events[i].id  = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
            }
            if (events.at(i).payload.data() && events.at(i).payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid payload type! Signature: "+QString::number(events.at(i).payload->getSignature(), 16)+" Setting null value!");
                events[i].payload = HkxSharedPtr();
            }
        }
    }
    setDataValidity(isvalid);
    return errors;
}

hkbStateMachineEventPropertyArray::~hkbStateMachineEventPropertyArray(){
    refCount--;
}
