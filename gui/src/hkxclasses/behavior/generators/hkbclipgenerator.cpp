#include "hkbclipgenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbcliptriggerarray.h"

using namespace UI;

#define MAX_TRIES 5

uint hkbClipGenerator::refCount = 0;

const QString hkbClipGenerator::classname = "hkbClipGenerator";

const QStringList hkbClipGenerator::PlaybackMode = {"MODE_SINGLE_PLAY", "MODE_LOOPING", "MODE_USER_CONTROLLED", "MODE_PING_PONG", "MODE_COUNT"};

hkbClipGenerator::hkbClipGenerator(HkxFile *parent, long ref, bool addToAnimData, const QString &animationname)
    : hkbGenerator(parent, ref),
      userData(0),
      animationName(animationname),
      cropStartAmountLocalTime(0),
      cropEndAmountLocalTime(0),
      startTime(0),
      playbackSpeed(1),
      enforcedDuration(0),
      userControlledTimeFraction(0),
      animationBindingIndex(-1),
      mode(PlaybackMode.first()),
      flags("0")
{
    setType(HKB_CLIP_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ClipGenerator_"+QString::number(refCount);
    if (animationname == ""){
        animationName = static_cast<BehaviorFile *>(parent)->getAnimationNameAt(0);
    }else {
        name = animationname;
    }
    /*if (addToAnimData && !par->addClipGenToAnimationData(name)){
        int count = 0;
        bool added = par->addClipGenToAnimationData(name);
        while (!added && count < MAX_TRIES){
            name = "ClipGenerator"+QString::number(refCount)+QString::number(qrand());
            added = par->addClipGenToAnimationData(name);
            count++;
        }
        if (!added){
            CRITICAL_ERROR_MESSAGE("hkbClipGenerator::hkbClipGenerator(): The clip generator could not be added to the animation data!");
        }
    }*/
}

const QString hkbClipGenerator::getClassname(){
    return classname;
}

QString hkbClipGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbClipGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "animationName"){
            animationName = reader.getElementValueAt(index);
            checkvalue((animationName == ""), "animationName");
        }else if (text == "triggers"){
            checkvalue(triggers.readShdPtrReference(index, reader), "triggers");
        }else if (text == "cropStartAmountLocalTime"){
            cropStartAmountLocalTime = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "cropStartAmountLocalTime");
        }else if (text == "cropEndAmountLocalTime"){
            cropEndAmountLocalTime = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "cropEndAmountLocalTime");
        }else if (text == "startTime"){
            startTime = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "startTime");
        }else if (text == "playbackSpeed"){
            playbackSpeed = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "playbackSpeed");
        }else if (text == "enforcedDuration"){
            enforcedDuration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "enforcedDuration");
        }else if (text == "userControlledTimeFraction"){
            userControlledTimeFraction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "userControlledTimeFraction");
        }else if (text == "animationBindingIndex"){
            animationBindingIndex = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "animationBindingIndex");
        }else if (text == "mode"){
            mode = reader.getElementValueAt(index);
            checkvalue(PlaybackMode.contains(mode), "mode");
        }else if (text == "flags"){
            flags = reader.getElementValueAt(index);
            checkvalue((flags != ""), "flags");
        }
    }
    index--;
    return true;
}

bool hkbClipGenerator::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
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
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writedatafield("animationName", animationName, false);
        writeref(triggers, "triggers");
        writedatafield("cropStartAmountLocalTime", QString::number(cropStartAmountLocalTime, char('f'), 6), false);
        writedatafield("cropEndAmountLocalTime", QString::number(cropEndAmountLocalTime, char('f'), 6), false);
        writedatafield("startTime", QString::number(startTime, char('f'), 6), false);
        writedatafield("playbackSpeed", QString::number(playbackSpeed, char('f'), 6), false);
        writedatafield("enforcedDuration", QString::number(enforcedDuration, char('f'), 6), false);
        writedatafield("userControlledTimeFraction", QString::number(userControlledTimeFraction, char('f'), 6), false);
        writedatafield("animationBindingIndex", QString::number(animationBindingIndex), false);
        writedatafield("mode", mode, false);
        writedatafield("flags", flags, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(triggers, "triggers");
    }
    return true;
}

int hkbClipGenerator::getNumberOfTriggers() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.data()){
        return static_cast<hkbClipTriggerArray *>(triggers.data())->getNumberOfTriggers();
    }
    return 0;
}

