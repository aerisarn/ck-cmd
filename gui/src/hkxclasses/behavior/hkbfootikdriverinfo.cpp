#include "hkbfootikdriverinfo.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbFootIkDriverInfo::refCount = 0;

const QString hkbFootIkDriverInfo::classname = "hkbFootIkDriverInfo";

hkbFootIkDriverInfo::hkbFootIkDriverInfo(HkxFile *parent, long ref)
    : HkxObject(parent, ref),
      raycastDistanceUp(0),
      raycastDistanceDown(0),
      originalGroundHeightMS(0),
      verticalOffset(0),
      collisionFilterInfo(0),
      forwardAlignFraction(0),
      sidewaysAlignFraction(0),
      sidewaysSampleWidth(0),
      lockFeetWhenPlanted(true),
      useCharacterUpVector(true),
      isQuadrupedNarrow(true)
{
    setType(HKB_FOOT_IK_DRIVER_INFO, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbFootIkDriverInfo::getClassname(){
    return classname;
}

void hkbFootIkDriverInfo::addLeg(){
    std::lock_guard <std::mutex> guard(mutex);
    legs.append(hkbFootIkDriverInfoLeg()), setIsFileChanged(true);
}

void hkbFootIkDriverInfo::removeLegAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < legs.size()) ? legs.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'leg' was not removed!");
}

hkbFootIkDriverInfo::hkbFootIkDriverInfoLeg * hkbFootIkDriverInfo::getLegAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < legs.size()){
        return &legs[index];
    }
    return nullptr;
}

int hkbFootIkDriverInfo::getNumberOfLegs() const{
    std::lock_guard <std::mutex> guard(mutex);
    return legs.size();
}

bool hkbFootIkDriverInfo::getIsQuadrupedNarrow() const{
    std::lock_guard <std::mutex> guard(mutex);
    return isQuadrupedNarrow;
}

void hkbFootIkDriverInfo::setIsQuadrupedNarrow(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != isQuadrupedNarrow) ? isQuadrupedNarrow = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'isQuadrupedNarrow' was not set!");
}

bool hkbFootIkDriverInfo::getUseCharacterUpVector() const{
    std::lock_guard <std::mutex> guard(mutex);
    return useCharacterUpVector;
}

void hkbFootIkDriverInfo::setUseCharacterUpVector(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != useCharacterUpVector) ? useCharacterUpVector = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'useCharacterUpVector' was not set!");
}

bool hkbFootIkDriverInfo::getLockFeetWhenPlanted() const{
    std::lock_guard <std::mutex> guard(mutex);
    return lockFeetWhenPlanted;
}

void hkbFootIkDriverInfo::setLockFeetWhenPlanted(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != lockFeetWhenPlanted) ? lockFeetWhenPlanted = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'lockFeetWhenPlanted' was not set!");
}

qreal hkbFootIkDriverInfo::getSidewaysSampleWidth() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sidewaysSampleWidth;
}

void hkbFootIkDriverInfo::setSidewaysSampleWidth(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != sidewaysSampleWidth) ? sidewaysSampleWidth = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sidewaysSampleWidth' was not set!");
}

qreal hkbFootIkDriverInfo::getSidewaysAlignFraction() const{
    std::lock_guard <std::mutex> guard(mutex);
    return sidewaysAlignFraction;
}

void hkbFootIkDriverInfo::setSidewaysAlignFraction(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != sidewaysAlignFraction) ? sidewaysAlignFraction = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'sidewaysAlignFraction' was not set!");
}

qreal hkbFootIkDriverInfo::getForwardAlignFraction() const{
    std::lock_guard <std::mutex> guard(mutex);
    return forwardAlignFraction;
}

void hkbFootIkDriverInfo::setForwardAlignFraction(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != forwardAlignFraction) ? forwardAlignFraction = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'forwardAlignFraction' was not set!");
}

int hkbFootIkDriverInfo::getCollisionFilterInfo() const{
    std::lock_guard <std::mutex> guard(mutex);
    return collisionFilterInfo;
}

void hkbFootIkDriverInfo::setCollisionFilterInfo(int value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != collisionFilterInfo && collisionFilterInfo >= 0 && collisionFilterInfo < 50/*TO DO*/) ? collisionFilterInfo = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'collisionFilterInfo' was not set!");
}

qreal hkbFootIkDriverInfo::getVerticalOffset() const{
    std::lock_guard <std::mutex> guard(mutex);
    return verticalOffset;
}

void hkbFootIkDriverInfo::setVerticalOffset(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != verticalOffset) ? verticalOffset = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'verticalOffset' was not set!");
}

qreal hkbFootIkDriverInfo::getOriginalGroundHeightMS() const{
    std::lock_guard <std::mutex> guard(mutex);
    return originalGroundHeightMS;
}

void hkbFootIkDriverInfo::setOriginalGroundHeightMS(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != originalGroundHeightMS) ? originalGroundHeightMS = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'originalGroundHeightMS' was not set!");
}

