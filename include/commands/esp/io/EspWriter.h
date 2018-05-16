#ifndef ESPWRITER_H
#define ESPWRITER_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>


class EspWriter
{
private:
	FILE* out;
	uint8_t* buffer;

public:
	EspWriter(const char* path);
	~EspWriter();

	inline uint32_t SwapType(uint32_t code)
	{
		return (code >> 24 |
			((code << 8) & 0x00FF0000) |
			((code >> 8) & 0x0000FF00) |
			code << 24);
	}

	//Write any type to file
	template <typename T>
	void Write(T data)
	{
		if (buffer)
		{
			memcpy(buffer, &data, sizeof(T));
			buffer += sizeof(T);
		}
		else 
		{
			fwrite(&data, sizeof(T), 1, out);
		}
	}

	inline void WriteType(uint32_t code)
	{
		Write<uint32_t>(SwapType(code));
	}
};

#endif //ESPWRITER_H