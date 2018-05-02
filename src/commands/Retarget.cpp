#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/log.h>

#include <map>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

#pragma region Niflib Headers
//////////////////////////////////////////////////////////////////////////
// Niflib Includes
//////////////////////////////////////////////////////////////////////////
#include <niflib.h>
#include <nif_io.h>
#include "obj/NiObject.h"
#include "obj/NiNode.h"
#include "obj/NiTexturingProperty.h"
#include "obj/NiSourceTexture.h"
#include "obj/NiTriBasedGeom.h"
#include "obj/NiTriBasedGeomData.h"
#include "obj/NiTriShape.h"
#include "obj/NiTriStrips.h"
#include <obj/NiControllerSequence.h>
#include <obj/NiControllerManager.h>
#include <obj/NiInterpolator.h>
#include <obj/NiTransformInterpolator.h>
#include <obj/NiTransformData.h>
#include <obj/NiTransformController.h>
#include <obj/NiTimeController.h>
#include <obj/NiTransformController.h>
#include <obj/NiTextKeyExtraData.h>
#include <obj/NiKeyframeController.h>
#include <obj/NiKeyframeData.h>
#include <obj/NiStringPalette.h>
#include <obj/NiBSplineTransformInterpolator.h>
#include <obj/NiDefaultAVObjectPalette.h>
#include <obj/NiMultiTargetTransformController.h>
#include <obj/NiGeomMorpherController.h>
#include <obj/NiMorphData.h>
#include <obj/NiBSplineCompFloatInterpolator.h>
#include <obj/NiFloatInterpolator.h>
#include <obj/NiFloatData.h>
#include <Key.h>

typedef Niflib::Key<float> FloatKey;
typedef Niflib::Key<Niflib::Quaternion> QuatKey;
typedef Niflib::Key<Niflib::Vector3> Vector3Key;
typedef Niflib::Key<string> StringKey;

#pragma endregion

#pragma region Havok Headers
//////////////////////////////////////////////////////////////////////////
// Havok Includes
//////////////////////////////////////////////////////////////////////////

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>

#include <cstdio>

// Scene
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkLoader.h>

// Physics
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

// Animation
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Animation/Rig/hkaSkeletonUtils.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Animation/Deprecated/DeltaCompressed/hkaDeltaCompressedAnimation.h>
#include <Animation/Animation/Animation/SplineCompressed/hkaSplineCompressedAnimation.h>
#include <Animation/Animation/Animation/Quantized/hkaQuantizedAnimation.h>
#include <Animation/Animation/Animation/Util/hkaAdditiveAnimationUtility.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>

#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>

#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>

//#pragma comment (lib, "hkBase.lib")
//#pragma comment (lib, "hkSerialize.lib")
//#pragma comment (lib, "hkSceneData.lib")
//#pragma comment (lib, "hkInternal.lib")
//#pragma comment (lib, "hkGeometryUtilities.lib")
//#pragma comment (lib, "hkVisualize.lib")
//#pragma comment (lib, "hkCompat.lib")
//#pragma comment (lib, "hkpCollide.lib")
//#pragma comment (lib, "hkpConstraintSolver.lib")
//#pragma comment (lib, "hkpDynamics.lib")
//#pragma comment (lib, "hkpInternal.lib")
//#pragma comment (lib, "hkpUtilities.lib")
//#pragma comment (lib, "hkpVehicle.lib")
//#pragma comment (lib, "hkaAnimation.lib")
//#pragma comment (lib, "hkaRagdoll.lib")
//#pragma comment (lib, "hkaInternal.lib")
//#pragma comment (lib, "hkgBridge.lib")

#define RETURN_FAIL_IF(COND, MSG) \
	HK_MULTILINE_MACRO_BEGIN \
	if(COND) { HK_ERROR(0x53a6a026, MSG); return 1; } \
	HK_MULTILINE_MACRO_END

#pragma endregion

//////////////////////////////////////////////////////////////////////////
// Our Includes
//////////////////////////////////////////////////////////////////////////
#include <float.h>
#include <cstdio>
#include <sys/stat.h>
using namespace Niflib;
using namespace std;

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


