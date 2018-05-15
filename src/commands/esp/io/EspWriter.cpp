#include "commands\esp\io\EspWriter.h"

EspWriter::EspWriter(const char* path)
{
	out = fopen(path, "wb");
}

EspWriter::~EspWriter()
{
	if (out)
	{
		fclose(out);
	}
}
