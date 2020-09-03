#include "hkbdelayedmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbDelayedModifier::refCount = 0;

const QString hkbDelayedModifier::classname = "hkbDelayedModifier";

hkbDelayedModifier::hkbDelayedModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(0),
      enable(true),
      delaySeconds(0),
      durationSeconds(0),
      secondsElapsed(0)
{
    setType(HKB_DELAYED_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "DelayedModifier_"+QString::number(refCount);
}

const QString hkbDelayedModifier::getClassname(){
    return classname;
}

QString hkbDelayedModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbDelayedModifier::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj && obj->getType() == TYPE_MODIFIER){
        modifier = HkxSharedPtr(obj);
        return true;
    }
    return false;
}

bool hkbDelayedModifier::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        modifier = HkxSharedPtr();
        return true;
    }
    return false;
}

qreal hkbDelayedModifier::getSecondsElapsed() const{
    std::lock_guard <std::mutex> guard(mutex);
    return secondsElapsed;
}

void hkbDelayedModifier::setSecondsElapsed(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != secondsElapsed) ? secondsElapsed = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'secondsElapsed' was not set!");
}

qreal hkbDelayedModifier::getDurationSeconds() const{
    std::lock_guard <std::mutex> guard(mutex);
    return durationSeconds;
}

void hkbDelayedModifier::setDurationSeconds(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != durationSeconds) ? durationSeconds = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'durationSeconds' was not set!");
}

qreal hkbDelayedModifier::getDelaySeconds() const{
    std::lock_guard <std::mutex> guard(mutex);
    return delaySeconds;
}

void hkbDelayedModifier::setDelaySeconds(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != delaySeconds) ? delaySeconds = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'delaySeconds' was not set!");
}

hkbModifier * hkbDelayedModifier::getModifier() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbModifier *>(modifier.data());
}

bool hkbDelayedModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbDelayedModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbDelayedModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbDelayedModifier::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (modifier.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> hkbDelayedModifier::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    (modifier.data()) ? list.append(static_cast<DataIconManager*>(modifier.data())) : NULL;
    return list;
}

int hkbDelayedModifier::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (modifier.data() == obj){
        return 0;
    }
    return -1;
}

bool hkbDelayedModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "modifier"){
            checkvalue(modifier.readShdPtrReference(index, reader), "modifier");
        }else if (text == "delaySeconds"){
            delaySeconds = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "delaySeconds");
        }else if (text == "durationSeconds"){
            durationSeconds = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "durationSeconds");
        }else if (text == "secondsElapsed"){
            secondsElapsed = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "secondsElapsed");
        }
    }
    index--;
    return true;
}

bool hkbDelayedModifier::write(HkxXMLWriter *writer){
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
        writeref(modifier, "modifier");
        writedatafield("delaySeconds", QString::number(delaySeconds, char('f'), 6));
        writedatafield("durationSeconds", QString::number(durationSeconds, char('f'), 6));
        writedatafield("secondsElapsed", QString::number(secondsElapsed, char('f'), 6));
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(modifier, "modifier");
    }
    return true;
}

bool hkbDelayedModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findModifier(modifier.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getType() != TYPE_MODIFIER){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'modifier' is not a modifier!");
            setDataValidity(false);
        }
        modifier = *ptr;
    }
    return true;
}

void hkbDelayedModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    modifier = HkxSharedPtr();
}

QString hkbDelayedModifier::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto temp = HkDynamicObject::evaluateDataValidity();
    if (temp != ""){
        errors.append(temp+getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid variable binding set!");
    }
    if (name == ""){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid name!");
    }
    if (!modifier.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null modifier!");
    }else if (modifier->getType() != HkxObject::TYPE_MODIFIER){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid modifier type! Signature: "+QString::number(modifier->getSignature(), 16)+" Setting null value!");
        modifier = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbDelayedModifier::~hkbDelayedModifier(){
    refCount--;
}
