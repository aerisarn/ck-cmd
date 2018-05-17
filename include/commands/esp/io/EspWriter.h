#ifndef ESPWRITER_H
#define ESPWRITER_H

#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <string>


class EspWriter
{
private:
	std::ofstream* out;

	inline uint32_t SwapType(uint32_t code)
	{
		return (code >> 24 |
			((code << 8) & 0x00FF0000) |
			((code >> 8) & 0x0000FF00) |
			code << 24);
	}

public:
	EspWriter(std::string path);
	~EspWriter();

	//Write any type to file
	template <typename T>
	void Write(T data)
	{
		out->write((char*)&data, sizeof(data));
	}

	inline void WriteType(uint32_t code)
	{
		Write<uint32_t>(SwapType(code));
	}

	inline void WriteZString(std::string str)
	{
		const char* s = str.c_str();
		out->write(s, str.size() + 1);
	}
};

#endif //ESPWRITER_H