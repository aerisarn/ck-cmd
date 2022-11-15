#include "stdafx.h"
#include <core/hkxcmd.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <commands/ImportKF.h>

//#include "log.h"
//
//#include <map>
//#include <direct.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <math.h>
//#pragma region Niflib Headers
////////////////////////////////////////////////////////////////////////////
//// Niflib Includes
////////////////////////////////////////////////////////////////////////////
//#include <niflib.h>
//#include <nif_io.h>
//#include "obj/NiObject.h"
//#include "obj/NiNode.h"
//#include "obj/NiTexturingProperty.h"
//#include "obj/NiSourceTexture.h"
//#include "obj/NiTriBasedGeom.h"
//#include "obj/NiTriBasedGeomData.h"
//#include "obj/NiTriShape.h"
//#include "obj/NiTriStrips.h"
//#include <obj/NiControllerSequence.h>
//#include <obj/NiControllerManager.h>
//#include <obj/NiInterpolator.h>
//#include <obj/NiTransformInterpolator.h>
//#include <obj/NiTransformData.h>
//#include <obj/NiTransformController.h>
//#include <obj/NiTimeController.h>
//#include <obj/NiTransformController.h>
//#include <obj/NiTextKeyExtraData.h>
//#include <obj/NiKeyframeController.h>
//#include <obj/NiKeyframeData.h>
//#include <obj/NiStringPalette.h>
//#include <obj/NiBSplineTransformInterpolator.h>
//#include <obj/NiDefaultAVObjectPalette.h>
//#include <obj/NiMultiTargetTransformController.h>
//#include <obj/NiGeomMorpherController.h>
//#include <obj/NiMorphData.h>
//#include <obj/NiBSplineCompFloatInterpolator.h>
//#include <obj/NiFloatInterpolator.h>
//#include <obj/NiFloatData.h>
//#include <Key.h>
//
//typedef Niflib::Key<float> FloatKey;
//typedef Niflib::Key<Niflib::Quaternion> QuatKey;
//typedef Niflib::Key<Niflib::Vector3> Vector3Key;
//typedef Niflib::Key<string> StringKey;
//
//#pragma endregion
//
//#pragma region Havok Headers
////////////////////////////////////////////////////////////////////////////
//// Havok Includes
////////////////////////////////////////////////////////////////////////////
//
//#include <Common/Base/hkBase.h>
//#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
//#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
//#include <Common/Base/System/Io/IStream/hkIStream.h>
//
//#include <cstdio>
//
//// Scene
//#include <Common/SceneData/Scene/hkxScene.h>
//#include <Common/Serialize/Util/hkRootLevelContainer.h>
//#include <Common/Serialize/Util/hkLoader.h>
//
//// Physics
//#include <Physics/Dynamics/Entity/hkpRigidBody.h>
//#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
//#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
//
//// Animation
//#include <Animation/Animation/Rig/hkaSkeleton.h>
//#include <Animation/Animation/Rig/hkaPose.h>
//#include <Animation/Animation/Rig/hkaSkeletonUtils.h>
//#include <Animation/Animation/hkaAnimationContainer.h>
//#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
//#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
//#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
//#include <Animation/Animation/Animation/Deprecated/DeltaCompressed/hkaDeltaCompressedAnimation.h>
//#include <Animation/Animation/Animation/SplineCompressed/hkaSplineCompressedAnimation.h>
//#include <Animation/Animation/Animation/Quantized/hkaQuantizedAnimation.h>
//#include <Animation/Animation/Animation/Util/hkaAdditiveAnimationUtility.h>
//#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
//
//#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
//#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
//#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
//
//#include <Common/Serialize/Util/hkLoader.h>
//#include <Common/Serialize/Util/hkRootLevelContainer.h>
//
//// Serialize
//#include <Common/Serialize/Util/hkSerializeUtil.h>
//
//#pragma endregion
//
//#pragma endregion
//
////////////////////////////////////////////////////////////////////////////
//// Our Includes
////////////////////////////////////////////////////////////////////////////
//#include <float.h>
//#include <cstdio>
//#include <sys/stat.h>

#include "AnimationExport.h"

#include <obj/NiBSplineBasisData.h>
#include <obj/NiBSplineData.h>
#include <obj/NiBSplineCompTransformInterpolator.h>

#include <interfaces\misc.h>

#include <core/HKXWrangler.h>
#include <core/EulerAngles.h>

using namespace Niflib;
using namespace std;
using namespace ConvertKF;

ImportKF::ImportKF()
{
}

ImportKF::~ImportKF()
{
}

string ImportKF::GetName() const
{
	return "importkf";
}

string ImportKF::GetHelp() const
{
	string name = GetName();
	transform(name.begin(), name.end(), name.begin(), ::tolower);

	// Usage: ck-cmd exportfbx
	string usage = "Usage: " + ExeCommandList::GetExeName() + " " + name + " <path_to_skeleton> <path_to_animations> [-e <path_to_export>]\r\n";

	const char help[] =
		R"(Converts KF format to HKX.
		
		Arguments:
			<path_to_skeleton> the skeleton for the animations to convert
			<path_to_animations> the animation directory containing fk files to convert
			<path_to_export> path to the output directory

		)";
	return usage + help;
}

string ImportKF::GetHelpShort() const
{
	return "TODO: Short help message for ImportFBX";
}


//////////////////////////////////////////////////////////////////////////
// Enumeration Types
//////////////////////////////////////////////////////////////////////////
namespace {
enum {
	IPOS_X_REF	=	0,
	IPOS_Y_REF	=	1,
	IPOS_Z_REF	=	2,
	IPOS_W_REF	=	3,
};
enum AccumType
{
	AT_NONE = 0,
	AT_X = 0x01,
	AT_Y = 0x02,
	AT_Z = 0x04,

	AT_XYZ = AT_X | AT_Y | AT_Z,
	AT_FORCE = 0x80000000,
};

enum PosRotScale
{
	prsPos = 0x1,
	prsRot = 0x2,
	prsScale = 0x4,
	prsDefault = prsPos | prsRot | prsScale,
};
}
//////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////

const unsigned int IntegerInf = 0x7f7fffff;
const unsigned int IntegerNegInf = 0xff7fffff;
const float FloatINF = *(float*)&IntegerInf;
const float FloatNegINF = *(float*)&IntegerNegInf;

//////////////////////////////////////////////////////////////////////////
// Structures
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////

static void HelpString(hkxcmd::HelpType type){
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("ConvertKF - Convert Gamebryo KF animation to Havok HKX animation."); break;
	case hkxcmd::htLong:  
		{
			char fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_splitpath(fullName, NULL, NULL, exeName, NULL);

			Log::Info("Usage: %s ConvertKF [-opts[modifiers]] [skel.hkx] [anim.kf] [anim.hkx]", exeName); 
			Log::Info("  Convert Gamebryo KF animation to Havok HKX animation" );
			Log::Info("  If a folder is specified then the folder will be searched for any projects and convert those." );
			Log::Info("");
			Log::Info("<Options>" );
			Log::Info("  skel.hkx      Path to Havok skeleton for animation binding." );
			Log::Info("  anim.kf       Path to Gamebryo animation to convert (Default: anim.hkx with kf ext)" );
			Log::Info("  anim.hkx      Path to Havok animation to write" );
			Log::Info("<Switches>" );
			Log::Info(" -d[:level]     Debug Level: ERROR,WARN,INFO,DEBUG,VERBOSE (Default: INFO)" );
			Log::Info("");
         Log::Info(" -v:<flags>     Havok Save Options");
         Log::Info("    DEFAULT     Save as Default Format (MSVC Win32 Packed)");
         Log::Info("    XML         Save as Packed Binary Xml Format");
         Log::Info("    WIN32       Save as Win32 Format");
         Log::Info("    AMD64       Save as AMD64 Format");
         Log::Info("    XBOX        Save as XBOX Format");
         Log::Info("    XBOX360     Save as XBOX360 Format");
         Log::Info("    TAGFILE     Save as TagFile Format");
         Log::Info("    TAGXML      Save as TagFile XML Format");
         Log::Info("");
         Log::Info(" -f <flags>         Havok saving flags (Defaults:  SAVE_TEXT_FORMAT|SAVE_TEXT_NUMBERS)");
         Log::Info("     SAVE_DEFAULT           = All flags default to OFF, enable whichever are needed");
         Log::Info("     SAVE_TEXT_FORMAT       = Use text (usually XML) format, default is binary format if available.");
         Log::Info("     SAVE_SERIALIZE_IGNORED_MEMBERS = Write members which are usually ignored.");
         Log::Info("     SAVE_WRITE_ATTRIBUTES  = Include extended attributes in metadata, default is to write minimum metadata.");
         Log::Info("     SAVE_CONCISE           = Doesn't provide any extra information which would make the file easier to interpret. ");
         Log::Info("                              E.g. additionally write hex floats as text comments.");
         Log::Info("     SAVE_TEXT_NUMBERS      = Floating point numbers output as text, not as binary.  ");
         Log::Info("                              Makes them easily readable/editable, but values may not be exact.");
         Log::Info("");
			Log::Info(" -n             Disable recursive file processing" );
			Log::Info("");
		}
		break;
	}
}

/*********************************************************************
Simple b-spline curve algorithm
Copyright 1994 by Keith Vertanen (vertankd@cda.mrs.umn.edu)
Released to the public domain (your mileage may vary)
Found at: Programmers Heaven (www.programmersheaven.com/zone3/cat415/6660.htm)
Modified by: Theo
- reformat and convert doubles to floats
- removed point structure in favor of arbitrary sized float array
**********************************************************************/
static void copy_floats(float* dest, float* src, int l)
{
	for (int i = 0; i < l; ++i)
		dest[i] = src[i];
}

// calculate the blending value
static float blend(int k, int t, int* u, float v)
{
	float value;
	if (t == 1) {			// base case for the recursion
		value = ((u[k] <= v) && (v < u[k + 1])) ? 1.0f : 0.0f;
	}
	else {
		if ((u[k + t - 1] == u[k]) && (u[k + t] == u[k + 1]))  // check for divide by zero
			value = 0;
		else if (u[k + t - 1] == u[k]) // if a term's denominator is zero,use just the other
			value = (u[k + t] - v) / (u[k + t] - u[k + 1]) * blend(k + 1, t - 1, u, v);
		else if (u[k + t] == u[k + 1])
			value = (v - u[k]) / (u[k + t - 1] - u[k]) * blend(k, t - 1, u, v);
		else
			value = (v - u[k]) / (u[k + t - 1] - u[k]) * blend(k, t - 1, u, v) +
			(u[k + t] - v) / (u[k + t] - u[k + 1]) * blend(k + 1, t - 1, u, v);
	}
	return value;
}

