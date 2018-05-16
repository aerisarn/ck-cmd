#include "commands\esp\io\EspWriter.h"

EspWriter::EspWriter(std::string path)
{
	out = fopen(path.c_str(), "wb");
}

EspWriter::~EspWriter()
{
	if (out)
	{
		fclose(out);
	}
}
