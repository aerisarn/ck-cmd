#include "hkbfootikmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"

using namespace UI;

uint hkbFootIkModifier::refCount = 0;

const QString hkbFootIkModifier::classname = "hkbFootIkModifier";

const QStringList hkbFootIkModifier::AlignMode = {
    "ALIGN_MODE_FORWARD_RIGHT",
    "ALIGN_MODE_FORWARD"
};

hkbFootIkModifier::hkbFootIkModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      raycastDistanceUp(0),
      raycastDistanceDown(0),
      originalGroundHeightMS(0),
      errorOut(0),
      verticalOffset(0),
      collisionFilterInfo(0),
      forwardAlignFraction(0),
      sidewaysAlignFraction(0),
      sidewaysSampleWidth(0),
      useTrackData(false),
      lockFeetWhenPlanted(false),
      useCharacterUpVector(true),
      alignMode(AlignMode.first())
{
    setType(HKB_FOOT_IK_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "FootIkModifier_"+QString::number(refCount);
}

const QString hkbFootIkModifier::getClassname(){
    return classname;
}

QString hkbFootIkModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbFootIkModifier::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numlegs;
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
        }else if (text == "onOffGain"){
            gains.onOffGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "enable");
        }else if (text == "groundAscendingGain"){
            gains.groundAscendingGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "groundAscendingGain");
        }else if (text == "groundDescendingGain"){
            gains.groundDescendingGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "groundDescendingGain");
        }else if (text == "footPlantedGain"){
            gains.footPlantedGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "footPlantedGain");
        }else if (text == "footRaisedGain"){
            gains.footRaisedGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "footRaisedGain");
        }else if (text == "footUnlockGain"){
            gains.footUnlockGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "footUnlockGain");
        }else if (text == "worldFromModelFeedbackGain"){
            gains.worldFromModelFeedbackGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "worldFromModelFeedbackGain");
        }else if (text == "errorUpDownBias"){
            gains.errorUpDownBias = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "errorUpDownBias");
        }else if (text == "alignWorldFromModelGain"){
            gains.alignWorldFromModelGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "alignWorldFromModelGain");
        }else if (text == "hipOrientationGain"){
            gains.hipOrientationGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "hipOrientationGain");
        }else if (text == "maxKneeAngleDifference"){
            gains.maxKneeAngleDifference = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxKneeAngleDifference");
        }else if (text == "ankleOrientationGain"){
            gains.ankleOrientationGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "ankleOrientationGain");
        }else if (text == "legs"){
            numlegs = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "legs");
            (numlegs > 0) ? index++ : NULL;
            for (auto j = 0; j < numlegs; j++, index++){
                legs.append(hkLeg());
                for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "originalAnkleTransformMS"){
                        legs.last().originalAnkleTransformMS = readQsTransform(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").originalAnkleTransformMS");
                    }else if (text == "kneeAxisLS"){
                        legs.last().kneeAxisLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").kneeAxisLS");
                    }else if (text == "footEndLS"){
                        legs.last().footEndLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").footEndLS");
                    }else if (text == "id"){
                        legs.last().id = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").id");
                    }else if (text == "payload"){
                        checkvalue(legs.last().payload.readShdPtrReference(index, reader), "legs.at("+QString::number(j)+").payload");
                    }else if (text == "footPlantedAnkleHeightMS"){
                        legs.last().footPlantedAnkleHeightMS = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").footPlantedAnkleHeightMS");
                    }else if (text == "footRaisedAnkleHeightMS"){
                        legs.last().footRaisedAnkleHeightMS = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").footRaisedAnkleHeightMS");
                    }else if (text == "maxAnkleHeightMS"){
                        legs.last().maxAnkleHeightMS = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").maxAnkleHeightMS");
                    }else if (text == "minAnkleHeightMS"){
                        legs.last().minAnkleHeightMS = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").minAnkleHeightMS");
                    }else if (text == "maxKneeAngleDegrees"){
                        legs.last().maxKneeAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").maxKneeAngleDegrees");
                    }else if (text == "minKneeAngleDegrees"){
                        legs.last().minKneeAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").minKneeAngleDegrees");
                    }else if (text == "verticalError"){
                        legs.last().verticalError = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").verticalError");
                    }else if (text == "maxAnkleAngleDegrees"){
                        legs.last().maxAnkleAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").maxAnkleAngleDegrees");
                    }else if (text == "hipIndex"){
                        legs.last().hipIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").hipIndex");
                    }else if (text == "kneeIndex"){
                        legs.last().kneeIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").kneeIndex");
                    }else if (text == "ankleIndex"){
                        legs.last().ankleIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").ankleIndex");
                    }else if (text == "hitSomething"){
                        legs.last().hitSomething = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").hitSomething");
                    }else if (text == "isPlantedMS"){
                        legs.last().isPlantedMS = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").isPlantedMS");
                    }else if (text == "isOriginalAnkleTransformMSSet"){
                        legs.last().isOriginalAnkleTransformMSSet = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").isOriginalAnkleTransformMSSet");
                        break;
                    }
                }
            }
            (numlegs > 0) ? index-- : NULL;
        }else if (text == "raycastDistanceUp"){
            raycastDistanceUp = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "raycastDistanceUp");
        }else if (text == "raycastDistanceDown"){
            raycastDistanceDown = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "raycastDistanceDown");
        }else if (text == "originalGroundHeightMS"){
            originalGroundHeightMS = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "originalGroundHeightMS");
        }else if (text == "errorOut"){
            errorOut = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "errorOut");
        }else if (text == "errorOutTranslation"){
            errorOutTranslation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "errorOutTranslation");
        }else if (text == "alignWithGroundRotation"){
            alignWithGroundRotation = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "alignWithGroundRotation");
        }else if (text == "verticalOffset"){
            verticalOffset = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "verticalOffset");
        }else if (text == "collisionFilterInfo"){
            collisionFilterInfo = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "collisionFilterInfo");
        }else if (text == "forwardAlignFraction"){
            forwardAlignFraction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "forwardAlignFraction");
        }else if (text == "sidewaysAlignFraction"){
            sidewaysAlignFraction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "sidewaysAlignFraction");
        }else if (text == "sidewaysSampleWidth"){
            sidewaysSampleWidth = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "sidewaysSampleWidth");
        }else if (text == "useTrackData"){
            useTrackData = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "useTrackData");
        }else if (text == "lockFeetWhenPlanted"){
            lockFeetWhenPlanted = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "lockFeetWhenPlanted");
        }else if (text == "useCharacterUpVector"){
            useCharacterUpVector = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "useCharacterUpVector");
        }else if (text == "alignMode"){
            alignMode = reader.getElementValueAt(index);
            checkvalue(AlignMode.contains(alignMode), "alignMode");
        }
    }
    index--;
    return true;
}