enum hkPackFormat
{
	HKPF_XML,
	HKPF_DEFAULT,
	HKPF_WIN32,
	HKPF_AMD64,
	HKPF_XBOX,
	HKPF_XBOX360,
};


static 
EnumLookupType SaveFlags[] = 
{
	{hkSerializeUtil::SAVE_DEFAULT,                   "SAVE_DEFAULT"},
	{hkSerializeUtil::SAVE_TEXT_FORMAT,               "SAVE_TEXT_FORMAT"},
	{hkSerializeUtil::SAVE_SERIALIZE_IGNORED_MEMBERS, "SAVE_SERIALIZE_IGNORED_MEMBERS"},
	{hkSerializeUtil::SAVE_WRITE_ATTRIBUTES,          "SAVE_WRITE_ATTRIBUTES"},
	{hkSerializeUtil::SAVE_CONCISE,                   "SAVE_CONCISE"},
	{hkSerializeUtil::SAVE_TEXT_NUMBERS,              "SAVE_TEXT_NUMBERS"},
	{0, NULL}
};

static 
EnumLookupType LogFlags[] = 
{
	{LOG_NONE,   "NONE"},
	{LOG_ALL,    "ALL"},
	{LOG_VERBOSE,"VERBOSE"},
	{LOG_DEBUG,  "DEBUG"},
	{LOG_INFO,   "INFO"},
	{LOG_WARN,   "WARN"},
	{LOG_ERROR,  "ERROR"},
	{0, NULL}
};