// figure out the knots
static void compute_intervals(int* u, int n, int t)
{
	for (int j = 0; j <= n + t; j++) {
		if (j < t)
			u[j] = 0;
		else if ((t <= j) && (j <= n))
			u[j] = j - t + 1;
		else if (j > n)
			u[j] = n - t + 2;  // if n-t=-2 then we're screwed, everything goes to 0
	}
}

static void compute_point(int* u, int n, int t, float v, int l, float* control, float* output)
{
	// initialize the variables that will hold our output
	for (int j = 0; j < l; j++)
		output[j] = 0;
	for (int k = 0; k <= n; k++) {
		float temp = blend(k, t, u, v);  // same blend is used for each dimension coordinate
		for (int j = 0; j < l; j++)
			output[j] = output[j] + control[k * l + j] * temp;
	}
}

/*********************************************************************
bspline(int n, int t, int l, float *control, float *output, int num_output)
Parameters:
n          - the number of control points minus 1
t          - the degree of the polynomial plus 1
l          - size of control and output float vector block
control    - control point array made up of float structure
output     - array in which the calculate spline points are to be put
num_output - how many points on the spline are to be calculated
Pre-conditions:
n+2>t  (no curve results if n+2<=t)
control array contains the number of points specified by n
output array is the proper size to hold num_output point structures
control and output vectors must be contiguous float arrays
**********************************************************************/
void bspline(int n, int t, int l, float* control, float* output, int num_output)
{
	float* calc = new float[l];
	int* u = new int[n + t + 1];
	compute_intervals(u, n, t);

	float increment = (float)(n - t + 2) / (num_output - 1);  // how much parameter goes up each time
	float interval = 0;
	for (int output_index = 0; output_index < num_output - 1; output_index++) {
		compute_point(u, n, t, interval, l, control, calc);
		copy_floats(&output[output_index * l], calc, l);
		interval = interval + increment;  // increment our parameter
	}
	copy_floats(&output[(num_output - 1) * l], &control[n * l], l); // put in the last points
	delete[] u;
	delete[] calc;
}

struct NiBSplineTransformInterpolatorAccessor {};

template<>
struct Accessor<NiBSplineTransformInterpolatorAccessor> {

	NiBSplineTransformInterpolatorRef& _interpolator;

	Accessor(NiBSplineTransformInterpolatorRef& interpolator) : _interpolator(interpolator) {}
};

struct NiBSplineDataAccessor {};

template<>
struct Accessor<NiBSplineDataAccessor> {

	NiBSplineData& _data;

	Accessor(NiBSplineData& data) : _data(data) {}

	vector<float> GetFloatControlPoints() const
	{
		return _data.floatControlPoints;
	}

	void SetFloatControlPoints(vector<float> value)
	{
		_data.floatControlPoints.clear();
		_data.numFloatControlPoints = value.size();

		for (unsigned int i = 0; i < value.size(); i++) {
			_data.floatControlPoints.push_back(value[i]);
		}
	}

	void AppendFloatControlPoints(vector<float> value)
	{
		_data.numFloatControlPoints += value.size();

		for (unsigned int i = 0; i < value.size(); i++) {
			_data.floatControlPoints.push_back(value[i]);
		}
	}

	int GetNumFloatControlPoints()
	{
		return _data.numFloatControlPoints;
	}

	vector<float> GetFloatControlPointRange(int offset, int count) const
	{
		vector<float> value;
		if (offset < 0 || count < 0 || ((offset + count) > int(_data.floatControlPoints.size())))
			throw runtime_error("Invalid offset or count.");
		vector<float>::const_iterator srcbeg = _data.floatControlPoints.begin(), srcend = _data.floatControlPoints.begin();
		std::advance(srcbeg, offset);
		std::advance(srcend, offset + count);
		return vector<float>(srcbeg, srcend);
	}

	vector<short > GetShortControlPoints() const
	{
		return _data.compactControlPoints;
	}

	void SetShortControlPoints(vector<short> value)
	{
		_data.compactControlPoints.clear();
		_data.numCompactControlPoints = value.size();

		for (unsigned int i = 0; i < value.size(); i++) {
			_data.compactControlPoints.push_back(value[i]);
		}
	}

	void AppendShortControlPoints(vector<short> value)
	{
		_data.numCompactControlPoints += value.size();

		for (unsigned int i = 0; i < value.size(); i++) {
			_data.compactControlPoints.push_back(value[i]);
		}
	}


	int GetNumShortControlPoints()
	{
		return _data.numCompactControlPoints;
	}


	vector<short > GetShortControlPointRange(int offset, int count) const
	{
		vector<short> value;
		if (offset < 0 || count < 0 || ((offset + count) > int(_data.compactControlPoints.size())))
			throw runtime_error("Invalid offset or count.");
		vector<short>::const_iterator srcbeg = _data.compactControlPoints.begin(), srcend = _data.compactControlPoints.begin();
		std::advance(srcbeg, offset);
		std::advance(srcend, offset + count);
		return vector<short>(srcbeg, srcend);
	}

};

static const int SizeofQuat = 4;
static const int SizeofTrans = 3;
static const int SizeofScale = 1;

template <typename interp>
int GetNumControlPoints(interp& interpolator)
{
	if (interpolator->GetBasisData())
		return interpolator->GetBasisData()->GetNumControlPoints();
	return 0;
}

