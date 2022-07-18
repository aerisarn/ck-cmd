#pragma region Headers
//#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <map>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#pragma region Niflib Headers
//////////////////////////////////////////////////////////////////////////
// Niflib Includes
//////////////////////////////////////////////////////////////////////////
#include <niflib.h>
#include <nif_io.h>
#include <nif_basic_types.h>
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

#pragma endregion

#pragma endregion

//////////////////////////////////////////////////////////////////////////
// Our Includes
//////////////////////////////////////////////////////////////////////////
#include <float.h>
#include <cstdio>
#include <sys/stat.h>

namespace ConvertKF {

	struct AnimationExport
	{
		AnimationExport(Niflib::NiControllerSequenceRef seq, hkRefPtr<hkaSkeleton> skeleton, hkRefPtr<hkaAnimationBinding> binding, const Niflib::NifInfo& info);

		bool doExport();
		bool exportNotes();
		bool exportController();

		//bool SampleAnimation( INode * node, Interval &range, PosRotScale prs, NiKeyframeDataRef data );
		Niflib::NiControllerSequenceRef seq;
		hkRefPtr<hkaAnimationBinding> binding;
		hkRefPtr<hkaSkeleton> skeleton;
		static bool noRootSiblings;
		const Niflib::NifInfo& _info;
	};

}