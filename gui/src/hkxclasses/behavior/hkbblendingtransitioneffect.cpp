#include "hkbblendingtransitioneffect.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"

using namespace UI;

uint hkbBlendingTransitionEffect::refCount = 0;

const QString hkbBlendingTransitionEffect::classname = "hkbBlendingTransitionEffect";

const QStringList hkbBlendingTransitionEffect::SelfTransitionMode = {
    "SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC",
    "SELF_TRANSITION_MODE_CONTINUE", "SELF_TRANSITION_MODE_RESET",
    "SELF_TRANSITION_MODE_BLEND"
};

const QStringList hkbBlendingTransitionEffect::EventMode = {
    "EVENT_MODE_DEFAULT",
    "EVENT_MODE_PROCESS_ALL",
    "EVENT_MODE_IGNORE_FROM_GENERATOR",
    "EVENT_MODE_IGNORE_TO_GENERATOR"
};

const QStringList hkbBlendingTransitionEffect::Flags = {
    "0",
    "FLAG_NONE",
    "FLAG_IGNORE_FROM_WORLD_FROM_MODEL",
    "FLAG_SYNC",
    "FLAG_IGNORE_TO_WORLD_FROM_MODEL"
};

const QStringList hkbBlendingTransitionEffect::EndMode = {
    "END_MODE_NONE",
    "END_MODE_TRANSITION_UNTIL_END_OF_FROM_GENERATOR",
    "END_MODE_CAP_DURATION_AT_END_OF_FROM_GENERATOR"
};

const QStringList hkbBlendingTransitionEffect::BlendCurve = {
    "BLEND_CURVE_SMOOTH",
    "BLEND_CURVE_LINEAR",
    "BLEND_CURVE_LINEAR_TO_SMOOTH",
    "BLEND_CURVE_SMOOTH_TO_LINEAR"
};

hkbBlendingTransitionEffect::hkbBlendingTransitionEffect(HkxFile *parent, long ref)
    : HkDynamicObject(parent, ref),
      userData(0),
      selfTransitionMode(SelfTransitionMode.first()),
      eventMode(EventMode.first()),
      duration(0.2),
      toGeneratorStartTimeFraction(0),
      flags(Flags.first()),
      endMode(EndMode.first()),
      blendCurve(BlendCurve.first())
{
    setType(HKB_BLENDING_TRANSITION_EFFECT, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "BlendingTransitionEffect_"+QString::number(refCount);
}

QString hkbBlendingTransitionEffect::getClassname(){
    return classname;
}

QString hkbBlendingTransitionEffect::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbBlendingTransitionEffect::operator ==(const hkbBlendingTransitionEffect & other) const{//TO DO: check bound variable...
    std::lock_guard <std::mutex> guard(mutex);
    if (name == other.name){
        return true;
    }
    if (
            userData != other.userData ||
            //name != other.name ||
            selfTransitionMode != other.selfTransitionMode ||
            eventMode != other.eventMode ||
            duration != other.duration ||
            toGeneratorStartTimeFraction != other.toGeneratorStartTimeFraction ||
            flags != other.flags ||
            endMode != other.endMode  ||
            blendCurve != other.blendCurve
            )
    {
        return false;
    }
    return true;
}

QVector<HkxObject *> hkbBlendingTransitionEffect::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector <HkxObject *> list;
    (getVariableBindingSetData()) ? list.append(getVariableBindingSetData()) : NULL;
    return list;
}

QString hkbBlendingTransitionEffect::getBlendCurve() const{
    std::lock_guard <std::mutex> guard(mutex);
    return blendCurve;
}

void hkbBlendingTransitionEffect::setBlendCurve(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < BlendCurve.size() && blendCurve != BlendCurve.at(index)) ? blendCurve = BlendCurve.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'blendCurve' was not set!");
}

QString hkbBlendingTransitionEffect::getEndMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return endMode;
}

void hkbBlendingTransitionEffect::setEndMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < EndMode.size() && endMode != EndMode.at(index)) ? endMode = EndMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'endMode' was not set!");
}

QString hkbBlendingTransitionEffect::getFlags() const{
    std::lock_guard <std::mutex> guard(mutex);
    return flags;
}