vector< Key<Quaternion> > SampleQuatRotateKeys(NiBSplineCompTransformInterpolatorRef interpolator, int npoints, int degree)
{
	vector< Key<Quaternion> > value;
	if ((interpolator->GetRotationHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) { // has rotation data
		int nctrl = interpolator->GetBasisData()->GetNumControlPoints();
		int npts = nctrl * SizeofQuat;
		Accessor<NiBSplineDataAccessor> spline_data(*interpolator->GetSplineData());
		vector<short> points = spline_data.GetShortControlPointRange(interpolator->GetRotationHandle(), npts);
		vector<float> control(npts);
		vector<float> output(npoints * SizeofQuat);
		for (int i = 0, j = 0; i < nctrl; ++i) {
			for (int k = 0; k < SizeofQuat; ++k)
				control[i * SizeofQuat + k] = float(points[j++]) / float(32767);
		}
		if (degree >= nctrl)
			degree = nctrl - 1;
		// fit data
		bspline(nctrl - 1, degree + 1, SizeofQuat, &control[0], &output[0], npoints);

		// copy to key
		float time = interpolator->GetStartTime();
		float incr = (interpolator->GetStopTime() - interpolator->GetStartTime()) / float(npoints);
		value.reserve(npoints);
		for (int i = 0, j = 0; i < npoints; i++) {
			Key<Quaternion> key;
			key.time = time;
			key.backward_tangent.Set(1.0f, 0.0f, 0.0f, 0.0f);
			key.forward_tangent.Set(1.0f, 0.0f, 0.0f, 0.0f);
			key.data.w = output[j++] * interpolator->GetRotationHalfRange() + interpolator->GetRotationOffset();
			key.data.x = output[j++] * interpolator->GetRotationHalfRange() + interpolator->GetRotationOffset();
			key.data.y = output[j++] * interpolator->GetRotationHalfRange() + interpolator->GetRotationOffset();
			key.data.z = output[j++] * interpolator->GetRotationHalfRange() + interpolator->GetRotationOffset();
			value.push_back(key);
			time += incr;
		}
	}
	return value;
}

vector< Key<Vector3> > SampleTranslateKeys(NiBSplineCompTransformInterpolatorRef interpolator, int npoints, int degree)
{
	vector< Key<Vector3> > value;
	if ((interpolator->GetTranslationHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) { // has translation data
		int nctrl = interpolator->GetBasisData()->GetNumControlPoints();
		int npts = nctrl * SizeofTrans;
		Accessor<NiBSplineDataAccessor> spline_data(*interpolator->GetSplineData());
		vector<short> points = spline_data.GetShortControlPointRange(interpolator->GetTranslationHandle(), npts);
		vector<float> control(npts);
		vector<float> output(npoints * SizeofTrans);
		for (int i = 0, j = 0; i < nctrl; ++i) {
			for (int k = 0; k < SizeofTrans; ++k)
				control[i * SizeofTrans + k] = float(points[j++]) / float(32767);
		}
		// fit data
		bspline(nctrl - 1, degree + 1, SizeofTrans, &control[0], &output[0], npoints);

		// copy to key
		float time = interpolator->GetStartTime();
		float incr = (interpolator->GetStopTime() - interpolator->GetStartTime()) / float(npoints);
		value.reserve(npoints);
		for (int i = 0, j = 0; i < npoints; i++) {
			Key<Vector3> key;
			key.time = time;
			key.backward_tangent.Set(0.0f, 0.0f, 0.0f);
			key.forward_tangent.Set(0.0f, 0.0f, 0.0f);
			key.data.x = output[j++] * interpolator->GetTranslationHalfRange() + interpolator->GetTranslationOffset();
			key.data.y = output[j++] * interpolator->GetTranslationHalfRange() + interpolator->GetTranslationOffset();
			key.data.z = output[j++] * interpolator->GetTranslationHalfRange() + interpolator->GetTranslationOffset();
			value.push_back(key);
			time += incr;
		}
	}
	return value;
}

vector< Key<float> > SampleScaleKeys(NiBSplineCompTransformInterpolatorRef interpolator, int npoints, int degree)
{
	vector< Key<float> > value;
	if ((interpolator->GetScaleHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) // has scale data
	{
		int nctrl = interpolator->GetBasisData()->GetNumControlPoints();
		int npts = nctrl * SizeofScale;
		Accessor<NiBSplineDataAccessor> spline_data(*interpolator->GetSplineData());
		vector<short> points = spline_data.GetShortControlPointRange(interpolator->GetScaleHandle(), npts);
		vector<float> control(npts);
		vector<float> output(npoints * SizeofScale);
		for (int i = 0, j = 0; i < nctrl; ++i) {
			control[i] = float(points[j++]) / float(32767);
		}
		// fit data
		bspline(nctrl - 1, degree + 1, SizeofScale, &control[0], &output[0], npoints);

		// copy to key
		float time = interpolator->GetStartTime();
		float incr = (interpolator->GetStopTime() - interpolator->GetStartTime()) / float(npoints);
		value.reserve(npoints);
		for (int i = 0, j = 0; i < npoints; i++) {
			Key<float> key;
			key.time = time;
			key.backward_tangent = 0.0f;
			key.forward_tangent = 0.0f;
			key.data = output[j++] * interpolator->GetScaleHalfRange() + interpolator->GetScaleOffset();
			value.push_back(key);
			time += incr;
		}
	}
	return value;
}


vector< Key<Quaternion> > SampleQuatRotateKeys(NiBSplineTransformInterpolatorRef& interpolator, int npoints, int degree)
{
	Accessor<NiBSplineTransformInterpolatorAccessor> accessor(interpolator);
	vector< Key<Quaternion> > value;
	if ((interpolator->GetRotationHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) { // has rotation data
		int nctrl = interpolator->GetBasisData()->GetNumControlPoints();
		int npts = nctrl * SizeofQuat;
		Accessor<NiBSplineDataAccessor> spline_data(*interpolator->GetSplineData());
		vector<float> points = spline_data.GetFloatControlPointRange(interpolator->GetRotationHandle(), npts);


		vector<float> control(npts);
		vector<float> output(npoints * SizeofQuat);
		for (int i = 0, j = 0; i < nctrl; ++i) {
			for (int k = 0; k < SizeofQuat; ++k)
				control[i * SizeofQuat + k] = float(points[j++]);
		}
		if (degree >= nctrl)
			degree = nctrl - 1;
		// fit data
		bspline(nctrl - 1, degree + 1, SizeofQuat, &control[0], &output[0], npoints);

		// copy to key
		float time = interpolator->GetStartTime();
		float incr = (interpolator->GetStopTime() - interpolator->GetStartTime()) / float(npoints);
		value.reserve(npoints);
		for (int i = 0, j = 0; i < npoints; i++) {
			Key<Quaternion> key;
			key.time = time;
			key.backward_tangent.Set(1.0f, 0.0f, 0.0f, 0.0f);
			key.forward_tangent.Set(1.0f, 0.0f, 0.0f, 0.0f);
			key.data.w = output[j++];
			key.data.x = output[j++];
			key.data.y = output[j++];
			key.data.z = output[j++];
			value.push_back(key);
			time += incr;
		}
	}
	return value;
}

vector< Key<Vector3> > SampleTranslateKeys(NiBSplineTransformInterpolatorRef& interpolator, int npoints, int degree)
{
	vector< Key<Vector3> > value;
	if ((interpolator->GetTranslationHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) { // has translation data
		int nctrl = interpolator->GetBasisData()->GetNumControlPoints();
		int npts = nctrl * SizeofTrans;
		Accessor<NiBSplineDataAccessor> spline_data(*interpolator->GetSplineData());
		vector<float> points = spline_data.GetFloatControlPointRange(interpolator->GetTranslationHandle(), npts);
		vector<float> control(npts);
		vector<float> output(npoints * SizeofTrans);
		for (int i = 0, j = 0; i < nctrl; ++i) {
			for (int k = 0; k < SizeofTrans; ++k)
				control[i * SizeofTrans + k] = float(points[j++]);
		}
		// fit data
		bspline(nctrl - 1, degree + 1, SizeofTrans, &control[0], &output[0], npoints);

		// copy to key
		float time = interpolator->GetStartTime();
		float incr = (interpolator->GetStopTime() - interpolator->GetStartTime()) / float(npoints);
		value.reserve(npoints);
		for (int i = 0, j = 0; i < npoints; i++) {
			Key<Vector3> key;
			key.time = time;
			key.backward_tangent.Set(0.0f, 0.0f, 0.0f);
			key.forward_tangent.Set(0.0f, 0.0f, 0.0f);
			key.data.x = output[j++];
			key.data.y = output[j++];
			key.data.z = output[j++];
			value.push_back(key);
			time += incr;
		}
	}
	return value;
}

vector< Key<float> > SampleScaleKeys(NiBSplineTransformInterpolatorRef& interpolator, int npoints, int degree)
{
	vector< Key<float> > value;
	if ((interpolator->GetScaleHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) // has scale data
	{
		int nctrl = interpolator->GetBasisData()->GetNumControlPoints();
		int npts = nctrl * SizeofScale;
		Accessor<NiBSplineDataAccessor> spline_data(*interpolator->GetSplineData());
		vector<float> points = spline_data.GetFloatControlPointRange(interpolator->GetScaleHandle(), npts);

		vector<float> control(npts);
		vector<float> output(npoints * SizeofScale);
		for (int i = 0, j = 0; i < nctrl; ++i) {
			control[i] = float(points[j++]) / float(32767);
		}
		// fit data
		bspline(nctrl - 1, degree + 1, SizeofScale, &control[0], &output[0], npoints);

		// copy to key
		float time = interpolator->GetStartTime();
		float incr = (interpolator->GetStopTime() - interpolator->GetStartTime()) / float(npoints);
		value.reserve(npoints);
		for (int i = 0, j = 0; i < npoints; i++) {
			Key<float> key;
			key.time = time;
			key.backward_tangent = 0.0f;
			key.forward_tangent = 0.0f;
			key.data = output[j++];
			value.push_back(key);
			time += incr;
		}
	}
	return value;
}

//////////////////////////////////////////////////////////////////////////
// Classes
//////////////////////////////////////////////////////////////////////////

namespace {

//vector< QuatKey > SampleQuatRotateKeys(NiInterpolatorRef interp, int npoints, float startTime, float endTime, int degree=3)
//{
//	if ( interp->IsDerivedType(NiBSplineTransformInterpolator::TYPE) )
//	{
//		NiBSplineTransformInterpolatorRef binterp = DynamicCast<NiBSplineTransformInterpolator>(interp);
//		return SampleQuatRotateKeys(binterp, npoints, degree);
//	}
//	else
//	{
//		vector< QuatKey > qk;
//		return qk;
//	}
//}

/*!
* Retrieves the sampled scale key data between start and stop time.
* \param npoints The number of data points to sample between start and stop time.
* \param degree N-th order degree of polynomial used to fit the data.
* \return A vector containing Key<Vector3> data which specify translation over time.
*/
//vector< Vector3Key > SampleTranslateKeys(NiInterpolatorRef interp, int npoints, float startTime, float endTime, int degree=3)
//{
//	vector< Vector3Key > vk;
//	return vk;
//}

/*!
* Retrieves the sampled scale key data between start and stop time.
* \param npoints The number of data points to sample between start and stop time.
* \param degree N-th order degree of polynomial used to fit the data.
* \return A vector containing Key<float> data which specify scale over time.
*/
//vector< FloatKey > SampleScaleKeys(NiInterpolatorRef interp, int npoints, float startTime, float endTime, int degree=3)
//{
//	vector< FloatKey > sk;
//	return sk;
//}

//struct AnimationExport
//{
//	AnimationExport(NiControllerSequenceRef seq, hkRefPtr<hkaSkeleton> skeleton, hkRefPtr<hkaAnimationBinding> binding);
//
//	bool doExport();
//	bool exportNotes( );
//	bool exportController();
//
//	//bool SampleAnimation( INode * node, Interval &range, PosRotScale prs, NiKeyframeDataRef data );
//	NiControllerSequenceRef seq;
//	hkRefPtr<hkaAnimationBinding> binding;
//	hkRefPtr<hkaSkeleton> skeleton;
//   static bool noRootSiblings;
//};
//bool AnimationExport::noRootSiblings = true;
//}

}
bool AnimationExport::noRootSiblings = true;

AnimationExport::AnimationExport(NiControllerSequenceRef seq, hkRefPtr<hkaSkeleton> skeleton, hkRefPtr<hkaAnimationBinding> binding, const NifInfo& info) :
_info(info)
{
	this->seq = seq;
	this->binding = binding;
	this->skeleton = skeleton;
}

bool AnimationExport::doExport()
{
	binding->m_originalSkeletonName = seq->GetAccumRootName().c_str();

	if (!exportNotes())
		return false;

	return exportController();
}

//TODO
//check StartAnimationDriven in bear annotations

/*
List of Text Keys

a: [L|R]
	Used to indicate when to switch between left and right attacks. The handtohandattackleft animation, 
	for example, has a text key informing the engine when it should switch to a right-handed attack (a:R).
Attach
	Used in Equip animations. Could indicate at what point to equip the weapon.
Attack
	Used by BlockAttack animations to indicate when a counter-attack should be registered.
Blend: <int>
	"Define a range of animation priorities for all bones in the animation. The existing priorities are 
	modified to fit within +- range, where int is the blending integer priority defined. This is applied to 
	the entire animation." Sinkhole. It is not clear for how long these animation priorities exist or whether 
	or not it is specific to the animation it is set for or if it changes a general setting in the game engine. 
	It is assumed that the blending begins with the Blend keyframe and ends when the animation ends and that 
	the blending only affects bone priorities for the animation it is set for. Some animations set Blend: 
	0 text keys, however, which appear to forcibly set blending to 0 for that animation. 
	I'm not sure why this would be required if the blending was restricted to the animation the key was set for, 
	though it might make sense if the Blending was a general setting for the engine and if certain animations 
	should not be blended. Examples of this can be found in the blockhit and handtohandblockhit animations.
Detach
	Used in Unequip animations. Could indicate at what point to unequip the weapon.
end
	Indicates the last frame of the animation. Required.
Enum: <string>
	Indicate when sound effects (like footsteps) should be played. Enum sound effects appear to be used by the engine 
	for specific events, such as footsteps and the sound made by a creature when it detects an enemy. 
	Other sound effects that the animator may require can be played using the Sound text key. 
	Note that the foot step sound effects do not have to be actual foot steps; 
	if your creature floats, flies, or swims it can be any sound that would typically be made by your creature 
	when moving. Standard sound effects include: 
	Left, Right, BackLeft, BackRight (for feet), Aware, Attack, Hit (for injury), and Death.
Hit
	Indicates the point in an animation at which an attack or spell hits its target. 
	If you fail to include this key your attack and spell animations will never actually hit.
Hold
	Used in bow animations. Presumably to indicate when to hold (pause) the animation sequence while the player 
	has the bow drawn but has not yet released the arrow.
m: [L|R]
	Used to indicate when the left or right foot is in contact with the ground and when to switch to the other foot. 
	This key is used in movement animations but doesn't appear to be used in other animations.
Release
	Used in bow animations. Presumably to indicate when to continue the animation sequence after the player has 
	released the arrow.
Sound: <string>
	Used to indicate when other sounds not expected by the engine should be played. Some sound effects are expected 
	in movement animations, equipping animations, etc., and are usually handled by the Enum text key. 
	All other sounds appear to be handled by the Sound key.
start
	Indicates the first frame of the animation. Required.
*/

bool isOblivionEngineAnnotation(const std::string& in )
{
	if (
		in == "a:L" ||
		in == "a:R" ||
		in == "Attach" ||
		in == "Attack" ||
		in.find("Blend:") == 0 ||
		in == "Detach" ||
		in.find("Enum:") == 0 ||
		in == "Hit" ||
		in == "Hold" ||
		in == "m:L" ||
		in == "m:R" ||
		in == "Release" ||
		in.find("Sound:") == 0
		)
	{
		return true;
	}
	return false;
}

std::set<std::pair<float, std::string>> convertOblivionAnnotations(const std::pair<float,std::string>& note, bool bow = false)
{
	std::set<std::pair<float, std::string>> out;

	//Attach -> weaponDraw (bow has also BeginWeaponDraw at 0.0)
	//Detach -> weaponSheathe
	
	//Sound: -> SoundPlay.
	//Enum:Left -> FootLeft 
	//Enum:Right -> FootRight
	//Enum:BackLeft -> FootFront
	//Enum:BackRight -> FootBack

	//Enum:Aware -> ?
	//Enum:Attack -> weaponSwing
	//Enum:Hit -> ?
	//Enum:Death -> ?

	//Hit -> preHitFrame, HitFrame
	//m:R -> SyncRight
	//m:L -> SyncLeft

	if (note.second == "Attach")
	{
		if (bow)
			out.insert({ 0., "BeginWeaponDraw" });
		out.insert({note.first, "weaponDraw"});
	}
	else if (note.second == "Detach") {
		out.insert({ note.first, "weaponSheathe" });
	}
	else if (note.second.find("Sound:") == 0) {
		std::string old = "Sound: ";
		std::string new_sound = note.second;
		new_sound.replace(new_sound.find(old), old.length(), "SoundPlay.");
		out.insert({ note.first, new_sound });
	}
	else if (note.second.find("Enum:Left") == 0) {
		out.insert({ note.first, "FootLeft" });
	}
	else if (note.second.find("Enum:Right") == 0) {
		out.insert({ note.first, "FootRight" });
	}
	else if (note.second.find("Enum:BackLeft") == 0) {
		out.insert({ note.first, "FootFront" });
	}
	else if (note.second.find("Enum:BackRight") == 0) {
		out.insert({ note.first, "FootBack" });
	}
	else if (note.second.find("Enum:Attack") == 0) {
		out.insert({ note.first, "weaponSwing" });
	}
	else if (note.second.find("Hit") == 0) {
		out.insert({ note.first - 0.15, "preHitFrame" });
		out.insert({ note.first, "HitFrame" });
	}
	else if (note.second.find("m:R") == 0) {
		out.insert({ note.first, "FootRight" });
		out.insert({ note.first, "SyncRight" });
	}
	else if (note.second.find("m:L") == 0) {
		out.insert({ note.first, "FootLeft" });
		out.insert({ note.first, "SyncLeft" });
	}

	return out;
}



bool AnimationExport::exportNotes()
{
#if 0
	vector<StringKey> textKeys;

	NiTextKeyExtraDataRef textKeyData = new NiTextKeyExtraData();
	seq->SetTextKey(textKeyData);

	//seq->SetName( note.m_text );

	AccumType accumType = AT_NONE;

	hkRefPtr<hkaAnimation> anim = binding->m_animation;
	const hkInt32 numAnnotationTracks = anim->m_annotationTracks.getSize();

	// Scan the animation for get up points
	for (hkInt32 t=0; t< numAnnotationTracks; ++t )
	{
		hkaAnnotationTrack& track = anim->m_annotationTracks[t];
		const hkInt32 numAnnotations = track.m_annotations.getSize();
		for( hkInt32 a = 0; a < numAnnotations; ++a )
		{
			hkaAnnotationTrack::Annotation& note = track.m_annotations[a];

			if ( seq->GetStartTime() == FloatINF || seq->GetStartTime() > note.m_time )
				seq->SetStartTime(note.m_time);
			if ( seq->GetStopTime() == FloatINF || seq->GetStopTime() < note.m_time )
				seq->SetStopTime(note.m_time);

			StringKey strkey;
			strkey.time = note.m_time;
			strkey.data = note.m_text;
			textKeys.push_back(strkey);
		}
	}

	textKeyData->SetKeys(textKeys);
#endif
	return true;
}

namespace {
	static inline Niflib::Vector3 TOVECTOR3(const hkVector4& v){
		return Niflib::Vector3(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2));
	}

	static inline Niflib::Vector4 TOVECTOR4(const hkVector4& v){
		return Niflib::Vector4(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
	}

	static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v){
		return hkVector4(v.x, v.y, v.z, v.w);
	}

	static inline Niflib::Quaternion TOQUAT(const ::hkQuaternion& q, bool inverse = false){
		Niflib::Quaternion qt(q.m_vec.getSimdAt(3), q.m_vec.getSimdAt(0), q.m_vec.getSimdAt(1), q.m_vec.getSimdAt(2));
		return inverse ? qt.Inverse() : qt;
	}

	static inline ::hkQuaternion TOQUAT(const Niflib::Quaternion& q, bool inverse = false){
		hkVector4 v(q.x, q.y, q.z, q.w);
		v.normalize4();
		::hkQuaternion qt(v.getSimdAt(0), v.getSimdAt(1), v.getSimdAt(2), v.getSimdAt(3));
		if (inverse) qt.setInverse(qt);
		return qt;
	}

	static inline Niflib::Quaternion TOQUAT(const hkRotation& rot, bool inverse = false){
		return TOQUAT(::hkQuaternion(rot), inverse);
	}
	static inline float Average(const Niflib::Vector3& val) {
		return (val.x + val.y + val.z) / 3.0f;
	}

	float QuatDot(const Quaternion& q, const Quaternion&p)
	{
		return q.w*p.w + q.x*p.x + q.y*p.y + q.z*p.z;
	}

	const float MY_FLT_EPSILON = 1e-5f;
	static inline bool EQUALS(float a, float b){
		return fabs(a - b) < MY_FLT_EPSILON;
	}
	static inline int COMPARE(float a, float b){
		float d = a - b;
		return (fabs(d) < MY_FLT_EPSILON ? 0 : (d > 0 ? 1 : -1));
	}

	static inline bool EQUALS(const Niflib::Vector3& a, const Niflib::Vector3& b){
		return (EQUALS(a.x,b.x) && EQUALS(a.y, b.y) && EQUALS(a.z, b.z) );
	}

	static inline bool EQUALS(const Niflib::Quaternion& a, const Niflib::Quaternion& b){
		return EQUALS(a.w, b.w) && EQUALS(a.x,b.x) && EQUALS(a.y, b.y) && EQUALS(a.z, b.z);
	}

	const float FramesPerSecond = 30.0f;
	//const float FramesIncrement = 0.0325f;
	const float FramesIncrement = 0.033333f;
}

struct BoneDataReference
{
	string name;
	NiTransformControllerRef transCont;
	NiTransformInterpolatorRef interpolator;
	NiTransformDataRef transData;
	vector<Vector3Key> trans;
	vector<QuatKey> rot;
	vector<FloatKey> scale;

	Vector3 lastTrans;
	Quaternion lastRotate;
	float lastScale;
};

static void FillTransforms( hkArray<hkQsTransform>& transforms, int boneIdx, int numTracks
					, const hkQsTransform& localTransform, PosRotScale prs = prsDefault
					, int from=0, int to=-1) 
{
	int n = transforms.getSize() / numTracks;
	if (n == 0)
		return;

	if (to == -1 || to >= n) to = n-1;

	if ((prs & prsDefault) == prsDefault)
	{
		for (int idx = from; idx <= to; ++idx)
		{
			hkQsTransform& transform = transforms[idx*numTracks + boneIdx];
			transform = localTransform;
		}
	}
	else
	{
		for (int idx = from; idx <= to; ++idx)
		{
			hkQsTransform& transform = transforms[idx*numTracks + boneIdx];
			if ((prs & prsPos) != 0)
				transform.setTranslation(localTransform.getTranslation());
			if ((prs & prsRot) != 0)
				transform.setRotation(localTransform.getRotation());
			if ((prs & prsScale) != 0)
				transform.setScale(localTransform.getScale());
		}
	}
}
static void SetTransformPosition(hkQsTransform& transform, hkVector4& p)
{
	if (p.getSimdAt(0) != FloatNegINF) transform.setTranslation(p);
}
static void SetTransformRotation(hkQsTransform& transform, ::hkQuaternion& q)
{
	if (q.m_vec.getSimdAt(3) != FloatNegINF) transform.setRotation(q);
}
static void SetTransformScale(hkQsTransform& transform, float s)
{
	if (s != FloatNegINF) transform.setScale(hkVector4(s,s,s));
}
static void PosRotScaleNode(hkQsTransform& transform, hkVector4& p, ::hkQuaternion& q, float s, PosRotScale prs)
{
	if (prs & prsScale) SetTransformScale(transform, s);
	if (prs & prsRot) SetTransformRotation(transform, q);
	if (prs & prsPos) SetTransformPosition(transform, p);
}
static void SetTransformPositionRange( hkArray<hkQsTransform>& transforms, int numTracks, int boneIdx
						   , float &currentTime, float lastTime, int &frame
						   , Vector3Key &first, Vector3Key &last)
{
	int n = transforms.getSize()/numTracks;
	hkVector4 p = TOVECTOR4(first.data);
	for ( ; COMPARE(currentTime, lastTime) <= 0 && frame < n; currentTime += FramesIncrement, ++frame)
	{
		hkQsTransform& transform = transforms[frame*numTracks + boneIdx];
		SetTransformPosition(transform, p);
	}
}
static void SetTransformRotationRange( hkArray<hkQsTransform>& transforms, int numTracks, int boneIdx
									  , float &currentTime, float lastTime, int &frame
									  , QuatKey &first, QuatKey &last)
{
	int n = transforms.getSize()/numTracks;
	::hkQuaternion q = TOQUAT(first.data);
	for ( ; COMPARE(currentTime, lastTime) <= 0&& frame < n; currentTime += FramesIncrement, ++frame)
	{
		hkQsTransform& transform = transforms[frame*numTracks + boneIdx];
		SetTransformRotation(transform, q);
	}
}
static void SetTransformRotationRange(hkArray<hkQsTransform>& transforms, int numTracks, int boneIdx
	, float& currentTime, float lastTime, int& frame
	, const FloatKey& x_first, const FloatKey& x_last
	, const FloatKey& y_first, const FloatKey& y_last
	, const FloatKey& z_first, const FloatKey& z_last
	)
{
	int n = transforms.getSize() / numTracks;
	hkRotation rot_z; rot_z.setAxisAngle(hkVector4(0., 0., 1.), x_first.data);
	hkRotation rot_y; rot_y.setAxisAngle(hkVector4(0., 1., 0.), y_first.data);
	hkRotation rot_x; rot_x.setAxisAngle(hkVector4(1., 0., 0.), z_first.data);

	hkRotation rot(rot_z), result; rot.mul(rot_y); rot.mul(rot_x);
	::hkQuaternion q(result);

	for (; COMPARE(currentTime, lastTime) <= 0 && frame < n; currentTime += FramesIncrement, ++frame)
	{
		hkQsTransform& transform = transforms[frame * numTracks + boneIdx];
		SetTransformRotation(transform, q);
	}
}
static void SetTransformScaleRange( hkArray<hkQsTransform>& transforms, int numTracks, int boneIdx
									  , float &currentTime, float lastTime, int &frame
									  , FloatKey &first, FloatKey &last)
{
	int n = transforms.getSize()/numTracks;
	for ( ; COMPARE(currentTime, lastTime) <= 0 && frame < n; currentTime += FramesIncrement, ++frame)
	{
		hkQsTransform& transform = transforms[frame*numTracks + boneIdx];
		SetTransformScale(transform, first.data);
	}
}

template <typename K>
static bool timeIndex(float time, const K& keysArray, int& i, int& j, float& x)
{
	int count;

	if ((count = keysArray.size()) > 0) {
		if (time <= keysArray[0].time) {
			i = j = 0;
			x = 0.0;

			return true;
		}

		if (time >= keysArray[count - 1].time /*nif->get<float>(array.child(count - 1, 0), "Time")*/) {
			i = j = count - 1;
			x = 0.0;

			return true;
		}

		if (i < 0 || i >= count)
			i = 0;

		float tI = keysArray[i].time; // nif->get<float>(array.child(i, 0), "Time");

		if (time > tI) {
			j = i + 1;
			float tJ;

			while (time >= (tJ = keysArray[j].time /*nif->get<float>(array.child(j, 0), "Time")*/)) {
				i = j++;
				tI = tJ;
			}

			x = (time - tI) / (tJ - tI);

			return true;
		}
		else if (time < tI) {
			j = i - 1;
			float tJ;

			while (time <= (tJ = keysArray[j].time /*nif->get<float>(array.child(j, 0), "Time")*/)) {
				i = j--;
				tI = tJ;
			}

			x = (time - tI) / (tJ - tI);

			// Quadratic Bug Fix

			// Invert x
			//	Previously, this branch was causing x to decrement from 1.0.
			//	(This works fine for linear interpolation apparently)
			x = 1.0 - x;

			// Swap I and J
			//	With x inverted, we must swap I and J or the animation will reverse.
			auto tmpI = i;
			i = j;
			j = tmpI;

			// End Bug Fix

			return true;
		}

		j = i;
		x = 0.0;

		return true;
	}

	return false;
}


template <typename T, typename K> bool interpolate(int interpolation_type, T& value, const K& array, float time, int& last)
{
	//const NifModel * nif = static_cast<const NifModel *>( array.model() );

	//if ( nif && array.isValid() ) {
		//QModelIndex frames = nif->getIndex( array, "Keys" );
	int next;
	float x;

	if (timeIndex(time, array, last, next, x)) {
		T v1(array[last].data); // nif->get<T>(frames.child(last, 0), "Value");
		T v2(array[next].data); // nif->get<T>( frames.child( next, 0 ), "Value" );

		switch (interpolation_type) {

		case 2:
		{
			// Quadratic
			/*
				In general, for keyframe values v1 = 0, v2 = 1 it appears that
				setting v1's corresponding "Backward" value to 1 and v2's
				corresponding "Forward" to 1 results in a linear interpolation.
			*/

			// Tangent 1
			T t1(array[last].backward_tangent); // nif->get<float>( frames.child( last, 0 ), "Backward" );
			// Tangent 2
			T t2(array[next].forward_tangent); // nif->get<float>(frames.child(next, 0), "Forward");

			float x2 = x * x;
			float x3 = x2 * x;

			// Cubic Hermite spline
			//	x(t) = (2t^3 - 3t^2 + 1)P1  + (-2t^3 + 3t^2)P2 + (t^3 - 2t^2 + t)T1 + (t^3 - t^2)T2

			value = v1 * (2.0f * x3 - 3.0f * x2 + 1.0f) + v2 * (-2.0f * x3 + 3.0f * x2) + t1 * (x3 - 2.0f * x2 + x) + t2 * (x3 - x2);

		}	return true;

		case 5:
			// Constant
			if (x < 0.5)
				value = v1;
			else
				value = v2;

			return true;
		default:
			value = v1 + (v2 - v1) * x;
			return true;
		}
		//	}
	}

	return false;
}

//template <> bool interpolate(int interpolation_type, float& value, const KeyGroup<float>& array, float time, int& last)
//{
//	return ::interpolate(interpolation_type, value, array, time, last);
//}
//
//template <> bool interpolate(int interpolation_type, ::Vector3& value, const KeyGroup<Niflib::Vector3>& array, float time, int& last)
//{
//	return ::interpolate(interpolation_type, value, array, time, last);
//}

//template <> bool interpolate(int interpolation_type, ::Color4& value, const KeyGroup<Niflib::Color4>& array, float time, int& last)
//{
//	return ::interpolate(interpolation_type, value, array, time, last);
//}
//
//template <> bool interpolate(int interpolation_type, ::Color3& value, const KeyGroup<Niflib::Color3>& array, float time, int& last)
//{
//	return ::interpolate(interpolation_type, value, array, time, last);
//}
//
//template <> bool interpolate(int interpolation_type, bool& value, const KeyGroup<bool>& array, float time, int& last)
//{
//	int next;
//	float x;
//	//const NifModel * nif = static_cast<const NifModel *>( iBlock.model() );
//
//	//if ( nif && array.isValid() ) {
//		//QModelIndex frames = nif->getIndex( array, "Keys" );
//
//
//	if (timeIndex(time, array, last, next, x)) {
//		value = array.keys[last].data; //nif->get<int>( frames.child( last, 0 ), "Value" );
//
//		return true;
//	}
//	//}
//
//	return false;
//}

//XYZ rotations
template <> bool interpolate(int interpolation_type, Niflib::Matrix33& value, const Niflib::array<3, KeyGroup<float > >& array, float time, int& last)
{

	int next;
	float x;

	if (!array.empty()) {
		float r[3] = {};

		for (int s = 0; s < 3 && s < array.size(); s++) {
			r[s] = 0;
			interpolate(interpolation_type, r[s], array[s].keys, time, last);
		}

		value = Niflib::Matrix33::euler(0, 0, r[2]) * Niflib::Matrix33::euler(0, r[1], 0) * Niflib::Matrix33::euler(r[0], 0, 0);

		return true;
	}

	return false;
}

//Quaternion rotations
template <> bool interpolate(int interpolation_type, Niflib::Quaternion& value, const vector<Key<Quaternion > >& array, float time, int& last)
{

	int next;
	float x;

	if (timeIndex(time, array, last, next, x)) {
		Niflib::Quaternion v1 = array[last].data;
		Niflib::Quaternion v2 = array[next].data;

		if (v1.Dot(v2) < 0)
			v1 = v1.Inverse(); // don't take the long path

		value = Quaternion::slerp(x, v1, v2);

		return true;
	}

	return false;
}

void importVectorOfKeys(vector<Vector3Key>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	int n = keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		Vector3Key* itr = &keys[0], * last = &keys[n - 1];
		SetTransformPositionRange(transforms, nbones, boneIdx, currentTime, (*itr).time, frame, *itr, *itr);
		for (int i = 1; i < n; ++i)
		{
			Vector3Key* next = &keys[i];
			SetTransformPositionRange(transforms, nbones, boneIdx, currentTime, (*next).time, frame, *itr, *next);
			itr = next;
		}
		SetTransformPositionRange(transforms, nbones, boneIdx, currentTime, duration, frame, *last, *last);
	}
}

void importVectorOfKeys(vector<FloatKey>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	int n = keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		FloatKey* itr = &keys[0], * last = &keys[n - 1];
		SetTransformScaleRange(transforms, nbones, boneIdx, currentTime, itr->time, frame, *itr, *itr);
		for (int i = 1; i < n; ++i)
		{
			FloatKey* next = &keys[i];
			SetTransformScaleRange(transforms, nbones, boneIdx, currentTime, next->time, frame, *itr, *next);
			itr = next;
		}
		SetTransformScaleRange(transforms, nbones, boneIdx, currentTime, duration, frame, *last, *last);
	}
}

void importVectorOfKeys(vector<QuatKey>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	int n = keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		QuatKey* itr = &keys[0], * last = &keys[n - 1];
		SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, itr->time, frame, *itr, *itr);
		for (int i = 1; i < n; ++i)
		{
			QuatKey* next = &keys[i];
			SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, next->time, frame, *itr, *next);
			itr = next;
		}
		SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, duration, frame, *last, *last);
	}
}



