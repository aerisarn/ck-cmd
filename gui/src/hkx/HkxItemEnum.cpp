#include "HkxItemEnum.h"

using namespace ckcmd::HKX;

HkxItemEnum::HkxItemEnum(int value, const hkClassEnum* enum_class) : _value(value), _enum_class(enum_class) {}

int HkxItemEnum::value() { return _value; }
void HkxItemEnum::setValue(int value) { _value = value; }


QString HkxItemEnum::value_literal()
{
	char* name = new char[256];
	_enum_class->getNameOfValue(_value, (const char**)&name);
	return QString::fromStdString(name);
}

QStringList HkxItemEnum::enumValues() {
	QStringList results;
	for (int i = 0; i < _enum_class->getNumItems(); i++)
	{
		char* name = new char[256];
		_enum_class->getNameOfValue(i, (const char**)&name);
		results.push_back(QString::fromStdString(name));
	}
	return results;
}