qreal hkbFootIkDriverInfo::getRaycastDistanceDown() const{
    std::lock_guard <std::mutex> guard(mutex);
    return raycastDistanceDown;
}

void hkbFootIkDriverInfo::setRaycastDistanceDown(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != raycastDistanceDown) ? raycastDistanceDown = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'raycastDistanceDown' was not set!");
}

qreal hkbFootIkDriverInfo::getRaycastDistanceUp() const{
    std::lock_guard <std::mutex> guard(mutex);
    return raycastDistanceUp;
}

void hkbFootIkDriverInfo::setRaycastDistanceUp(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != raycastDistanceUp) ? raycastDistanceUp = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'raycastDistanceUp' was not set!");
}

bool hkbFootIkDriverInfo::readData(const HkxXmlReader &reader, long & index){
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
        if (text == "legs"){
            numlegs = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "legs");
            (numlegs > 0) ? index++ : NULL;
            for (auto j = 0; j < numlegs; j++, index++){
                legs.append(hkbFootIkDriverInfoLeg());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "kneeAxisLS"){
                        legs.last().kneeAxisLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").kneeAxisLS");
                    }else if (text == "footEndLS"){
                        legs.last().footEndLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "legs.at("+QString::number(j)+").footEndLS");
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
        }else if (text == "raycastDistanceUp"){
            raycastDistanceUp = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "raycastDistanceUp");
        }else if (text == "originalGroundHeightMS"){
            originalGroundHeightMS = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "originalGroundHeightMS");
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
        }else if (text == "lockFeetWhenPlanted"){
            lockFeetWhenPlanted = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "lockFeetWhenPlanted");
        }else if (text == "useCharacterUpVector"){
            useCharacterUpVector = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "useCharacterUpVector");
        }else if (text == "isQuadrupedNarrow"){
            isQuadrupedNarrow = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "isQuadrupedNarrow");
        }
    }
    index--;
    return true;
}

bool hkbFootIkDriverInfo::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"legs", QString::number(legs.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < legs.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("kneeAxisLS", legs[i].kneeAxisLS.getValueAsString());
            writedatafield("footEndLS", legs[i].footEndLS.getValueAsString());
            writedatafield("footPlantedAnkleHeightMS", QString::number(legs.at(i).footPlantedAnkleHeightMS, char('f'), 6));
            writedatafield("footRaisedAnkleHeightMS", QString::number(legs.at(i).footRaisedAnkleHeightMS, char('f'), 6));
            writedatafield("maxAnkleHeightMS", QString::number(legs.at(i).maxAnkleHeightMS, char('f'), 6));
            writedatafield("minAnkleHeightMS", QString::number(legs.at(i).minAnkleHeightMS, char('f'), 6));
            writedatafield("maxKneeAngleDegrees", QString::number(legs.at(i).maxKneeAngleDegrees, char('f'), 6));
            writedatafield("minKneeAngleDegrees", QString::number(legs.at(i).minKneeAngleDegrees, char('f'), 6));
            writedatafield("maxAnkleAngleDegrees", QString::number(legs.at(i).maxAnkleAngleDegrees, char('f'), 6));
            writedatafield("hipIndex", QString::number(legs.at(i).hipIndex));
            writedatafield("kneeIndex", QString::number(legs.at(i).kneeIndex));
            writedatafield("ankleIndex", QString::number(legs.at(i).ankleIndex));
            writer->writeLine(writer->object, false);
        }
        if (legs.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("raycastDistanceUp", QString::number(raycastDistanceUp, char('f'), 6));
        writedatafield("raycastDistanceDown", QString::number(raycastDistanceDown, char('f'), 6));
        writedatafield("originalGroundHeightMS", QString::number(originalGroundHeightMS, char('f'), 6));
        writedatafield("verticalOffset", QString::number(verticalOffset, char('f'), 6));
        writedatafield("collisionFilterInfo", QString::number(collisionFilterInfo));
        writedatafield("forwardAlignFraction", QString::number(forwardAlignFraction, char('f'), 6));
        writedatafield("sidewaysAlignFraction", QString::number(sidewaysAlignFraction, char('f'), 6));
        writedatafield("sidewaysSampleWidth", QString::number(sidewaysSampleWidth, char('f'), 6));
        writedatafield("lockFeetWhenPlanted", getBoolAsString(lockFeetWhenPlanted));
        writedatafield("useCharacterUpVector", getBoolAsString(useCharacterUpVector));
        writedatafield("isQuadrupedNarrow", getBoolAsString(isQuadrupedNarrow));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool hkbFootIkDriverInfo::link(){
    return true;
}

QString hkbFootIkDriverInfo::evaluateDataValidity(){    //TO DO: ...
    std::lock_guard <std::mutex> guard(mutex);
    if (!legs.isEmpty()){
        setDataValidity(true);
        return QString();
    }
    setDataValidity(false);
    return QString();
}

hkbFootIkDriverInfo::~hkbFootIkDriverInfo(){
    refCount--;
}
