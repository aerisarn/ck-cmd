#include "hkbprojectstringdata.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/xml/hkxxmlreader.h"
#include "src/filetypes/projectfile.h"

#include <hkbProjectStringData_1.h>

using namespace UI;

uint UI::hkbProjectStringData::refCount = 0;

const QString UI::hkbProjectStringData::classname = "hkbProjectStringData";

UI::hkbProjectStringData::hkbProjectStringData(HkxFile *parent, long ref, const QString &characterfilename)
    : HkxObject(parent, ref)
{
    setType(HKB_PROJECT_STRING_DATA, TYPE_OTHER);
    parent->addObjectToFile(this, ref);
    refCount++;
    (characterfilename != "") ? characterFilenames.append(characterfilename) : NULL;
}

const QString UI::hkbProjectStringData::getClassname(){
    return classname;
}

QString UI::hkbProjectStringData::getCharacterFilePathAt(int index) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (index >= 0 && index < characterFilenames.size()){
        return QString(characterFilenames.at(index)).replace("\\", "/").section("/", -2, -1);
    }
    return "";
}
bool UI::hkbProjectStringData::readData(const HkxBinaryHandler& handler, const void* object) {
	const ::hkbProjectStringData* typedObject = (const ::hkbProjectStringData*)(object);
	auto ref = handler.getElementIndex(object);
	setReference(ref);

	auto numElems = 0;
	auto getstrings = [&](QStringList & strings, const hkArray<hkStringPtr> & field) {
		numElems = field.getSize();
		if (numElems > 0) {
			for (int index = 0; index < numElems; index++) {
				strings.append(field[index].cString());
			}
		}
	};
	auto getpath = [&](QString & field, const hkStringPtr & fieldname) {
		field = fieldname.cString();
		if (field == "") {
			LogFile::writeToLog(": " + getClassname() + ": readData()!\nFailed to properly read '" + fieldname + "' data field!\nObject Reference: " + ref);
		}
	};
	getstrings(animationFilenames, typedObject->m_animationFilenames);
	getstrings(behaviorFilenames, typedObject->m_behaviorFilenames);
	getstrings(characterFilenames, typedObject->m_characterFilenames);
	getstrings(eventNames, typedObject->m_eventNames);
	getpath(animationPath, typedObject->m_animationPath);
	getpath(behaviorPath, typedObject->m_behaviorPath);
	getpath(characterPath, typedObject->m_characterPath);
	getpath(fullPathToSource, typedObject->m_fullPathToSource);

	return true;
}

bool UI::hkbProjectStringData::readData(const HkxXmlReader &reader, long & index){
    std::lock_guard <std::mutex> guard(mutex);
    auto ok = true;
    auto numElems = 0;
    auto ref = reader.getNthAttributeValueAt(index - 1, 0);
    QByteArray text;
    auto getstrings = [&](QStringList & strings, const QString & fieldname){
        numElems = reader.getNthAttributeValueAt(index, 1).toInt(&ok);
        if (!ok){
            LogFile::writeToLog(": "+getClassname()+": readData()!\nFailed to properly read '"+fieldname+"' data!\nObject Reference: "+ref);
        }
        if (numElems > 0){
            index++;
            numElems = numElems + index;
            for (; index < numElems; index++){
                if (reader.getElementNameAt(index) != "hkcstring" || index >= reader.getNumElements()){
                    LogFile::writeToLog(": "+getClassname()+": readData()!\nFailed to properly read 'behaviorFilenames' data!\nObject Reference: "+ref);
                }
                strings.append(reader.getElementValueAt(index));
            }
        }
    };
    auto getpath = [&](QString & field, const QString & fieldname){
        field = reader.getElementValueAt(index);
        if (field == ""){
            LogFile::writeToLog(": "+getClassname()+": readData()!\nFailed to properly read '"+fieldname+"' data field!\nObject Reference: "+ref);
        }
    };
    for (; index < reader.getNumElements() && reader.getNthAttributeNameAt(index, 1) != "class"; index++){
        text = reader.getNthAttributeValueAt(index, 0);
        if (text == "animationFilenames"){
            getstrings(animationFilenames, "animationFilenames");
        }else if (text == "behaviorFilenames"){
            getstrings(behaviorFilenames, "behaviorFilenames");
        }else if (text == "characterFilenames"){
            getstrings(characterFilenames, "characterFilenames");
        }else if (text == "eventNames"){
            getstrings(eventNames, "eventNames");
        }else if (text == "animationPath"){
            getpath(animationPath, "animationPath");
        }else if (text == "behaviorPath"){
            getpath(behaviorPath, "behaviorPath");
        }else if (text == "characterPath"){
            getpath(characterPath, "characterPath");
        }else if (text == "fullPathToSource"){
            getpath(fullPathToSource, "fullPathToSource");
        }
    }
    index--;
    return true;
}

