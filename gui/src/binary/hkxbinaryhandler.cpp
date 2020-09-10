#include <mutex>
#include "src/filetypes/behaviorfile.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"
#include "hkxbinaryhandler.h"

using namespace UI;

HkxBinaryHandler::HkxBinaryHandler(HkxFile *file)
    : hkxBinaryFile(file)
{
}

bool HkxBinaryHandler::parse(){
	try {
		hkIstream stream(hkxBinaryFile->getFileName().toLocal8Bit().data());
		hkStreamReader *reader = stream.getStreamReader();
		hkResource* resource = hkSerializeLoadResource(reader, objects);
	}
	catch (...) {
		return false;
	}
	return true;
}

void HkxBinaryHandler::setFile(HkxFile *file){
    hkxBinaryFile = file;
}

int HkxBinaryHandler::getNumElements() const{
	return objects.getSize();
}

void HkxBinaryHandler::clear(){
	objects.clear();
	written_objects.clear();
}

const hkClass* HkxBinaryHandler::getElementClass(size_t index) const
{
	return index < objects.getSize() ?
		objects[index].m_class : NULL;
}

void* HkxBinaryHandler::getElementObject(size_t index) const
{
	return index < objects.getSize() ?
		objects[index].m_object : NULL;
}

size_t HkxBinaryHandler::getElementIndex(const void* object) const {
	for (size_t index = 0; index < objects.getSize(); index++) {
		if (objects[index].m_object == object) {
			return index;
		}
	}
	return (size_t)-1;
}

//translate enums
QString HkxBinaryHandler::readEnum(const char* enumName, const hkClass* definition, size_t value) {
	auto enum_definition = definition->getEnumByName(enumName);
	if (enum_definition)
	{
		const char * name = NULL;
		enum_definition->getNameOfValue(value, &name);
		return QString::fromLocal8Bit(name);
	}
	return {};
}

size_t HkxBinaryHandler::writeEnum(const char* enumName, const hkClass* definition, const char*  value) {
	int num_value = -1;
	auto enum_definition = definition->getEnumByName(enumName);
	if (enum_definition)
	{
		enum_definition->getValueOfName(value, &num_value);
	}
	return num_value;
}

//translate types
UI::hkQuadVariable HkxBinaryHandler::readVector4(const ::hkVector4& in) {
	return {
		in.getSimdAt(0),
		in.getSimdAt(1),
		in.getSimdAt(2),
		in.getSimdAt(3)
	};
}

::hkVector4 HkxBinaryHandler::writeVector4(const UI::hkQuadVariable& in) {
	return hkVector4(in.x, in.y, in.z, in.w);
}

bool HkxBinaryHandler::writeToFile() {
	auto result = false;
	if (hkxBinaryFile) {
		written_objects.clear();
		auto rootContainer = hkxBinaryFile->getRootObject().get<UI::hkRootLevelContainer>()->getBinaryRoot(*this);
		hkPackFormat pkFormat = HKPF_DEFAULT;
		hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;
		hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);
		//string out = out_path_abs+"\\" + out_name + ".hkx";
		//fs::create_directories(fhkxXmlFile->fileName());
		hkOstream stream(hkxBinaryFile->fileName().toLocal8Bit().data());
		hkVariant root = { &rootContainer, &rootContainer.staticClass() };
		hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
		if (res != HK_SUCCESS)
		{
			//Log::Error("Havok reports save failed.");
		}
		//packFileOptions = GetWriteOptionsFromFormat(HKPF_AMD64);
		//hkOstream stream_se(hkxBinaryFile->fileName().toLocal8Bit().data());
		//res = hkSerializeUtilSave(HKPF_AMD64, root, stream_se, flags, packFileOptions);
		//if (res != HK_SUCCESS)
		//{
		//	//Log::Error("Havok reports save failed.");
		//}
		result = true;
	}
	return result;
}

bool HkxBinaryHandler::getIsWritten(void* source) {
	return written_objects.find(source) != written_objects.end();
}