static 
EnumLookupType PackFlags[] = 
{
	{HKPF_XML,   "XML"},
	{HKPF_DEFAULT, "DEFAULT"},
	{HKPF_WIN32, "WIN32"},
	{HKPF_AMD64, "AMD64"},
	{HKPF_XBOX,  "XBOX"},
	{HKPF_XBOX360, "XBOX360"},
	{0, NULL}
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
static hkResource* hkSerializeUtilLoad( hkStreamReader* stream
								, hkSerializeUtil::ErrorDetails* detailsOut=HK_NULL
								, const hkClassNameRegistry* classReg=HK_NULL
								, hkSerializeUtil::LoadOptions options=hkSerializeUtil::LOAD_DEFAULT )
{
	__try
	{
		return hkSerializeUtil::load(stream, detailsOut, classReg, options);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		//if (detailsOut == NULL)
		//	detailsOut->id = hkSerializeUtil::ErrorDetails::ERRORID_LOAD_FAILED;
		return NULL;
	}
}

static hkPackfileWriter::Options GetWriteOptionsFromFormat(hkPackFormat format)
{
	hkPackfileWriter::Options options;
	options.m_layout = hkStructureLayout::MsvcWin32LayoutRules;

	switch(format)
	{
	case HKPF_XML:
	case HKPF_DEFAULT:
	case HKPF_WIN32:
		options.m_layout = hkStructureLayout::MsvcWin32LayoutRules;
		break;
	case HKPF_AMD64:
		options.m_layout = hkStructureLayout::MsvcAmd64LayoutRules;
		break;
	case HKPF_XBOX:
		options.m_layout = hkStructureLayout::MsvcXboxLayoutRules;
		break;
	case HKPF_XBOX360:
		options.m_layout = hkStructureLayout::Xbox360LayoutRules;
		break;
	}
	return options;
}

static void HelpString(hkxcmd::HelpType type){
	switch (type)
	{
	case hkxcmd::htShort: Log::Info("Retarget - Convert Havok HKX animation to Gamebryo HKX animation using another skeleton"); break;
	case hkxcmd::htLong:  
		{
			char fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_splitpath(fullName, NULL, NULL, exeName, NULL);

			Log::Info("Usage: %s Retarget [-opts[modifiers]] [baseskel.hkx] [anim.hkx] [output_anim.hkx]", exeName); 
			Log::Info("  Convert Havok HKX animation to Gamebryo HKX animation using another skeleton" );
			Log::Info("");
			Log::Info("<Options>" );
			Log::Info("  skel.hkx      Path to Havok skeleton for animation binding." );
			Log::Info("  anim.kf       Path to Gamebryo animation to convert (Default: anim.hkx with kf ext)" );
			Log::Info("  anim.hkx      Path to Havok animation to write" );
			Log::Info("<Switches>" );
			Log::Info(" -d[:level]     Debug Level: ERROR,WARN,INFO,DEBUG,VERBOSE (Default: INFO)" );
			Log::Info("");
			Log::Info(" -v:<flags>     Havok Packfile saving flags");
			Log::Info("    DEFAULT     Save as Default Format (MSVC Win32 Packed)");
			Log::Info("    XML         Save as Xml Format");
			Log::Info("    WIN32       Save as Win32 Format");
			Log::Info("    AMD64       Save as AMD64 Format");
			Log::Info("    XBOX        Save as XBOX Format");
			Log::Info("    XBOX360     Save as XBOX360 Format");
			Log::Info("");
			Log::Info(" -f:<flags>     Havok saving flags (Defaults:  SAVE_DEFAULT)");
			Log::Info("    SAVE_DEFAULT           = All flags default to OFF, enable whichever are needed");
			Log::Info("    SAVE_TEXT_FORMAT       = Use text (usually XML) format, default is binary format if available.");
			Log::Info("    SAVE_SERIALIZE_IGNORED_MEMBERS = Write members which are usually ignored.");
			Log::Info("    SAVE_WRITE_ATTRIBUTES  = Include extended attributes in metadata, default is to write minimum metadata.");
			Log::Info("    SAVE_CONCISE           = Doesn't provide any extra information which would make the file easier to interpret. ");
			Log::Info("                              E.g. additionally write hex floats as text comments.");
			Log::Info("    SAVE_TEXT_NUMBERS      = Floating point numbers output as text, not as binary.  ");
			Log::Info("                              Makes them easily readable/editable, but values may not be exact.");
			Log::Info("");
		}
		break;
	}
}



//////////////////////////////////////////////////////////////////////////
// Classes
//////////////////////////////////////////////////////////////////////////

static void FillTransforms( hkArray<hkQsTransform>& transforms, int boneIdx, int nbones
						   , const hkQsTransform& localTransform, PosRotScale prs = prsDefault
						   , int from=0, int to=-1) 
{
	int n = transforms.getSize() / nbones;
	if (n == 0)
		return;

	if (to == -1 || to >= n) to = n-1;

	if ((prs & prsDefault) == prsDefault)
	{
		for (int idx = from; idx <= to; ++idx)
		{
			hkQsTransform& transform = transforms[idx*nbones + boneIdx];
			transform = localTransform;
		}
	}
	else
	{
		for (int idx = from; idx <= to; ++idx)
		{
			hkQsTransform& transform = transforms[idx*nbones + boneIdx];
			if ((prs & prsPos) != 0)
				transform.setTranslation(localTransform.getTranslation());
			if ((prs & prsRot) != 0)
				transform.setRotation(localTransform.getRotation());
			if ((prs & prsScale) != 0)
				transform.setScale(localTransform.getScale());
		}
	}
}

//////////////////////////////////////////////////////////////////////////

hkRefPtr<hkaAnimation> RetargetAnimation( hkRefPtr<hkaSkeleton> baseSkel
										 , hkRefPtr<hkaSkeleton> srcSkel
										 , hkRefPtr<hkaAnimationBinding> srcBinding 
										 ) 
{
	int nbones = baseSkel->m_bones.getSize();
	int numTracks = nbones;
	hkRefPtr<hkaAnimation> srcAnim = srcBinding->m_animation;
	float duration = srcAnim->m_duration;
	int nframes = srcAnim->getNumOriginalFrames();

	hkRefPtr<hkaInterleavedUncompressedAnimation> tempAnim = new hkaInterleavedUncompressedAnimation();
	tempAnim->m_duration = duration;
	tempAnim->m_numberOfTransformTracks = nbones;
	tempAnim->m_numberOfFloatTracks = srcAnim->m_numberOfFloatTracks;
	tempAnim->m_transforms.setSize(numTracks*nframes, hkQsTransform::getIdentity());
	tempAnim->m_floats.setSize(tempAnim->m_numberOfFloatTracks);

	hkaAnnotationTrack defaultAnnotation;
	defaultAnnotation.m_trackName="";
	tempAnim->m_annotationTracks.setSize(numTracks, defaultAnnotation);

	map<string, int> boneMap;
	for (int i=0; i<nbones; ++i)
	{
		boneMap[string(baseSkel->m_bones[i].m_name)] = i;
		//boneMap.setValue(boneMap.findOrInsertKey(baseSkel->m_bones[i].m_name, i), i);
	}

	int srcNBones = srcSkel->m_bones.getSize();
	int srcNTracks = srcAnim->m_numberOfTransformTracks;
	int srcNFloats = srcAnim->m_numberOfFloatTracks;
	hkLocalArray<int> iBoneMap(srcNBones), rBoneMap(nbones);
	iBoneMap.setSize(srcNBones, -1);
	rBoneMap.setSize(nbones, -1);
	for (int i=0;i<srcNBones;++i)
	{
		hkStringPtr skName = srcSkel->m_bones[i].m_name;
		map<string, int>::iterator itr = boneMap.find(string(skName));
		if (itr == boneMap.end())
		{
			Log::Warn("Found bone '%s' not present in base skeleton", (LPCTSTR)skName);
		}
		else
		{
			iBoneMap[i] = itr->second;
			rBoneMap[itr->second] = i;
		}
	}

	// copy annotation tracks from src to temp
	for (int i=0,n=srcAnim->m_annotationTracks.getSize(); i<n; ++i)
	{
		int idx = iBoneMap[i];
		tempAnim->m_annotationTracks[idx] = srcAnim->m_annotationTracks[i];
	}

	hkArray<hkQsTransform>& transforms = tempAnim->m_transforms;

	// Initialize transforms to base skeleton bind pose for bones missing in source
	for (int i=0; i<nbones; ++i)
	{
		int boneIdx = i;
		if (rBoneMap[boneIdx] == -1)
		{
			Log::Verbose("Defaulting transforms to bind pose for '%s'", (LPCSTR)baseSkel->m_bones[i].m_name);
			hkQsTransform localTransform = baseSkel->m_referencePose[boneIdx];
			FillTransforms(transforms, boneIdx, nbones, localTransform);
		}
	}

	// Walk through source animation and transfer animation
	hkArray<hkQsTransform> srcTransforms;
	srcTransforms.setSize(srcNTracks, hkQsTransform::getIdentity());
	hkArray<float> srcFloats;
	srcFloats.setSize(srcNFloats, 0.0f);

	hkReal incrFrame = srcAnim->m_duration / (hkReal)nframes;
	int nCurrentFrame = 0;

	hkArray<float>::iterator curFrameTrackFloats = tempAnim->m_floats.begin();
	hkArray<hkQsTransform>::iterator curFrameTrackTransforms = transforms.begin();
	for (hkReal time = 0.0f; time<srcAnim->m_duration; time += incrFrame, nCurrentFrame+=numTracks)
	{
		srcAnim->sampleTracks(time, srcTransforms.begin(), srcFloats.begin(), NULL);
		//srcAnim->samplePartialTracks(time, srcNTracks, srcTransforms.begin(), srcNFloats, srcFloats.begin(), HK_NULL);
		hkaSkeletonUtils::normalizeRotations (srcTransforms.begin(), srcNTracks);
		for (int i=0; i<srcNTracks; ++i)
		{
			int boneIdx = iBoneMap[i];
			hkArray<hkQsTransform>::iterator boneTransform = curFrameTrackTransforms + boneIdx;
			*boneTransform = srcTransforms[i];
		}
		for (int i=0; i<srcNFloats; ++i)
		{
			int boneIdx = iBoneMap[i];
			hkArray<float>::iterator floatTransform = curFrameTrackFloats + boneIdx;
			*floatTransform = srcFloats[i];
		}
		std::advance(curFrameTrackTransforms, srcNTracks);
		std::advance(curFrameTrackFloats, srcNFloats);
	}

	hkaSkeletonUtils::normalizeRotations (transforms.begin(), transforms.getSize()); 

	// create the animation with default settings
	//if (stricmp(animType, "hkaInterleavedUncompressedAnimation") == 0)	{
	//	return hkRefPtr<hkaAnimation>(tempAnim);
	//}
	//else 
	//{
	//	if (stricmp(animType, "hkaSplineCompressedAnimation") != 0)
	//	{
	//		Log::Warn("Source animation of type '%s' is being compiled to type 'hkaSplineCompressedAnimation'");
	//	}
	{
		hkaSplineCompressedAnimation::TrackCompressionParams tparams;
		hkaSplineCompressedAnimation::AnimationCompressionParams aparams;
		//tparams.m_rotationTolerance = 0.001f;
		//tparams.m_rotationQuantizationType = hkaSplineCompressedAnimation::TrackCompressionParams::THREECOMP40;
		hkRefPtr<hkaSplineCompressedAnimation> outAnim = new hkaSplineCompressedAnimation( *tempAnim.val(), tparams, aparams ); 
		return hkRefPtr<hkaAnimation>(outAnim);
	}
	return hkRefPtr<hkaAnimation>();	
}

static bool ExportFile(const char *baseSkel, const char *sourceAnim, const char *destAnim
					   , const hkPackfileWriter::Options& packFileOptions, hkSerializeUtil::SaveOptionBits flags)
{
	hkResource* baseResource = NULL;
	hkResource* srcResource = NULL;
	hkaAnimationContainer *baseAnimCont = NULL;
	hkaAnimationContainer *srcAnimCont = NULL;
	hkaSkeleton* baseSkeleton = NULL;

	hkSerializeUtil::FormatDetails detailsOut;

	Log::Verbose("Begin ExportFile");
	Log::Info("Base Skeleton: %s", baseSkel);
	Log::Info("Source File: %s", sourceAnim);
	Log::Info("Output File: %s", destAnim);

	int nErrors = 0;

	// Read back skeleton file
	{
		hkIstream stream(baseSkel);
		hkStreamReader *reader = stream.getStreamReader();
		hkBool32 isLoadable = hkSerializeUtil::isLoadable( reader );
		if (!isLoadable)
		{
			Log::Warn("Base skeleton file reports that it is not loadable");
		}
		if (baseResource = hkSerializeUtilLoad(reader))
		{
			const char * hktypename = baseResource->getContentsTypeName();
			void * contentPtr = baseResource->getContentsPointer(HK_NULL, HK_NULL);
			if (hkRootLevelContainer* scene = baseResource->getContents<hkRootLevelContainer>())
			{
				baseAnimCont = scene->findObject<hkaAnimationContainer>();
				if (baseAnimCont == NULL)
				{
					Log::Error("Base File does not contain an animation container");
					++nErrors;
				}
			}
			else
			{
				Log::Error("Base File does not a root level container");
				++nErrors;
			}
		}
		else
		{
			Log::Error("Base File could not be loaded");
			++nErrors;
		}
	}
	if (baseAnimCont != NULL)
	{
		hkIstream stream(sourceAnim);
		hkStreamReader *reader = stream.getStreamReader();

		// details will be used later when writing packfile
		hkSerializeUtil::detectFormat( reader, detailsOut );
		hkBool32 isLoadable = hkSerializeUtil::isLoadable( reader );
		if (!isLoadable)
		{
			Log::Warn("Source file reports that it is not loadable");
		}
		if (srcResource = hkSerializeUtilLoad(reader))
		{
			const char * hktypename = srcResource->getContentsTypeName();
			void * contentPtr = srcResource->getContentsPointer(HK_NULL, HK_NULL);
			if (hkRootLevelContainer* scene = srcResource->getContents<hkRootLevelContainer>())
			{
				srcAnimCont = scene->findObject<hkaAnimationContainer>();
				if (srcAnimCont == NULL)
				{
					Log::Error("Source File does not contain an animation container");
					++nErrors;
				}
			}
			else
			{
				Log::Error("Source File does not a root level container");
				++nErrors;
			}
		}
		else
		{
			Log::Error("Source File could not be loaded");
			++nErrors;
		}
	}
	if (baseAnimCont != NULL && srcAnimCont != NULL)
	{
		// Get skeleton names from the animation file
		//if (inputSkelNames.empty()) 
		//{
		//	for (int i=0,n=srcAnimCont->m_skeletons.getSize(); i<n; ++i)
		//		inputSkelNames.push_back( string(srcAnimCont->m_skeletons[i]->m_name) );
		//}
		//if (inputSkelNames.empty())
		//{
		//	Log::Error("Skeleton names not specified or found in source animation file");
		//	++nErrors;
		//}
		//else
		//{
		//	// verify skeletons are in source
		//	for (int i=0,n=baseAnimCont->m_skeletons.getSize(); i<n; ++i)
		//	{
		//		string skName = baseAnimCont->m_skeletons[i];
		//		if ( std::find(inputSkelNames.begin(), inputSkelNames.end(), skName) == inputSkelNames.end() )
		//		{
		//			Log::Error("Could not locate skeleton '%s' in Base Skeleton", skName.c_str());
		//			++nErrors;
		//			break;
		//		}
		//	}
		//}
		if (nErrors == 0)
		{
			hkRootLevelContainer rootCont;
			hkRefPtr<hkaAnimationContainer> outSkelAnimCont = new hkaAnimationContainer();
			rootCont.m_namedVariants.pushBack( hkRootLevelContainer::NamedVariant("Merged Animation Container", outSkelAnimCont.val(), &outSkelAnimCont->staticClass()) );

			int nBindings = srcAnimCont->m_bindings.getSize();
			if (nBindings== 0)
			{
				Log::Error("Source Animation does not contain any bindings.");
				++nErrors;
			}
			else
			{
				for (int idx=0; idx<nBindings && nErrors == 0; ++idx)
				{
					hkRefPtr<hkaAnimationBinding> srcBinding = srcAnimCont->m_bindings[idx];

					for (int i=0,n = srcBinding->m_transformTrackToBoneIndices.getSize();i<n;++i)
					{
						if ( srcBinding->m_transformTrackToBoneIndices[i] != i )
						{
							Log::Error("Source Animation Binding contains bone remapping which is not supported at this time.");
							++nErrors;
							break;
						}
					}
					for (int i=0,n = srcBinding->m_floatTrackToFloatSlotIndices.getSize();i<n;++i)
					{
						if ( srcBinding->m_floatTrackToFloatSlotIndices[i] != i )
						{
							Log::Error("Source Animation Binding contains float remapping which is not supported at this time.");
							++nErrors;
							break;
						}
					}
					if (nErrors > 0)
						continue;				

					hkStringPtr skName = srcBinding->m_originalSkeletonName;
					if (skName.getLength() == 0)
					{
						Log::Error("Source Animation Binding Name is empty.");
						++nErrors;
					}
					else
					{
						hkRefPtr<hkaSkeleton> baseSkeleton, srcSkeleton;
						for (int i=0,n=baseAnimCont->m_skeletons.getSize(); i<n; ++i)
						{
							hkRefPtr<hkaSkeleton> skeleton = baseAnimCont->m_skeletons[i];
							if ( skName == skeleton->m_name ) {
								baseSkeleton = skeleton;
								break;
							}
						}								
						if (baseSkeleton == NULL)
						{
							Log::Error("No skeletons found in base with name '%s'", (LPCTSTR)skName);
							++nErrors;
						}
						else
						{
							for (int i=0,n=srcAnimCont->m_skeletons.getSize(); i<n; ++i)
							{
								hkRefPtr<hkaSkeleton> skeleton = srcAnimCont->m_skeletons[i];
								if ( skName == skeleton->m_name ) {
									srcSkeleton = skeleton;
									break;
								}
							}								
							if (srcSkeleton == NULL)
							{
								Log::Error("No skeletons found in source with name '%s'", (LPCTSTR)skName);
								++nErrors;
							}
							else
							{
								hkRefPtr<hkaAnimation> outAnim = RetargetAnimation(baseSkeleton, srcSkeleton, srcBinding);
								if (outAnim == NULL)
								{
									Log::Error("Output animation not exported");
									++nErrors;
								}
								else
								{
									hkRefPtr<hkaAnimationBinding> dstBinding = new hkaAnimationBinding();
									outSkelAnimCont->m_bindings.append(&dstBinding, 1);
									dstBinding->m_originalSkeletonName = skName;
									dstBinding->m_animation = outAnim;
									dstBinding->m_blendHint = srcBinding->m_blendHint;
									outSkelAnimCont->m_animations.pushBack(outAnim);								
								}
							}
						}
					}
				}
			}

			if (nErrors == 0)
			{
				char outfiledir[MAX_PATH];
				strcpy(outfiledir, destAnim);
				PathRemoveFileSpec(outfiledir);
				CreateDirectories(outfiledir);
				Log::Info("Writing '%s'", destAnim);

				hkResult res;
				hkOstream stream(destAnim);
				if ((flags & hkSerializeUtil::SAVE_TEXT_FORMAT) == 0)
				{
					res = hkSerializeUtil::savePackfile(&rootCont, rootCont.staticClass(), stream.getStreamWriter(), packFileOptions, NULL, flags);
				}
				else
				{
					res = hkSerializeUtil::save( &rootCont, rootCont.staticClass(), stream.getStreamWriter(), flags );
				}
			}
			else
			{
				Log::Error("Export failed for '%s'", destAnim);
			}
		}
	}
	if (srcResource)
		srcResource->removeReference();
	if (baseResource)
		baseResource->removeReference();
	return (nErrors == 0);
}

static void HK_CALL errorReport(const char* msg, void* userContext)
{
	Log::Debug("%s", msg);
}

static bool ExecuteCmd(hkxcmdLine &cmdLine)
{
	bool recursion = true;
	string skelName, strBase, strSource, strDest;
	int argc = cmdLine.argc;
	char **argv = cmdLine.argv;
	hkPackFormat pkFormat = HKPF_DEFAULT;
	hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;

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

			case 'b':
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
					strBase = param;
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
					strSource = param;
				}
				break;

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
					strDest = param;
				}
				break;

			case 'n':
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
					skelName = param;
				}
				break;


			default:
				Log::Error("Unknown argument specified '%s'", arg);
				return false;
			}
		} else if (strBase.empty()) {
			strBase = arg;
		} else if (strSource.empty()) {
			strSource = arg;
		} else if (strDest.empty()) {
			strDest = arg;
		} else {
			Log::Error("Unknown argument specified '%s'", arg);
			return false;
		}
	}
