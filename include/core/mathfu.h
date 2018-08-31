/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant 
 * of patent rights can be found in the PATENTS file in the same directory.

 * modified by aerisarn to support EIGEN
 */
#ifndef CKCMD_MATHFU_H
#define CKCMD_MATHFU_H

#if defined ( _WIN32 )
 // Tell Windows not to define min() and max() macros
#define NOMINMAX
#include <Windows.h>
#endif



#if defined ( _WIN32 )
 // this is defined in fbxmath.h
#undef isnan
#endif

#include <fbxsdk.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

//#include <mathfu/vector.h>
//#include <mathfu/matrix.h>
//#include <mathfu/quaternion.h>
//#include <mathfu/rect.h>

/**
 * All the mathfu:: implementations of our core data types.
 */

template<class T, int d>
struct Bounds
{
	Eigen::Array<T, d, 1> min;
	Eigen::Array<T, d, 1> max;
    bool initialized = false;

    void Clear() {
        min = Eigen::Array<T, d, 1>();
        max = Eigen::Array<T, d, 1>();
        initialized = false;
    }

    void AddPoint(const Eigen::Array<T, d, 1> &p) {
        if (initialized) {
            for (int ii = 0; ii < d; ii ++) {
                min(ii) = std::min(min(ii), p(ii));
                max(ii) = std::max(max(ii), p(ii));
            }
        } else {
            min = p;
            max = p;
            initialized = true;
        }
    }
};

typedef Eigen::Vector4i				Vec4i;
typedef Eigen::Matrix4i				Mat4i;
typedef Eigen::Vector2f				Vec2f;
typedef Eigen::Vector3f				Vec3f;
typedef Eigen::Vector4f				Vec4f;
typedef Eigen::Matrix2f				Mat2f;
typedef Eigen::Matrix3f				Mat3f;
typedef Eigen::Matrix4f				Mat4f;
typedef Eigen::Quaternion<float>	Quatf;
typedef Bounds<float, 3>            Boundsf;

const Vec2f VEC2F_ZERO = Vec2f (0.0f, 0.0f);
const Vec3f VEC3F_ONE  = Vec3f (1.0f, 1.0f, 1.0f );
const Vec3f VEC3F_ZERO = Vec3f (0.0f, 0.0f, 0.0f);
const Vec4f VEC4F_ONE  = Vec4f (1.0f, 1.0f, 1.0f, 1.0f);
const Vec4f VEC4F_ZERO = Vec4f (0.0f, 0.0f, 0.0f, 0.0f);

template<class T, int d> static inline std::vector<T> toStdVec(const Eigen::Array <T, d, d> &vec)
{
    std::vector<T> result(d);
    for (int ii = 0; ii < d; ii ++) {
        result[ii] = vec[ii];
    }
    return result;
}

template<class T> std::vector<T> toStdVec(const Eigen::Quaternion<T> &quat) {
    return std::vector<T> { quat.vector()[0], quat.vector()[1], quat.vector()[2], quat.scalar() };
}

static inline Vec3f toVec3f(const FbxVector4 &v) {
    return Vec3f((float) v[0], (float) v[1], (float) v[2]);
}

static inline Vec4f toVec4f(const FbxVector4 &v) {
    return Vec4f((float) v[0], (float) v[1], (float) v[2], (float) v[3]);
}

static inline Mat4f toMat4f(const FbxAMatrix &m) {
    auto result = Mat4f();
    for (int row = 0; row < 4; row ++) {
        for (int col = 0; col < 4; col ++) {
            result(row, col) = (float) m[row][col];
        }
    }
    return result;
}

static inline Quatf toQuatf(const FbxQuaternion &q) {
    return Quatf((float) q[3], (float) q[0], (float) q[1], (float) q[2]);
}

//UTILS

#if defined( _MSC_VER )
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

namespace StringUtils {

	static const unsigned int MAX_PATH_LENGTH = 1024;

	inline int CompareNoCase(const std::string &s1, const std::string &s2)
	{
		return strncasecmp(s1.c_str(), s2.c_str(), MAX_PATH_LENGTH);
	}
}

#endif //CKCMD_MATHFU_H
