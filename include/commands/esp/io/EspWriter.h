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
};

#endif //ESPWRITER_H