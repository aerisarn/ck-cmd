#include "hkbproxymodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbProxyModifier::refCount = 0;

const QString hkbProxyModifier::classname = "hkbProxyModifier";

const QStringList hkbProxyModifier::PhantomType = {"PHANTOM_TYPE_SIMPLE", "PHANTOM_TYPE_CACHING"};
const QStringList hkbProxyModifier::LinearVelocityMode = {"LINEAR_VELOCITY_MODE_WORLD", "LINEAR_VELOCITY_MODE_MODEL"};

hkbProxyModifier::hkbProxyModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      horizontalGain(0),
      verticalGain(0),
      maxHorizontalSeparation(0),
      limitHeadingDegrees(0),
      maxVerticalSeparation(0),
      verticalDisplacementError(0),
      verticalDisplacementErrorGain(0),
      maxVerticalDisplacement(0),
      minVerticalDisplacement(0),
      capsuleHeight(0),
      capsuleRadius(0),
      maxSlopeForRotation(0),
      collisionFilterInfo(0),
      ignoreIncomingRotation(true),
      ignoreCollisionDuringRotation(true),
      ignoreIncomingTranslation(true),
      includeDownwardMomentum(true),
      followWorldFromModel(true),
      isTouchingGround(true),
      phantomType(PhantomType.first()),
      linearVelocityMode(LinearVelocityMode.first())
{
    setType(HKB_PROXY_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ProxyModifier_"+QString::number(refCount);
}

const QString hkbProxyModifier::getClassname(){
    return classname;
}

QString hkbProxyModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbProxyModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "dynamicFriction"){
            proxyInfo.dynamicFriction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "dynamicFriction");
        }else if (text == "staticFriction"){
            proxyInfo.staticFriction = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "staticFriction");
        }else if (text == "keepContactTolerance"){
            proxyInfo.keepContactTolerance = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "keepContactTolerance");
        }else if (text == "up"){
            proxyInfo.up = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "up");
        }else if (text == "keepDistance"){
            proxyInfo.keepDistance = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "keepDistance");
        }else if (text == "contactAngleSensitivity"){
            proxyInfo.contactAngleSensitivity = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "contactAngleSensitivity");
        }else if (text == "userPlanes"){
            proxyInfo.userPlanes = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "userPlanes");
        }else if (text == "maxCharacterSpeedForSolver"){
            proxyInfo.maxCharacterSpeedForSolver = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxCharacterSpeedForSolver");
        }else if (text == "characterStrength"){
            proxyInfo.characterStrength = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "characterStrength");
        }else if (text == "characterMass"){
            proxyInfo.characterMass = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "characterMass");
        }else if (text == "maxSlope"){
            proxyInfo.maxSlope = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxSlope");
        }else if (text == "penetrationRecoverySpeed"){
            proxyInfo.penetrationRecoverySpeed = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "penetrationRecoverySpeed");
        }else if (text == "maxCastIterations"){
            proxyInfo.maxCastIterations = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "maxCastIterations");
        }else if (text == "refreshManifoldInCheckSupport"){
            proxyInfo.refreshManifoldInCheckSupport = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "refreshManifoldInCheckSupport");
        }else if (text == "linearVelocity"){
            linearVelocity = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "linearVelocity");
        }else if (text == "horizontalGain"){
            horizontalGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "horizontalGain");
        }else if (text == "verticalGain"){
            verticalGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "verticalGain");
        }else if (text == "maxHorizontalSeparation"){
            maxHorizontalSeparation = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxHorizontalSeparation");
        }else if (text == "limitHeadingDegrees"){
            limitHeadingDegrees = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "limitHeadingDegrees");
        }else if (text == "maxVerticalSeparation"){
            maxVerticalSeparation = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxVerticalSeparation");
        }else if (text == "verticalDisplacementError"){
            verticalDisplacementError = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "verticalDisplacementError");
        }else if (text == "verticalDisplacementErrorGain"){
            verticalDisplacementErrorGain = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "verticalDisplacementErrorGain");
        }else if (text == "maxVerticalDisplacement"){
            maxVerticalDisplacement = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxVerticalDisplacement");
        }else if (text == "minVerticalDisplacement"){
            minVerticalDisplacement = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "minVerticalDisplacement");
        }else if (text == "capsuleHeight"){
            capsuleHeight = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "capsuleHeight");
        }else if (text == "capsuleRadius"){
            capsuleRadius = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "capsuleRadius");
        }else if (text == "maxSlopeForRotation"){
            maxSlopeForRotation = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "maxSlopeForRotation");
        }else if (text == "collisionFilterInfo"){
            collisionFilterInfo = reader.getElementValueAt(index).toInt(&ok);
            checkvalue(ok, "collisionFilterInfo");
        }else if (text == "phantomType"){
            phantomType = reader.getElementValueAt(index);
            checkvalue(PhantomType.contains(phantomType), "phantomType");
        }else if (text == "linearVelocityMode"){
            linearVelocityMode = reader.getElementValueAt(index);
            checkvalue(LinearVelocityMode.contains(linearVelocityMode), "linearVelocityMode");
        }else if (text == "ignoreIncomingRotation"){
            ignoreIncomingRotation = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "ignoreIncomingRotation");
        }else if (text == "ignoreCollisionDuringRotation"){
            ignoreCollisionDuringRotation = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "ignoreCollisionDuringRotation");
        }else if (text == "ignoreIncomingTranslation"){
            ignoreIncomingTranslation = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "ignoreIncomingTranslation");
        }else if (text == "includeDownwardMomentum"){
            includeDownwardMomentum = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "includeDownwardMomentum");
        }else if (text == "followWorldFromModel"){
            followWorldFromModel = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "followWorldFromModel");
        }else if (text == "isTouchingGround"){
            isTouchingGround = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "isTouchingGround");
        }
    }
    index--;
    return true;
}

