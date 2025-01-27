#pragma once
#include "nif_math.h"

Niflib::Matrix33 MatrixFromEuler(float x, float y, float z);
Niflib::Quaternion QuaternionSlerp(float t, const Niflib::Quaternion& p, const Niflib::Quaternion& q);
	