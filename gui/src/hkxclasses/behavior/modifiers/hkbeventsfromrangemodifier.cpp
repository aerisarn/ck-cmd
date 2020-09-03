#include "hkbeventsfromrangemodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbeventrangedataarray.h"

using namespace UI;

uint hkbEventsFromRangeModifier::refCount = 0;

const QString hkbEventsFromRangeModifier::classname = "hkbEventsFromRangeModifier";

hkbEventsFromRangeModifier::hkbEventsFromRangeModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      inputValue(0),
      lowerBound(0)
{
    setType(HKB_EVENTS_FROM_RANGE_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "EventsFromRangeModifier_"+QString::number(refCount);
}

const QString hkbEventsFromRangeModifier::getClassname(){
    return classname;
}

QString hkbEventsFromRangeModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbEventsFromRangeModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "variableBindingSet"){
            checkvalue(getVariableBindingSet().readShdPtrReference(index, reader), "variableBindingSet");
        }else if (text == "userData"){
            userData = reader.getElementValueAt(index).toULong(&ok);
            checkvalue(ok, "userData");
        }else if (text == "name"){
            name = reader.getElementValueAt(index);
            checkvalue((name != ""), "name");
        }else if (text == "enable"){
            enable = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "enable");
        }else if (text == "inputValue"){
            inputValue = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "inputValue");
        }else if (text == "lowerBound"){
            lowerBound = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "lowerBound");
        }else if (text == "eventRanges"){
            checkvalue(eventRanges.readShdPtrReference(index, reader), "eventRanges");
        }
    }
    index--;
    return true;
}

bool hkbEventsFromRangeModifier::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    auto writeref = [&](const HkxSharedPtr & shdptr, const QString & name){
        QString refString = "null";
        (shdptr.data()) ? refString = shdptr->getReferenceString() : NULL;
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), refString);
    };
    auto writechild = [&](const HkxSharedPtr & shdptr, const QString & datafield){
        if (shdptr.data() && !shdptr->write(writer))
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write '"+datafield+"'!!!");
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writeref(getVariableBindingSet(), "variableBindingSet");
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("enable", getBoolAsString(enable));
        writedatafield("inputValue", QString::number(inputValue, char('f'), 6));
        writedatafield("lowerBound", QString::number(lowerBound, char('f'), 6));
        writeref(eventRanges, "eventRanges");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(eventRanges, "eventRanges");
    }
    return true;
}

int hkbEventsFromRangeModifier::getNumberOfRanges() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventRanges.data()){
        return static_cast<hkbEventRangeDataArray *>(eventRanges.data())->eventData.size();
    }
    return 0;
}

bool hkbEventsFromRangeModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventRanges.constData() && eventRanges.constData()->isEventReferenced(eventindex)){
        return true;
    }
    return false;
}

void hkbEventsFromRangeModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (eventRanges.data()) ? eventRanges->updateEventIndices(eventindex) : NULL;
}

void hkbEventsFromRangeModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    (eventRanges.data()) ? eventRanges->mergeEventIndex(oldindex, newindex) : NULL;
}

void hkbEventsFromRangeModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    (eventRanges.data()) ? eventRanges->fixMergedEventIndices(dominantfile) : NULL;
}

void hkbEventsFromRangeModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (eventRanges.data()) ? eventRanges->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> hkbEventsFromRangeModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    (eventRanges.data()) ? list.append(eventRanges.data()) : NULL;
    return list;
}

bool hkbEventsFromRangeModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbEventsFromRangeModifier *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_EVENTS_FROM_RANGE_MODIFIER){
        obj = static_cast<hkbEventsFromRangeModifier *>(recessiveObject);
        if (eventRanges.data()){
            if (obj->eventRanges.data()){
                eventRanges->merge(obj->eventRanges.data());
            }
        }else if (obj->eventRanges.data()){
            eventRanges = obj->eventRanges;
            getParentFile()->addObjectToFile(obj->eventRanges.data(), 0);
            obj->eventRanges->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
            QVector <HkxObject *> others = obj->eventRanges->getChildrenOtherTypes();
            for (auto i = 0; i < others.size(); i++){
                getParentFile()->addObjectToFile(others.at(i), -1);
            }
        }
        injectWhileMerging(obj);
        return true;
    }
    return false;
}

hkbEventRangeDataArray *hkbEventsFromRangeModifier::getEventRanges() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbEventRangeDataArray *>(eventRanges.data());
}

void hkbEventsFromRangeModifier::setEventRanges(hkbEventRangeDataArray *value){
    std::lock_guard <std::mutex> guard(mutex);
    eventRanges = HkxSharedPtr(value);
}

qreal hkbEventsFromRangeModifier::getLowerBound() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lowerBound;
}

void hkbEventsFromRangeModifier::setLowerBound(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lowerBound) ? lowerBound = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lowerBound' was not set!");
}

qreal hkbEventsFromRangeModifier::getInputValue() const{
    std::lock_guard <std::mutex> guard(mutex);
    return inputValue;
}

void hkbEventsFromRangeModifier::setInputValue(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != inputValue) ? inputValue = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'inputValue' was not set!");
}

bool hkbEventsFromRangeModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbEventsFromRangeModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbEventsFromRangeModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbEventsFromRangeModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventRanges.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_EVENT_RANGE_DATA_ARRAY){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'HKB_EVENT_RANGE_DATA_ARRAY' is not a modifier!");
            setDataValidity(false);
        }
        eventRanges = *ptr;
    }
    return true;
}

void hkbEventsFromRangeModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    eventRanges = HkxSharedPtr();
}

QString hkbEventsFromRangeModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (eventRanges.data()){
        if (eventRanges->getSignature() != HKB_EVENT_RANGE_DATA_ARRAY){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid eventRanges type! Signature: "+QString::number(eventRanges->getSignature(), 16)+" Setting default value!");
            eventRanges = HkxSharedPtr();
        }else if (static_cast<hkbEventRangeDataArray *>(eventRanges.data())->eventData.size() < 1){
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": eventRanges has no eventData! Setting null value!");
            eventRanges = HkxSharedPtr();
        }else if (eventRanges->isDataValid() && eventRanges->evaluateDataValidity() != ""){
            isvalid = false;
            //errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid eventRanges data!");
        }
    }else if (!eventRanges.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null eventRanges! Setting default value!");
        eventRanges = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbEventsFromRangeModifier::~hkbEventsFromRangeModifier(){
    refCount--;
}
