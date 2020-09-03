#include "bscyclicblendtransitiongenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint BSCyclicBlendTransitionGenerator::refCount = 0;

const QString BSCyclicBlendTransitionGenerator::classname = "BSCyclicBlendTransitionGenerator";

const QStringList BSCyclicBlendTransitionGenerator::BlendCurve = {"BLEND_CURVE_SMOOTH", "BLEND_CURVE_LINEAR", "BLEND_CURVE_LINEAR_TO_SMOOTH", "BLEND_CURVE_SMOOTH_TO_LINEAR"};

BSCyclicBlendTransitionGenerator::BSCyclicBlendTransitionGenerator(HkxFile *parent, long ref)
    :hkbGenerator(parent, ref),
    userData(0),
    fBlendParameter(1),
    fTransitionDuration(0),
    eBlendCurve(BlendCurve.first())
{
    setType(BS_CYCLIC_BLEND_TRANSITION_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "CyclicBlendTransitionGenerator_"+QString::number(refCount);
}

const QString BSCyclicBlendTransitionGenerator::getClassname(){
    return classname;
}

QString BSCyclicBlendTransitionGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

void BSCyclicBlendTransitionGenerator::nullEventToCrossBlend(){
    std::lock_guard <std::mutex> guard(mutex);
    eventToCrossBlend.id = -1;
    eventToCrossBlend.payload = HkxSharedPtr();
    static_cast<BehaviorFile *>(getParentFile())->removeOtherData();
    setIsFileChanged(true);
}

void BSCyclicBlendTransitionGenerator::nullEventToFreezeBlendValue(){
    std::lock_guard <std::mutex> guard(mutex);
    eventToFreezeBlendValue.id = -1;
    eventToFreezeBlendValue.payload = HkxSharedPtr();
    static_cast<BehaviorFile *>(getParentFile())->removeOtherData();
    setIsFileChanged(true);
}

bool BSCyclicBlendTransitionGenerator::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getSignature() == HKB_BLENDER_GENERATOR){
            pBlenderGenerator = HkxSharedPtr(obj);
            return true;
        }
    }
    return false;
}

bool BSCyclicBlendTransitionGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        pBlenderGenerator = HkxSharedPtr();
        return true;
    }
    return false;
}

void BSCyclicBlendTransitionGenerator::setEBlendCurve(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < BlendCurve.size() && eBlendCurve != BlendCurve.at(index)) ? eBlendCurve = BlendCurve.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eBlendCurve' was not set!");
}

void BSCyclicBlendTransitionGenerator::setFTransitionDuration(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != fTransitionDuration) ? fTransitionDuration = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fTransitionDuration' was not set!");
}

void BSCyclicBlendTransitionGenerator::setFBlendParameter(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != fBlendParameter) ? fBlendParameter = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fBlendParameter' was not set!");
}

void BSCyclicBlendTransitionGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

QString BSCyclicBlendTransitionGenerator::getBlenderGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(pBlenderGenerator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

QString BSCyclicBlendTransitionGenerator::getEBlendCurve() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eBlendCurve;
}

qreal BSCyclicBlendTransitionGenerator::getFTransitionDuration() const{
    std::lock_guard <std::mutex> guard(mutex);
    return fTransitionDuration;
}

int BSCyclicBlendTransitionGenerator::getEventToCrossBlendId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToCrossBlend.id;
}

int BSCyclicBlendTransitionGenerator::getEventToFreezeBlendValueId() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eventToFreezeBlendValue.id;
}

bool BSCyclicBlendTransitionGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pBlenderGenerator.data()){
        return true;
    }
    return false;
}

bool BSCyclicBlendTransitionGenerator::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (eventToFreezeBlendValue.id == eventindex || eventToCrossBlend.id == eventindex){
        return true;
    }
    return false;
}

void BSCyclicBlendTransitionGenerator::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    if (eventToFreezeBlendValue.id > eventindex){
        eventToFreezeBlendValue.id--;
    }
    if (eventToCrossBlend.id > eventindex){
        eventToCrossBlend.id--;
    }
}

void BSCyclicBlendTransitionGenerator::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    if (eventToFreezeBlendValue.id == oldindex){
        eventToFreezeBlendValue.id = newindex;
    }
    if (eventToCrossBlend.id == oldindex){
        eventToCrossBlend.id = newindex;
    }
}

