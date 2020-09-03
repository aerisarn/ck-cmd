#include "hkbbehaviorgraph.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/behavior/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/behavior/generators/hkbstatemachine.h"

using namespace UI;

uint hkbBehaviorGraph::refCount = 0;

const QString hkbBehaviorGraph::classname = "hkbBehaviorGraph";

const QStringList hkbBehaviorGraph::VariableMode = {"VARIABLE_MODE_DISCARD_WHEN_INACTIVE", "VARIABLE_MODE_MAINTAIN_VALUES_WHEN_INACTIVE"};

hkbBehaviorGraph::hkbBehaviorGraph(HkxFile *parent, long ref)
    : hkbGenerator(parent, ref),
      userData(0),
      variableMode(VariableMode.first())
{
    setType(HKB_BEHAVIOR_GRAPH, TYPE_GENERATOR);
    parent->addObjectToFile(this, ref);
    refCount++;
    name = "BehaviorGraph_"+QString::number(refCount);
}

const QString hkbBehaviorGraph::getClassname(){
    return classname;
}

void hkbBehaviorGraph::setData(hkbBehaviorGraphData *graphdata){
    std::lock_guard <std::mutex> guard(mutex);
    if (graphdata){
        data = HkxSharedPtr(graphdata);
    }
}

hkbStateMachine *hkbBehaviorGraph::getRootGenerator() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (rootGenerator.data() && rootGenerator->getSignature() == HKB_STATE_MACHINE){
        return static_cast<hkbStateMachine *>(rootGenerator.data());
    }
    return nullptr;
}

void hkbBehaviorGraph::setName(const QString &newname){
    std::lock_guard <std::mutex> guard(mutex);
    (newname != name && newname != "") ? name = newname, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'name' was not set!");
}

QString hkbBehaviorGraph::getRootGeneratorName() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString rootname("NONE");
    hkbStateMachine *rootgen = static_cast<hkbStateMachine *>(rootGenerator.data());
    (rootgen) ? rootname = rootgen->getName() : LogFile::writeToLog(getClassname()+" Cannot get child name!");
    return rootname;
}

QString hkbBehaviorGraph::getName() const{
    std::lock_guard <std::mutex> guard(mutex);
    return name;
}

bool hkbBehaviorGraph::insertObjectAt(int , DataIconManager *obj){
    std::lock_guard <std::mutex> guard(mutex);
    if (obj->getSignature() == HKB_STATE_MACHINE){
        rootGenerator = HkxSharedPtr(obj);
        return true;
    }
    return false;
}

bool hkbBehaviorGraph::removeObjectAt(int index){
    std::lock_guard <std::mutex> guard(mutex);
    if (!index || index == -1){
        rootGenerator = HkxSharedPtr();
        return true;
    }
    return false;
}

void hkbBehaviorGraph::setVariableMode(const QString &value){
    std::lock_guard <std::mutex> guard(mutex);
    (value != variableMode && VariableMode.contains(value)) ? variableMode = value, setIsFileChanged(true) : LogFile::writeToLog(getClassname()+": 'variableMode' was not set!");
}

QString hkbBehaviorGraph::getVariableMode() const{
    std::lock_guard <std::mutex> guard(mutex);
    return variableMode;
}

bool hkbBehaviorGraph::hasChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    if (rootGenerator.data()){
        return true;
    }
    return false;
}

QVector<DataIconManager *> hkbBehaviorGraph::getChildren() const{
    std::lock_guard <std::mutex> guard(mutex);
    QVector<DataIconManager *> list;
    if (rootGenerator.data()){
        list.append(static_cast<DataIconManager*>(rootGenerator.data()));
    }
    return list;
}

int hkbBehaviorGraph::getIndexOfObj(DataIconManager *obj) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (rootGenerator.data() == obj){
        return 0;
    }
    return -1;
}

bool hkbBehaviorGraph::readData(const HkxXmlReader &reader, long & index){
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
        }else if (text == "eBlendCurve"){
            variableMode = reader.getElementValueAt(index);
            checkvalue(VariableMode.contains(variableMode), "variableMode");
        }else if (text == "rootGenerator"){
            checkvalue(rootGenerator.readShdPtrReference(index, reader), "rootGenerator");
        }else if (text == "data"){
            checkvalue(data.readShdPtrReference(index, reader), "data");
        }
    }
    index--;
    return true;
}

bool hkbBehaviorGraph::write(HkxXMLWriter *writer){
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
        writedatafield("variableMode", variableMode, false);
        writeref(rootGenerator, "rootGenerator");
        writeref(data, "data");
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        writechild(getVariableBindingSet(), "variableBindingSet");
        writechild(rootGenerator, "rootGenerator");
        writechild(data, "data");
    }
    return true;
}

bool hkbBehaviorGraph::link(){
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
    ptr = static_cast<BehaviorFile *>(getParentFile())->findGenerator(rootGenerator.getShdPtrReference());
    linkdata(TYPE_GENERATOR, HKB_STATE_MACHINE, rootGenerator, "rootGenerator", false);
    ptr = &static_cast<BehaviorFile *>(getParentFile())->getGraphData();
    linkdata(TYPE_OTHER, HKB_BEHAVIOR_GRAPH_DATA, data, "data", false);
    return true;
}

void hkbBehaviorGraph::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    HkDynamicObject::unlink();
    rootGenerator = HkxSharedPtr();
    data = HkxSharedPtr();
}

QString hkbBehaviorGraph::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    bool temp;
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
    if (!rootGenerator.data()){
        appenderror("rootGenerator", "Null rootGenerator!", NULL_SIGNATURE);
    }else if (rootGenerator->getType() != HkxObject::TYPE_GENERATOR){
        appenderror("rootGenerator", "rootGenerator is invalid type! Setting null value!", rootGenerator->getSignature());
        rootGenerator = HkxSharedPtr();
    }
    if (!data.data()){
        appenderror("data", "Null data!", NULL_SIGNATURE);
    }else if (data->getSignature() != HKB_BEHAVIOR_GRAPH_DATA){
        appenderror("data", "data is invalid type! Setting null value!", data->getSignature());
        data = HkxSharedPtr();
    }
    temp = VariableMode.contains(variableMode);
    if (!temp){
        appenderror("variableMode", "variableMode is an invalid value! Setting default value!", NULL_SIGNATURE);
        variableMode = VariableMode.first();
    }
    setDataValidity(isvalid);
    return errors;
}

hkbBehaviorGraph::~hkbBehaviorGraph(){
    refCount--;
}
