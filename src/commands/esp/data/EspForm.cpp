#include <commands\esp\data\EspForm.h>

// Field header operations
EspFieldHeader::EspFieldHeader(EspUInt32 fType, EspUInt16 fSize)
	: type(fType), size(fSize) {}

// Form header operations
EspFormHeader::EspFormHeader() : Type(0), DataSize(0), Flags(0),
	FormID(0), Version(0), Revision(0), Unknown(0) {}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize, 
		EspUInt32 flags, EspFormID formID) 
	: Version(0), Revision(0), Unknown(0)
{
	SetType(type);
	SetDataSize(dataSize);
	SetFlags(flags);
	SetFormID(formID);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision)
	: Version(0), Unknown(0)
{
	SetType(type);
	SetDataSize(dataSize);
	SetFlags(flags);
	SetFormID(formID);
	SetRevision(revision);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision,
		EspUInt32 version) 
	: Unknown(0)
{
	SetType(type);
	SetDataSize(dataSize);
	SetFlags(flags);
	SetFormID(formID);
	SetVersion(version);
	SetRevision(revision);
	SetUnknown(0);
}

EspFormHeader::EspFormHeader(EspUInt32 type, EspUInt32 dataSize,
		EspUInt32 flags, EspFormID formID, EspUInt32 revision,
		EspUInt32 version, EspUInt16 unknown)
{
	SetType(type);
	SetDataSize(dataSize);
	SetFlags(flags);
	SetFormID(formID);
	SetVersion(version);
	SetRevision(revision);
	SetUnknown(unknown);
}

void EspFormHeader::Write(EspWriter& w)
{
	w.WriteType(GetType());
	w.Write<EspUInt32>(GetDataSize());
	w.Write<EspUInt32>(GetFlags());
	w.Write<EspFormID>(GetFormID());
	w.Write<EspUInt32>(GetRevision());
	w.Write<EspUInt32>(GetVersion());
	w.Write<EspUInt16>(GetUnknown());
}

// Form operations
const EspFormHeader& EspForm::GetHeader()
{
	return header;
}

void EspForm::SetHeader(const EspFormHeader& h)
{
	header = h;
}

void EspForm::WriteField(EspFieldHeader& h, EspWriter& w)
{
	if (h.size > 0xFFFF)
	{
		WriteField(EspFieldHeader('XXXX', 4), w);
		w.Write<EspUInt32>(h.size);
		h.size = 0;
	}
	w.Write<EspFieldHeader>(h);
}