void BSCyclicBlendTransitionGenerator::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            auto fixIndex = [&](int & id){ if (id < 0){return;}
                thiseventname = recdata->getEventNameAt(id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                id = eventindex;
            };
            fixIndex(eventToFreezeBlendValue.id);
            fixIndex(eventToCrossBlend.id);
            setIsMerged(true);
        }
    }
}

void BSCyclicBlendTransitionGenerator::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    if (eventToFreezeBlendValue.payload.data()){
        eventToFreezeBlendValue.payload->setReference(++ref);
    }
    if (eventToCrossBlend.payload.data()){
        eventToCrossBlend.payload->setReference(++ref);
    }
}

bool BSCyclicBlendTransitionGenerator::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    BSCyclicBlendTransitionGenerator *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == BS_CYCLIC_BLEND_TRANSITION_GENERATOR){
        recobj = static_cast<BSCyclicBlendTransitionGenerator *>(recessiveObject);
        injectWhileMerging(recobj);
        if (!eventToFreezeBlendValue.payload.data() && recobj->eventToFreezeBlendValue.payload.data()){
            getParentFile()->addObjectToFile(recobj->eventToFreezeBlendValue.payload.data(), -1);
        }
        if (!eventToCrossBlend.payload.data() && recobj->eventToCrossBlend.payload.data()){
            getParentFile()->addObjectToFile(recobj->eventToCrossBlend.payload.data(), -1);
        }
        recobj->fixMergedEventIndices(static_cast<BehaviorFile *>(getParentFile()));
    }else{
        return false;
    }
    return true;
}

QVector<HkxObject *> BSCyclicBlendTransitionGenerator::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    auto append = [&](const HkxSharedPtr & shdptr){
        shdptr.data() ? list.append(shdptr.data()) : NULL;
    };
    append(eventToFreezeBlendValue.payload);
    append(eventToCrossBlend.payload);
    return list;
}

QVector<DataIconManager *> BSCyclicBlendTransitionGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (pBlenderGenerator.data()){
        list.append(static_cast<DataIconManager*>(pBlenderGenerator.data()));
    }
    return list;
}

int BSCyclicBlendTransitionGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pBlenderGenerator.data() == obj){
        return 0;
    }
    return -1;
}

bool BSCyclicBlendTransitionGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "pBlenderGenerator"){
            checkvalue(pBlenderGenerator.readShdPtrReference(index, reader), "pBlenderGenerator");
        }else if (text == "EventToFreezeBlendValue"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToFreezeBlendValue.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToFreezeBlendValue.id");
                }else if (text == "payload"){
                    checkvalue(eventToFreezeBlendValue.payload.readShdPtrReference(index, reader), "eventToFreezeBlendValue.payload");
                    break;
                }
            }
            index--;
        }else if (text == "EventToCrossBlend"){
            index++;
            for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                text = reader.getNthAttributeValueAt(index, 0);
                if (text == "id"){
                    eventToCrossBlend.id = reader.getElementValueAt(index).toInt(&ok);
                    checkvalue(ok, "eventToCrossBlend.id");
                }else if (text == "payload"){
                    checkvalue(eventToCrossBlend.payload.readShdPtrReference(index, reader), "eventToCrossBlend.payload");
                    break;
                }
            }
            index--;
        }else if (text == "fBlendParameter"){
            fBlendParameter = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "fBlendParameter");
        }else if (text == "fTransitionDuration"){
            fTransitionDuration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "fTransitionDuration");
        }else if (text == "eBlendCurve"){
            eBlendCurve = reader.getElementValueAt(index);
            checkvalue(BlendCurve.contains(eBlendCurve), "eBlendCurve");
        }
    }
    index--;
    return true;
}

bool BSCyclicBlendTransitionGenerator::write(HkxXMLWriter *writer){
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
        writeref(pBlenderGenerator, "pBlenderGenerator");
        writedatafield("EventToFreezeBlendValue", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToFreezeBlendValue.id), false);
        writeref(eventToFreezeBlendValue.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("EventToCrossBlend", "", false);
        writer->writeLine(writer->object, true);
        writedatafield("id", QString::number(eventToCrossBlend.id), false);
        writeref(eventToCrossBlend.payload, "payload");
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("fBlendParameter", QString::number(fBlendParameter, char('f'), 6), false);
        writedatafield("fTransitionDuration", QString::number(fTransitionDuration, char('f'), 6), false);
        writedatafield("eBlendCurve", eBlendCurve, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(pBlenderGenerator, "pBlenderGenerator");
        writechild(eventToFreezeBlendValue.payload, "eventToFreezeBlendValue.payload");
        writechild(eventToCrossBlend.payload, "eventToCrossBlend.payload");
    }
    return true;
}

