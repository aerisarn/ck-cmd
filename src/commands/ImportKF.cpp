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

#include <interfaces\misc.h>

using namespace Niflib;
using namespace std;
using namespace ConvertKF;

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

int GetNumControlPoints(NiBSplineTransformInterpolatorRef& interpolator)
{
	if (interpolator->GetBasisData())
		return interpolator->GetBasisData()->GetNumControlPoints();
	return 0;
}

vector< Key<Quaternion> > SampleQuatRotateKeys(NiBSplineTransformInterpolatorRef& interpolator, int npoints, int degree)
{
	Accessor<NiBSplineTransformInterpolatorAccessor> accessor(interpolator);
	vector< Key<Quaternion> > value;
	if ((interpolator->GetTranslationHandle() != USHRT_MAX) && interpolator->GetSplineData() && interpolator->GetBasisData()) { // has rotation data
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

bool AnimationExport::exportNotes( )
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

	hkRotation rot(rot_z); rot.setMul(rot, rot_y); rot.setMul(rot, rot_x);
	::hkQuaternion q(rot);

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


void importVectorOfKeys(vector<Vector3Key>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	int n = keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		Vector3Key* itr = &keys[0], *last = &keys[n - 1];
		SetTransformPositionRange(transforms, nbones, boneIdx, currentTime, (*itr).time, frame, *itr, *itr);
		for (int i = 1; i<n; ++i)
		{
			Vector3Key* next = &keys[i];
			SetTransformPositionRange(transforms, nbones, boneIdx, currentTime, (*next).time, frame, *itr, *next);
			itr = next;
		}
		SetTransformPositionRange(transforms, nbones, boneIdx, currentTime, duration, frame, *last, *last);
	}
}

void importVectorOfKeys(const Niflib::array<3, KeyGroup<float > >& rotations, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	
	int n = rotations[0].keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		const FloatKey* x_itr = &rotations[0].keys[0], * x_last = &rotations[0].keys[n - 1];
		const FloatKey* y_itr = &rotations[1].keys[0], * y_last = &rotations[1].keys[n - 1];
		const FloatKey* z_itr = &rotations[2].keys[0], * z_last = &rotations[2].keys[n - 1];
		SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, (*x_itr).time, frame, *x_itr, *x_itr, *y_itr, *y_itr, *z_itr, *z_itr);
		for (int i = 1; i < n; ++i)
		{
			const FloatKey* x_next = &rotations[0].keys[i];
			const FloatKey* y_next = &rotations[1].keys[i];
			const FloatKey* z_next = &rotations[2].keys[i];
			SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, (*x_next).time, frame, *x_itr, *x_next, *y_itr, *y_next, *z_itr, *z_next);
			x_itr = x_next;
			y_itr = y_next;
			z_itr = z_next;
		}
		SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, (*x_last).time, frame, *x_itr, *x_last, *y_itr, *y_last, *z_itr, *z_last);		
	}
}

void importVectorOfKeys(vector<QuatKey>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	int n = keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		QuatKey* itr = &keys[0], *last = &keys[n - 1];
		SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, itr->time, frame, *itr, *itr);
		for (int i = 1; i<n; ++i)
		{
			QuatKey* next = &keys[i];
			SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, next->time, frame, *itr, *next);
			itr = next;
		}
		SetTransformRotationRange(transforms, nbones, boneIdx, currentTime, duration, frame, *last, *last);
	}
}

void importVectorOfKeys(vector<FloatKey>& keys, hkArray<hkQsTransform>& transforms, int nbones, int boneIdx, float duration) {
	int n = keys.size();
	if (n > 0)
	{
		int frame = 0;
		float currentTime = 0.0f;
		FloatKey* itr = &keys[0], *last = &keys[n - 1];
		SetTransformScaleRange(transforms, nbones, boneIdx, currentTime, itr->time, frame, *itr, *itr);
		for (int i = 1; i<n; ++i)
		{
			FloatKey* next = &keys[i];
			SetTransformScaleRange(transforms, nbones, boneIdx, currentTime, next->time, frame, *itr, *next);
			itr = next;
		}
		SetTransformScaleRange(transforms, nbones, boneIdx, currentTime, duration, frame, *last, *last);
	}
}

template<typename T>
bool getFieldIsValid(T* object, unsigned int field, const NifInfo& info) {
	vector<unsigned int> valid_translations_fields = object->GetValidFieldsIndices(info);
	return find(valid_translations_fields.begin(), valid_translations_fields.end(), field)
		!= valid_translations_fields.end();
}

//TODO: other key types?