void importVectorOfKeys(int interpolationType, vector<Vector3Key>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration, int nframes) {
	int n = keys.size();
	if (n > 0)
	{
		const float frameTime = duration / (nframes - 1);
		int lastKey = 0;

		for (int frame = 0; frame < nframes; frame++)
		{
			float time = frame * frameTime;
			Vector3 value;
			interpolate(interpolationType, value, keys, time, lastKey);
			hkQsTransform& transform = transforms[frame * nbones + boneIdx];
			hkVector4 v(value.x, value.y, value.z);
			SetTransformPosition(transform, v);
		}
	}
}

void importVectorOfKeys(int interpolationType, const Niflib::array<3, KeyGroup<float > >& rotations, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration, int nframes) {

	if (rotations[0].keys.size() > 0 && rotations[1].keys.size() > 0 && rotations[2].keys.size() > 0)
	{

		const float frameTime = duration / (nframes - 1);
		int lastKeyX = 0;
		int lastKeyY = 0;
		int lastKeyZ = 0;

		for (int frame = 0; frame < nframes; frame++)
		{
			float time = frame * frameTime;
			float x = 0., y = 0., z = 0.;
			bool interpolateX = interpolate(rotations[0].interpolation, x, rotations[0].keys, time, lastKeyX);
			bool interpolateY = interpolate(rotations[1].interpolation, y, rotations[1].keys, time, lastKeyY);
			bool interpolateZ = interpolate(rotations[2].interpolation, z, rotations[2].keys, time, lastKeyZ);
			hkQsTransform& transform = transforms[frame * nbones + boneIdx];

			hkRotation rot_z; rot_z.setIdentity();
			if (interpolateZ)
				rot_z.setAxisAngle(hkVector4(0., 0., 1.), z);
			hkRotation rot_y; rot_y.setIdentity();
			if (interpolateY)
				rot_y.setAxisAngle(hkVector4(0., 1., 0.), y);
			hkRotation rot_x; rot_x.setIdentity();
			if (interpolateX)
				rot_x.setAxisAngle(hkVector4(1., 0., 0.), x);

			hkRotation rot(rot_z); rot.mul(rot_y); rot.mul(rot_x);
			::hkQuaternion q(rot);

			if (interpolateX || interpolateY || interpolateZ)
				SetTransformRotation(transform, q);

		}	
	}
	else {
		Log::Warn("Found XYZ key vector with null keys component");
	}
}

