#ifndef FORM_H
#define FORM_H

#define FORMHEADER_MEMBER(type, name) \
private: \
	type name; \
public: \
	const type& get##name##() const { return name; } \
	void set##name##(const type& val) { name = val; }

#include <commands\esp\io\EspTypes.h>

struct EspFieldHeader
{
	EspUInt32 type;
	EspUInt16 size;
};

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
		EspFormID formID, EspUInt32 revision);
	EspFormHeader(EspUInt32 type, EspUInt32 dataSize, EspUInt32 flags,
		EspFormID formID, EspUInt32 revision, EspUInt32 version);
	EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision,
		EspUInt32 version, EspUInt16 unknown);
};

class EspForm
{
public:
	EspForm() {}
	EspForm(const EspFormHeader& h) : header(h) {}
	virtual ~EspForm() {}

	virtual void Save() = 0;
	const EspFormHeader& GetHeader() const;
	void SetHeader(const EspFormHeader& h);

protected:
	EspFormHeader header;
};

#endif //FORM_H