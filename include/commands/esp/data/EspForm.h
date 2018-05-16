#ifndef FORM_H
#define FORM_H

#define FORMHEADER_MEMBER(type, name) \
private: \
	type name; \
public: \
	const type& Get##name##() const { return name; } \
	void Set##name##(const type& val) { name = val; }

#include <commands\esp\io\EspTypes.h>
#include <commands\esp\io\EspWriter.h>

struct EspFieldHeader
{
	EspUInt32 type;
	EspUInt16 size;

	EspFieldHeader(EspUInt32 type, EspUInt16 size);
};

typedef struct EspFieldHeader EspFieldHeader;

class EspFormHeader
{
	FORMHEADER_MEMBER(EspUInt32, Type)
	FORMHEADER_MEMBER(EspUInt32, DataSize)
	FORMHEADER_MEMBER(EspUInt32, Flags)
	FORMHEADER_MEMBER(EspFormID, FormID)
	FORMHEADER_MEMBER(EspUInt32, Revision)
	FORMHEADER_MEMBER(EspUInt32, Version)
	FORMHEADER_MEMBER(EspUInt16, Unknown)

public:
	EspFormHeader();
	EspFormHeader(EspUInt32 type, EspUInt32 dataSize, EspUInt32 flags,
		EspFormID formID);
	EspFormHeader(EspUInt32 type, EspUInt32 dataSize, EspUInt32 flags,
		EspFormID formID, EspUInt32 version);
	EspFormHeader(EspUInt32 type, EspUInt32 dataSize, EspUInt32 flags,
		EspFormID formID, EspUInt32 revision, EspUInt32 version);
	EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision,
		EspUInt32 version, EspUInt16 unknown);
	
	void Write(EspWriter& w);
};

class EspForm
{
public:
	EspForm() {}
	EspForm(const EspFormHeader& h) : header(h) {}
	virtual ~EspForm() {}

	const EspFormHeader& GetHeader();
	void SetHeader(const EspFormHeader& h);

protected:
	EspFormHeader header;
	virtual void Write(EspWriter& w) = 0;
	void WriteField(EspFieldHeader& h, EspWriter& w);
};

#endif //FORM_H