void importVectorOfKeys(int interpolationType, vector<QuatKey>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration, int nframes) {
	int n = keys.size();
	if (n > 0)
	{
		const float frameTime = duration / (nframes - 1);
		int lastKey = 0;

		for (int frame = 0; frame < nframes; frame++)
		{
			float time = frame * frameTime;
			Niflib::Quaternion value;
			interpolate(interpolationType, value, keys, time, lastKey);
			hkQsTransform& transform = transforms[frame * nbones + boneIdx];
			::hkQuaternion q = TOQUAT(value);
			SetTransformRotation(transform, q);
		}
	}
}

void importVectorOfKeys(int interpolationType, vector<FloatKey>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration, int nframes) {
	int n = keys.size();
	if (n > 0)
	{

		const float frameTime = duration / (nframes - 1);
		int lastKey = 0;

		for (int frame = 0; frame < nframes; frame++)
		{
			float time = frame * frameTime;
			float s;
			interpolate(interpolationType, s, keys, time, lastKey);
			hkQsTransform& transform = transforms[frame * nbones + boneIdx];

			SetTransformScale(transform, s);

		}
	}
}

template<typename T>
bool getFieldIsValid(T* object, unsigned int field, const NifInfo& info) {
	vector<unsigned int> valid_translations_fields = object->GetValidFieldsIndices(info);
	return find(valid_translations_fields.begin(), valid_translations_fields.end(), field)
		!= valid_translations_fields.end();
}