#pragma endregion

	if (strBase.empty()){
		HelpString(hkxcmd::htLong);
		return false;
	}
	if (strSource.empty()){
		Log::Error("Source animation file not specified.");
		return false;
	}
	if (strDest.empty()){
		Log::Error("Destination animation file not specified.");
		return false;
	}
	if (pkFormat == HKPF_XML) // set text format to indicate xml
	{
		flags = (hkSerializeUtil::SaveOptionBits)(flags | hkSerializeUtil::SAVE_TEXT_FORMAT);
	}

	hkMallocAllocator baseMalloc;
	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( &baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024) );
	hkBaseSystem::init( memoryRouter, errorReport );

	char base[MAX_PATH], source[MAX_PATH], dest[MAX_PATH];
	GetFullPathName(strBase.c_str(), MAX_PATH, base, NULL);
	GetFullPathName(strSource.c_str(), MAX_PATH, source, NULL);
	GetFullPathName(strDest.c_str(), MAX_PATH, dest, NULL);

	//stringlist skelNames = TokenizeString(skelNames, ",.", true);
	ExportFile(base, source, dest, GetWriteOptionsFromFormat(pkFormat), flags);

	hkBaseSystem::quit();
	hkMemoryInitUtil::quit();


	return true;
}

REGISTER_COMMAND(Retarget, HelpString, ExecuteCmd);
