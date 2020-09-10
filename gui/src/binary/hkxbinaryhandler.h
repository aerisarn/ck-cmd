#ifndef HKXBINARYREADER_H
#define HKXBINARYREADER_H

#include <windows.h>
#include <map>

#include <QByteArray>
#include <QVector>

#include "src/utility.h"

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>

class HkxFile;
class hkClass;

class HkxBinaryHandler final
{
public:
	HkxBinaryHandler(HkxFile *file = nullptr);
	HkxBinaryHandler& operator=(const HkxBinaryHandler&) = delete;
	HkxBinaryHandler(const HkxBinaryHandler &) = delete;
    ~HkxBinaryHandler() = default;
public:
    void setFile(HkxFile *file);
    bool parse();
    void clear();
    int getNumElements() const;
	const hkClass* getElementClass(size_t index) const;
	void* getElementObject(size_t index) const;
	size_t getElementIndex(const void* object) const;
	bool writeToFile();

	template<typename T>
	T& add(void* source) {
		std::lock_guard <std::mutex> guard(mutex);
		T* new_object = new T();
		hkVariant v;
		v.m_class = new_object->getClassType();
		v.m_object = &*new_object;
		written_objects[source] = objects.getSize();
		objects.pushBack(v);
		return *new_object;
	}

	template<typename T>
	T& add(void* source, const hkClass* hkclass, size_t ref) {
		std::lock_guard <std::mutex> guard(mutex);
		T* new_object = new T();
		hkVariant v;
		v.m_class = hkclass;
		v.m_object = &*new_object;
		if (ref == size_t(-1))
		{
			written_objects[source] = objects.getSize();
			objects.pushBack(v);
		}
		else
		{
			if (objects.getSize() <= ref)
				objects.setSize(ref + 1);
			written_objects[source] = ref;
			objects[ref] = v;
		}
		return *new_object;
	}

	template<typename T>
	T* get(void* source) {
		if (written_objects.find(source) == written_objects.end())
			return NULL;
		return (T*)written_objects[source];
	}

	bool getIsWritten(void* source);

	static QString readEnum(const char* enumName, const hkClass* definition, size_t value);
	static size_t writeEnum(const char* enumName, const hkClass* definition, const char*  value);
	static UI::hkQuadVariable readVector4(const ::hkVector4& in);
	static ::hkVector4 HkxBinaryHandler::writeVector4(const UI::hkQuadVariable& in);


private:
    HkxFile *hkxBinaryFile;
	hkArray<hkVariant> objects;
	std::map<void*, size_t> written_objects;
	std::mutex mutex;
};

#endif // HKXBINARYREADER_H