void hkbBlendingTransitionEffect::setFlags(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < Flags.size() && flags != Flags.at(index)) ? flags = Flags.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'flags' was not set!");
}

void hkbBlendingTransitionEffect::setFlags(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != flags && value != "") ? flags = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'flags' was not set!");
}

qreal hkbBlendingTransitionEffect::getToGeneratorStartTimeFraction() const{
    std::lock_guard <std::mutex> guard(mutex);
    return toGeneratorStartTimeFraction;
}

void hkbBlendingTransitionEffect::setToGeneratorStartTimeFraction(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != toGeneratorStartTimeFraction) ? toGeneratorStartTimeFraction = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'toGeneratorStartTimeFraction' was not set!");
}

qreal hkbBlendingTransitionEffect::getDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return duration;
}

void hkbBlendingTransitionEffect::setDuration(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != duration) ? duration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'duration' was not set!");
}

QString hkbBlendingTransitionEffect::getEventMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventMode;
}

void hkbBlendingTransitionEffect::setEventMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < EventMode.size() && eventMode != EventMode.at(index)) ? eventMode = EventMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eventMode' was not set!");
}

QString hkbBlendingTransitionEffect::getSelfTransitionMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return selfTransitionMode;
}

void hkbBlendingTransitionEffect::setSelfTransitionMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < SelfTransitionMode.size() && selfTransitionMode != SelfTransitionMode.at(index)) ? selfTransitionMode = SelfTransitionMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'selfTransitionMode' was not set!");
}

void hkbBlendingTransitionEffect::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbBlendingTransitionEffect::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "selfTransitionMode"){
            selfTransitionMode = reader.getElementValueAt(index);
            checkvalue(SelfTransitionMode.contains(selfTransitionMode), "selfTransitionMode");
        }else if (text == "eventMode"){
            eventMode = reader.getElementValueAt(index);
            checkvalue(EventMode.contains(eventMode), "eventMode");
        }else if (text == "duration"){
            duration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "duration");
        }else if (text == "toGeneratorStartTimeFraction"){
            toGeneratorStartTimeFraction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "toGeneratorStartTimeFraction");
        }else if (text == "flags"){
            flags = reader.getElementValueAt(index);
            checkvalue(Flags.contains(flags), "flags");
        }else if (text == "endMode"){
            endMode = reader.getElementValueAt(index);
            checkvalue(EndMode.contains(endMode), "endMode");
        }else if (text == "blendCurve"){
            blendCurve = reader.getElementValueAt(index);
            checkvalue(BlendCurve.contains(blendCurve), "blendCurve");
        }
    }
    index--;
    return true;
}

bool hkbBlendingTransitionEffect::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writedatafield("variableBindingSet", refString);
        writedatafield("userData", QString::number(userData));
        writedatafield("name", name);
        writedatafield("selfTransitionMode", selfTransitionMode);
        writedatafield("eventMode", eventMode);
        writedatafield("duration", QString::number(duration, char('f'), 6));
        writedatafield("toGeneratorStartTimeFraction", QString::number(toGeneratorStartTimeFraction, char('f'), 6));
        writedatafield("flags", flags);
        writedatafield("endMode", endMode);
        writedatafield("blendCurve", blendCurve);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+":  write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool hkbBlendingTransitionEffect::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+":  link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbBlendingTransitionEffect::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbBlendingTransitionEffect::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkstrings = [&](QString & datafield, const QStringList & list, const QString & fieldname){
        if (!list.contains(datafield)){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid "+fieldname+"! Setting default value!");
            datafield = list.first();
        }
    };
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    checkstrings(selfTransitionMode, SelfTransitionMode, "selfTransitionMode");
    checkstrings(eventMode, EventMode, "eventMode");
    //checkstrings(flags, Flags, "flags");
    checkstrings(endMode, EndMode, "endMode");
    checkstrings(blendCurve, BlendCurve, "blendCurve");
    setDataValidity(isvalid);
    return errors;
}

hkbBlendingTransitionEffect::~hkbBlendingTransitionEffect(){
    refCount--;
}
