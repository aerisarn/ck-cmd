#include <commands\esp\data\EspFile.h>

EspFile::EspFile(EspFormHeader& fileHeader, EspTES4Hedr& hedr, EspUInt32 intv)
{
	header = new EspTES4Form(hedr, intv);
	header->SetHeader(fileHeader);
}

EspFile::~EspFile()
{
	delete header;
}

void EspFile::Write(EspWriter& w)
{
	header->Write(w);
}

EspTES4Form* EspFile::GetHeader()
{
	return header;
}
