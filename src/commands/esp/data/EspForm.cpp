#include <commands\esp\data\EspForm.h>

// Form header operations
EspFormHeader::EspFormHeader() : Type(0), DataSize(0), Flags(0),
	FormID(0), Version(0), Revision(0), Unknown(0) {}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize, 
		EspUInt32 flags, EspFormID formID) 
	: Version(0), Revision(0), Unknown(0)
{
	setType(type);
	setDataSize(dataSize);
	setFlags(flags);
	setFormID(formID);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize, 
		EspUInt32 flags, EspFormID formID, EspUInt32 version)
	: Revision(0), Unknown(0)
{
	setType(type);
	setDataSize(dataSize);
	setFlags(flags);
	setFormID(formID);
	setVersion(version);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision)
	: Version(0), Unknown(0)
{
	setType(type);
	setDataSize(dataSize);
	setFlags(flags);
	setFormID(formID);
	setRevision(revision);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision,
		EspUInt32 version) 
	: Unknown(0)
{
	setType(type);
	setDataSize(dataSize);
	setFlags(flags);
	setFormID(formID);
	setVersion(version);
	setRevision(revision);
	setUnknown(0);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision,
		EspUInt32 version, EspUInt16 unknown)
{
	setType(type);
	setDataSize(dataSize);
	setFlags(flags);
	setFormID(formID);
	setVersion(version);
	setRevision(revision);
	setUnknown(unknown);
}

// Form operations
const EspFormHeader& EspForm::GetHeader() const
{
	return header;
}

void EspForm::SetHeader(const EspFormHeader& h)
{
	header = h;
}
