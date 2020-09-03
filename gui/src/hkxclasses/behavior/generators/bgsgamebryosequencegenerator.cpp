#include "bgsgamebryosequencegenerator.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"

using namespace UI;

uint BGSGamebryoSequenceGenerator::refCount = 0;

const QString BGSGamebryoSequenceGenerator::classname = "BGSGamebryoSequenceGenerator";

const QStringList BGSGamebryoSequenceGenerator::BlendModeFunction = {"BMF_NONE", "BMF_PERCENT", "BMF_ONE_MINUS_PERCENT"};

BGSGamebryoSequenceGenerator::BGSGamebryoSequenceGenerator(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0)
{
    setType(BGS_GAMEBYRO_SEQUENCE_GENERATOR, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "BGSGamebryoSequenceGenerator_"+QString::number(refCount);
}

const QString BGSGamebryoSequenceGenerator::getClassname(){
    return classname;
}

bool BGSGamebryoSequenceGenerator::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "pSequence"){
            pSequence = reader.getElementValueAt(index);
            checkvalue((pSequence == ""), "pSequence");
        }else if (text == "eBlendModeFunction"){
            eBlendModeFunction = reader.getElementValueAt(index);
            checkvalue(BlendModeFunction.contains(eBlendModeFunction), "eBlendModeFunction");
        }else if (text == "fPercent"){
            fPercent = reader.getElementValueAt(index).toDouble(&ok);
            checkvalue(ok, "fPercent");
        }
    }
    index--;
    return true;
}

bool BGSGamebryoSequenceGenerator::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        if (getVariableBindingSetData()){
            refString = getVariableBindingSet()->getReferenceString();
        }
        writedatafield("variableBindingSet", refString, false);
        writedatafield("userData", QString::number(userData), false);
        writedatafield("name", name, false);
        writedatafield("pSequence", pSequence, true);
        writedatafield("eBlendModeFunction", eBlendModeFunction, false);
        writedatafield("fPercent", QString::number(fPercent, char('f'), 6), false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (getVariableBindingSetData() && !getVariableBindingSet()->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'variableBindingSet'!!!");
        }
    }
    return true;
}

bool BGSGamebryoSequenceGenerator::link(){
    return true;
}

void BGSGamebryoSequenceGenerator::setFPercent(const qreal &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != fPercent) ? fPercent = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'fPercent' was not set!");
}

void BGSGamebryoSequenceGenerator::setEBlendModeFunction(int index){
    std::lock_guard <std::mutex> guard(mutex);
    (index >= 0 && index < BlendModeFunction.size() && eBlendModeFunction != BlendModeFunction.at(index)) ? eBlendModeFunction = BlendModeFunction.at(index), setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'eBlendModeFunction' was not set!");
}

void BGSGamebryoSequenceGenerator::setPSequence(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != pSequence && value != "") ? pSequence = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'pSequence' was not set!");
}

void BGSGamebryoSequenceGenerator::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

qreal BGSGamebryoSequenceGenerator::getFPercent() const{
    std::lock_guard <std::mutex> guard(mutex);
    return fPercent;
}

QString BGSGamebryoSequenceGenerator::getEBlendModeFunction() const{
    std::lock_guard <std::mutex> guard(mutex);
    return eBlendModeFunction;
}

QString BGSGamebryoSequenceGenerator::getPSequence() const{
    std::lock_guard <std::mutex> guard(mutex);
    return pSequence;
}

QString BGSGamebryoSequenceGenerator::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

QString BGSGamebryoSequenceGenerator::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    bool temp;
    auto isvalid = true;
    auto appenderror = [&](bool value, const QString & fieldname){
        if (!value){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": Invalid '"+fieldname+"'!");
        }
    };
    appenderror((HkDynamicObject::evaluateDataValidity() == ""), "hkbVariableBindingSet");
    appenderror((name != ""), "name");
    appenderror((pSequence == ""), "pSequence");
    temp = BlendModeFunction.contains(eBlendModeFunction);
    appenderror(temp, "eBlendModeFunction");
    if (!temp){
        eBlendModeFunction = BlendModeFunction.first();
    }
    setDataValidity(isvalid);
    return errors;
}

BGSGamebryoSequenceGenerator::~BGSGamebryoSequenceGenerator(){
    refCount--;
}