//TODO: other key types?

string getPalettedString(NiStringPaletteRef nipalette, unsigned int offset) {
	if (nullptr != nipalette)
	{
		StringPalette s_palette = nipalette->GetPalette();
		const char* c_palette = s_palette.palette.c_str();
		for (size_t i = offset; i < s_palette.length; i++)
		{
			if (c_palette[i] == 0)
				return string(&c_palette[offset], &c_palette[i]);
		}
	}
	else {
		Log::Warn("Null NiPalette!");
	}
	return "";
}


bool AnimationExport::exportController()
{
	vector<Niflib::ControlledBlock> blocks = seq->GetControlledBlocks();
	int nbones = skeleton->m_bones.getSize();

	//Find accum bone

   //if (AnimationExport::noRootSiblings)
   //{
	  // // Remove bones not children of root.  This is a bit of a kludge.  
	  // //  Basically search for the first node after the root which also has no parent
	  // //  This is typically Camera3. We then truncate tracks to exclude nodes appearing after.
	  // for (int i=1; i<nbones; ++i)
	  // {
		 //  if (skeleton->m_parentIndices[i] < 0)
		 //  {
			//   nbones = i;
			//   break;
		 //  }
	  // }
   //}
	int numTracks = nbones;

	float duration = seq->GetStopTime() - seq->GetStartTime();
	int nframes = (int)roundf(duration / FramesIncrement);

	float frameTime = duration / nframes;

	int nCurrentFrame = 0;

	hkRefPtr<hkaInterleavedUncompressedAnimation> tempAnim = new hkaInterleavedUncompressedAnimation();
	tempAnim->m_duration = duration;
	tempAnim->m_numberOfTransformTracks = numTracks;
	tempAnim->m_numberOfFloatTracks = 0;//anim->m_numberOfFloatTracks;
	tempAnim->m_transforms.setSize(numTracks*nframes, hkQsTransform::getIdentity());
	tempAnim->m_floats.setSize(tempAnim->m_numberOfFloatTracks);
	tempAnim->m_annotationTracks.setSize(numTracks);

	if (seq->GetTextKeys() != NULL)
	{
		auto data = seq->GetTextKeys();
		multimap<float, string> annotations;
		for (const auto& key : data->GetTextKeys())
		{
			if (isOblivionEngineAnnotation(key.data))
			{
				//TODO bow
				auto converted = convertOblivionAnnotations({ key.time, key.data });
				for (auto& entry : converted)
				{
					annotations.insert({entry.first, entry.second});
				}
			}
			else {
				annotations.insert({ key.time, key.data });
			}
		}
		auto& root_annotations = tempAnim->m_annotationTracks[0];
		for (auto& entry : annotations)
		{
			hkaAnnotationTrack::Annotation anno;
			anno.m_text = entry.second.c_str();
			anno.m_time = entry.first;
			root_annotations.m_annotations.pushBack(anno);

			_root_info.events.push_back(
				{
					entry.first,
					entry.second
				}
			);
		}
	}

	hkArray<hkQsTransform>& transforms = tempAnim->m_transforms;

	typedef map<string, int, ltstr> StringIntMap;
	StringIntMap boneMap;
	for (int i=0; i<nbones; ++i)
	{
		string name = skeleton->m_bones[i].m_name;
		boneMap[name] = i;
	}

	if (boneMap.find(seq->GetAccumRootName()) == boneMap.end())
	{
		Log::Error("Unable to find sequence accumulation root into the skeleton!");
		return false;
	}
	int accum_root_index = boneMap.at(seq->GetAccumRootName());
	hkQsTransform rootTransform = skeleton->m_referencePose[accum_root_index];
	//rootTransform.setInverse(rootTransform);

	//Controlled links
	for ( vector<Niflib::ControlledBlock>::iterator bitr = blocks.begin(); bitr != blocks.end(); ++bitr)
	{
		int offset = bitr->nodeNameOffset;
		string nodename = getPalettedString(seq->GetStringPalette(), offset);
		StringIntMap::iterator boneitr = boneMap.find(nodename);
		if (boneitr == boneMap.end())
		{
			Log::Warn("Unknown bone '%s' found in animation. Skipping.", nodename.c_str());
			continue;
		}

		int boneIdx = boneitr->second;
		hkQsTransform localTransform = skeleton->m_referencePose[boneIdx];
		NiTransformInterpolatorRef generic_interp = DynamicCast<NiTransformInterpolator>((*bitr).interpolator);
		if (NULL != generic_interp)
		{
			auto translation = TOVECTOR4(generic_interp->GetTransform().translation);
			SetTransformPosition(localTransform, translation);
			if (generic_interp->GetTransform().rotation.x != FloatNegINF)
			{
				auto rotation = TOQUAT(generic_interp->GetTransform().rotation);
				SetTransformRotation(localTransform, rotation);
			}
			auto scale = generic_interp->GetTransform().scale;
			SetTransformScale(localTransform, scale);
		}

		FillTransforms(transforms, boneIdx, nbones, localTransform); // prefill transforms with bindpose

		if (NiBSplineCompTransformInterpolatorRef interp = DynamicCast<NiBSplineCompTransformInterpolator>((*bitr).interpolator))
		{
			auto translation = TOVECTOR4(interp->GetTransform().translation);
			SetTransformPosition(localTransform, translation);
			if (interp->GetTransform().rotation.x != FloatNegINF)
			{
				auto rotation = TOQUAT(interp->GetTransform().rotation);
				SetTransformRotation(localTransform, rotation);
			}
			auto scale = interp->GetTransform().scale;
			SetTransformScale(localTransform, scale);

			FillTransforms(transforms, boneIdx, nbones, localTransform); // prefill transforms with bindpose

			int npoints = GetNumControlPoints(interp);

			if (npoints > 3)
			{
				importVectorOfKeys(SampleTranslateKeys(interp, nframes, 3), transforms, nbones, boneIdx, duration);
				importVectorOfKeys(SampleQuatRotateKeys(interp, nframes, 3), transforms, nbones, boneIdx, duration);
				importVectorOfKeys(SampleScaleKeys(interp, nframes, 3), transforms, nbones, boneIdx, duration);
			}
			else if (npoints == 0)
			{
				Log::Warn("Found NiBSplineCompTransformInterpolator with no control points!");
			}
			else
				throw runtime_error("Not enough points to calculate the spline");
		}
		else if (NiTransformInterpolatorRef interp = DynamicCast<NiTransformInterpolator>((*bitr).interpolator))
		{
			// Havok animations are basically linear interpolated animations
			// see hkaInterleavedUncompressedAnimation
			// so for both constant and quadratic interpolation types
			// we should really interpolate and sample them at least at 24hz 

			if (NiTransformDataRef data = interp->GetData())
			{
				auto translation = TOVECTOR4(interp->GetTransform().translation);
				SetTransformPosition(localTransform, translation);
				if (interp->GetTransform().rotation.x != FloatNegINF)
				{
					auto rotation = TOQUAT(interp->GetTransform().rotation);
					SetTransformRotation(localTransform, rotation);
				}
				auto scale = interp->GetTransform().scale;
				SetTransformScale(localTransform, scale);

				FillTransforms(transforms, boneIdx, nbones, localTransform); // prefill transforms with bindpose

				if (data != NULL) {

					//translations:
					importVectorOfKeys(data->GetTranslations().interpolation, data->GetTranslations().keys, transforms, nbones, boneIdx, duration, nframes);

					//rotations:
					if (getFieldIsValid(&*data, NiKeyframeData::FIELDS::rotationType, _info)) {
						if (data->GetRotationType() == XYZ_ROTATION_KEY) {
							//single float groups with tangents
							importVectorOfKeys(Niflib::XYZ_ROTATION_KEY, data->GetXyzRotations(), transforms, nbones, boneIdx, duration, nframes);
						}
						else {
							//threat as quaternion?
							importVectorOfKeys(data->GetRotationType(), data->GetQuaternionKeys(), transforms, nbones, boneIdx, duration, nframes);
						}
					}
					else {
						importVectorOfKeys(data->GetRotationType(), data->GetQuaternionKeys(), transforms, nbones, boneIdx, duration, nframes);
					}

					//scale
					importVectorOfKeys(data->GetScales().interpolation, data->GetScales().keys, transforms, nbones, boneIdx, duration, nframes);
				}
			}
		}
		//else if (NiKeyBasedInterpolatorRef interp = DynamicCast<NiKeyBasedInterpolator>((*bitr).interpolator)) {
		//	//niboolinterpolator
		//	//nifloatinterpolator
		//	//nipathinterpolator
		//	//nipoint3interpolator
		//}
	}

	//Extract Motion
	for (int f = 0; f < nframes; f++)
	{
		hkQsTransform& root_transform = tempAnim->m_transforms[nbones * f];
		hkQsTransform& pelvis_transform = tempAnim->m_transforms[nbones * f + 1]; //assume pelvis is the second bone

		hkQsTransform pelvis_world_transform; pelvis_world_transform.setMul(root_transform, pelvis_transform);
		
		// the root movement is x, y, rotation round z
		root_transform.setTranslation(
			hkVector4(
				0.,//pelvis_world_transform.getTranslation()(0),
				0.,//pelvis_world_transform.getTranslation()(1),
				0.
			)
		);
		auto quat = pelvis_world_transform.getRotation();
		Quat QuatRotNew = { quat(0), quat(1), quat(2), quat(3) };
		EulerAngles z_eul = Eul_FromQuat(QuatRotNew, EulOrdZXYs);
		z_eul.x = 0; z_eul.y = 0;
		EulerAngles xy_eul = Eul_FromQuat(QuatRotNew, EulOrdZXYs);
		xy_eul.z = 0;
		auto z_quat = Eul_ToQuat(z_eul);
		auto xy_quat = Eul_ToQuat(xy_eul);

		_root_info.translations.push_back
		({
			f * (frameTime),
			hkVector4(pelvis_world_transform.getTranslation()(0), pelvis_world_transform.getTranslation()(1), 0.0)
			});

		_root_info.rotations.push_back
		({
			f* (frameTime),
			{(float)z_quat.x, (float)z_quat.y, (float)z_quat.z, (float)z_quat.w}
		});


		rootTransform.setRotation({0., 0., 0., 1.});

		//Pelvis Movement gets the rest;
		pelvis_transform.setTranslation(
			hkVector4(
				0.,
				0.,
				pelvis_world_transform.getTranslation()(2)
			)
		);
		pelvis_transform.setRotation({ (float)xy_quat.x, (float)xy_quat.y, (float)xy_quat.z, (float)xy_quat.w });
	}

	if (_root_info.translations.empty()) {
		_root_info.translations.push_back
		({
			duration,
			hkVector4(0.0, 0.0, 0.0)
			});
	}

	if (_root_info.rotations.empty()) {
		_root_info.rotations.push_back
		({
			duration,
			::hkQuaternion(0.0, 0.0, 0.0, 1.0)
			});
	}

	hkaSkeletonUtils::normalizeRotations (transforms.begin(), transforms.getSize()); 

	// create the animation with default settings
	{
		//hkaSplineCompressedAnimation::TrackCompressionParams tparams;
		//hkaSplineCompressedAnimation::AnimationCompressionParams aparams;

		//tparams.m_rotationTolerance = 0.001f;
		//tparams.m_rotationQuantizationType = hkaSplineCompressedAnimation::TrackCompressionParams::THREECOMP40;

		//hkRefPtr<hkaSplineCompressedAnimation> outAnim = new hkaSplineCompressedAnimation( *tempAnim.val(), tparams, aparams ); 
		binding->m_animation = tempAnim;
	}
	
	return true;
}