hkReferencedObject* UI::hkbProjectStringData::write(HkxBinaryHandler& handler)
{
	auto numElems = 0;
	auto setstrings = [&](const QStringList & strings, hkArray<hkStringPtr> & field) {
		numElems = strings.count();
		field.clear();
		if (numElems > 0) {
			for (int index = 0; index < numElems; index++) {
				field.pushBack(strings[index].toLocal8Bit().data());
			}
		}
	};
	auto setpath = [&](const QString & field, hkStringPtr & fieldname) {
		fieldname = field.toLocal8Bit().data();
	};

	if (!handler.getIsWritten(this)) {
		::hkbProjectStringData& typedObject = handler.add<::hkbProjectStringData>(this, &hkbProjectStringDataClass, getReference());
		setstrings(animationFilenames, typedObject.m_animationFilenames);
		setstrings(behaviorFilenames, typedObject.m_behaviorFilenames);
		setstrings(characterFilenames, typedObject.m_characterFilenames);
		setstrings(eventNames, typedObject.m_eventNames);
		setpath(animationPath, typedObject.m_animationPath);
		setpath(behaviorPath, typedObject.m_behaviorPath);
		setpath(characterPath, typedObject.m_characterPath);
		setpath(fullPathToSource, typedObject.m_fullPathToSource);
		return &typedObject;
	}
	return handler.get<hkReferencedObject>(this);
}


bool UI::hkbProjectStringData::write(HkxXMLWriter *writer){
    std::lock_guard <std::mutex> guard(mutex);
    auto writedatafield = [&](const QString & name, const QString & value, bool allownull){
        writer->writeLine(writer->parameter, QStringList(writer->name), QStringList(name), value, allownull);
    };
    if (writer && !getIsWritten()){
        QStringList list1 = QStringList({writer->name, writer->clas, writer->signature});
        QStringList list2 = QStringList({getReferenceString(), getClassname(), "0x"+QString::number(getSignature(), 16)});
        writer->writeLine(writer->object, list1, list2, "");
        QStringList list3 = QStringList({writer->name, writer->numelements});
        QStringList list4 = QStringList({"animationFilenames", QString::number(animationFilenames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < animationFilenames.size(); i++){
            writedatafield("", animationFilenames.at(i), false);
        }
        if (animationFilenames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"behaviorFilenames", QString::number(behaviorFilenames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < behaviorFilenames.size(); i++){
            writedatafield("", behaviorFilenames.at(i), false);
        }
        if (behaviorFilenames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"characterFilenames", QString::number(characterFilenames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < characterFilenames.size(); i++){
            writedatafield("", QString(characterFilenames.at(i)).replace("/", "\\"), false);
        }
        if (characterFilenames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        list3 = QStringList({writer->name, writer->numelements});
        list4 = QStringList({"eventNames", QString::number(eventNames.size())});
        writer->writeLine(writer->parameter, list3, list4, "");
        for (auto i = 0; i < eventNames.size(); i++){
            writedatafield("", eventNames.at(i), false);
        }
        if (eventNames.size() > 0){
            writer->writeLine(writer->parameter, false);
        }
        writedatafield("animationPath", animationPath, true);
        writedatafield("behaviorPath", behaviorPath, true);
        writedatafield("characterPath", characterPath, true);
        writedatafield("fullPathToSource", fullPathToSource, true);
        writer->writeLine(writer->object, false);
        setIsWritten();
        writer->writeLine("\n");
    }
    return true;
}

bool UI::hkbProjectStringData::link(){
    return true;
}

QString UI::hkbProjectStringData::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    QString errors;
    auto isvalid = true;
    auto checkstring = [&](const QStringList & list, const QString & fieldname){
        for (auto i = 0; i < list.size(); i++){
            if (list.at(i) == ""){
                isvalid = false;
                errors.append(": "+getClassname()+": Ref: "+getReferenceString()+": Invalid "+fieldname+" at "+QString::number(i)+"!");
            }
        }
    };
    checkstring(animationFilenames, "animationFilenames");
    checkstring(behaviorFilenames, "behaviorFilenames");
    checkstring(characterFilenames, "characterFilenames");
    checkstring(eventNames, "eventNames");
    //checkstring(QStringList(animationPath), "eventNames");
    //checkstring(QStringList(behaviorPath), "behaviorPath");
    checkstring(QStringList(characterPath), "characterPath");
    //checkstring(QStringList(fullPathToSource), "fullPathToSource");
    setDataValidity(true);
    return errors;
}

UI::hkbProjectStringData::~hkbProjectStringData(){
    refCount--;
}