bool hkbFootIkModifier::write(HkxXMLWriter *writer){
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
        writedatafield("controlData", "");
        writer->writeLine(writer->object, true);
        writedatafield("gains", "");
        writer->writeLine(writer->object, true);
        writedatafield("onOffGain", QString::number(gains.onOffGain, char('f'), 6));
        writedatafield("groundAscendingGain", QString::number(gains.groundAscendingGain, char('f'), 6));
        writedatafield("groundDescendingGain", QString::number(gains.groundDescendingGain, char('f'), 6));
        writedatafield("footPlantedGain", QString::number(gains.footPlantedGain, char('f'), 6));
        writedatafield("footRaisedGain", QString::number(gains.footRaisedGain, char('f'), 6));
        writedatafield("footUnlockGain", QString::number(gains.footUnlockGain, char('f'), 6));
        writedatafield("worldFromModelFeedbackGain", QString::number(gains.worldFromModelFeedbackGain, char('f'), 6));
        writedatafield("errorUpDownBias", QString::number(gains.errorUpDownBias, char('f'), 6));
        writedatafield("alignWorldFromModelGain", QString::number(gains.alignWorldFromModelGain, char('f'), 6));
        writedatafield("hipOrientationGain", QString::number(gains.hipOrientationGain, char('f'), 6));
        writedatafield("maxKneeAngleDifference", QString::number(gains.maxKneeAngleDifference, char('f'), 6));
        writedatafield("ankleOrientationGain", QString::number(gains.ankleOrientationGain, char('f'), 6));
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"legs", QString::number(legs.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < legs.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("originalAnkleTransformMS", legs[i].originalAnkleTransformMS.getValueAsString());
            writedatafield("kneeAxisLS", legs[i].kneeAxisLS.getValueAsString());
            writedatafield("footEndLS", legs[i].footEndLS.getValueAsString());
            writedatafield("id", QString::number(legs.at(i).id));
            writeref(legs.at(i).payload, "legs.at("+QString::number(i)+").payload");
            writedatafield("footPlantedAnkleHeightMS", QString::number(legs.at(i).footPlantedAnkleHeightMS, char('f'), 6));
            writedatafield("footRaisedAnkleHeightMS", QString::number(legs.at(i).footRaisedAnkleHeightMS, char('f'), 6));
            writedatafield("maxAnkleHeightMS", QString::number(legs.at(i).maxAnkleHeightMS, char('f'), 6));
            writedatafield("minAnkleHeightMS", QString::number(legs.at(i).minAnkleHeightMS, char('f'), 6));
            writedatafield("maxKneeAngleDegrees", QString::number(legs.at(i).maxKneeAngleDegrees, char('f'), 6));
            writedatafield("minKneeAngleDegrees", QString::number(legs.at(i).minKneeAngleDegrees, char('f'), 6));
            writedatafield("verticalError", QString::number(legs.at(i).verticalError, char('f'), 6));
            writedatafield("maxAnkleAngleDegrees", QString::number(legs.at(i).maxAnkleAngleDegrees, char('f'), 6));
            writedatafield("hipIndex", QString::number(legs.at(i).hipIndex));
            writedatafield("kneeIndex", QString::number(legs.at(i).kneeIndex));
            writedatafield("ankleIndex", QString::number(legs.at(i).ankleIndex));
            writedatafield("hitSomething", getBoolAsString(legs.at(i).hitSomething));
            writedatafield("isPlantedMS", getBoolAsString(legs.at(i).isPlantedMS));
            writedatafield("isOriginalAnkleTransformMSSet", getBoolAsString(legs.at(i).isOriginalAnkleTransformMSSet));
            writer->writeLine(writer->object, false);
        }
        if (legs.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("raycastDistanceUp", QString::number(raycastDistanceUp, char('f'), 6));
        writedatafield("raycastDistanceDown", QString::number(raycastDistanceDown, char('f'), 6));
        writedatafield("originalGroundHeightMS", QString::number(originalGroundHeightMS, char('f'), 6));
        writedatafield("errorOut", QString::number(errorOut, char('f'), 6));
        writedatafield("errorOutTranslation", errorOutTranslation.getValueAsString());
        writedatafield("alignWithGroundRotation", alignWithGroundRotation.getValueAsString());
        writedatafield("verticalOffset", QString::number(verticalOffset, char('f'), 6));
        writedatafield("collisionFilterInfo", QString::number(collisionFilterInfo));
        writedatafield("forwardAlignFraction", QString::number(forwardAlignFraction, char('f'), 6));
        writedatafield("sidewaysAlignFraction", QString::number(sidewaysAlignFraction, char('f'), 6));
        writedatafield("sidewaysSampleWidth", QString::number(sidewaysSampleWidth, char('f'), 6));
        writedatafield("useTrackData", getBoolAsString(useTrackData));
        writedatafield("lockFeetWhenPlanted", getBoolAsString(lockFeetWhenPlanted));
        writedatafield("useCharacterUpVector", getBoolAsString(useCharacterUpVector));
        writedatafield("alignMode", alignMode);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        for (auto i = 0; i < legs.size(); i++){
            writechild(legs.at(i).payload, "legs.at("+QString::number(i)+").payload");
        }
    }
    return true;
}