void ImportKF::ExportAnimations(const NifFolderType& in 
	, const hkRefPtr<hkaSkeleton>& skeleton
	, const string& outdir
	, std::map<fs::path, ckcmd::HKX::RootMovement>& rootMovements)
{
	auto& animlist = std::get<2>(in);
	for (auto& itr = animlist.begin(); itr != animlist.end(); ++itr)
	{
		string animfile = itr->first.string();
		string subfolder = itr->first.parent_path().filename().string();
		string project_subfolder = fs::path(outdir).filename().string();
		string creature_subfolder = fs::path(outdir).parent_path().filename().string();
		if (subfolder == creature_subfolder)
			subfolder = "";

		string out_relative = (fs::path(project_subfolder) / subfolder / itr->first.filename().replace_extension(".hkx")).string();
		string outfile = (fs::path(outdir).parent_path() / out_relative).string(); // / itr->first.filename().replace_extension(".hkx")).string();
		Log::Verbose("ExportAnimation Starting '%s'", animfile.c_str());


		//Niflib::NifOptions options;
		//options.exceptionOnErrors = false;
		NifInfo info;
		vector<NiControllerSequenceRef> blocks = DynamicCast<NiControllerSequence>(itr->second);

		int nbindings = blocks.size();
		if (nbindings == 0)
		{
			Log::Error("Animation file contains no animation bindings.  Not exporting.");
		}
		else if (nbindings != 1)
		{
			Log::Warn("Animation file contains more than one animation binding.  Not exporting the additionals.");
			nbindings = 1;
		}
		else
		{
			for (int i = 0, n = nbindings; i < n; ++i)
			{
				char fname[MAX_PATH];
				_splitpath(animfile.c_str(), NULL, NULL, fname, NULL);

				NiControllerSequenceRef seq = blocks[i];
				hkRootLevelContainer rootCont;
				hkRefPtr<hkaAnimationContainer> skelAnimCont = new hkaAnimationContainer();
				hkRefPtr<hkaAnimationBinding> newBinding = new hkaAnimationBinding();
				skelAnimCont->m_bindings.append(&newBinding, 1);
				rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Merged Animation Container", skelAnimCont.val(), &skelAnimCont->staticClass()));

				Log::Verbose("ExportAnimation Exporting to '%s'", outfile.c_str());

				AnimationExport exporter(seq, skeleton, newBinding, info);
				if (exporter.doExport())
				{

					Log::Info("Exporting '%s'", outfile.c_str());
					skelAnimCont->m_animations.pushBack(newBinding->m_animation);
					fs::create_directories(fs::path(outfile).parent_path());
					ckcmd::HKX::HKXWrapper().write_xml(&rootCont, outfile);


/*					hkOstream stream(outfile.c_str());
					hkVariant root = { &rootCont, &rootCont.staticClass() };
					hkPackFormat pkFormat = HKPF_DEFAULT;
					hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);
					auto flags = (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_TEXT_FORMAT | hkSerializeUtil::SAVE_TEXT_NUMBERS);
					hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
					if (res != HK_SUCCESS)
					{
						Log::Error("Havok reports save failed for file %s.", outfile.c_str());
					}*/
					rootMovements[out_relative] = exporter._root_info;
				}
				else
				{
					Log::Error("Export failed for '%s'", outfile.c_str());
				}
			}
		}
	}
}

void ImportKF::ExportAnimations(const string& rootdir, const fs::path& skelfile
                      , const vector<fs::path>& animlist, const string& outdir
                      , hkPackFormat pkFormat, const hkPackfileWriter::Options& packFileOptions
                      , hkSerializeUtil::SaveOptionBits flags
					  , ckcmd::HKX::RootMovement& root_info
                      , bool norelativepath)
{
	hkResource* skelResource = NULL;
	hkResource* animResource = NULL;
	hkaSkeleton* skeleton = NULL;

	Log::Verbose("ExportAnimation('%s','%s','%s')", rootdir.c_str(), skelfile.c_str(), outdir.c_str());
	// Read back a serialized file
	{
		hkIstream stream(skelfile.string().c_str());
		hkStreamReader *reader = stream.getStreamReader();
      skelResource = hkSerializeLoadResource(reader);
      if (skelResource == NULL)
      {
         Log::Warn("Skeleton File is not loadable: '%s'", skelfile);
      }
      else
		{
			const char * hktypename = skelResource->getContentsTypeName();
			void * contentPtr = skelResource->getContentsPointer(HK_NULL, HK_NULL);
			hkRootLevelContainer* scene = skelResource->getContents<hkRootLevelContainer>();
			hkaAnimationContainer *skelAnimCont = scene->findObject<hkaAnimationContainer>();
			if ( !skelAnimCont->m_skeletons.isEmpty() )
				skeleton = skelAnimCont->m_skeletons[0];
		}
	}
	if (skeleton != NULL)
	{
		for (vector<fs::path>::const_iterator itr = animlist.begin(); itr != animlist.end(); ++itr)
		{
			string animfile = (*itr).string();
			Log::Verbose("ExportAnimation Starting '%s'", animfile.c_str());


			char outfile[MAX_PATH], relout[MAX_PATH];
			LPCSTR extn = PathFindExtension(outdir.c_str());
			if (stricmp(extn, ".kf") == 0)
			{
				strcpy(outfile, outdir.c_str());
			}
			else // assume its a folder
			{
				if (norelativepath)
				{
					PathCombine(outfile, outdir.c_str(), PathFindFileName(animfile.c_str()));
				}
				else
				{
					char relpath[MAX_PATH];
					PathRelativePathTo(relpath, rootdir.c_str(), FILE_ATTRIBUTE_DIRECTORY, animfile.c_str(), 0);
					PathCombine(outfile, outdir.c_str(), relpath);
					GetFullPathName(outfile, MAX_PATH, outfile, NULL);
				}				
				PathRemoveExtension(outfile);
				PathAddExtension(outfile, ".hkx");
			}
			char workdir[MAX_PATH];
			_getcwd(workdir, MAX_PATH);
			PathRelativePathTo(relout, workdir, FILE_ATTRIBUTE_DIRECTORY, outfile, 0);

			Log::Verbose("ExportAnimation Reading '%s'", animfile.c_str());


			//Niflib::NifOptions options;
			//options.exceptionOnErrors = false;
			NifInfo info;
			vector<NiControllerSequenceRef> blocks = DynamicCast<NiControllerSequence>(Niflib::ReadNifList(animfile, &info /*, &options*/));

			int nbindings = blocks.size();
			if ( nbindings == 0)
			{
				Log::Error("Animation file contains no animation bindings.  Not exporting.");
			}
			else if ( nbindings != 1)
			{
				Log::Error("Animation file contains more than one animation binding.  Not exporting.");
			}
			else
			{
				for ( int i=0, n=nbindings; i<n; ++i)
				{
					char fname[MAX_PATH];
					_splitpath(animfile.c_str(), NULL, NULL, fname, NULL);

					NiControllerSequenceRef seq = blocks[i];
					hkRootLevelContainer rootCont;
					hkRefPtr<hkaAnimationContainer> skelAnimCont = new hkaAnimationContainer();
					hkRefPtr<hkaAnimationBinding> newBinding = new hkaAnimationBinding();
					skelAnimCont->m_bindings.append(&newBinding, 1);
					rootCont.m_namedVariants.pushBack( hkRootLevelContainer::NamedVariant("Merged Animation Container", skelAnimCont.val(), &skelAnimCont->staticClass()) );

					Log::Verbose("ExportAnimation Exporting '%s'", outfile);

					AnimationExport exporter(seq, skeleton, newBinding, info);
					if ( exporter.doExport() )
					{
						char outfiledir[MAX_PATH];
						strcpy(outfiledir, outfile);
						PathRemoveFileSpec(outfiledir);
						CreateDirectories(outfiledir);

						Log::Info("Exporting '%s'", outfile);
						skelAnimCont->m_animations.pushBack(newBinding->m_animation);

						hkOstream stream(outfile);
						hkVariant root = { &rootCont, &rootCont.staticClass() };
						flags = (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_TEXT_FORMAT | hkSerializeUtil::SAVE_TEXT_NUMBERS);
						hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
						if ( res != HK_SUCCESS )
						{
							Log::Error("Havok reports save failed.");
						}
						root_info = exporter._root_info;
					}
					else
					{
						Log::Error("Export failed for '%s'", relout);
					}
				}
			}
		}
	}


	if (skelResource) skelResource->removeReference();
}
//////////////////////////////////////////////////////////////////////////

