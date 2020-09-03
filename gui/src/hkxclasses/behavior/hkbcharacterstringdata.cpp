#include "hkbcharacterstringdata.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/projectfile.h"

using namespace UI;

uint hkbCharacterStringData::refCount = 0;

const QString hkbCharacterStringData::classname = "hkbCharacterStringData";

hkbCharacterStringData::hkbCharacterStringData(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    setType(HKB_CHARACTER_STRING_DATA, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
}

const QString hkbCharacterStringData::getClassname(){
    return classname;
}

int hkbCharacterStringData::getCharacterPropertyIndex(const QString &name) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < characterPropertyNames.size(); i++){
        if (characterPropertyNames.at(i) == name){
            return i;
        }
    }
    return -1;
}

QStringList hkbCharacterStringData::getAnimationNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    return animationNames;
}

QString hkbCharacterStringData::getCharacterPropertyNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (characterPropertyNames.size() > index && index >= 0){
        return characterPropertyNames.at(index);
    }
    return "";
}

void hkbCharacterStringData::addAnimation(const QString & name){
    std::lock_guard <std::mutex> guard(mutex);
    if (!animationNames.contains(name, Qt::CaseInsensitive)){
        animationNames.append(name);
        setIsFileChanged(true);
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": addAnimation()!\nAnimation: "+name+": already exists in the character file!!");
    }
}

QString hkbCharacterStringData::getAnimationNameAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (animationNames.size() > index && index >= 0){
        return animationNames.at(index);
    }
    return "";
}

int hkbCharacterStringData::getAnimationIndex(const QString &name) const{
    std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < animationNames.size(); i++){
        if (!name.compare(animationNames.at(i), Qt::CaseInsensitive)){
            return i;
        }
    }
    return -1;
}

int hkbCharacterStringData::addCharacterPropertyName(const QString &name, bool * wasadded){
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    if (characterPropertyNames.contains(name)){
        index = characterPropertyNames.indexOf(name);
        (wasadded) ? *wasadded = false : NULL;
    }else{
        characterPropertyNames.append(name);
        index = characterPropertyNames.size() - 1;
        (wasadded) ? *wasadded = false : NULL;
    }
    return index;
}

void hkbCharacterStringData::setCharacterPropertyNameAt(int index, const QString &name){
    std::lock_guard <std::mutex> guard(mutex);
    (characterPropertyNames.size() > index && index > -1) ? characterPropertyNames.replace(index, name) : NULL;
}

QString hkbCharacterStringData::getRagdollName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return ragdollName;
}

int hkbCharacterStringData::getNumberOfAnimations() const{
    std::lock_guard <std::mutex> guard(mutex);
    return animationNames.size();
}

QString hkbCharacterStringData::getRigName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return rigName;
}

void hkbCharacterStringData::setVariableNameAt(int index, const QString &name){
    std::lock_guard <std::mutex> guard(mutex);
    (characterPropertyNames.size() > index && index > -1) ? characterPropertyNames.replace(index, name), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'setVariableNameAt' failed!");
}

QString hkbCharacterStringData::getBehaviorFilename() const{
    std::lock_guard <std::mutex> guard(mutex);
    return behaviorFilename;
}

void hkbCharacterStringData::setBehaviorFilename(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != "") ? behaviorFilename = value : NULL;
}

void hkbCharacterStringData::setRagdollName(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != "") ? ragdollName = value : NULL;
}

void hkbCharacterStringData::setRigName(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != "") ? rigName = value : NULL;
}

void hkbCharacterStringData::setName(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != "") ? name = value : NULL;
}

QStringList hkbCharacterStringData::getCharacterPropertyNames() const{
    std::lock_guard <std::mutex> guard(mutex);
    return characterPropertyNames;
}

QString hkbCharacterStringData::getLastCharacterPropertyName() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (!characterPropertyNames.isEmpty()){
        return characterPropertyNames.last();
    }
    return "";
}

void hkbCharacterStringData::generateAppendCharacterPropertyName(const QString &type){
    std::lock_guard <std::mutex> guard(mutex);
    QString newname = "NEW_"+type+"_";
    generateAppendStringToList(characterPropertyNames, newname, QChar('_'));
}

void hkbCharacterStringData::removeCharacterPropertyNameAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < characterPropertyNames.size()) ? characterPropertyNames.removeAt(index) : NULL;
}