bool BSCyclicBlendTransitionGenerator::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto linkdata = [&](HkxType type, HkxSignature sig, HkxSharedPtr & shdptr, const QString & fieldname, bool nullallowed){
        if (ptr){
            if (!ptr->data()){
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link '"+fieldname+"' data field!");
                setDataValidity(false);
            }else if ((*ptr)->getType() != type || (sig != NULL_SIGNATURE && (*ptr)->getSignature() != sig) ||
                      ((*ptr)->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA || (*ptr)->getSignature() == HKB_STATE_MACHINE_STATE_INFO || (*ptr)->getSignature() == HKB_BLENDER_GENERATOR_CHILD))
            {
                LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'"+fieldname+"' data field is linked to invalid child!");
                setDataValidity(false);
            }
            shdptr = *ptr;
        }else if (!nullallowed){
            setDataValidity(false);
        }
    };
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(pBlenderGenerator.getShdPtrReference());
    linkdata(TYPE_GENERATOR, HKB_BLENDER_GENERATOR, pBlenderGenerator, "pBlenderGenerator", false);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToFreezeBlendValue.payload.getShdPtrReference());
    linkdata(TYPE_OTHER, HKB_STRING_EVENT_PAYLOAD, eventToFreezeBlendValue.payload, "eventToFreezeBlendValue.payload", true);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(eventToCrossBlend.payload.getShdPtrReference());
    linkdata(TYPE_OTHER, HKB_STRING_EVENT_PAYLOAD, eventToCrossBlend.payload, "eventToCrossBlend.payload", true);
    return true;
}

void BSCyclicBlendTransitionGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    pBlenderGenerator = HkxSharedPtr();
    eventToFreezeBlendValue.payload = HkxSharedPtr();
    eventToCrossBlend.payload = HkxSharedPtr();
}

QString BSCyclicBlendTransitionGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto appenderror = [&](const QString & fieldname, const QString & errortype, HkxSignature sig){
        QString sigstring;
        if (sig != NULL_SIGNATURE)
            sigstring = " Signature of invalid type: "+QString::number(sig, 16);
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+fieldname+": "+errortype+"!"+sigstring+"\n");
    };
    if (HkDynamicObject::evaluateDataValidity() != ""){
        appenderror("variableBindingSet", "Invalid data", NULL_SIGNATURE);
    }
    if (name == ""){
        appenderror("name", "Invalid name", NULL_SIGNATURE);
    }
    if (eventToFreezeBlendValue.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        appenderror("eventToFreezeBlendValue.id", "Event ID out of range! Setting to max index in range!", NULL_SIGNATURE);
        eventToFreezeBlendValue.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (eventToCrossBlend.id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
        appenderror("eventToCrossBlend.id", "Event ID out of range! Setting to max index in range!", NULL_SIGNATURE);
        eventToCrossBlend.id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
    }
    if (eventToFreezeBlendValue.payload.data() && eventToFreezeBlendValue.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        appenderror("eventToFreezeBlendValue.payload", "Payload is invalid type! Setting null value!", eventToFreezeBlendValue.payload.data()->getSignature());
        eventToFreezeBlendValue.payload = HkxSharedPtr();
    }
    if (eventToCrossBlend.payload.data() && eventToCrossBlend.payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
        appenderror("eventToCrossBlend.payload", "Payload is invalid type! Setting null value!", eventToCrossBlend.payload.data()->getSignature());
        eventToCrossBlend.payload = HkxSharedPtr();
    }
    if (!pBlenderGenerator.data()){
        appenderror("pBlenderGenerator", "Null pBlenderGenerator!", NULL_SIGNATURE);
    }else if (pBlenderGenerator->getSignature() != HKB_BLENDER_GENERATOR){
        appenderror("pBlenderGenerator", "pBlenderGenerator is invalid type! Setting null value!", pBlenderGenerator->getSignature());
        pBlenderGenerator = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSCyclicBlendTransitionGenerator::~BSCyclicBlendTransitionGenerator(){
    refCount--;
}
