#include "bsoffsetanimationgenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BSOffsetAnimationGenerator::refCount = 0;

const QString BSOffsetAnimationGenerator::classname = "BSOffsetAnimationGenerator";

BSOffsetAnimationGenerator::BSOffsetAnimationGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0),
      fOffsetVariable(0),
      fOffsetRangeStart(0),
      fOffsetRangeEnd(0)
{
    setType(BS_OFFSET_ANIMATION_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "OffsetAnimationGenerator_"+QString::number(refCount);
}

const QString BSOffsetAnimationGenerator::getClassname(){
    return classname;
}

QString BSOffsetAnimationGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool BSOffsetAnimationGenerator::insertObjectAt(int index, DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (obj->getSignature() == HKB_CLIP_GENERATOR && index == 1){
            pOffsetClipGenerator = HkxSharedPtr(obj);
        }else if (obj->getType() == TYPE_GENERATOR){
            pDefaultGenerator = HkxSharedPtr(obj);
        }else{
            return false;
        }
        return true;
    }
    return false;
}

bool BSOffsetAnimationGenerator::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index){
        pDefaultGenerator = HkxSharedPtr();
    }else if (index == 1){
        pOffsetClipGenerator = HkxSharedPtr();
    }else if (index == -1){
        pDefaultGenerator = HkxSharedPtr();
        pOffsetClipGenerator = HkxSharedPtr();
    }else{
        return false;
    }
    return true;
}

void BSOffsetAnimationGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

qreal BSOffsetAnimationGenerator::getFOffsetRangeEnd() const{
    std::lock_guard <std::mutex> guard(mutex);
    return fOffsetRangeEnd;
}

void BSOffsetAnimationGenerator::setFOffsetRangeEnd(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != fOffsetRangeEnd) ? fOffsetRangeEnd = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fOffsetRangeEnd' was not set!");
}

qreal BSOffsetAnimationGenerator::getFOffsetRangeStart() const{
    std::lock_guard <std::mutex> guard(mutex);
    return fOffsetRangeStart;
}

void BSOffsetAnimationGenerator::setFOffsetRangeStart(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != fOffsetRangeStart) ? fOffsetRangeStart = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fOffsetRangeStart' was not set!");
}

qreal BSOffsetAnimationGenerator::getFOffsetVariable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return fOffsetVariable;
}

void BSOffsetAnimationGenerator::setFOffsetVariable(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != fOffsetVariable) ? fOffsetVariable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fOffsetVariable' was not set!");
}

QString BSOffsetAnimationGenerator::getDefaultGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(pDefaultGenerator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

QString BSOffsetAnimationGenerator::getOffsetClipGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString genname("NONE");
    auto gen = static_cast<hkbGenerator *>(pOffsetClipGenerator.data());
    (gen) ? genname = gen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return genname;
}

bool BSOffsetAnimationGenerator::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data() || pOffsetClipGenerator.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> BSOffsetAnimationGenerator::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (pDefaultGenerator.data()){
        list.append(static_cast<DataIconManager*>(pDefaultGenerator.data()));
    }
    if (pOffsetClipGenerator.data()){
        list.append(static_cast<DataIconManager*>(pOffsetClipGenerator.data()));
    }
    return list;
}

int BSOffsetAnimationGenerator::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (pDefaultGenerator.data() == obj){
        return 0;
    }else if (pOffsetClipGenerator.data() == obj){
        return 1;
    }
    return -1;
}

bool BSOffsetAnimationGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "pDefaultGenerator"){
            checkvalue(pDefaultGenerator.readShdPtrReference(index, reader), "pDefaultGenerator");
        }else if (text == "pOffsetClipGenerator"){
            checkvalue(pOffsetClipGenerator.readShdPtrReference(index, reader), "pOffsetClipGenerator");
        }else if (text == "fOffsetVariable"){
            fOffsetVariable = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "fOffsetVariable");
        }else if (text == "fOffsetRangeStart"){
            fOffsetRangeStart = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "fOffsetRangeStart");
        }else if (text == "fOffsetRangeEnd"){
            fOffsetRangeEnd = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "fOffsetRangeEnd");
        }
    }
    index--;
    return true;
}

bool BSOffsetAnimationGenerator::write(HkxXMLWriter *writer){
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
        writeref(pDefaultGenerator, "pDefaultGenerator");
        writeref(pOffsetClipGenerator, "pOffsetClipGenerator");
        writedatafield("fOffsetVariable", QString::number(fOffsetVariable, char('f'), 6), false);
        writedatafield("fOffsetRangeStart", QString::number(fOffsetRangeStart, char('f'), 6), false);
        writedatafield("fOffsetRangeEnd", QString::number(fOffsetRangeEnd, char('f'), 6), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(pDefaultGenerator, "pDefaultGenerator");
        writechild(pOffsetClipGenerator, "pOffsetClipGenerator");
    }
    return true;
}

bool BSOffsetAnimationGenerator::link(){
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
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(pDefaultGenerator.getShdPtrReference());
    linkdata(TYPE_GENERATOR, NULL_SIGNATURE, pDefaultGenerator, "pDefaultGenerator", false);
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(pOffsetClipGenerator.getShdPtrReference());
    linkdata(TYPE_GENERATOR, HKB_CLIP_GENERATOR, pOffsetClipGenerator, "pOffsetClipGenerator", false);
    return true;
}

void BSOffsetAnimationGenerator::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    pDefaultGenerator = HkxSharedPtr();
    pOffsetClipGenerator = HkxSharedPtr();
}

QString BSOffsetAnimationGenerator::evaluateDataValidity(){
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
    if (!pDefaultGenerator.data()){
        appenderror("pDefaultGenerator", "Null pDefaultGenerator!", NULL_SIGNATURE);
    }else if (pDefaultGenerator->getType() != HkxObject::TYPE_GENERATOR){
        appenderror("pDefaultGenerator", "pDefaultGenerator is invalid type! Setting null value!", pDefaultGenerator->getSignature());
        pDefaultGenerator = HkxSharedPtr();
    }
    if (!pOffsetClipGenerator.data()){
        appenderror("pOffsetClipGenerator", "Null pOffsetClipGenerator!", NULL_SIGNATURE);
    }else if (pOffsetClipGenerator->getSignature() != HKB_CLIP_GENERATOR){
        appenderror("pOffsetClipGenerator", "pOffsetClipGenerator is invalid type! Setting null value!", pOffsetClipGenerator->getSignature());
        pOffsetClipGenerator = HkxSharedPtr();
    }
    setDataValidity(isvalid);
    return errors;
}

BSOffsetAnimationGenerator::~BSOffsetAnimationGenerator(){
    refCount--;
}