bool hkbProxyModifier::write(HkxXMLWriter *writer){
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
        writedatafield("enable", getBoolAsString(enable));
        writedatafield("proxyInfo", "");
        writer->writeLine(writer->object, true);
        writedatafield("dynamicFriction", QString::number(proxyInfo.dynamicFriction, char('f'), 6));
        writedatafield("staticFriction", QString::number(proxyInfo.staticFriction, char('f'), 6));
        writedatafield("keepContactTolerance", QString::number(proxyInfo.keepContactTolerance));
        writedatafield("up", proxyInfo.up.getValueAsString());
        writedatafield("keepDistance", QString::number(proxyInfo.keepDistance, char('f'), 6));
        writedatafield("contactAngleSensitivity", QString::number(proxyInfo.contactAngleSensitivity, char('f'), 6));
        writedatafield("userPlanes", QString::number(proxyInfo.userPlanes));
        writedatafield("maxCharacterSpeedForSolver", QString::number(proxyInfo.maxCharacterSpeedForSolver, char('f'), 6));
        writedatafield("characterStrength", QString::number(proxyInfo.characterStrength, char('f'), 6));
        writedatafield("characterMass", QString::number(proxyInfo.characterMass, char('f'), 6));
        writedatafield("maxSlope", QString::number(proxyInfo.maxSlope, char('f'), 6));
        writedatafield("penetrationRecoverySpeed", QString::number(proxyInfo.penetrationRecoverySpeed, char('f'), 6));
        writedatafield("maxCastIterations", QString::number(proxyInfo.maxCastIterations));
        writedatafield("id", getBoolAsString(proxyInfo.refreshManifoldInCheckSupport));
        writer->writeLine(writer->object, false);
        writer->writeLine(writer->parameter, false);
        writedatafield("linearVelocity", linearVelocity.getValueAsString());
        writedatafield("horizontalGain", QString::number(horizontalGain, char('f'), 6));
        writedatafield("verticalGain", QString::number(verticalGain, char('f'), 6));
        writedatafield("maxHorizontalSeparation", QString::number(maxHorizontalSeparation, char('f'), 6));
        writedatafield("limitHeadingDegrees", QString::number(limitHeadingDegrees, char('f'), 6));
        writedatafield("maxVerticalSeparation", QString::number(maxVerticalSeparation, char('f'), 6));
        writedatafield("verticalDisplacementError", QString::number(verticalDisplacementError, char('f'), 6));
        writedatafield("verticalDisplacementErrorGain", QString::number(verticalDisplacementErrorGain, char('f'), 6));
        writedatafield("maxVerticalDisplacement", QString::number(maxVerticalDisplacement, char('f'), 6));
        writedatafield("minVerticalDisplacement", QString::number(minVerticalDisplacement, char('f'), 6));
        writedatafield("capsuleHeight", QString::number(capsuleHeight, char('f'), 6));
        writedatafield("capsuleRadius", QString::number(capsuleRadius, char('f'), 6));
        writedatafield("maxSlopeForRotation", QString::number(maxSlopeForRotation, char('f'), 6));
        writedatafield("collisionFilterInfo", QString::number(collisionFilterInfo));
        writedatafield("phantomType", phantomType);
        writedatafield("linearVelocityMode", linearVelocityMode);
        writedatafield("ignoreIncomingRotation", getBoolAsString(ignoreIncomingRotation));
        writedatafield("ignoreCollisionDuringRotation", getBoolAsString(ignoreCollisionDuringRotation));
        writedatafield("ignoreIncomingTranslation", getBoolAsString(ignoreIncomingTranslation));
        writedatafield("includeDownwardMomentum", getBoolAsString(includeDownwardMomentum));
        writedatafield("followWorldFromModel", getBoolAsString(followWorldFromModel));
        writedatafield("isTouchingGround", getBoolAsString(isTouchingGround));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool hkbProxyModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    return true;
}

void hkbProxyModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
}

QString hkbProxyModifier::evaluateDataValidity(){  //TO DO...
    std::lock_guard <std::mutex> guard(mutex);
    if (HkDynamicObject::evaluateDataValidity() != "" || (name == "") || (!PhantomType.contains(phantomType)) || (!LinearVelocityMode.contains(linearVelocityMode))){
        setDataValidity(false);
        return QString();
    }else{
        setDataValidity(true);
        return QString();
    }
}

hkbProxyModifier::~hkbProxyModifier(){
    refCount--;
}
