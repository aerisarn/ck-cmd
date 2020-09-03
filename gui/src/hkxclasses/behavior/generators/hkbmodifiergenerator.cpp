#include "hkbmodifiergenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint hkbModifierGenerator::refCount = 0;

const QString hkbModifierGenerator::classname = "hkbModifierGenerator";

hkbModifierGenerator::hkbModifierGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(1)
{
    setType(HKB_MODIFIER_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "ModifierGenerator_"+QString::number(refCount);
}

const QString hkbModifierGenerator::getClassname(){
    return classname;
}

QString hkbModifierGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbModifierGenerator::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getType() == TYPE_MODIFIER){
            modifier = HkxSharedPtr(obj);
        }else if (obj->getType() == TYPE_GENERATOR){
            generator = HkxSharedPtr(obj);
        }else{
            return false;
        }
        return true;
    }
    return false;
}

bool hkbModifierGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index){
        modifier = HkxSharedPtr();
    }else if (index == 1){
        generator = HkxSharedPtr();
    }else if (index == -1){
        modifier = HkxSharedPtr();
        generator = HkxSharedPtr();
    }else{
        return false;
    }
    return true;
}

void hkbModifierGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbModifierGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (generator.data() || modifier.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> hkbModifierGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    auto getchildren = [&](DataIconManager *ptr){
        ptr ? list.append(ptr) : ptr;
    };
    getchildren(static_cast<DataIconManager*>(modifier.data()));
    getchildren(static_cast<DataIconManager*>(generator.data()));
    return list;
}

QString hkbModifierGenerator::getGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(generator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

QString hkbModifierGenerator::getModifierName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString modname("NONE");
    auto gen = static_cast<hkbGenerator *>(modifier.data());
    (gen) ? modname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return modname;
}

int hkbModifierGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (modifier.data() == obj){
        return 0;
    }else if (generator.data() == obj){
        return 1;
    }
    return -1;
}

bool hkbModifierGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "modifier"){
            checkvalue(modifier.readShdPtrReference(index, reader), "modifier");
        }else if (text == "generator"){
            checkvalue(generator.readShdPtrReference(index, reader), "generator");
        }
    }
    index--;
    return true;
}

bool hkbModifierGenerator::write(HkxXMLWriter *writer){
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
        writeref(modifier, "modifier");
        writeref(generator, "generator");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(modifier, "modifier");
        writechild(generator, "generator");
    }
    return true;
}

bool hkbModifierGenerator::link(){
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
    ptr = static_cast<BehaviorFile *>(getParentFile())->findModifier(modifier.getShdPtrReference());
    linkdata(TYPE_MODIFIER, NULL_SIGNATURE, modifier, "modifier", false);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(generator.getShdPtrReference());
    linkdata(TYPE_GENERATOR, NULL_SIGNATURE, generator, "generator", false);
    return true;
}

void hkbModifierGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    modifier = HkxSharedPtr();
    generator = HkxSharedPtr();
}

QString hkbModifierGenerator::evaluateDataValidity(){
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
    if (!modifier.data()){
        appenderror("modifier", "Null modifier!", NULL_SIGNATURE);
    }else if (modifier->getType() != HkxObject::TYPE_MODIFIER){
        appenderror("modifier", "modifier is invalid type! Setting null value!", modifier->getSignature());
        modifier = HkxSharedPtr();
    }
    if (!generator.data()){
        appenderror("generator", "Null generator!", NULL_SIGNATURE);
    }else if (generator->getType() != TYPE_GENERATOR){
        appenderror("generator", "generator is invalid type! Setting null value!", generator->getSignature());
        generator = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbModifierGenerator::~hkbModifierGenerator(){
    refCount--;
}