bool hkbCharacterStringData::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    int numElems;
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    auto readstrings =[&](QStringList & list, const QString & fieldname){
        numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
        checkvalue(ok, fieldname);
        (numElems > 0) ? index++ : NULL;
        for (auto j = 0; j < numElems && index < reader.getNumElements(); j++, index++){
            list.append(reader.getElementValueAt(index));
            checkvalue((list.last() != ""), fieldname+".at("+QString::number(j)+")");
        }
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "deformableSkinNames"){
            readstrings(deformableSkinNames, "deformableSkinNames");
        }else if (text == "rigidSkinNames"){
            readstrings(rigidSkinNames, "rigidSkinNames");
        }else if (text == "animationNames"){
            readstrings(animationNames, "animationNames");
        }else if (text == "characterPropertyNames"){
            readstrings(characterPropertyNames, "characterPropertyNames");
        }else if (text == "retargetingSkeletonMapperFilenames"){
            readstrings(retargetingSkeletonMapperFilenames, "retargetingSkeletonMapperFilenames");
        }else if (text == "lodNames"){
            readstrings(lodNames, "lodNames");
        }else if (text == "mirroredSyncPointSubstringsA"){
            readstrings(mirroredSyncPointSubstringsA, "mirroredSyncPointSubstringsA");
        }else if (text == "mirroredSyncPointSubstringsB"){
            readstrings(mirroredSyncPointSubstringsB, "mirroredSyncPointSubstringsB");
        }else if (text == "name"){
            name = reader.getElementValueAt(index);
            checkvalue((name != ""), "name");
        }else if (text == "rigName"){
            rigName = reader.getElementValueAt(index);
            checkvalue((rigName != ""), "rigName");
        }else if (text == "ragdollName"){
            ragdollName = reader.getElementValueAt(index);
            checkvalue((ragdollName != ""), "ragdollName");
        }else if (text == "behaviorFilename"){
            behaviorFilename = reader.getElementValueAt(index);
            checkvalue((behaviorFilename != ""), "behaviorFilename");
        }
    }
    index--;
    return true;
}

bool hkbCharacterStringData::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value);
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        QStringList list3 = QStringList({writer->name, writer->numelements});
        QStringList list4 = QStringList({"deformableSkinNames", QString::number(deformableSkinNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < deformableSkinNames.size(); i++){
            writedatafield("", deformableSkinNames.at(i));
        }
        if (deformableSkinNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"rigidSkinNames", QString::number(rigidSkinNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < rigidSkinNames.size(); i++){
            writedatafield("", rigidSkinNames.at(i));
        }
        if (rigidSkinNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"animationNames", QString::number(animationNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < animationNames.size(); i++){
            writedatafield("", QString(animationNames.at(i)).replace("/", "\\"));
        }
        if (animationNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"animationFilenames", QString::number(animationFilenames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < animationFilenames.size(); i++){
            writedatafield("", animationFilenames.at(i));
        }
        if (animationFilenames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"characterPropertyNames", QString::number(characterPropertyNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < characterPropertyNames.size(); i++){
            writedatafield("", characterPropertyNames.at(i));
        }
        if (characterPropertyNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"retargetingSkeletonMapperFilenames", QString::number(retargetingSkeletonMapperFilenames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < retargetingSkeletonMapperFilenames.size(); i++){
            writedatafield("", retargetingSkeletonMapperFilenames.at(i));
        }
        if (retargetingSkeletonMapperFilenames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"lodNames", QString::number(lodNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < lodNames.size(); i++){
            writedatafield("", lodNames.at(i));
        }
        if (lodNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"mirroredSyncPointSubstringsA", QString::number(mirroredSyncPointSubstringsA.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < mirroredSyncPointSubstringsA.size(); i++){
            writedatafield("", mirroredSyncPointSubstringsA.at(i));
        }
        if (mirroredSyncPointSubstringsA.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"mirroredSyncPointSubstringsB", QString::number(mirroredSyncPointSubstringsB.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < mirroredSyncPointSubstringsB.size(); i++){
            writedatafield("", mirroredSyncPointSubstringsB.at(i));
        }
        if (mirroredSyncPointSubstringsB.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("name", name);
        writedatafield("rigName", QString(rigName).replace("/", "\\"));
        writedatafield("ragdollName", QString(ragdollName).replace("/", "\\"));
        writedatafield("behaviorFilename", QString(behaviorFilename).replace("/", "\\"));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool hkbCharacterStringData::link(){
    return true;
}

QString hkbCharacterStringData::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkstring = [&](const QStringList & list, const QString & fieldname){
        for (auto i = 0; i < list.size(); i++){
            if (list.at(i) == ""){
                isvalid = false;
                errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid "+fieldname+" at "+QString::number(i)+"!");
            }
        }
    };
    checkstring(deformableSkinNames, "deformableSkinNames");
    checkstring(rigidSkinNames, "rigidSkinNames");
    checkstring(animationNames, "animationNames");
    checkstring(animationFilenames, "animationFilenames");
    checkstring(characterPropertyNames, "characterPropertyNames");
    checkstring(retargetingSkeletonMapperFilenames, "retargetingSkeletonMapperFilenames");
    checkstring(lodNames, "lodNames");
    checkstring(mirroredSyncPointSubstringsA, "mirroredSyncPointSubstringsA");
    checkstring(mirroredSyncPointSubstringsB, "mirroredSyncPointSubstringsB");
    checkstring(QStringList(name), "name");
    checkstring(QStringList(rigName), "rigName");
    checkstring(QStringList(ragdollName), "ragdollName");
    checkstring(QStringList(behaviorFilename), "behaviorFilename");
    setDataValidity(isvalid);
    return QString();
}

hkbCharacterStringData::~hkbCharacterStringData(){
    refCount--;
}
