#ifndef ESPTYPES_H
#define ESPTYPES_H

#include <stdint.h>
#include <string>

// Numerical and character types
typedef char EspChar;
typedef wchar_t EspWChar;
typedef int8_t EspInt8;
typedef uint8_t EspUInt8;
typedef int16_t EspInt16;
typedef uint16_t EspUInt16;
typedef int32_t EspInt32;
typedef uint32_t EspUInt32;
typedef int64_t EspInt64;
typedef uint64_t EspUInt64;
typedef float EspFloat32;
typedef double EspFloat64;

// Form information
typedef uint32_t EspFormID;
typedef uint32_t EspIRef;
typedef uint64_t EspHash;

// String struct definitions
struct EspBString
{
	EspUInt8 length;
	char* str;
};

struct EspBZString
{
	EspUInt8 length;
	std::string str;
};

struct EspWString
{
	EspUInt16 length;
	char* str;
};

struct EspWZString
{
	EspUInt16 length;
	std::string str;
};

// ESP strings
typedef uint32_t EspLString;
typedef struct EspBString EspBString;
typedef struct EspBZString EspBZString;
typedef struct EspWString EspWString;
typedef struct EspWZString EspWZString;

#endif //ESPTYPES_H