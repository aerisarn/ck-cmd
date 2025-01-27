#include <core/NiflibHelper.h>

using namespace Niflib;

Niflib::Matrix33 MatrixFromEuler(float x, float y, float z)
{
	float sinX = sin(x);
	float cosX = cos(x);
	float sinY = sin(y);
	float cosY = cos(y);
	float sinZ = sin(z);
	float cosZ = cos(z);

	Matrix33 m{};

	m[0][0] = cosY * cosZ;
	m[0][1] = -cosY * sinZ;
	m[0][2] = sinY;
	m[1][0] = sinX * sinY * cosZ + sinZ * cosX;
	m[1][1] = cosX * cosZ - sinX * sinY * sinZ;
	m[1][2] = -sinX * cosY;
	m[2][0] = sinX * sinZ - cosX * sinY * cosZ;
	m[2][1] = cosX * sinY * sinZ + sinX * cosZ;
	m[2][2] = cosX * cosY;

	return m;
}

inline float correction(float t, float fCos)
{
	const float s = 0.8228677f;
	const float kc = 0.5855064f;
	float factor = 1.0f - s * fCos;
	float k = kc * factor * factor;
	return t * (k * t * (2.0f * t - 3.0f) + 1.0f + k);
}

inline float lerp(float v0, float v1, float perc)
{
	return v0 + perc * (v1 - v0);
}

inline float ISqrt_approx_in_neighborhood(float s)
{
	static const float NEIGHBORHOOD = 0.959066f;
	static const float SCALE = 1.000311f;
	static const float ADDITIVE_CONSTANT = SCALE / (float)sqrt(NEIGHBORHOOD);
	static const float FACTOR = SCALE * (-0.5f / (NEIGHBORHOOD * (float)sqrt(NEIGHBORHOOD)));
	return ADDITIVE_CONSTANT + (s - NEIGHBORHOOD) * FACTOR;
}

Quaternion fast_normalize(Quaternion val)
{
	float s = val.Dot(val);
	float k = ISqrt_approx_in_neighborhood(s);

	if (s <= 0.91521198f) {
		k *= ISqrt_approx_in_neighborhood(k * k * s);

		if (s <= 0.65211970f) {
			k *= ISqrt_approx_in_neighborhood(k * k * s);
		}
	}

	auto retVal = val * k;
	return retVal;
}

Niflib::Quaternion QuaternionSlerp(float t, const Niflib::Quaternion& p, const Niflib::Quaternion& q)
{
	// Copyright (c) 2002 Jonathan Blow
	//  "Hacking Quaternions", The Inner Product, March 2002
	//   http://number-none.com/product/Hacking%20Quaternions/index.html

	float tprime;

	if (t <= 0.5f) {
		tprime = correction(t, p.Dot(q));
	}
	else {
		tprime = 1.0f - correction(1.0f - t, p.Dot(q));
	}



	Quaternion result(lerp(p.x, q.x, tprime), lerp(p.y, q.y, tprime),
		lerp(p.z, q.z, tprime), lerp(p.w, q.w, tprime));
	fast_normalize(result);
	return result;
}