QString hkbClipGenerator::getAnimationName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return animationName;
}

bool hkbClipGenerator::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.constData() && triggers.constData()->isEventReferenced(eventindex)){
        return true;
    }
    return false;
}

void hkbClipGenerator::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.data()){
        triggers->updateEventIndices(eventindex);
    }
}

void hkbClipGenerator::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.data()){
        triggers->mergeEventIndex(oldindex, newindex);
    }
}

void hkbClipGenerator::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    if (triggers.data() && dominantfile){
        triggers->fixMergedEventIndices(dominantfile);
        dominantfile->addObjectToFile(triggers.data(), -1);
    }
}

bool hkbClipGenerator::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_CLIP_GENERATOR){
        auto obj = static_cast<hkbClipGenerator *>(recessiveObject);
        injectWhileMerging(obj);
        if (triggers.data()){
            if (obj->triggers.data()){
                triggers->merge(obj->triggers.data());
            }
        }else if (obj->triggers.data()){
            triggers = obj->triggers;
            obj->triggers->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
            getParentFile()->addObjectToFile(obj->triggers.data(), -1);
        }
        return true;
    }
    return false;
}

void hkbClipGenerator::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    triggers.data() ? triggers->setReference(++ref) : triggers;
}

QVector<HkxObject *> hkbClipGenerator::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    triggers.data() ? list.append(triggers.data()) : triggers;
    return list;
}

SkyrimClipGeneratoData hkbClipGenerator::getClipGeneratorAnimData(ProjectAnimData *parent, uint animationIndex) const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector <SkyrimClipTrigger> animTrigs;
    qreal trigtime;
    auto trigs = static_cast<hkbClipTriggerArray *>(triggers.data());
    if (trigs){
        for (auto i = 0; i < trigs->triggers.size(); i++){
            if (trigs->triggers.at(i).relativeToEndOfClip){
                trigtime = trigs->triggers.at(i).localTime + static_cast<BehaviorFile *>(getParentFile())->getAnimationDurationFromAnimData(animationName);
            }else{
                trigtime = trigs->triggers.at(i).localTime;
            }
            animTrigs.append(SkyrimClipTrigger(trigtime, static_cast<BehaviorFile *>(getParentFile())->getEventNameAt(trigs->triggers.at(i).event.id)));
        }
    }
    return SkyrimClipGeneratoData(parent, name, animationIndex, playbackSpeed, cropStartAmountLocalTime, cropEndAmountLocalTime, animTrigs);
}

void hkbClipGenerator::setName(const QString &oldclipname, const QString &newclipname){
    std::lock_guard <std::mutex> guard(mutex);
    (newclipname != name && newclipname != "") ? name = newclipname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
    //static_cast<BehaviorFile *>(getParentFile())->setClipNameAnimData(oldclipname, newclipname);    //Unsafe...
}

void hkbClipGenerator::setAnimationName(int index, const QString &animationname){
    std::lock_guard <std::mutex> guard(mutex);
    (animationname != animationName && animationname != "" && animationname.contains(".hkx", Qt::CaseInsensitive)) ? animationName = animationname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'animationName' was not set!");
    //static_cast<BehaviorFile *>(getParentFile())->setAnimationIndexAnimData(index, name);    //Unsafe...
}

void hkbClipGenerator::setPlaybackSpeed(qreal speed){
    std::lock_guard <std::mutex> guard(mutex);
    (speed != playbackSpeed) ? playbackSpeed = speed, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'playbackSpeed' was not set!");
    //static_cast<BehaviorFile *>(getParentFile())->setPlaybackSpeedAnimData(name, speed);    //Unsafe...
}

void hkbClipGenerator::setCropStartAmountLocalTime(qreal time){
    std::lock_guard <std::mutex> guard(mutex);
    (time != cropStartAmountLocalTime) ? cropStartAmountLocalTime = time, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'cropStartAmountLocalTime' was not set!");
    //static_cast<BehaviorFile *>(getParentFile())->setCropStartAmountLocalTimeAnimData(name, time);    //Unsafe...
}

void hkbClipGenerator::setCropEndAmountLocalTime(qreal time){
    std::lock_guard <std::mutex> guard(mutex);
    (time != cropEndAmountLocalTime) ? cropEndAmountLocalTime = time, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'cropEndAmountLocalTime' was not set!");
    //static_cast<BehaviorFile *>(getParentFile())->setCropEndAmountLocalTimeAnimData(name, time);    //Unsafe...
}

