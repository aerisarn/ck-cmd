#include "hkbevaluateexpressionmodifier.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbexpressiondataarray.h"

using namespace UI;

uint hkbEvaluateExpressionModifier::refCount = 0;

const QString hkbEvaluateExpressionModifier::classname = "hkbEvaluateExpressionModifier";

hkbEvaluateExpressionModifier::hkbEvaluateExpressionModifier(HkxFile *parent, long ref)
    : hkbModifier(parent, ref),
      userData(2),
      enable(true)
{
    setType(HKB_EVALUATE_EXPRESSION_MODIFIER, TYPE_MODIFIER);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "EvaluateExpressionModifier_"+QString::number(refCount);
}

const QString hkbEvaluateExpressionModifier::getClassname(){
    return classname;
}

QString hkbEvaluateExpressionModifier::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbEvaluateExpressionModifier::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "expressions"){
            checkvalue(expressions.readShdPtrReference(index, reader), "expressions");
        }
    }
    index--;
    return true;
}

bool hkbEvaluateExpressionModifier::write(HkxXMLWriter *writer){
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
        writeref(expressions, "expressions");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(expressions, "expressions");
    }
    return true;
}

int hkbEvaluateExpressionModifier::getNumberOfExpressions() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (expressions.data()){
        return static_cast<hkbExpressionDataArray *>(expressions.data())->expressionsData.size();
    }
    return 0;
}

bool hkbEvaluateExpressionModifier::isEventReferenced(int eventindex) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (expressions.constData() && expressions.constData()->isEventReferenced(eventindex)){
        return true;
    }
    return false;
}

void hkbEvaluateExpressionModifier::updateEventIndices(int eventindex){
    std::lock_guard <std::mutex> guard(mutex);
    (expressions.data()) ? expressions->updateEventIndices(eventindex) : NULL;
}

void hkbEvaluateExpressionModifier::fixMergedEventIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    (expressions.data()) ? expressions->fixMergedEventIndices(dominantfile) : NULL;
}

bool hkbEvaluateExpressionModifier::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbEvaluateExpressionModifier *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_EVALUATE_EXPRESSION_MODIFIER){
        obj = static_cast<hkbEvaluateExpressionModifier *>(recessiveObject);
        if (expressions.data()){
            if (obj->expressions.data()){
                expressions->merge(obj->expressions.data());
            }
        }else if (obj->expressions.data()){
            expressions = obj->expressions;
            getParentFile()->addObjectToFile(obj->expressions.data(), 0);   //TO DO: fix merged event indices???
        }
        injectWhileMerging(obj);
        return true;
    }
    return false;
}

void hkbEvaluateExpressionModifier::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    setBindingReference(++ref);
    (expressions.data()) ? expressions->updateReferences(++ref) : NULL;
}

QVector<HkxObject *> hkbEvaluateExpressionModifier::getChildrenOtherTypes() const{
    QVector<HkxObject *> list;
    (expressions.data()) ? list.append(expressions.data()) : NULL;
    return list;
}

void hkbEvaluateExpressionModifier::setExpressions(hkbExpressionDataArray *value){
    std::lock_guard <std::mutex> guard(mutex);
    expressions = value;
}

hkbExpressionDataArray *hkbEvaluateExpressionModifier::getExpressions() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbExpressionDataArray *>(expressions.data());
}

bool hkbEvaluateExpressionModifier::getEnable() const{
    std::lock_guard <std::mutex> guard(mutex);
    return enable;
}

void hkbEvaluateExpressionModifier::setEnable(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != enable) ? enable = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'enable' was not set!");
}

void hkbEvaluateExpressionModifier::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

bool hkbEvaluateExpressionModifier::link(){
    std::lock_guard <std::mutex> guard(mutex);
    if (!static_cast<HkDynamicObject *>(this)->linkVar()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'variableBindingSet' data field!\nObject Name: "+name);
    }
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(expressions.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_EXPRESSION_DATA_ARRAY){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": linkVar()!\nThe linked object 'expressions' is not a HKB_EXPRESSION_DATA_ARRAY!");
            setDataValidity(false);
        }
        expressions = *ptr;
    }
    return true;
}

void hkbEvaluateExpressionModifier::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    expressions = HkxSharedPtr();
}

QString hkbEvaluateExpressionModifier::evaluateDataValidity(){
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
    if (expressions.data()){
        if (expressions->getSignature() != HKB_EXPRESSION_DATA_ARRAY){
            isvalid = false;
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid expressions type! Signature: "+QString::number(expressions->getSignature(), 16)+" Setting default value!");
            expressions = HkxSharedPtr();
        }else if (static_cast<hkbExpressionDataArray *>(expressions.data())->expressionsData.size() < 1){
            errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": expressions has no expressionsData! Setting null value!");
            expressions = HkxSharedPtr();
        }else if (expressions->isDataValid() && expressions->evaluateDataValidity() != ""){
            isvalid = false;
            //errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Invalid expressions data!");
        }
    }else if (!expressions.data()){
        isvalid = false;
        errors.append(getParentFilename()+": "+getClassname()+": Ref: "+getReferenceString()+": "+name+": Null expressions!");
    }
    setDataValidity(isvalid);
    return errors;
}

hkbEvaluateExpressionModifier::~hkbEvaluateExpressionModifier(){
    refCount--;
}
