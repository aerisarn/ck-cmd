#include "hkbhandikdriverinfo.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbHandIkDriverInfo::refCount = 0;

const QString hkbHandIkDriverInfo::classname = "hkbHandIkDriverInfo";

const QStringList hkbHandIkDriverInfo::BlendCurve = {
    "BLEND_CURVE_SMOOTH",
    "BLEND_CURVE_LINEAR",
    "BLEND_CURVE_LINEAR_TO_SMOOTH",
    "BLEND_CURVE_SMOOTH_TO_LINEAR"
};

hkbHandIkDriverInfo::hkbHandIkDriverInfo(HkxFile *parent, long ref)
    : HkxObject(parent, ref),
      fadeInOutCurve(BlendCurve.first())
{
    setType(HKB_HAND_IK_DRIVER_INFO, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbHandIkDriverInfo::getClassname(){
    return classname;
}

int hkbHandIkDriverInfo::getNumberOfHands() const{
    std::lock_guard <std::mutex> guard(mutex);
    return hands.size();
}

void hkbHandIkDriverInfo::addHand(){
    std::lock_guard <std::mutex> guard(mutex);
    hands.append(hkbHandIkDriverInfoHand()), setIsFileChanged(true);
}

void hkbHandIkDriverInfo::removeHandAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < hands.size()) ? hands.removeAt(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'hand' was not removed!");
}

hkbHandIkDriverInfo::hkbHandIkDriverInfoHand * hkbHandIkDriverInfo::getHandAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < hands.size()){
        return &hands[index];
    }
    return nullptr;
}

QString hkbHandIkDriverInfo::getFadeInOutCurve() const{
    std::lock_guard <std::mutex> guard(mutex);
    return fadeInOutCurve;
}

void hkbHandIkDriverInfo::setFadeInOutCurve(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < BlendCurve.size() && fadeInOutCurve != BlendCurve.at(index)) ? fadeInOutCurve = BlendCurve.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fadeInOutCurve' was not set!");
}

bool hkbHandIkDriverInfo::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numhands;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "hands"){
            numhands = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "hands");
            (numhands > 0) ? index++ : NULL;
            for (auto j = 0; j < numhands; j++, index++){
                hands.append(hkbHandIkDriverInfoHand());
                for (; index < reader.getNumElements(); index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "elbowAxisLS"){
                        hands.last().elbowAxisLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").elbowAxisLS");
                    }else if (text == "backHandNormalLS"){
                        hands.last().backHandNormalLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").backHandNormalLS");
                    }else if (text == "handOffsetLS"){
                        hands.last().handOffsetLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").handOffsetLS");
                    }else if (text == "handOrienationOffsetLS"){
                        hands.last().handOrienationOffsetLS = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").handOrienationOffsetLS");
                    }else if (text == "maxElbowAngleDegrees"){
                        hands.last().maxElbowAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").maxElbowAngleDegrees");
                    }else if (text == "minElbowAngleDegrees"){
                        hands.last().minElbowAngleDegrees = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").minElbowAngleDegrees");
                    }else if (text == "shoulderIndex"){
                        hands.last().shoulderIndex = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").shoulderIndex");
                    }else if (text == "shoulderSiblingIndex"){
                        hands.last().shoulderSiblingIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").shoulderSiblingIndex");
                    }else if (text == "elbowIndex"){
                        hands.last().elbowIndex = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").elbowIndex");
                    }else if (text == "elbowSiblingIndex"){
                        hands.last().elbowSiblingIndex = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").elbowSiblingIndex");
                    }else if (text == "wristIndex"){
                        hands.last().wristIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").wristIndex");
                    }else if (text == "enforceEndPosition"){
                        hands.last().enforceEndPosition = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").enforceEndPosition");
                    }else if (text == "enforceEndRotation"){
                        hands.last().enforceEndRotation = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").enforceEndRotation");
                    }else if (text == "localFrameName"){
                        hands.last().localFrameName = reader.getElementValueAt(index);
                        //checkvalue((hands.last().localFrameName != "", "hands.at("+QString::number(j)+").localFrameName");
                        break;
                    }
                }
            }
            (numhands > 0) ? index-- : NULL;
        }else if (text == "fadeInOutCurve"){
            fadeInOutCurve = reader.getElementValueAt(index);
            checkvalue(BlendCurve.contains(fadeInOutCurve), "fadeInOutCurve");
        }
    }
    index--;
    return true;
}

bool hkbHandIkDriverInfo::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"hands", QString::number(hands.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < hands.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("elbowAxisLS", hands[i].elbowAxisLS.getValueAsString());
            writedatafield("backHandNormalLS", hands[i].backHandNormalLS.getValueAsString());
            writedatafield("handOffsetLS", hands[i].handOffsetLS.getValueAsString());
            writedatafield("handOrienationOffsetLS", hands[i].handOrienationOffsetLS.getValueAsString());
            writedatafield("maxElbowAngleDegrees", QString::number(hands.at(i).maxElbowAngleDegrees, char('f'), 6));
            writedatafield("minElbowAngleDegrees", QString::number(hands.at(i).minElbowAngleDegrees, char('f'), 6));
            writedatafield("shoulderIndex", QString::number(hands.at(i).shoulderIndex));
            writedatafield("shoulderSiblingIndex", QString::number(hands.at(i).shoulderSiblingIndex));
            writedatafield("elbowIndex", QString::number(hands.at(i).elbowIndex));
            writedatafield("elbowSiblingIndex", QString::number(hands.at(i).elbowSiblingIndex));
            writedatafield("wristIndex", QString::number(hands.at(i).wristIndex));
            writedatafield("enforceEndPosition", getBoolAsString(hands.at(i).enforceEndPosition));
            writedatafield("enforceEndRotation", getBoolAsString(hands.at(i).enforceEndRotation));
            writer->writeLine(writer->parameter, QStringList(writer->name), QStringList("localFrameName"), hands.at(i).localFrameName, true);
            writer->writeLine(writer->object, false);
        }
        if (hands.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("fadeInOutCurve", fadeInOutCurve);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool hkbHandIkDriverInfo::link(){
    return true;
}

QString hkbHandIkDriverInfo::evaluateDataValidity(){    //TO DO: improve...
    std::lock_guard <std::mutex> guard(mutex);
    if (!hands.isEmpty()){
        setDataValidity(true);
        return QString();
    }
    setDataValidity(false);
    return QString();
}

hkbHandIkDriverInfo::~hkbHandIkDriverInfo(){
    refCount--;
}
