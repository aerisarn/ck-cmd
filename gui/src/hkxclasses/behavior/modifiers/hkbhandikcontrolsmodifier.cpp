#include "hkbhandikcontrolsmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbHandIkControlsModifier::refCount = 0;

const QString hkbHandIkControlsModifier::classname = "hkbHandIkControlsModifier";

const QStringList hkbHandIkControlsModifier::HandleChangeMode = {
    "HANDLE_CHANGE_MODE_ABRUPT",
    "HANDLE_CHANGE_MODE_CONSTANT_VELOCITY"
};

hkbHandIkControlsModifier::hkbHandIkControlsModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true)
{
    setType(HKB_HAND_IK_CONTROLS_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "HandIkControlsModifier_"+QString::number(refCount);
}

const QString hkbHandIkControlsModifier::getClassname(){
    return classname;
}

QString hkbHandIkControlsModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

void hkbHandIkControlsModifier::addHand(hkbHandIkControlsModifier::hkHand hand){
    std::lock_guard <std::mutex> guard(mutex);
    hands.append(hand), setIsFileChanged(true);
}

void hkbHandIkControlsModifier::removeHand(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < hands.size()) ? hands.removeAt(index), setIsFileChanged(true) : NULL;
}

bool hkbHandIkControlsModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "hands"){
            numhands = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
            checkvalue(ok, "hands");
            (numhands > 0) ? index++ : NULL;
            for (auto j = 0; j < numhands; j++, index++){
                hands.append(hkHand());
                for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
                    text = reader.getNthAttributeValueAt(index, 0);
                    if (text == "targetPosition"){
                        hands.last().controlData.targetPosition = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.targetPosition");
                    }else if (text == "targetRotation"){
                        hands.last().controlData.targetRotation = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.targetRotation");
                    }else if (text == "targetNormal"){
                        hands.last().controlData.targetNormal = readVector4(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.targetNormal");
                    }else if (text == "targetHandle"){
                        checkvalue(hands.last().controlData.targetHandle.readShdPtrReference(index, reader), "hands.at("+QString::number(j)+").controlData.targetHandle");
                    }else if (text == "transformOnFraction"){
                        hands.last().controlData.transformOnFraction = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.transformOnFraction");
                    }else if (text == "normalOnFraction"){
                        hands.last().controlData.normalOnFraction = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.normalOnFraction");
                    }else if (text == "fadeInDuration"){
                        hands.last().controlData.fadeInDuration = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.fadeInDuration");
                    }else if (text == "fadeOutDuration"){
                        hands.last().controlData.fadeOutDuration = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.fadeOutDuration");
                    }else if (text == "extrapolationTimeStep"){
                        hands.last().controlData.extrapolationTimeStep = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.extrapolationTimeStep");
                    }else if (text == "handleChangeSpeed"){
                        hands.last().controlData.handleChangeSpeed = reader.getElementValueAt(index).toDouble(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.handleChangeSpeed");
                    }else if (text == "handleChangeMode"){
                        hands.last().controlData.handleChangeMode = reader.getElementValueAt(index);
                        checkvalue(HandleChangeMode.contains(hands.last().controlData.handleChangeMode), "hands.at("+QString::number(j)+").controlData.handleChangeMode");
                    }else if (text == "fixUp"){
                        hands.last().controlData.fixUp = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").controlData.fixUp");
                    }else if (text == "handIndex"){
                        hands.last().handIndex = reader.getElementValueAt(index).toInt(&ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").handIndex");
                    }else if (text == "enable"){
                        hands.last().enable = toBool(reader.getElementValueAt(index), &ok);
                        checkvalue(ok, "hands.at("+QString::number(j)+").enable");
                        break;
                    }
                }
            }
            (numhands > 0) ? index-- : NULL;
        }
    }
    index--;
    return true;
}

bool hkbHandIkControlsModifier::write(HkxXMLWriter *writer){
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
        list1 = QStringList({writer->name, writer->numelements});
        list2 = QStringList({"hands", QString::number(hands.size())});
        writer->writeLine(writer->parameter, list1, list2, "");
        for (auto i = 0; i < hands.size(); i++){
            writer->writeLine(writer->object, true);
            writedatafield("controlData", "");
            writer->writeLine(writer->object, true);
            writedatafield("targetPosition", hands.at(i).controlData.targetPosition.getValueAsString());
            writedatafield("targetRotation", hands.at(i).controlData.targetRotation.getValueAsString());
            writedatafield("targetNormal", hands.at(i).controlData.targetNormal.getValueAsString());
            writeref(hands.at(i).controlData.targetHandle, "targetHandle");
            writedatafield("transformOnFraction", QString::number(hands.at(i).controlData.transformOnFraction, char('f'), 6));
            writedatafield("normalOnFraction", QString::number(hands.at(i).controlData.normalOnFraction, char('f'), 6));
            writedatafield("fadeInDuration", QString::number(hands.at(i).controlData.fadeInDuration, char('f'), 6));
            writedatafield("fadeOutDuration", QString::number(hands.at(i).controlData.fadeOutDuration, char('f'), 6));
            writedatafield("extrapolationTimeStep", QString::number(hands.at(i).controlData.extrapolationTimeStep, char('f'), 6));
            writedatafield("handleChangeSpeed", QString::number(hands.at(i).controlData.handleChangeSpeed, char('f'), 6));
            writedatafield("handleChangeMode", hands.at(i).controlData.handleChangeMode);
            writedatafield("fixUp", getBoolAsString(hands.at(i).controlData.fixUp));
            writer->writeLine(writer->object, false);
            writer->writeLine(writer->parameter, false);
            writedatafield("handIndex", QString::number(hands.at(i).handIndex));
            writedatafield("enable", getBoolAsString(hands.at(i).enable));
            writer->writeLine(writer->object, false);
        }
        if (hands.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        for (auto i = 0; i < hands.size(); i++){
            writechild(hands.at(i).controlData.targetHandle, "hands.at("+QString::number(i)+").controlData.targetHandle");
        }
    }
    return true;
}

int hkbHandIkControlsModifier::getNumberOfHands() const{
    std::lock_guard <std::mutex> guard(mutex);
    return hands.size();
}

bool hkbHandIkControlsModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbHandIkControlsModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbHandIkControlsModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbHandIkControlsModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbHandIkControlsModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbHandIkControlsModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    if (hands.isEmpty()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": hands is empty!");
    }else{
        for (auto i = 0; i < hands.size(); i++){
            if (hands.at(i).handIndex >= static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones()){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": handIndex at "+QString::number(i)+" out of range! Setting to last bone index!");
                hands[i].handIndex = static_cast<BehaviorFile *>(getParentFile())->getNumberOfBones() - 1;
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

hkbHandIkControlsModifier::~hkbHandIkControlsModifier(){
    refCount--;
}