static void findFiles(fs::path startingDir, string extension, vector<fs::path>& results) {
	if (!exists(startingDir) || !is_directory(startingDir)) return;
	for (auto& dirEntry : fs::recursive_directory_iterator(startingDir))
	{
		if (fs::is_directory(dirEntry.path()))
			continue;

		std::string entry_extension = dirEntry.path().extension().string();
		transform(entry_extension.begin(), entry_extension.end(), entry_extension.begin(), ::tolower);
		if (entry_extension == extension) {
			results.push_back(dirEntry.path().string());
		}
	}
}

static void ExportProject( const string &projfile, const char * rootPath, const char * outdir
                          , hkPackFormat pkFormat, const hkPackfileWriter::Options& packFileOptions
                          , hkSerializeUtil::SaveOptionBits flags, bool recursion)
{
	vector<fs::path> skelfiles, animfiles;
	char projpath[MAX_PATH], skelpath[MAX_PATH], animpath[MAX_PATH];

	if ( wildmatch("*skeleton.hkx", projfile) )
	{
		skelfiles.push_back(projfile);

		GetFullPathName(projfile.c_str(), MAX_PATH, projpath, NULL);
		PathRemoveFileSpec(projpath);
		PathAddBackslash(projpath);
		findFiles(rootPath, ".kf", animfiles);
	}
	if (skelfiles.empty())
	{
		Log::Warn("No skeletons found. Skipping '%s'", projpath);
	}
	else if (skelfiles.size() != 1)
	{
		Log::Warn("Multiple skeletons found. Skipping '%s'", projpath);
	}
	else if (animfiles.empty())
	{
		Log::Warn("No Animations found. Skipping '%s'", projpath);
	}
	else
	{
		ckcmd::HKX::RootMovement movement;
		ImportKF::ExportAnimations(string(rootPath), skelfiles[0],animfiles, outdir, pkFormat, packFileOptions, flags, movement, false);
	}
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


bool ImportKF::InternalRunCommand(map<string, docopt::value> parsedArgs)
{
	string importKF, animationPaths, exportPath;

	importKF = parsedArgs["<path_to_skeleton>"].asString();
	animationPaths = parsedArgs["<path_to_animations>"].asString();
	exportPath = parsedArgs["<path_to_export>"].asString();

	bool recursion = true;
	vector<string> paths;
	paths.push_back(importKF);
	paths.push_back(animationPaths);
	paths.push_back(exportPath);
	hkPackFormat pkFormat = HKPF_DEFAULT;
	hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;
    AnimationExport::noRootSiblings = true;

	if (paths.empty()){
		HelpString(hkxcmd::htLong);
		return false;
	}

	hkMallocAllocator baseMalloc;
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024) );
	hkBaseSystem::init( memoryRouter, debugReport );
	LoadDefaultRegistry();

   if (pkFormat == HKPF_XML || pkFormat == HKPF_TAGXML) // set text format to indicate xml
   {
      flags = (hkSerializeUtil::SaveOptionBits)(flags | hkSerializeUtil::SAVE_TEXT_FORMAT);
   }
	hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);

	// search for projects and guess the layout
	if (PathIsDirectory(paths[0].c_str()))
	{
		char searchPath[MAX_PATH], rootPath[MAX_PATH];
		GetFullPathName(paths[0].c_str(), MAX_PATH, rootPath, NULL);
		strcpy(searchPath, rootPath);
		PathAddBackslash(searchPath);
		strcat(searchPath, "*skeleton.hkx");

		vector<string> files;
		FindFiles(files, searchPath, recursion);
		for (vector<string>::iterator itr = files.begin(); itr != files.end(); )
		{
			if (wildmatch("*\\skeleton.hkx", (*itr)))
			{
				++itr;
			}
			else
			{
				Log::Verbose("Ignoring '%s' due to inexact skeleton.hkx file match", (*itr).c_str());
				itr = files.erase(itr);
			}			
		}


		if (files.empty())
		{
			Log::Warn("No files found");
			return false;
		}

		char outdir[MAX_PATH];
		if (paths.size() > 1)
		{
			GetFullPathName(paths[1].c_str(), MAX_PATH, outdir, NULL);
		}
		else
		{
			strcpy(outdir, rootPath);
		}

		for (vector<string>::iterator itr = files.begin(); itr != files.end(); ++itr)
		{
			string projfile = (*itr).c_str();
			ExportProject(projfile, rootPath, outdir, pkFormat, packFileOptions, flags, recursion);
		}
	}
	else
	{
		string skelpath = (paths.size() >= 0) ? paths[0] : string();

		if (skelpath.empty())
		{
			Log::Error("Skeleton file not specified");
			HelpString(hkxcmd::htLong);
		}
		else if ( wildmatch("*project.hkx", skelpath) )
		{
			// handle specification of project by name
			char rootPath[MAX_PATH];
			GetFullPathName(skelpath.c_str(), MAX_PATH, rootPath, NULL);
			PathRemoveFileSpec(rootPath);

			if (paths.size() > 2)
			{
				Log::Error("Too many arguments specified");
				HelpString(hkxcmd::htLong);
			}
			else
			{
				char outdir[MAX_PATH];
				if (paths.size() >= 1){
					GetFullPathName(paths[1].c_str(), MAX_PATH, outdir, NULL);
				} else { 
					strcpy(outdir, rootPath); 
				}
				ExportProject(skelpath, rootPath, outdir, pkFormat, packFileOptions, flags, recursion);
			}
		}
		else
		{
			// handle specification of skeleton + animation + output
			if ( !PathFileExists(skelpath.c_str()) )
			{
				Log::Error("Skeleton file not found at '%s'", skelpath.c_str());
			}
			else
			{
				// set relative path to current directory
				char rootPath[MAX_PATH];
				_getcwd(rootPath, MAX_PATH);

				if (paths.size() > 3)
				{
					Log::Error("Too many arguments specified");
					HelpString(hkxcmd::htLong);
				}
				else
				{
					bool norelativepath = true;
					if (paths.size() == 1) // output files inplace
					{
						char animDir[MAX_PATH], tempdir[MAX_PATH];
						strcpy(tempdir, skelpath.c_str());
						PathRemoveFileSpec(tempdir);
						PathCombine(animDir, tempdir, "..\animations");
						PathAddBackslash(animDir);
						ExportProject(skelpath, rootPath, rootPath, pkFormat, packFileOptions, flags, recursion);
					}
					else if (paths.size() == 2) // second path will be output
					{
						char outdir[MAX_PATH], tempdir[MAX_PATH];
						strcpy(outdir, paths[1].c_str());
						strcpy(tempdir, skelpath.c_str());
						PathRemoveFileSpec(tempdir);
						PathAddBackslash(tempdir);
						PathCombine(rootPath,tempdir,"..\\animations");
						GetFullPathName(rootPath, MAX_PATH, rootPath, NULL);
						GetFullPathName(outdir, MAX_PATH, outdir, NULL);
						ExportProject(skelpath, rootPath, outdir, pkFormat, packFileOptions, flags, recursion);
					}
					else // second path is animation, third is output
					{
						string animpath = paths[1];
						//if (PathIsDirectory(animpath.c_str()))
						//{
						//	strcpy(rootPath, animpath.c_str());
						//	animpath += string("\\*.hkx");
						//	norelativepath = false;
						//}
						vector<fs::path> animfiles;
						findFiles(animpath, ".kf", animfiles);

						if (animfiles.empty())
						{
							Log::Warn("No Animations found. Skipping '%s'", animpath.c_str());
						}
						else
						{

							char outdir[MAX_PATH];
							if (paths.size() >= 2){
								GetFullPathName(paths[2].c_str(), MAX_PATH, outdir, NULL);
							} else { 
								strcpy(outdir, rootPath); 
							}
							ckcmd::HKX::RootMovement movement;
							ExportAnimations(string(rootPath), skelpath, animfiles, outdir, pkFormat, packFileOptions, flags, movement, norelativepath);
						}

					}
				}
			}
		}
	}
	return true;
}

//Havok initialization

static void HK_CALL errorReport(const char* msg, void*)
{
	Log::Error("%s", msg);
}

static hkThreadMemory* threadMemory = NULL;
static char* stackBuffer = NULL;
static void InitializeHavok()
{
	// Initialize the base system including our memory system
	hkMemoryRouter* pMemoryRouter(hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(5000000)));
	hkBaseSystem::init(pMemoryRouter, errorReport);
	LoadDefaultRegistry();
}

static void CloseHavok()
{
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();
}