bool AnimationExport::exportController()
{
	vector<Niflib::ControlledBlock> blocks = seq->GetControlledBlocks();
	int nbones = skeleton->m_bones.getSize();

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


	int nCurrentFrame = 0;

	hkRefPtr<hkaInterleavedUncompressedAnimation> tempAnim = new hkaInterleavedUncompressedAnimation();
	tempAnim->m_duration = duration;
	tempAnim->m_numberOfTransformTracks = numTracks;
	tempAnim->m_numberOfFloatTracks = 0;//anim->m_numberOfFloatTracks;
	tempAnim->m_transforms.setSize(numTracks*nframes, hkQsTransform::getIdentity());
	tempAnim->m_floats.setSize(tempAnim->m_numberOfFloatTracks);
	tempAnim->m_annotationTracks.setSize(numTracks);

	hkArray<hkQsTransform>& transforms = tempAnim->m_transforms;

	typedef map<string, int, ltstr> StringIntMap;
	StringIntMap boneMap;
	for (int i=0; i<nbones; ++i)
	{
		string name = skeleton->m_bones[i].m_name;
		boneMap[name] = i;
	}



	//Controlled links
	for ( vector<Niflib::ControlledBlock>::iterator bitr = blocks.begin(); bitr != blocks.end(); ++bitr)
	{
		int offset = bitr->nodeNameOffset;
		//string nodename = seq->GetStringPalette()->GetSubStr(offset);
		string nodename = Niflib::GetSubStr(seq->GetStringPalette(), offset);
		StringIntMap::iterator boneitr = boneMap.find(nodename);
		if (boneitr == boneMap.end())
		{
			Log::Warn("Unknown bone '%s' found in animation. Skipping.", nodename.c_str());
			continue;
		}

		int boneIdx = boneitr->second;
		hkQsTransform localTransform = skeleton->m_referencePose[boneIdx];

		FillTransforms(transforms, boneIdx, nbones, localTransform); // prefill transforms with bindpose

		if ( NiTransformInterpolatorRef interp = DynamicCast<NiTransformInterpolator>((*bitr).interpolator) )
		{
			if (NiTransformDataRef data = interp->GetData())
			{
				NiQuatTransform interp_local = interp->GetTransform();
				//TODO: check these;

				if (data != NULL) {

					//translations:
					importVectorOfKeys(data->GetTranslations().keys, transforms, nbones, boneIdx, duration);

					//rotations:
					if (getFieldIsValid(&*data, NiKeyframeData::FIELDS::rotationType, _info)) {
						if (data->GetRotationType() == XYZ_ROTATION_KEY) {
							//single float groups with tangents
							importVectorOfKeys(data->GetXyzRotations(), transforms, nbones, boneIdx, duration);
						}
						else {
							//threat as quaternion?
							importVectorOfKeys(data->GetQuaternionKeys(), transforms, nbones, boneIdx, duration);
						}
					}
					else {
						importVectorOfKeys(data->GetQuaternionKeys(), transforms, nbones, boneIdx, duration);
					}
					importVectorOfKeys(data->GetScales().keys, transforms, nbones, boneIdx, duration);
				}
			}
		}
		else if (NiBSplineTransformInterpolatorRef interp = DynamicCast<NiBSplineTransformInterpolator>((*bitr).interpolator))
		{
			int npoints = GetNumControlPoints(interp);

			if (npoints > 3)
			{
				importVectorOfKeys(SampleTranslateKeys(interp, npoints, 3), transforms, nbones, boneIdx, duration);
				importVectorOfKeys(SampleQuatRotateKeys(interp, npoints, 3), transforms, nbones, boneIdx, duration);
				importVectorOfKeys(SampleScaleKeys(interp, npoints, 3), transforms, nbones, boneIdx, duration);
			}
			else
				throw runtime_error("Not enough points to calculate the spline");
		}
		else if (NiKeyBasedInterpolatorRef interp = DynamicCast<NiKeyBasedInterpolator>((*bitr).interpolator)) {
			//niboolinterpolator
			//nifloatinterpolator
			//nipathinterpolator
			//nipoint3interpolator
		}
	}

	hkaSkeletonUtils::normalizeRotations (transforms.begin(), transforms.getSize()); 

	// create the animation with default settings
	{
		hkaSplineCompressedAnimation::TrackCompressionParams tparams;
		hkaSplineCompressedAnimation::AnimationCompressionParams aparams;

		tparams.m_rotationTolerance = 0.001f;
		tparams.m_rotationQuantizationType = hkaSplineCompressedAnimation::TrackCompressionParams::THREECOMP40;

		hkRefPtr<hkaSplineCompressedAnimation> outAnim = new hkaSplineCompressedAnimation( *tempAnim.val(), tparams, aparams ); 
		binding->m_animation = outAnim;
	}
	
	return true;
}

