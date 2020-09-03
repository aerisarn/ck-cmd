#include "hkbgeneratortransitioneffect.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbGeneratorTransitionEffect::refCount = 0;

const QString hkbGeneratorTransitionEffect::classname = "hkbGeneratorTransitionEffect";

const QStringList hkbGeneratorTransitionEffect::SelfTransitionMode = {
    "SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC",
    "SELF_TRANSITION_MODE_CONTINUE", "SELF_TRANSITION_MODE_RESET",
    "SELF_TRANSITION_MODE_BLEND"
};

const QStringList hkbGeneratorTransitionEffect::EventMode = {
    "EVENT_MODE_DEFAULT",
    "EVENT_MODE_PROCESS_ALL",
    "EVENT_MODE_IGNORE_FROM_GENERATOR",
    "EVENT_MODE_IGNORE_TO_GENERATOR"
};

hkbGeneratorTransitionEffect::hkbGeneratorTransitionEffect(HkxFile *parent, long ref)
    : HkDynamicObject(parent, ref),
      userData(0),
      selfTransitionMode(SelfTransitionMode.first()),
      eventMode(EventMode.first()),
      blendInDuration(0),
      blendOutDuration(0),
      syncToGeneratorStartTime(false)
{
    setType(HKB_GENERATOR_TRANSITION_EFFECT, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "GeneratorTransitionEffect_"+QString::number(refCount);
}

const QString hkbGeneratorTransitionEffect::getClassname(){
    return classname;
}

QString hkbGeneratorTransitionEffect::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbGeneratorTransitionEffect::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "transitionGenerator"){
            checkvalue(transitionGenerator.readShdPtrReference(index, reader), "transitionGenerator");
        }else if (text == "blendInDuration"){
            blendInDuration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "blendInDuration");
        }else if (text == "blendOutDuration"){
            blendOutDuration = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "blendOutDuration");
        }else if (text == "syncToGeneratorStartTime"){
            syncToGeneratorStartTime = toBool(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "syncToGeneratorStartTime");
        }
    }
    index--;
    return true;
}

bool hkbGeneratorTransitionEffect::write(HkxXMLWriter *writer){
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
        writedatafield("selfTransitionMode", selfTransitionMode);
        writedatafield("eventMode", eventMode);
        writeref(transitionGenerator, "transitionGenerator");
        writedatafield("blendInDuration", QString::number(blendInDuration, char('f'), 6));
        writedatafield("blendOutDuration", QString::number(blendOutDuration, char('f'), 6));
        writedatafield("syncToGeneratorStartTime", getBoolAsString(syncToGeneratorStartTime));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        //writechild(transitionGenerator, "transitionGenerator");
    }
    return true;
}

bool hkbGeneratorTransitionEffect::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+":  link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(transitionGenerator.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getType() != TYPE_GENERATOR){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object is not a TYPE_GENERATOR!");
            setDataValidity(false);
        }
        transitionGenerator = *ptr;
    }
    return true;
}

void hkbGeneratorTransitionEffect::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    transitionGenerator = HkxSharedPtr();
}

QString hkbGeneratorTransitionEffect::evaluateDataValidity(){   //TO DO: improve..
    std::lock_guard <std::mutex> guard(mutex);
    if (HkDynamicObject::evaluateDataValidity() != "" || (!SelfTransitionMode.contains(selfTransitionMode)) || (!EventMode.contains(eventMode)) || (!transitionGenerator.data() || transitionGenerator->getType() != TYPE_GENERATOR) || (name == "")){
        setDataValidity(false);
        return QString();
    }else{
        setDataValidity(true);
        return QString();
    }
}

hkbGeneratorTransitionEffect::~hkbGeneratorTransitionEffect(){
    refCount--;
}
