#include <src/utility/Containers.h>

bool ckcmd::HKX::isWordVariable(int type)
{
	return type == hkbVariableInfo::VARIABLE_TYPE_BOOL ||
		type == hkbVariableInfo::VARIABLE_TYPE_INT8 ||
		type == hkbVariableInfo::VARIABLE_TYPE_INT16 ||
		type == hkbVariableInfo::VARIABLE_TYPE_INT32 ||
		type == hkbVariableInfo::VARIABLE_TYPE_REAL;
}

bool ckcmd::HKX::isQuadVariable(int type)
{
	return type == hkbVariableInfo::VARIABLE_TYPE_VECTOR3 ||
		type == hkbVariableInfo::VARIABLE_TYPE_VECTOR4 ||
		type == hkbVariableInfo::VARIABLE_TYPE_QUATERNION;
}

bool ckcmd::HKX::isRefVariable(int type)
{
	return type == hkbVariableInfo::VARIABLE_TYPE_POINTER;
}