int hkbFootIkModifier::getNumberOfLegs() const{
    std::lock_guard <std::mutex> guard(mutex);
    return legs.size();
}

bool hkbFootIkModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < legs.size(); i++){
        if (legs.at(i).id == eventindex){
            return true;
        }
    }
    return false;
}

void hkbFootIkModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < legs.size(); i++){
        (legs.at(i).id > eventindex) ? legs[i].id-- : NULL;
    }
}

void hkbFootIkModifier::mergeEventIndex(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < legs.size(); i++){
        (legs.at(i).id == oldindex) ? legs[i].id = newindex : NULL;
    }
}

void hkbFootIkModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
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
            for (auto i = 0; i < legs.size(); i++){
                thiseventname = recdata->getEventNameAt(legs.at(i).id);
                eventindex = domdata->getIndexOfEvent(thiseventname);
                if (eventindex == -1 && thiseventname != ""){
                    domdata->addEvent(thiseventname);
                    eventindex = domdata->getNumberOfEvents() - 1;
                }
                legs[i].id = eventindex;
            }
            setIsMerged(true);
        }
    }
}


void hkbFootIkModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    for (auto i = 0; i < legs.size(); i++){
        (legs.at(i).payload.data()) ? legs[i].payload->updateReferences(++ref) : NULL;
    }
}

