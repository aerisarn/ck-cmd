#ifndef ESPFILE_H
#define ESPFILE_H

#include <commands\esp\data\EspTES4Form.h>

class EspFile
{
private:
	EspTES4Form* header;

public:
	EspFile(EspFormHeader& fileHeader, EspTES4Hedr& hedr, EspUInt32 intv);
	~EspFile();

	EspTES4Form* GetHeader();

	void Write(EspWriter& w);
};

#endif //ESPFILE_H