#include "commands\esp\io\EspWriter.h"

EspWriter::EspWriter(std::string path)
{
	out = new std::ofstream(path, std::ios::binary);
}

EspWriter::~EspWriter()
{
	out->close();
	delete out;
}