QVector<HkxObject *> hkbFootIkModifier::getChildrenOtherTypes() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<HkxObject *> list;
    for (auto i = 0; i < legs.size(); i++){
        (legs.at(i).payload.data()) ? list.append(legs.at(i).payload.data()) : NULL;
    }
    return list;
}

bool hkbFootIkModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbFootIkModifier *recobj;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_FOOT_IK_MODIFIER){
        recobj = static_cast<hkbFootIkModifier *>(recessiveObject);
        injectWhileMerging(recobj);
        for (auto i = 0; i < legs.size(); i++){
            if (legs.at(i).payload.data()){
                if (!legs.at(i).payload.data() && recobj->legs.at(i).payload.data()){
                    getParentFile()->addObjectToFile(recobj->legs.at(i).payload.data(), -1);
                }
            }
        }
        return true;
    }
    return false;
}

bool hkbFootIkModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    HkxSharedPtr *ptr;
    for (auto i = 0; i < legs.size(); i++){
        ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(legs.at(i).payload.getShdPtrReference());
        if (ptr){
            if ((*ptr)->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                return false;
            }
            legs[i].payload = *ptr;
        }
    }
    return true;
}

void hkbFootIkModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    for (auto i = 0; i < legs.size(); i++){
        legs[i].payload = HkxSharedPtr();
    }
}

QString hkbFootIkModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (legs.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": legs is empty!");
    }else{
        for (auto i = 0; i < legs.size(); i++){
            if (legs.at(i).id >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": id in legs at "+QString::number(i)+" out of range! Setting to max index in range!");
                legs[i].id = static_cast<BehaviorFile *>(getParentFile())->getNumberOfEvents() - 1;
            }
            if (legs.at(i).payload.data() && legs.at(i).payload->getSignature() != HKB_STRING_EVENT_PAYLOAD){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid payload type! Signature: "+QString::number(legs.at(i).payload->getSignature(), 16)+" Setting null value!");
                legs[i].payload = HkxSharedPtr();
            }
        }
    }
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    setDataValidity(isvalid);
    return errors;
}

hkbFootIkModifier::~hkbFootIkModifier(){
    refCount--;
}
