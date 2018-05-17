//#include "stdafx.h"
//
//#include <core/hkxcmd.h>
//#include <core/hkxutils.h>
//#include <core/hkfutils.h>
//#include <core/log.h>
//
//#include <cstdio>
//#include <sys/stat.h>
//
//#include <Common/Base/hkBase.h>
//#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
//#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
//#include <Common/Base/System/Io/IStream/hkIStream.h>
//#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>
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
//#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
//#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
//
//#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
//#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
//#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>
//
//#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
//#include <Physics\Dynamics\World\hkpPhysicsSystem.h>
//#include <Physics\Utilities\Serialize\hkpPhysicsData.h>
//
//// Animation
//#include <Animation/Animation/Rig/hkaSkeleton.h>
//#include <Animation/Animation/hkaAnimationContainer.h>
//#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
//#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
//#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
//#include <Animation/Animation/Rig/hkaPose.h>
//#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
//#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
//#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
//
//// Serialize
//#include <Common/Serialize/Util/hkSerializeUtil.h>
//
//// Niflib
//#include <niflib.h>
//#include <obj/NiObject.h>
//#include <obj/NiNode.h>
//#include <obj/bhkBlendCollisionObject.h>
//#include <obj/NiControllerSequence.h>
//#include <obj/NiStringPalette.h>
//#include <obj/NiTriStrips.h>
//#include <obj/NiStringExtraData.h>
//
//#include <obj/bhkShape.h>
//#include <obj/bhkSphereShape.h>
//#include <obj/bhkCapsuleShape.h>
//#include <obj/bhkRigidBody.h>
//#include <obj/bhkConstraint.h>
//#include <obj/bhkBallAndSocketConstraint.h>
//#include <obj/bhkBallSocketConstraintChain.h>
//#include <obj/bhkBreakableConstraint.h>
//#include <obj/bhkMalleableConstraint.h>
//#include <obj/bhkPhantom.h>
//#include <obj/bhkRagdollConstraint.h>
//#include <obj/bhkRagdollSystem.h>
//#include <obj/bhkPrismaticConstraint.h>
//#include <obj/bhkHingeConstraint.h>
//#include <obj/bhkLimitedHingeConstraint.h>
//
//#include <bs\AnimDataFile.h>
//#include <bs\AnimSetDataFile.h>
//
//#include <experimental/filesystem>
//
//#include <hkbProjectData_2.h>
//#include <hkbCharacterData_7.h>
//#include <hkbClipGenerator_2.h>
//#include <hkbBehaviorReferenceGenerator_0.h>
//#include <Common/Base/Container/String/hkStringBuf.h>
//
//// GAME
//#include <core/games.h>
//
//// BSA
//#include <core/bsa.h>
//
////
////// FBX
//#include <fbxsdk.h>
//#include <core/FBXCommon.h> // samples common path, todo better way
////
//#include <core/MathHelper.h>
//#include <core/EulerAngles.h>
////
////// FBX Function prototypes.
//bool CreateScene(FbxManager* pSdkManager, FbxScene* pScene, AnimData::ClipMovementData& block); // create FBX scene
//FbxNode* CreateSkeleton(FbxScene* pScene, const char* pName); // create the actual skeleton
//void AnimateSkeleton(FbxScene* pScene, FbxNode* pSkeletonRoot, AnimData::ClipMovementData& block); // add animation to it
////
//// global so we can access this later
//class hkLoader* m_loader;
//class hkaSkeleton* m_skeleton;
//class hkaAnimation* m_animation;
//class hkaAnimationBinding* m_binding;
//
//bool bAnimationGiven = false;
//
//using namespace std;
//
//static void HelpString(hkxcmd::HelpType type){
//	switch (type)
//	{
//	case hkxcmd::htShort: Log::Info("Skeleton - Dump the transform or float list for a given skeleton"); break;
//	case hkxcmd::htLong:  
//		{
//			char fullName[MAX_PATH], exeName[MAX_PATH];
//			GetModuleFileName(NULL, fullName, MAX_PATH);
//			_splitpath(fullName, NULL, NULL, exeName, NULL);
//			Log::Info("Usage: %s DumpList [-opts[modifiers]] [infile] [outfile]", exeName);
//			Log::Info("  Dump the transform or float list for a given skeleton.");
//			Log::Info("    This is useful when exporting animation to get bone list synchronized with source.");
//			Log::Info("");
//			Log::Info("<Switches>");
//			Log::Info(" -i <path>          Input File or directory");
//			Log::Info(" -o <path>          Output File - Defaults to input file with '-out' appended");
//			Log::Info("");
//		}
//		break;
//	}
//}
//
//namespace fs = std::experimental::filesystem;
//
//static std::vector<fs::path> findFile(const std::experimental::filesystem::path& dir, const std::string& filename) {
//	std::vector<fs::path> foundPaths;
//	std::string to_lower_name = filename;
//	std::transform(to_lower_name.begin(), to_lower_name.end(), to_lower_name.begin(), ::tolower);
//	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(dir))
//	{
//		if (fs::is_directory(dirEntry.path()))
//			continue;
//		string current_filename = dirEntry.path().filename().replace_extension().string();
//		std::transform(current_filename.begin(), current_filename.end(), current_filename.begin(), ::tolower);
//		if (current_filename == to_lower_name) {
//			//check if in the upper folder there'a anothe file named as this
//			foundPaths.push_back(dirEntry.path());
//		}
//	}
//	return foundPaths;
//}
//
//static inline hkRootLevelContainer* loadHavokFile(std::string path) {
//	try {
//		hkIstream stream(path.c_str());
//		hkStreamReader *reader = stream.getStreamReader();
//		hkResource* skelResource = hkSerializeLoadResource(reader);
//
//		hkRootLevelContainer* container = skelResource->getContents<hkRootLevelContainer>();
//		HK_ASSERT2(0x27343437, container != HK_NULL, "Could not load asset");
//		return container;
//	}
//	catch (...) {
//		Log::Error("Can't read file: %s", path);
//	}
//}
//
//static inline hkRootLevelContainer* loadHavokFile(std::string path, hkArray<hkVariant>& obj) {
//	try {
//		hkIstream stream(path.c_str());
//		hkStreamReader *reader = stream.getStreamReader();
//		hkResource* skelResource = hkSerializeLoadResource(reader, obj);
//
//		hkRootLevelContainer* container = skelResource->getContents<hkRootLevelContainer>();
//		HK_ASSERT2(0x27343437, container != HK_NULL, "Could not load asset");
//		return container;
//	}
//	catch (...) {
//		Log::Error("Can't read file: %s", path);
//	}
//}
//
//static fs::path getShortestPath(std::vector<fs::path>& paths) {
//	if (paths.empty())
//		return fs::path();
//	fs::path returnPath = paths[0];
//	for (fs::path p : paths) {
//		if (p.string().size() < returnPath.string().size())
//			returnPath = p;
//	}
//	return returnPath;
//}
//
//typedef map<string, hkRootLevelContainer*> containers_map_t;
//
//static hkRootLevelContainer* getFile(containers_map_t& loaded_containers, string file)
//{
//	containers_map_t::const_iterator container = loaded_containers.find(file);
//	if (container == loaded_containers.end()) {
//		loaded_containers[file] = loadHavokFile(file.c_str());
//		return loaded_containers[file];
//	}
//	return loaded_containers[file];
//}
//
//bool CreateScene(FbxManager *pSdkManager, FbxScene* pScene, AnimData::ClipMovementData& block)
//{
//	// create scene info
//	FbxDocumentInfo* sceneInfo = FbxDocumentInfo::Create(pSdkManager, "SceneInfo");
//	sceneInfo->mTitle = "Converted Havok File";
//	sceneInfo->mSubject = "A file converted from Havok formats to FBX using havok2fbx.";
//	sceneInfo->mAuthor = "havok2fbx";
//	sceneInfo->mRevision = "rev. 1.0";
//	sceneInfo->mKeywords = "havok animation";
//	sceneInfo->mComment = "no particular comments required.";
//
//	FbxAxisSystem directXAxisSys(FbxAxisSystem::EUpVector::eZAxis, FbxAxisSystem::EFrontVector::eParityEven, FbxAxisSystem::eRightHanded);
//	directXAxisSys.ConvertScene(pScene);
//
//	// we need to add the sceneInfo before calling AddThumbNailToScene because
//	// that function is asking the scene for the sceneInfo.
//	pScene->SetSceneInfo(sceneInfo);
//	FbxNode* lSkeletonRoot = CreateSkeleton(pScene, "Skeleton");
//
//	// Build the node tree.
//	FbxNode* lRootNode = pScene->GetRootNode();
//	lRootNode->AddChild(lSkeletonRoot);
//
//	// Animation only if specified
//	if (bAnimationGiven)
//	{
//		AnimateSkeleton(pScene, lSkeletonRoot, block);
//	}
//
//	return true;
//}
//
//// Utility to make sure we always return the right index for the given node
//// Very handy for skeleton hierachy work in the FBX SDK
//FbxNode* GetNodeIndexByName(FbxScene* pScene, std::string NodeName)
//{
//	// temp hacky
//	FbxNode* NodeToReturn = FbxNode::Create(pScene, "empty");
//
//	for (int i = 0; i < pScene->GetNodeCount(); i++)
//	{
//		std::string CurrentNodeName = pScene->GetNode(i)->GetName();
//
//		if (CurrentNodeName == NodeName)
//		{
//			//std::cout << "FOUND BONE AND ITS VALID!!!!!" << "\n";
//			NodeToReturn = pScene->GetNode(i);
//		}
//	}
//
//	return NodeToReturn;
//}
//
//int GetNodeIDByName(FbxScene* pScene, std::string NodeName)
//{
//	int NodeNumber = 0;
//
//	for (int i = 0; i < pScene->GetNodeCount(); i++)
//	{
//		std::string CurrentNodeName = pScene->GetNode(i)->GetName();
//
//		if (CurrentNodeName == NodeName)
//		{
//			NodeNumber = i;
//		}
//	}
//
//	return NodeNumber;
//}
//
//// Create the skeleton first
//FbxNode* CreateSkeleton(FbxScene* pScene, const char* pName)
//{
//	// get number of bones and apply reference pose
//	const int numBones = m_skeleton->m_bones.getSize();
//	const int numFrames = m_skeleton->m_localFrames.getSize();
//	const int numFloats = m_skeleton->m_floatSlots.getSize();
//
//	std::cout << "\nSkeleton file has been loaded!" << " Number of Bones: " << numBones << "\n";
//
//	// create base limb objects first
//	for (hkInt16 b = 0; b < numBones; b++)
//	{
//		const hkaBone& bone = m_skeleton->m_bones[b];
//
//		hkQsTransform localTransform = m_skeleton->m_referencePose[b];
//		const hkVector4& pos = localTransform.getTranslation();
//		const hkQuaternion& rot = localTransform.getRotation();
//
//		FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene, pName);
//
//		if ((b == 0))
//			lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eRoot);
//		else
//			lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);
//
//		lSkeletonLimbNodeAttribute1->Size.Set(1.0);
//		FbxNode* BaseJoint = FbxNode::Create(pScene, bone.m_name);
//		BaseJoint->SetNodeAttribute(lSkeletonLimbNodeAttribute1);
//
//		// Set Translation
//		BaseJoint->LclTranslation.Set(FbxVector4(pos.getSimdAt(0), pos.getSimdAt(1), pos.getSimdAt(2)));
//
//		// convert quat to euler
//		Quat QuatTest = { rot.m_vec.getSimdAt(0), rot.m_vec.getSimdAt(1), rot.m_vec.getSimdAt(2), rot.m_vec.getSimdAt(3) };
//		EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
//		BaseJoint->LclRotation.Set(FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z)));
//
//		pScene->GetRootNode()->AddChild(BaseJoint);
//	}
//
//	// process parenting and transform now
//	for (int c = 0; c < numBones; c++)
//	{
//		const hkInt32& parent = m_skeleton->m_parentIndices[c];
//
//		if (parent != -1)
//		{
//			const char* ParentBoneName = m_skeleton->m_bones[parent].m_name;
//			const char* CurrentBoneName = m_skeleton->m_bones[c].m_name;
//			std::string CurBoneNameString = CurrentBoneName;
//			std::string ParentBoneNameString = ParentBoneName;
//
//			FbxNode* ParentJointNode = GetNodeIndexByName(pScene, ParentBoneName);
//			FbxNode* CurrentJointNode = GetNodeIndexByName(pScene, CurrentBoneName);
//			ParentJointNode->AddChild(CurrentJointNode);
//		}
//	}
//
//	//processing floats
//	for (int f = 0; f < numFloats; f++)
//	{
//		const char* floatSlot = m_skeleton->m_floatSlots[f];
//		char buffer[100] = { 0 };
//		int number_base = 10;
//		std::string output = "float_track_" + std::string(itoa(f, buffer, number_base));
//		FbxProperty floatProperty = FbxProperty::Create(pScene->GetRootNode(), FbxGetDataTypeFromEnum(eFbxFloat), output.c_str(), floatSlot);
//		floatProperty.Set(m_skeleton->m_referenceFloats[f]);
//	}
//
//	return pScene->GetRootNode();
//}
//
//// Create animation stack.
//void AnimateSkeleton(FbxScene* pScene, FbxNode* pSkeletonRoot, AnimData::ClipMovementData& block)
//{
//	FbxString lAnimStackName;
//	FbxTime lTime;
//	int lKeyIndex = 0;
//
//	// First animation stack
//	// TODO: add support for reading in multipile havok anims into a single FBX container
//	lAnimStackName = "HavokAnimation";
//	FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, lAnimStackName);
//
//	// The animation nodes can only exist on AnimLayers therefore it is mandatory to
//	// add at least one AnimLayer to the AnimStack. And for the purpose of this example,
//	// one layer is all we need.
//	FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(pScene, "Base Layer");
//	lAnimStack->AddMember(lAnimLayer);
//
//	// havok related animation stuff now
//	const int numBones = m_skeleton->m_bones.getSize();
//
//	int FrameNumber = m_animation->getNumOriginalFrames();
//	int TrackNumber = m_animation->m_numberOfTransformTracks;
//	int FloatNumber = m_animation->m_numberOfFloatTracks;
//
//	float AnimDuration = m_animation->m_duration;
//	hkReal incrFrame = m_animation->m_duration / (hkReal)(FrameNumber - 1);
//
//	/* don't need this right now
//	if (FloatNumber == 0)
//	{
//	FBXSDK_printf("\nERROR: Number of tracks is zero inside animation! Aborting.\n");
//	return;
//	}
//	*/
//	// dont know how to deal with this
//	// TODO: better way to detect this?
//	if (TrackNumber > numBones)
//	{
//		FBXSDK_printf("\nERROR: Number of tracks is not equal to bones\n");
//		return;
//	}
//
//	hkLocalArray<float> floatsOut(FloatNumber);
//	hkLocalArray<hkQsTransform> transformOut(TrackNumber);
//	floatsOut.setSize(FloatNumber);
//	transformOut.setSize(TrackNumber);
//	hkReal startTime = 0.0;
//
//	hkArray<hkInt16> tracks;
//	tracks.setSize(TrackNumber);
//	for (int i = 0; i<TrackNumber; ++i) tracks[i] = i;
//
//	hkReal time = startTime;
//
//	FbxAnimCurve* lCurve_Trans_X;
//	FbxAnimCurve* lCurve_Trans_Y;
//	FbxAnimCurve* lCurve_Trans_Z;
//	FbxAnimCurve* lCurve_Rot_X;
//	FbxAnimCurve* lCurve_Rot_Y;
//	FbxAnimCurve* lCurve_Rot_Z;
//
//	// used to store the bone id used inside the FBX scene file
//	int* BoneIDContainer;
//	BoneIDContainer = new int[numBones];
//	std::map<int, FbxProperty> floatMap;
//
//
//	// store IDs once to cut down process time
//	// TODO utilize for skeleton code aswell
//	for (int y = 0; y < numBones; y++)
//	{
//		const char* CurrentBoneName = m_skeleton->m_bones[y].m_name;
//		std::string CurBoneNameString = CurrentBoneName;
//		BoneIDContainer[y] = GetNodeIDByName(pScene, CurrentBoneName);
//
//		std::cout << "\n Bone:" << CurBoneNameString << " ID: " << BoneIDContainer[y] << "\n";
//	}
//
//	for (int y = 0; y < FloatNumber; y++) {
//
//		int float_index = m_binding->m_floatTrackToFloatSlotIndices[y];
//		char buffer[100] = { 0 };
//		int number_base = 10;
//		std::string output = "float_track_" + std::string(itoa(float_index, buffer, number_base));
//		FbxProperty floatProperty = pSkeletonRoot->FindProperty(output.c_str());
//		if (!floatProperty.IsValid())
//			throw std::runtime_error("Invalid Float Track: " + output);
//		floatProperty.ModifyFlag(FbxPropertyFlags::EFlags::eAnimatable, true);
//		FbxAnimCurveNode* floatTrack = lAnimLayer->CreateCurveNode(floatProperty);
//		if (floatTrack == NULL)
//			throw std::runtime_error("Unable to create curve for property: " + output);
//		floatMap[y] = floatProperty;
//	}
//
//	// loop through keyframes
//	for (int iFrame = 0; iFrame<FrameNumber; ++iFrame, time += incrFrame)
//	{
//		m_animation->samplePartialTracks(time, TrackNumber, transformOut.begin(), FloatNumber, floatsOut.begin(), NULL);
//		hkaSkeletonUtils::normalizeRotations(transformOut.begin(), TrackNumber);
//
//		for (int i = 0; i < FloatNumber; ++i) {
//			FbxAnimCurve* track = NULL;
//			if (iFrame == 0) {
//				track = floatMap[i].GetCurve(lAnimLayer, true);
//			}
//			else {
//				track = floatMap[i].GetCurve(lAnimLayer, false);
//			}
//
//			// todo support for anything else beside 30 fps?
//			lTime.SetTime(0, 0, 0, iFrame, 0, 0, lTime.eFrames30);
//			track->KeyModifyBegin();
//			lKeyIndex = track->KeyAdd(lTime);
//			track->KeySetValue(lKeyIndex, floatsOut[i]);
//			track->KeyModifyEnd();
//		}
//
//		// assume 1-to-1 transforms
//		// loop through animated bones
//		for (int i = 0; i<TrackNumber; ++i)
//		{
//			//const char* CurrentBoneName = m_skeleton->m_bones[i].m_name;
//			//std::string CurBoneNameString = CurrentBoneName;
//			FbxNode* CurrentJointNode = pScene->GetNode(BoneIDContainer[i]);
//
//			// create curves on frame zero otherwise just get them
//			if (iFrame == 0)
//			{
//				// Translation
//				lCurve_Trans_X = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
//				lCurve_Trans_Y = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
//				lCurve_Trans_Z = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
//
//				// Rotation
//				lCurve_Rot_X = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
//				lCurve_Rot_Y = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
//				lCurve_Rot_Z = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
//			}
//			else
//			{
//				// Translation
//				lCurve_Trans_X = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
//				lCurve_Trans_Y = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
//				lCurve_Trans_Z = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
//
//				// Rotation
//				lCurve_Rot_X = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
//				lCurve_Rot_Y = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
//				lCurve_Rot_Z = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
//			}
//
//			hkQsTransform& transform = transformOut[i];
//			const hkVector4& anim_pos = transform.getTranslation();
//			const hkQuaternion& anim_rot = transform.getRotation();
//
//			// todo support for anything else beside 30 fps?
//			lTime.SetTime(0, 0, 0, iFrame, 0, 0, lTime.eFrames30);
//
//			if (i == 0) {
//				//Extract from block
//				//block.getMovementData()
//			}
//
//			// Translation first
//			lCurve_Trans_X->KeyModifyBegin();
//			lKeyIndex = lCurve_Trans_X->KeyAdd(lTime);
//			lCurve_Trans_X->KeySetValue(lKeyIndex, anim_pos.getSimdAt(0));
//			lCurve_Trans_X->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
//			lCurve_Trans_X->KeyModifyEnd();
//
//			lCurve_Trans_Y->KeyModifyBegin();
//			lKeyIndex = lCurve_Trans_Y->KeyAdd(lTime);
//			lCurve_Trans_Y->KeySetValue(lKeyIndex, anim_pos.getSimdAt(1));
//			lCurve_Trans_Y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
//			lCurve_Trans_Y->KeyModifyEnd();
//
//			lCurve_Trans_Z->KeyModifyBegin();
//			lKeyIndex = lCurve_Trans_Z->KeyAdd(lTime);
//			lCurve_Trans_Z->KeySetValue(lKeyIndex, anim_pos.getSimdAt(2));
//			lCurve_Trans_Z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
//			lCurve_Trans_Z->KeyModifyEnd();
//
//			// Rotation
//			Quat QuatRotNew = { anim_rot.m_vec.getSimdAt(0), anim_rot.m_vec.getSimdAt(1), anim_rot.m_vec.getSimdAt(2), anim_rot.m_vec.getSimdAt(3) };
//			EulerAngles inAngs_Animation = Eul_FromQuat(QuatRotNew, EulOrdXYZs);
//
//			lCurve_Rot_X->KeyModifyBegin();
//			lKeyIndex = lCurve_Rot_X->KeyAdd(lTime);
//			lCurve_Rot_X->KeySetValue(lKeyIndex, float(rad2deg(inAngs_Animation.x)));
//			lCurve_Rot_X->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
//			lCurve_Rot_X->KeyModifyEnd();
//
//			lCurve_Rot_Y->KeyModifyBegin();
//			lKeyIndex = lCurve_Rot_Y->KeyAdd(lTime);
//			lCurve_Rot_Y->KeySetValue(lKeyIndex, float(rad2deg(inAngs_Animation.y)));
//			lCurve_Rot_Y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
//			lCurve_Rot_Y->KeyModifyEnd();
//
//			lCurve_Rot_Z->KeyModifyBegin();
//			lKeyIndex = lCurve_Rot_Z->KeyAdd(lTime);
//			lCurve_Rot_Z->KeySetValue(lKeyIndex, float(rad2deg(inAngs_Animation.z)));
//			lCurve_Rot_Z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
//			lCurve_Rot_Z->KeyModifyEnd();
//		}
//	}
//}
//
//void convertToFBX(hkaSkeleton* skeleton, hkRootLevelContainer* animationContainer, const char* fbxfile, 
//	AnimData::ClipMovementData& block) {
//	{
//		// load skeleton first!
//		//m_loader = new hkLoader();
//
//		//{
//		//	hkIstream stream(havokskeleton);
//		//	hkStreamReader *reader = stream.getStreamReader();
//		//	hkResource* skelResource = hkSerializeLoadResource(reader);
//
//		//	/*hkStringBuf assetFile(havokskeleton); hkAssetManagementUtil::getFilePath(assetFile);
//		//	hkRootLevelContainer* container = m_loader->load( HK_GET_DEMOS_ASSET_FILENAME(assetFile.cString()) );
//		//	*/
//		//	hkRootLevelContainer* container = skelResource->getContents<hkRootLevelContainer>();
//		//	HK_ASSERT2(0x27343437, container != HK_NULL, "Could not load asset");
//		//	hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>(container->findObjectByType(hkaAnimationContainerClass.getName()));
//
//		//	HK_ASSERT2(0x27343435, ac && (ac->m_skeletons.getSize() > 0), "No skeleton loaded");
//			m_skeleton = skeleton;
//		//}
//
//		// if we do not have any animation specified proceed to exporting the skeleton data otherwise use animation
//		// Get the animation and the binding
//		//if (bAnimationGiven)
//		//{
//		//	{
//		//		hkStringBuf assetFile(havokanim); hkAssetManagementUtil::getFilePath(assetFile);
//
//		//		hkRootLevelContainer* container = m_loader->load(HK_GET_DEMOS_ASSET_FILENAME(assetFile.cString()));
//		//		HK_ASSERT2(0x27343437, container != HK_NULL, "Could not load asset");
//				hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>(animationContainer->findObjectByType(hkaAnimationContainerClass.getName()));
//
//				HK_ASSERT2(0x27343435, ac && (ac->m_animations.getSize() > 0), "No animation loaded");
//				m_animation = ac->m_animations[0];
//
//				HK_ASSERT2(0x27343435, ac && (ac->m_bindings.getSize() > 0), "No binding loaded");
//				m_binding = ac->m_bindings[0];
//		//	}
//		//}
//
//		// todo delete stuff after usage
//	}
//
//	// after gathering havok data, write the stuff now into a FBX
//	// INIT FBX
//	FbxManager* lSdkManager = NULL;
//	FbxScene* lScene = NULL;
//	bool lResult;
//
//	// Prepare the FBX SDK.
//	InitializeSdkObjects(lSdkManager, lScene);
//
//	// Create the scene.
//	lResult = CreateScene(lSdkManager, lScene, block);
//	lScene->GetGlobalSettings().SetSystemUnit(FbxSystemUnit(2.54, 1.0));
//	FbxAxisSystem maxSystem(FbxAxisSystem::EUpVector::eZAxis, (FbxAxisSystem::EFrontVector) - 2, FbxAxisSystem::ECoordSystem::eRightHanded);
//	lScene->GetGlobalSettings().SetAxisSystem(maxSystem);
//	//max.ConvertScene(lScene); // we desire to convert the scene from Y-Up to Z-Up
//
//
//	if (lResult == false)
//	{
//		FBXSDK_printf("\n\nAn error occurred while creating the scene...\n");
//		//DestroySdkObjects(lSdkManager, lResult);
//		return;
//	}
//
//	// Save the scene to FBX.
//	// check if the user has given a FBX filename/path if not use the same location as the havok input
//	// The example can take an output file name as an argument.
//	const char* lSampleFileName = NULL;
//	std::string fbx_extension = ".fbx";
//	std::string combined_path;
//
//	//if (fbxfile != NULL)
//	//{
//		lSampleFileName = fbxfile;
//	//}
//	//else
//	//{
//	//	// get havok skel path and trim the extension from it		
//	//	combined_path = remove_extension(havok_path_backup) + fbx_extension;
//	//	lSampleFileName = combined_path.c_str();
//
//	//	std::cout << "\n" << "Saving FBX to: " << lSampleFileName << "\n";
//	//}
//
//	FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");
//	lExporter->SetFileExportVersion(FBX_2012_00_COMPATIBLE, FbxSceneRenamer::ERenamingMode::eNone);
//	lExporter->Initialize(lSampleFileName, -1, lSdkManager->GetIOSettings());
//	lExporter->Export(lScene);
//	lExporter->Destroy();
//
//	lResult = SaveScene(lSdkManager, lScene, lSampleFileName);
//
//	if (lResult == false)
//	{
//		FBXSDK_printf("\n\nAn error occurred while saving the scene...\n");
//		DestroySdkObjects(lSdkManager, lResult);
//		return;
//	}
//
//	// Destroy all objects created by the FBX SDK.
//	DestroySdkObjects(lSdkManager, lResult);
//}
//
//static void findGenerators(set<hkbClipGenerator*>& generators, fs::path behavior_file, const fs::path project_path) {
//	hkArray<hkVariant> objs;
//
//	hkRootLevelContainer* container = loadHavokFile(behavior_file.string().c_str(), objs);
//	for (hkVariant v : objs) {
//		if (v.m_class->getName() == hkbClipGeneratorClass.getName()) {
//			hkbClipGenerator* this_clip = (hkbClipGenerator*)v.m_object;
//			bool found = false;
//			for (hkbClipGenerator* g : generators)
//				if (g->m_name == this_clip->m_name) {
//					found = true;
//					break;
//				}
//			if (!found)
//				generators.insert((hkbClipGenerator*)v.m_object);
//		}
//		if (v.m_class->getName() == hkbBehaviorReferenceGeneratorClass.getName()) {
//			hkbBehaviorReferenceGenerator* behaviorReference = (hkbBehaviorReferenceGenerator*)v.m_object;
//			fs::path relative_behavior_path = fs::path((const char*)behaviorReference->m_behaviorName);
//			fs::path inner_behavior_file = project_path / relative_behavior_path;
//			if (!fs::exists(inner_behavior_file))
//				throw runtime_error("Missing behavior file!");
//			findGenerators(generators, inner_behavior_file, project_path);
//		}
//	}
//}
//
//static bool ExecuteCmd(hkxcmdLine &cmdLine)
//{
//	string inpath;
//	string outpath;
//	int argc = cmdLine.argc;
//	char **argv = cmdLine.argv;
//	hkSerializeUtil::SaveOptionBits flags = (hkSerializeUtil::SaveOptionBits)(hkSerializeUtil::SAVE_TEXT_FORMAT|hkSerializeUtil::SAVE_TEXT_NUMBERS);
//
//#pragma region Handle Input Args
//	for (int i = 0; i < argc; i++)
//	{
//		char *arg = argv[i];
//		if (arg == NULL)
//			continue;
//		if (arg[0] == '-' || arg[0] == '/')
//		{
//
//			switch (tolower(arg[1]))
//			{
//			case 'f':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//						break;
//					flags = (hkSerializeUtil::SaveOptionBits)StringToFlags(param, SaveFlags, hkSerializeUtil::SAVE_DEFAULT);
//				} break;
//
//			case 'd':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//					{
//						Log::SetLogLevel(LOG_DEBUG);
//						break;
//					}
//					else
//					{
//						Log::SetLogLevel((LogLevel)StringToEnum(param, LogFlags, LOG_INFO));
//					}
//				} break;
//
//			case 'o':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//						break;
//					if (outpath.empty())
//					{
//						outpath = param;
//					}
//					else
//					{
//						Log::Error("Output file already specified as '%s'", outpath.c_str());
//					}
//				} break;
//
//			case 'i':
//				{
//					const char *param = arg+2;
//					if (*param == ':' || *param=='=') ++param;
//					argv[i] = NULL;
//					if ( param[0] == 0 && ( i+1<argc && ( argv[i+1][0] != '-' || argv[i+1][0] != '/' ) ) ) {
//						param = argv[++i];
//						argv[i] = NULL;
//					}
//					if ( param[0] == 0 )
//						break;
//					if (inpath.empty())
//					{
//						inpath = param;
//					}
//					else
//					{
//						Log::Error("Input file already specified as '%s'", inpath.c_str());
//					}
//				} break;
//
//			default:
//				Log::Error("Unknown argument specified '%s'", arg);
//				break;
//			}
//		}
//		else if (inpath.empty())
//		{
//			inpath = arg;
//		}
//		else if (outpath.empty())
//		{
//			outpath = arg;
//		}
//	}
//#pragma endregion
//
//	//read off bsa
//
//	hkMallocAllocator baseMalloc;
//	hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault(&baseMalloc, hkMemorySystem::FrameInfo(1024 * 1024));
//	hkBaseSystem::init(memoryRouter, errorReport);
//
//	std::experimental::filesystem::path bsa_extracted_path("D:\\skywind\\3dsMax\\import\\Skyrim\\havok_bin\\meshes");
//
//	std::ifstream t("D:\\skywind\\3dsMax\\import\\Skyrim\\meshes\\animationdatasinglefile.txt");
//	std::string str;
//
//	t.seekg(0, std::ios::end);
//	str.reserve(t.tellg());
//	t.seekg(0, std::ios::beg);
//
//	str.assign((std::istreambuf_iterator<char>(t)),
//		std::istreambuf_iterator<char>());
//
//	AnimData::AnimDataFile f;
//	f.parse(str);
//
//	std::ifstream t1("D:\\skywind\\3dsMax\\import\\Skyrim\\meshes\\animationsetdatasinglefile.txt");
//	std::string str1;
//
//	t1.seekg(0, std::ios::end);
//	str1.reserve(t1.tellg());
//	t1.seekg(0, std::ios::beg);
//
//	str1.assign((std::istreambuf_iterator<char>(t1)),
//		std::istreambuf_iterator<char>());
//
//	AnimData::AnimSetDataFile f1;
//	f1.parse(str1);
//
//	AnimData::StringListBlock& projectList = f.getProjectList();
//	set<string> projectSet;
//	for (string p : projectList.getStrings()) {
//		std::string current_cached_project = p;
//		std::transform(current_cached_project.begin(), current_cached_project.end(), current_cached_project.begin(), ::tolower);
//		projectSet.insert(current_cached_project);
//	}
//
//	std::cerr << "Found " << projectList.getStrings().size() << " Havok Skyrim Prokects" << std::endl;
//	int index = 0;
//
//	//Search assets:
//	stringlist excludes;
//	stringlist includes;
//
//	includes.push_back("*.hkx");
//	vector<string> hkxprojects;
//	set<hkbProjectData*> projects;
//	set<hkbCharacterData*> characters;
//	containers_map_t hkx_files;
//
//	for (auto& dirEntry : std::experimental::filesystem::recursive_directory_iterator(bsa_extracted_path))
//	{
//		if (fs::is_directory(dirEntry.path()))
//			continue;
//		std::string extension = dirEntry.path().extension().string();
//		if (extension == ".hkx") {
//			hkxprojects.push_back(dirEntry.path().string());
//		}
//	}
//	int projects_count = 0;
//	int projects_in_list = 0;
//	int missing_characters = 0;
//	//FindFiles(hkxprojects, bsa_extracted_path.string().c_str(), excludes, true, includes);
//	for (vector<string>::iterator itr = hkxprojects.begin(); itr != hkxprojects.end(); ++itr)
//	{
//		Log::Info("Found HKX file: %s\n", itr->c_str());
//		hkArray<hkVariant> obj;
////		container->findObjectByType(hkbProjectDataClass.getName()
//		hkbProjectData* project = getFile(hkx_files, itr->c_str())->findObject<hkbProjectData>();
//		AnimData::ProjectDataBlock projectMovementData;
//		if (project != HK_NULL) {
//			Log::Info("[Project File: %s\n", fs::path(itr->c_str()).filename().string().c_str());
//			string tk_name = fs::path(itr->c_str()).filename().replace_extension().string();
//			if (projectSet.find(tk_name + ".txt")== projectSet.end()) {
//				Log::Info("Unknown projectname: ", tk_name);
//				//throw runtime_error("Unknown projectname: " + tk_name);
//			}
//			else {
//				//find movement data
//				
//			}
//			projects_count++;
//			projects.insert(project);
//			for (hkStringPtr cpath : project->m_stringData->m_characterFilenames) {
//				Log::Info("[Character File: %s\n", cpath);
//				fs::path relative_char_path = fs::path((const char*)cpath);
//				fs::path character_file = fs::path(itr->c_str()).remove_filename() / relative_char_path;
//				if (!fs::exists(character_file)) {
//					Log::Info("Unknown character_file: ", tk_name);
//					missing_characters++;
//				}
//				else {
//					Log::Info("[Character File: %s\n", character_file.filename().string().c_str());
//					hkbCharacterData* character = loadHavokFile(character_file.string().c_str())->findObject<hkbCharacterData>();
//					if (character != HK_NULL) {
//						fs::path project_path = fs::path(itr->c_str()).remove_filename();
//						Log::Info("Character: %s\n", character->m_stringData->m_name);
//						characters.insert(character);
//						Log::Info("Rig: %s\n", character->m_stringData->m_rigName);
//						fs::path relative_rig_path = fs::path((const char*)character->m_stringData->m_rigName);
//						fs::path rig_file = project_path / relative_rig_path;
//						if (!fs::exists(rig_file))
//							throw runtime_error("Missing rig file!");
//						Log::Info("Ragdoll: %s\n", character->m_stringData->m_ragdollName);
//						fs::path relative_ragdoll_path = fs::path((const char*)character->m_stringData->m_ragdollName);
//						fs::path ragdoll_file = project_path / relative_ragdoll_path;
//						if (fs::exists(ragdoll_file)) {
//							//Character, convertza
//							fs::path relative_behavior_path = fs::path((const char*)character->m_stringData->m_behaviorFilename);
//							fs::path behavior_file = project_path / relative_behavior_path;
//							if (!fs::exists(behavior_file))
//								throw runtime_error("Missing behavior file!");
//							stringlist projectList = f.getProjectList().getStrings();
//							int project_cache_index = 0;
//							for (string s : projectList) {
//								string s_lower = s; 
//								std::transform(s_lower.begin(), s_lower.end(), s_lower.begin(), ::tolower);
//								if (s_lower == tk_name + ".txt") {
//									break;
//								}
//								project_cache_index++;
//							}
//							//get the block
//							AnimData::ProjectBlock block = f.getProjectBlockList()[project_cache_index];
//							hkArray<hkVariant> obj;
//							hkRootLevelContainer* container = loadHavokFile(behavior_file.string().c_str(), obj);
//							//hkbProjectData* project = getFile(hkx_files, itr->c_str())->findObject<hkbProjectData>();
//							// get clips
//							set<hkbClipGenerator*> generators;
//							findGenerators(generators, behavior_file, project_path);
//							//verify project blocks and clips
//							if (block.getHasAnimationCache() && generators.size()!=block.getClips().size())
//								throw runtime_error("Incoherent generators!");
//						}
//					}
//				}
//			}
//		}
//	}
//
//
//	//for (std::string project : projectList.getStrings()) {
//	//	std::cout << "Project: " << project << std::endl;
//	//	//actually not updated and used only in their subfolders by the CK
//	//	//AnimData::StringListBlock& files = f.getProjectBlockList()[index++].getProjectFiles();
//	//	fs::path project_path = getShortestPath(findFile(bsa_extracted_path, fs::path(project).replace_extension().string()));
//	//	fs::path files_path = project_path;
//	//	files_path.remove_filename();
//
//	//	hkRootLevelContainer* container = loadHavokFile(project_path);
//	//	hkbProjectData* pd = reinterpret_cast<hkbProjectData*>(container->findObjectByType(hkbProjectDataClass.getName()));
//	//	hkbProjectStringData* psd = pd->m_stringData;
//
//	//	//Beth only uses Character Data;
//	//	hkStringPtr character_file = psd->m_characterFilenames[0];
//	//	std::cout << "Character file: " << character_file << std::endl;
//	//	std::experimental::filesystem::path file_path = files_path / fs::path((const char*)character_file);
//	//	if (!fs::exists(file_path))
//	//		throw new runtime_error("File not found!");
//
//
//		/*fs::path files_path = project_path;
//		files_path.remove_filename();
//		std::cout << "Files: " << files.getStrings().size() << std::endl;
//		for (std::string file : files.getStrings()) {
//		std::cout << "\t" << file << std::endl;
//		std::experimental::filesystem::path file_path = files_path / fs::path(file);
//		std::cout << "file location: " << file_path << std::endl;
//		if (!fs::exists(file_path))
//		throw new runtime_error("File not found!");
//		}
//	}*/
//
//	
//
//	hkBaseSystem::quit();
//	hkMemoryInitUtil::quit();
//
//
//	return true;
//}
//static bool SafeExecuteCmd(hkxcmdLine &cmdLine)
//{
//   __try{
//      return ExecuteCmd(cmdLine);
//   } __except (EXCEPTION_EXECUTE_HANDLER){
//      return false;
//   }
//}
//
//REGISTER_COMMAND(Project, HelpString, SafeExecuteCmd);
//