hkbClipTriggerArray *hkbClipGenerator::getTriggers() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbClipTriggerArray *>(triggers.data());
}

QString hkbClipGenerator::getFlags() const{
    std::lock_guard <std::mutex> guard(mutex);
    return flags;
}

void hkbClipGenerator::setFlags(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != flags && value != "") ? flags = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'flags' was not set!");
}

QString hkbClipGenerator::getMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return mode;
}

void hkbClipGenerator::setMode(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < PlaybackMode.size() && mode != PlaybackMode.at(index)) ? mode = PlaybackMode.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'mode' was not set!");
}

int hkbClipGenerator::getAnimationBindingIndex() const{
    std::lock_guard <std::mutex> guard(mutex);
    return animationBindingIndex;
}

void hkbClipGenerator::setAnimationBindingIndex(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != animationBindingIndex) ? animationBindingIndex = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'animationBindingIndex' was not set!");
}

qreal hkbClipGenerator::getUserControlledTimeFraction() const{
    std::lock_guard <std::mutex> guard(mutex);
    return userControlledTimeFraction;
}

void hkbClipGenerator::setUserControlledTimeFraction(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != userControlledTimeFraction) ? userControlledTimeFraction = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'userControlledTimeFraction' was not set!");
}

qreal hkbClipGenerator::getEnforcedDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enforcedDuration;
}

void hkbClipGenerator::setEnforcedDuration(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enforcedDuration) ? enforcedDuration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enforcedDuration' was not set!");
}

qreal hkbClipGenerator::getPlaybackSpeed() const{
    std::lock_guard <std::mutex> guard(mutex);
    return playbackSpeed;
}

qreal hkbClipGenerator::getStartTime() const{
    std::lock_guard <std::mutex> guard(mutex);
    return startTime;
}

void hkbClipGenerator::setStartTime(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != startTime) ? startTime = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'startTime' was not set!");
}

qreal hkbClipGenerator::getCropEndAmountLocalTime() const{
    std::lock_guard <std::mutex> guard(mutex);
    return cropEndAmountLocalTime;
}

qreal hkbClipGenerator::getCropStartAmountLocalTime() const{
    std::lock_guard <std::mutex> guard(mutex);
    return cropStartAmountLocalTime;
}

void hkbClipGenerator::setTriggers(hkbClipTriggerArray *value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != triggers.data()) ? triggers = HkxSharedPtr(value), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'triggers' was not set!");
}

bool hkbClipGenerator::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(triggers.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_CLIP_TRIGGER_ARRAY){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'triggers' data field is linked to invalid child!\nObject Name: "+name);
            setDataValidity(false);
        }
        triggers = *ptr;
    }
    return true;
}

void hkbClipGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    triggers = HkxSharedPtr();
}

QString hkbClipGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto valid = true;
    auto list = static_cast<BehaviorFile *>(getParentFile())->getAnimationNames();
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (!list.contains(animationName, Qt::CaseInsensitive)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid animationName! Fixing!");
        if (!list.isEmpty()){
            animationName = list.first();
        }
    }
    if (triggers.data()){
        if (triggers->getSignature() != HKB_CLIP_TRIGGER_ARRAY){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid triggers type! Signature: "+QString::number(triggers->getSignature(), 16)+" Setting null value!");
            triggers = HkxSharedPtr();
        }else if (static_cast<hkbClipTriggerArray *>(triggers.data())->triggers.size() < 1){
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": triggers has no triggers! Setting null value!");
            triggers = HkxSharedPtr();
        }else if (triggers->isDataValid() && triggers->evaluateDataValidity() != ""){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid triggers data!");
        }
    }
    if (!PlaybackMode.contains(mode)){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid mode! Setting default value!");
        mode = PlaybackMode.first();
    }
    if (flags.toUInt(&valid) >= INVALID_FLAG || !valid){    //TO DO: Fix this...
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid flags!");
    }
    setDataValidity(isvalid);
    return errors;
}

hkbClipGenerator::~hkbClipGenerator(){
    //static_cast<BehaviorFile *>(getParentFile())->removeClipGenFromAnimData(animationName.section("\\", -1, -1), name);
    refCount--;
}