static void ExportAnimations(const string& rootdir, const string& skelfile
                      , const vector<string>& animlist, const string& outdir
                      , hkPackFormat pkFormat, const hkPackfileWriter::Options& packFileOptions
                      , hkSerializeUtil::SaveOptionBits flags
                      , bool norelativepath = false)
{
	hkResource* skelResource = NULL;
	hkResource* animResource = NULL;
	hkaSkeleton* skeleton = NULL;

	Log::Verbose("ExportAnimation('%s','%s','%s')", rootdir.c_str(), skelfile.c_str(), outdir.c_str());
	// Read back a serialized file
	{
		hkIstream stream(skelfile.c_str());
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
		for (vector<string>::const_iterator itr = animlist.begin(); itr != animlist.end(); ++itr)
		{
			string animfile = (*itr);
			Log::Verbose("ExportAnimation Starting '%s'", animfile.c_str());


			char outfile[MAX_PATH], relout[MAX_PATH];
			LPCSTR extn = PathFindExtension(outdir.c_str());
			if (stricmp(extn, ".hkx") == 0)
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
						hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
						if ( res != HK_SUCCESS )
						{
							Log::Error("Havok reports save failed.");
						}
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

static void ExportProject( const string &projfile, const char * rootPath, const char * outdir
                          , hkPackFormat pkFormat, const hkPackfileWriter::Options& packFileOptions
                          , hkSerializeUtil::SaveOptionBits flags, bool recursion)
{
	vector<string> skelfiles, animfiles;
	char projpath[MAX_PATH], skelpath[MAX_PATH], animpath[MAX_PATH];

	if ( wildmatch("*skeleton.hkx", projfile) )
	{
		skelfiles.push_back(projfile);

		GetFullPathName(projfile.c_str(), MAX_PATH, projpath, NULL);
		PathRemoveFileSpec(projpath);
		PathAddBackslash(projpath);
		PathCombine(animpath, projpath, "..\\animations\\*.hkx");
		FindFiles(animfiles, animpath, recursion);	
	}
	else
	{
		GetFullPathName(projfile.c_str(), MAX_PATH, projpath, NULL);
		PathRemoveFileSpec(projpath);
		PathAddBackslash(projpath);
		PathCombine(skelpath, projpath, "character assets\\*skeleton.hkx");
		FindFiles(skelfiles, skelpath, recursion);
		PathCombine(animpath, projpath, "animations\\*.hkx");
		FindFiles(animfiles, animpath, recursion);
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
		ExportAnimations(string(rootPath), skelfiles[0],animfiles, outdir, pkFormat, packFileOptions, flags, false);
	}
}

static void HK_CALL debugReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}


static bool ExecuteCmd(hkxcmdLine &cmdLine)
{
	bool recursion = true;
	vector<string> paths;
	int argc = cmdLine.argc;
	char **argv = cmdLine.argv;
	hkPackFormat pkFormat = HKPF_DEFAULT;
	hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;
   AnimationExport::noRootSiblings = true;

#pragma region Handle Input Args
	for (int i = 0; i < argc; i++)
	{
		char *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch (tolower(arg[1]))
			{
			case 'f':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					flags = (hkSerializeUtil::SaveOptionBits)StringToFlags(param, SaveFlags, hkSerializeUtil::SAVE_DEFAULT);
				} break;

			case 'n':
				recursion = false;
				break;

         case 's':
            AnimationExport::noRootSiblings = false;
            break;

			case 'v':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					pkFormat = (hkPackFormat)StringToEnum(param, PackFlags, HKPF_DEFAULT);
				} break;

			case 'd':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
					{
						Log::SetLogLevel(LOG_DEBUG);
						break;
					}
					else
					{
						Log::SetLogLevel((LogLevel)StringToEnum(param, LogFlags, LOG_INFO));
					}
				} break;

			case 'o':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					paths.push_back(string(param));
				}
				break;

			case 'i':
				{
					const char *param = arg+2;
					if (*param == ':' || *param=='=') ++param;
					argv[i] = NULL;
					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
						param = argv[++i];
						argv[i] = NULL;
					}
					if ( param[0] == 0 )
						break;
					paths.insert(paths.begin(), 1, string(param));
				}
				break;


			default:
				Log::Error("Unknown argument specified '%s'", arg);
				return false;
			}
		}
		else
		{
			paths.push_back(arg);
		}
	}
#pragma endregion

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
						if (PathIsDirectory(animpath.c_str()))
						{
							strcpy(rootPath, animpath.c_str());
							animpath += string("\\*.hkx");
							norelativepath = false;
						}
						vector<string> animfiles;
						FindFiles(animfiles, animpath.c_str(), recursion);

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

							ExportAnimations(string(rootPath), skelpath, animfiles, outdir, pkFormat, packFileOptions, flags, norelativepath);
						}

					}
				}
			}
		}
	}
	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();


	return true;
}
static bool SafeExecuteCmd(hkxcmdLine &cmdLine)
{
   __try{
      return ExecuteCmd(cmdLine);
   } __except (EXCEPTION_EXECUTE_HANDLER){
      return false;
   }
}

REGISTER_COMMAND(ConvertKF, HelpString, SafeExecuteCmd);
