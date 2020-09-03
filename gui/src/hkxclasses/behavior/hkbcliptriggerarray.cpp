#include "hkbcliptriggerarray.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"

using namespace UI;

uint hkbClipTriggerArray::refCount = 0;

const QString hkbClipTriggerArray::classname = "hkbClipTriggerArray";

hkbClipTriggerArray::hkbClipTriggerArray(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_CLIP_TRIGGER_ARRAY, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbClipTriggerArray::getClassname(){
    return classname;
}

void hkbClipTriggerArray::addTrigger(const HkTrigger & trigger){
    std::lock_guard <std::mutex> guard(mutex);
    triggers.append(trigger), setIsFileChanged(true);
}

void hkbClipTriggerArray::setTriggerIdAt(int index, int id){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index && index >= 0 && triggers.at(index).event.id != id && id < static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()) ? triggers[index].event.id = id, setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set id!");
}

void hkbClipTriggerArray::setPayloadAt(int index, hkbStringEventPayload *load){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index && index >= 0) ? triggers[index].event.payload = HkxSharedPtr(load), setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set payload!");
}

void hkbClipTriggerArray::setLocalTimeAt(int index, qreal time){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index && index >= 0 && triggers.at(index).localTime != time) ? triggers[index].localTime = time, setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set localTime!");
}

void hkbClipTriggerArray::setRelativeToEndOfClipAt(int index, bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index && index >= 0 && triggers.at(index).relativeToEndOfClip != value) ? triggers[index].relativeToEndOfClip = value, setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set relativeToEndOfClip!");
}

void hkbClipTriggerArray::setAcyclicAt(int index, bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index && index >= 0 && triggers.at(index).acyclic != value) ? triggers[index].acyclic = value, setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set relativeToEndOfClip!");
}

void hkbClipTriggerArray::setIsAnnotationAt(int index, bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index && index >= 0 && triggers.at(index).isAnnotation != value) ? triggers[index].isAnnotation = value, setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to set isAnnotation!");
}

int hkbClipTriggerArray::getTriggerIdAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.size() > index && index >= 0){
        return triggers.at(index).event.id;
    }
    return -1;
}

hkbStringEventPayload *hkbClipTriggerArray::getPayloadAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.size() > index && index >= 0){
        return static_cast<hkbStringEventPayload *>(triggers.at(index).event.payload.data());
    }
    return nullptr;
}

qreal hkbClipTriggerArray::getLocalTimeAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.size() > index && index >= 0){
        return triggers.at(index).localTime;
    }
    return 0;
}

bool hkbClipTriggerArray::getRelativeToEndOfClipAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.size() > index && index >= 0){
        return triggers.at(index).relativeToEndOfClip;
    }
    return false;
}

bool hkbClipTriggerArray::getAcyclicAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.size() > index && index >= 0){
        return triggers.at(index).acyclic;
    }
    return false;
}

bool hkbClipTriggerArray::getIsAnnotationAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.size() > index && index >= 0){
        return triggers.at(index).isAnnotation;
    }
    return false;
}

void hkbClipTriggerArray::removeTrigger(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (triggers.size() > index) ? triggers.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getParentFilename()+": "+getClassname()+": failed to remove trigger!");
}

int hkbClipTriggerArray::getLastTriggerIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return triggers.size() - 1;
}

bool hkbClipTriggerArray::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numtriggers;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "triggers"){
            numtriggers = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "triggers");
            (numtriggers > 0) ? index++ : NULL;
            for (auto j = 0; j < numtriggers; j++, index++){
                triggers.append(HkTrigger());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "localTime"){
                        triggers.last().localTime = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "triggers.at("+QString::number(j)+").localTime");
                    }else if (text == "id"){
                        triggers.last().event.id = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "triggers.at("+QString::number(j)+").id");
                    }else if (text == "payload"){
                        checkvalue(triggers.last().event.payload.readShdPtrReference(index, reader), "triggers.at("+QString::number(j)+").payload");
                    }else if (text == "relativeToEndOfClip"){
                        triggers.last().relativeToEndOfClip = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "triggers.at("+QString::number(j)+").relativeToEndOfClip");
                    }else if (text == "acyclic"){
                        triggers.last().acyclic = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "triggers.at("+QString::number(j)+").acyclic");
                    }else if (text == "isAnnotation"){
                        triggers.last().isAnnotation = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "triggers.at("+QString::number(j)+").isAnnotation");
                        break;
                    }
                }
            }
            (numtriggers > 0) ? index-- : NULL;
        }
    }
    index--;
    return true;
}

