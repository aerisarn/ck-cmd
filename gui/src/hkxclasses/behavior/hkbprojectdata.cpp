#include "hkbprojectdata.h"
#include "hkbprojectstringdata.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/projectfile.h"

#include <hkbProjectData_2.h>
#include <hkbTransitionEffect_0.h>

using namespace UI;

uint UI::hkbProjectData::refCount = 0;

const QString UI::hkbProjectData::classname = "hkbProjectData";

const QStringList UI::hkbProjectData::EventMode = {"EVENT_MODE_IGNORE_FROM_GENERATOR"};

UI::hkbProjectData::hkbProjectData(HkxFile *parent, long ref, hkbProjectStringData *stringdata)
    : HkxObject(parent, ref),
      worldUpWS(0, 0, 1, 0),
      defaultEventMode(EventMode.first())
{
    setType(HKB_PROJECT_DATA, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
    if (stringdata){
        stringData = HkxSharedPtr(stringdata);
    }
}

const QString UI::hkbProjectData::getClassname(){
    return classname;
}

bool UI::hkbProjectData::readData(const HkxBinaryHandler& handler, const void* object) {
	const ::hkbProjectData* typedObject = (const ::hkbProjectData*)(object);
	auto ref = handler.getElementIndex(object);
	setReference(ref);
	auto checkvalue = [&](bool value, const QString & fieldname) {
		(!value) ? LogFile::writeToLog(getParentFilename() + ": " + getClassname() + ": readData()!\n'" + fieldname + "' has invalid data!\nObject Reference: " + QString::number(ref)) : NULL;
	};

	worldUpWS = HkxBinaryHandler::readVector4(typedObject->m_worldUpWS);
	checkvalue(stringData.readShdPtrReference(typedObject->m_stringData.val(), handler), "stringData");
	defaultEventMode = HkxBinaryHandler::readEnum("EventMode", &hkbTransitionEffectClass, typedObject->m_defaultEventMode);
	checkvalue(EventMode.contains(defaultEventMode), "defaultEventMode");
	return true;
}

hkReferencedObject* UI::hkbProjectData::write(HkxBinaryHandler& handler)
{
	if (!handler.getIsWritten(this)) {
		::hkbProjectData& object = handler.add<::hkbProjectData>(this, &hkbProjectDataClass, getReference());
		object.m_worldUpWS = HkxBinaryHandler::writeVector4(worldUpWS);
		object.m_defaultEventMode = (hkbTransitionEffect::EventMode)HkxBinaryHandler::writeEnum("EventMode", &hkbTransitionEffectClass, defaultEventMode.toLocal8Bit().data());
		object.m_stringData = dynamic_cast<::hkbProjectStringData*>(stringData->write(handler));
		return &object;
	}
	return handler.get<hkReferencedObject>(this);
}

bool UI::hkbProjectData::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    bool ok;
    QByteArray text;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    auto checkvalue = [&](bool value, const QString & fieldname){
        (!value) ? LogFile::writeToLog(getParentFilename()+": "+getClassname()+": readData()!\n'"+fieldname+"' has invalid data!\nObject Reference: "+ref) : NULL;
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "worldUpWS"){
            worldUpWS = readVector4(reader.getElementValueAt(index), &ok);
            checkvalue(ok, "raycastDistanceUp");
        }else if (text == "stringData"){
            checkvalue(stringData.readShdPtrReference(index, reader), "stringData");
        }else if (text == "defaultEventMode"){
            defaultEventMode = reader.getElementValueAt(index);
            checkvalue(EventMode.contains(defaultEventMode), "defaultEventMode");
        }
    }
    index--;
    return true;
}

bool UI::hkbProjectData::write(HkxXMLWriter *writer){
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    std::lock_guard <std::mutex> guard(mutex);
    auto result = true;
    if (writer && !getIsWritten()){
        QString refString = "null";
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        writedatafield("worldUpWS", worldUpWS.getValueAsString(), false);
        if (stringData.data()){
            refString = stringData->getReferenceString();
        }
        writedatafield("stringData", refString, false);
        writedatafield("defaultEventMode", defaultEventMode, false);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
        if (stringData.data() && !stringData->write(writer)){
            LogFile::writeToLog(getParentFilename()+": "+getClassname()+": write()!\nUnable to write 'stringData'!!!");
            result = false;
        }
    }
    return result;
}

bool UI::hkbProjectData::link(){
    std::lock_guard <std::mutex> guard(mutex);
    HkxSharedPtr *ptr;
    auto file = dynamic_cast<ProjectFile *>(getParentFile());
    if (file){
        ptr = file->findProjectStringData(stringData.getShdPtrReference());
    }else{
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nParent file type is invalid!!!");
    }
    if (!ptr->data()){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\nFailed to properly link 'stringData' data field!");
        setDataValidity(false);
    }else if ((*ptr)->getSignature() != HKB_PROJECT_STRING_DATA){
        LogFile::writeToLog(getParentFilename()+": "+getClassname()+": link()!\n'stringData' data field is linked to invalid child!");
        setDataValidity(false);
        stringData = *ptr;
    }else{
        stringData = *ptr;
    }
    return true;
}

void UI::hkbProjectData::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    stringData = HkxSharedPtr();
}

QString UI::hkbProjectData::evaluateDataValidity(){   //TO DO...
    /*std::lock_guard <std::mutex> guard(mutex);
    if (!EventMode.contains(defaultEventMode)){
    }else if (!stringData.data() || stringData->getSignature() != HKB_PROJECT_STRING_DATA){
    }else{
        setDataValidity(true);
        return QString();
    }
    setDataValidity(false);*/
    return QString();
}

UI::hkbProjectData::~hkbProjectData(){
    refCount--;
}