bool hkbClipTriggerArray::write(HkxXMLWriter *writer){
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
        list2 = QStringList({"triggers", QString::number(triggers.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < triggers.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("localTime", QString::number(triggers.at(i).localTime, char('f'), 6));
            writedatafield("event", "");
            writer->writeLine(writer->object, true);
            writedatafield("id", QString::number(triggers.at(i).event.id));
            if (triggers.at(i).event.payload.data()){
                refString = triggers.at(i).event.payload->getReferenceString();
            }else{
                refString = "null";
            }
            writedatafield("payload", refString);
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writedatafield("relativeToEndOfClip", getBoolAsString(triggers.at(i).relativeToEndOfClip));
            writedatafield("acyclic", getBoolAsString(triggers.at(i).acyclic));
            writedatafield("isAnnotation", getBoolAsString(triggers.at(i).isAnnotation));
            writer->writeLine(writer->object, false);
        }
        if (triggers.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        for (auto i = 0; i < triggers.size(); i++){
            if (triggers.at(i).event.payload.data() && !triggers.at(i).event.payload->write(writer)){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'payload' at"+QString::number(i)+"!!!");
            }
        }
    }
    return true;
}

bool hkbClipTriggerArray::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < triggers.size(); i++){
        if (triggers.at(i).event.id == eventindex){
            return true;
        }
    }
    return false;
}

void hkbClipTriggerArray::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < triggers.size(); i++){
        (triggers.at(i).event.id > eventindex) ? triggers[i].event.id-- : NULL;
    }
}

void hkbClipTriggerArray::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < triggers.size(); i++){
        (triggers.at(i).event.id == oldindex) ? triggers[i].event.id = newindex : NULL;
    }
}

void hkbClipTriggerArray::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            for (auto i = 0; i < triggers.size(); i++){
                thiseventname = recdata->getEventNameAt(triggers.at(i).event.id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                triggers[i].event.id = eventindex;
            }
            setIsMerged(true);
        }
    }
}

bool hkbClipTriggerArray::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbBehaviorGraphData *recdata;
    hkbBehaviorGraphData *domdata;
    QString othereventname;
    int eventindex;
    bool found;
    hkbClipTriggerArray *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_CLIP_TRIGGER_ARRAY){
        obj = static_cast<hkbClipTriggerArray *>(recessiveObject);
        //obj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
        recdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(obj->getParentFile())->getBehaviorGraphData());
        domdata = static_cast<hkbBehaviorGraphData *>(static_cast<BehaviorFile *>(getParentFile())->getBehaviorGraphData());
        for (auto i = 0; i < obj->triggers.size(); i++){
            found = false;
            for (auto j = 0; j < triggers.size(); j++){
                if (triggers.at(j).event.id == obj->triggers.at(i).event.id){
                    found = true;
                }
            }
            if (!found){
                othereventname = recdata->getEventNameAt(obj->triggers.at(i).event.id);
                eventindex = domdata->getIndexOfEvent(othereventname);
                if (eventindex == -1 && othereventname != ""){
                    domdata->addEvent(othereventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                    obj->triggers[i].event.id = eventindex;
                    triggers.append(obj->triggers.at(i));
                }
            }
        }
        setIsMerged(true);
        return true;
    }
    return false;
}

void hkbClipTriggerArray::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    for (auto i = 0; i < triggers.size(); i++){
        (triggers.at(i).event.payload.data()) ? triggers[i].event.payload->updateReferences(++ref) : NULL;
    }
}

QVector<HkxObject *> hkbClipTriggerArray::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    for (auto i = 0; i < triggers.size(); i++){
        (triggers.at(i).event.payload.data()) ? list.append(triggers.at(i).event.payload.data()) : NULL;
    }
    return list;
}

bool hkbClipTriggerArray::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    for (auto i = 0; i < triggers.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(triggers.at(i).event.payload.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                return false;
            }
            triggers[i].event.payload = *ptr;
        }
    }
    return true;
}

QString hkbClipTriggerArray::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (triggers.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": triggers is empty!");
    }else{
        for (auto i = 0; i < triggers.size(); i++){
            if (triggers.at(i).event.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": id in triggers at "+QString::number(i)+" out of range! Setting to last event index!");
                triggers[i].event.id  = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
            }
            if (triggers.at(i).event.payload.data() && triggers.at(i).event.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid payload type! Signature: "+QString::number(triggers.at(i).event.payload->getSignature(), 16)+" Setting null value!");
                triggers[i].event.payload = HkxSharedPtr();
            }
        }
    }
    setDataValidity(isvalid);
    return errors;
}

int hkbClipTriggerArray::getNumberOfTriggers() const{
    std::lock_guard <std::mutex> guard(mutex);
    return triggers.size();
}

hkbClipTriggerArray::~hkbClipTriggerArray(){
    refCount--;
}
