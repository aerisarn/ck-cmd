#include "stdafx.h"

#include <commands/Project.h>

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/hkfutils.h>
#include <core/log.h>

#include <cstdio>
#include <sys/stat.h>

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>

// Scene
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkLoader.h>

// Physics
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>
#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>

#include <Physics\Dynamics\Constraint\Bilateral\Ragdoll\hkpRagdollConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\BallAndSocket\hkpBallAndSocketConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Hinge\hkpHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\LimitedHinge\hkpLimitedHingeConstraintData.h>
#include <Physics\Dynamics\Constraint\Bilateral\Prismatic\hkpPrismaticConstraintData.h>
#include <Physics\Dynamics\Constraint\Malleable\hkpMalleableConstraintData.h>

#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <Physics\Dynamics\World\hkpPhysicsSystem.h>
#include <Physics\Utilities\Serialize\hkpPhysicsData.h>

// Animation
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
#include <Animation/Animation/Animation/Deprecated/DeltaCompressed/hkaDeltaCompressedAnimation.h>
#include <Animation/Animation/Animation/SplineCompressed/hkaSplineCompressedAnimation.h>
#include <Animation/Animation/Animation/Quantized/hkaQuantizedAnimation.h>
#include <Animation/Animation/Animation/Util/hkaAdditiveAnimationUtility.h>

#include <obj\NiNode.h>
#include <obj\NiSequence.h>
#include <nif_math.h>

#include <map>

#include <hkbProjectStringData_1.h>
#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbVariableValueSet_0.h>
#include <hkbCharacterStringData_5.h>
#include <hkbMirroredSkeletonInfo_0.h>
#include <hkbCharacterDataCharacterControllerInfo_0.h>
#include <Common\Serialize\ResourceDatabase\hkResourceHandle.h>
#include <Animation\Animation\hkaAnimationContainer.h>
#include <hkbBehaviorGraph_1.h>
#include <hkbStateMachine_4.h>
#include <hkbBlendingTransitionEffect_1.h>
#include <BGSGamebryoSequenceGenerator_2.h>

#include <fbxsdk.h>

#include <filesystem>
namespace fs = std::experimental::filesystem;

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>
namespace ckcmd {
	namespace HKX {



#define BEHAVIORS_SUBFOLDER "behaviors"
#define CHARACTERS_SUBFOLDER "characters"
#define ASSETS_SUBFOLDER "assets"

		using namespace std;
		using namespace Niflib;

		class HKXWrapper {
			string out_name;
			string out_path;
			string out_path_abs;
			string prefix;

			map<fs::path, hkRootLevelContainer> out_data;

			void write(hkRootLevelContainer& rootCont, string subfolder = "", string name = "") {
				hkPackFormat pkFormat = HKPF_DEFAULT;
				hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;
				hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);
				fs::path final_out_path = fs::path(out_path_abs) / subfolder / string(name + ".hkx");
				//string out = out_path_abs+"\\" + out_name + ".hkx";
				fs::create_directories(final_out_path.parent_path());
				hkOstream stream(final_out_path.string().c_str());
				hkVariant root = { &rootCont, &rootCont.staticClass() };
				hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
				if (res != HK_SUCCESS)
				{
					Log::Error("Havok reports save failed.");
				}
			}

			hkRootLevelContainer* read(const fs::path& path, hkArray<hkVariant>& objects) {
				// Read back a serialized file
				hkIstream stream(path.string().c_str());
				hkStreamReader *reader = stream.getStreamReader();
				hkResource* resource = hkSerializeLoadResource(reader, objects);
				if (resource)
				{
					return resource->getContents<hkRootLevelContainer>();
				}
				return NULL;
			}

			hkRootLevelContainer* read(const fs::path& path) {
				// Read back a serialized file
				hkIstream stream(path.string().c_str());
				hkStreamReader *reader = stream.getStreamReader();
				hkResource* resource = hkSerializeLoadResource(reader);
				if (resource)
				{
					return resource->getContents<hkRootLevelContainer>();
				}
				return NULL;
			}

			hkRootLevelContainer* read(const uint8_t* data, const size_t& size, hkArray<hkVariant>& objects) {
				// Read back a serialized file
				hkIstream stream(data, size);
				hkStreamReader *reader = stream.getStreamReader();
				hkResource* resource = hkSerializeLoadResource(reader, objects);
				if (resource)
				{
					return resource->getContents<hkRootLevelContainer>();
				}
				return NULL;
			}

			hkRootLevelContainer* read(const uint8_t* data, const size_t& size) {
				// Read back a serialized file
				hkIstream stream(data, size);
				hkStreamReader *reader = stream.getStreamReader();
				hkResource* resource = hkSerializeLoadResource(reader);
				if (resource)
				{
					return resource->getContents<hkRootLevelContainer>();
				}
				return NULL;
			}

			template<typename hkRootType> 
			hkRefPtr<hkRootType> load(const fs::path& path, hkRootLevelContainer* root) {
				root = read(path);
				hkRefPtr<hkRootType> project;
				project = (hkRootType*)root->findObjectByType(project->getClassType()->getName());
				return project;
			}

			template<typename hkRootType>
			hkRefPtr<hkRootType> load(const uint8_t* data, const size_t& size, hkRootLevelContainer* root) {
				root = read(data, size);
				hkRefPtr<hkRootType> project;
				project = (hkRootType*)root->findObjectByType(project->getClassType()->getName());
				return project;
			}

			hkRefPtr<hkbProjectData> load_project(const fs::path& path) {
				hkRootLevelContainer* root;
				hkRefPtr<hkbProjectData> project = load<hkbProjectData>(path, root);
			}

			hkRefPtr<hkbProjectData> load_project(const uint8_t* data, const size_t& size) {
				hkRootLevelContainer* root;
				hkRefPtr<hkbProjectData> project = load<hkbProjectData>(data, size, root);
			}

			void retarget_project(hkRootLevelContainer* root, hkRefPtr<hkbProjectData>, const string& output_project_name, const fs::path& output_dir) 
			{

			}

			void create_project() {
				hkbProjectStringData string_data;
				string_data.m_characterFilenames.pushBack(CHARACTERS_SUBFOLDER"\\character.hkx");

				hkbProjectData data;
				data.m_worldUpWS = hkVector4(0.000000, 0.000000, 1.000000, 0.000000);
				data.m_stringData = &string_data;
				
				hkRootLevelContainer container;				
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("hkbProjectData", &data, &data.staticClass()));

				write(container, "", out_name);
			}

			void create_character() {
				hkbCharacterData data;
				hkbVariableValueSet values;
				hkbCharacterStringData string_data;
				hkbMirroredSkeletonInfo skel_info;

				skel_info.m_mirrorAxis = hkVector4(1.000000, 0.000000, 0.000000, 0.000000);

				// hkbCharacterStringData
				string_data.m_name = "character";
				string_data.m_rigName = ASSETS_SUBFOLDER"\\skeleton.hkx";
				string_data.m_behaviorFilename = BEHAVIORS_SUBFOLDER"\\behavior.hkx";

				//data
				hkbCharacterDataCharacterControllerInfo char_info;
				char_info.m_capsuleHeight = 1.700000;
				char_info.m_capsuleRadius = 0.400000;
				char_info.m_collisionFilterInfo = 1;
				char_info.m_characterControllerCinfo = NULL;
				data.m_characterControllerInfo = char_info;

				data.m_modelUpMS = hkVector4(0.000000, 0.000000, 1.000000, 0.000000);
				data.m_modelForwardMS = hkVector4(1.000000, 0.000000, 0.000000, 0.000000);
				data.m_modelRightMS = hkVector4(-0.000000, -1.000000, -0.000000, 0.000000);

				data.m_characterPropertyValues = &values;
				data.m_stringData = &string_data;
				data.m_mirroredSkeletonInfo = &skel_info;
				data.m_scale = 1.0;

				hkRootLevelContainer container;
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("hkbCharacterData", &data, &data.staticClass()));

				write(container, CHARACTERS_SUBFOLDER, "character");
			}

			void create_skeleton() {
				hkaAnimationContainer anim_container;
				hkMemoryResourceContainer mem_container;
				hkaSkeleton skeleton;

				skeleton.m_name = "x_SingleBone";
				skeleton.m_parentIndices.setSize(1);
				skeleton.m_bones.setSize(1);
				skeleton.m_referencePose.setSize(1);
				skeleton.m_parentIndices[0] = -1;
				skeleton.m_bones[0].m_name = "x_SingleBone";
				skeleton.m_bones[0].m_lockTranslation = false;
				skeleton.m_referencePose[0].setTranslation(hkVector4(0.000000, 0.000000, 0.000000, 0.000000));
				skeleton.m_referencePose[0].setRotation(::hkQuaternion(0.000000, 0.000000, 0.000000, 1.000000));
				skeleton.m_referencePose[0].setScale(hkVector4(1.000000, 1.000000, 1.000000, 0.000000));

				anim_container.m_skeletons.pushBack(&skeleton);

				hkRootLevelContainer container;
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Merged Animation Container", &anim_container, &anim_container.staticClass()));
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Resource Data", &mem_container, &mem_container.staticClass()));

				write(container, ASSETS_SUBFOLDER, "skeleton");
			}

			hkQsTransform setBoneTransform(FbxNode* pNode) {
				FbxAMatrix matrixGeo;
				matrixGeo.SetIdentity();
				if (pNode->GetNodeAttribute())
				{
					const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
					const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
					const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
					matrixGeo.SetT(lT);
					matrixGeo.SetR(lR);
					matrixGeo.SetS(lS);
				}
				FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

				matrixGeo = localMatrix * matrixGeo;
				hkQsTransform hk_trans;

				const FbxVector4 lT = matrixGeo.GetT();
				const FbxQuaternion lR = matrixGeo.GetQ();
				const FbxVector4 lS = matrixGeo.GetS();

				hk_trans.setTranslation(hkVector4(lT[0], lT[1], lT[2]));
				hk_trans.setRotation(::hkQuaternion(lR[0], lR[1], lR[2], lR[3]));
				hk_trans.setScale(hkVector4(lS[0], lS[1], lS[2], 0.000000));

				return hk_trans;
			}


			void create_behavior(const set<string>& sequences_names) {
				hkbBehaviorGraph graph;
				hkbStateMachine root_fsm;
				hkbBehaviorGraphData root_data;
				hkbBehaviorGraphStringData root_string_data;
				hkbVariableValueSet root_data_init_vars;
				hkbBlendingTransitionEffect transition_effect;

				size_t event_count = 0;
				map<string, int> event_map;
				vector<string> events;
				//Default events: Start, End, Next
				event_map[prefix + "Start"] = event_count++;
				events.push_back(prefix + "Start");
				event_map[prefix + "End"] = event_count++;
				events.push_back(prefix + "End");
				event_map[prefix + "Next"] = event_count++;
				events.push_back(prefix + "Next");

				//String Data
				root_string_data.m_eventNames.setSize(event_count);
				int count = 0;
				for (string event: events)
					root_string_data.m_eventNames[count++] = event.c_str();

				//Data
				root_data.m_eventInfos.setSize(event_count);
				for (int e = 0; e < event_count; e++)
					root_data.m_eventInfos[e].m_flags = 0;

				root_data.m_variableInitialValues = &root_data_init_vars;
				root_data.m_stringData = &root_string_data;

				//prepare transition effect
				transition_effect.m_name = "zero_duration";
				transition_effect.m_userData = 0;
				transition_effect.m_selfTransitionMode = hkbBlendingTransitionEffect::SELF_TRANSITION_MODE_CONTINUE_IF_CYCLIC_BLEND_IF_ACYCLIC;
				transition_effect.m_eventMode = hkbBlendingTransitionEffect::EVENT_MODE_DEFAULT;
				transition_effect.m_duration = 0.0;
				transition_effect.m_toGeneratorStartTimeFraction = 0.0;
				transition_effect.m_flags = 0;
				transition_effect.m_endMode = hkbBlendingTransitionEffect::END_MODE_NONE;
				transition_effect.m_blendCurve = hkbBlendCurveUtils::BLEND_CURVE_SMOOTH;

				//Root FSM
				root_fsm.m_name = "root_fsm";
				root_fsm.m_startStateId = 0;
				root_fsm.m_returnToPreviousStateEventId = -1;
				root_fsm.m_randomTransitionEventId = -1;
				root_fsm.m_transitionToNextHigherStateEventId = -1;
				root_fsm.m_transitionToNextLowerStateEventId = -1;
				root_fsm.m_syncVariableIndex = -1;
				root_fsm.m_userData = 0;
				root_fsm.m_eventToSendWhenStateOrTransitionChanges.m_id = -1;
				root_fsm.m_eventToSendWhenStateOrTransitionChanges.m_payload = NULL;
				root_fsm.m_wrapAroundStateId = false;
				root_fsm.m_maxSimultaneousTransitions = 32;
				root_fsm.m_startStateMode = hkbStateMachine::START_STATE_MODE_DEFAULT;
				root_fsm.m_selfTransitionMode = hkbStateMachine::SELF_TRANSITION_MODE_NO_TRANSITION;

				int state_index = 0;
				for (const string& sequence : sequences_names) {
					hkRefPtr<hkbStateMachineStateInfo> state = new hkbStateMachineStateInfo(); state_index++;
					//create notify events;
					hkRefPtr<hkbStateMachineEventPropertyArray> enter_notification = new hkbStateMachineEventPropertyArray();
					enter_notification->m_events.setSize(1);
					enter_notification->m_events[0].m_id = event_map[prefix + "Start"];
					enter_notification->m_events[0].m_payload = NULL;
					state->m_enterNotifyEvents = enter_notification;

					hkRefPtr<hkbStateMachineEventPropertyArray> exit_notification = new hkbStateMachineEventPropertyArray();
					exit_notification->m_events.setSize(1);
					exit_notification->m_events[0].m_id = event_map[prefix + "End"];
					exit_notification->m_events[0].m_payload = NULL;
					state->m_exitNotifyEvents = exit_notification;

					if (sequences_names.size() > 1) {
						//create transition to the next state;
						hkRefPtr<hkbStateMachineTransitionInfoArray> transition = new hkbStateMachineTransitionInfoArray();
						transition->m_transitions.setSize(1);
						transition->m_transitions[0].m_triggerInterval.m_enterEventId = -1;
						transition->m_transitions[0].m_triggerInterval.m_exitEventId = -1;
						transition->m_transitions[0].m_triggerInterval.m_enterTime = 0.0;
						transition->m_transitions[0].m_triggerInterval.m_exitTime = 0.0;

						transition->m_transitions[0].m_initiateInterval.m_enterEventId = -1;
						transition->m_transitions[0].m_initiateInterval.m_exitEventId = -1;
						transition->m_transitions[0].m_initiateInterval.m_enterTime = 0.0;
						transition->m_transitions[0].m_initiateInterval.m_exitTime = 0.0;

						transition->m_transitions[0].m_transition = &transition_effect;
						transition->m_transitions[0].m_eventId = event_map[prefix + "Next"];
						int next_state_id = state_index != sequences_names.size() ? state_index : 0;
						transition->m_transitions[0].m_toStateId = next_state_id;
						transition->m_transitions[0].m_fromNestedStateId = 0;
						transition->m_transitions[0].m_toNestedStateId = 0;
						transition->m_transitions[0].m_priority = 0;
						transition->m_transitions[0].m_flags = hkbStateMachineTransitionInfo::FLAG_DISABLE_CONDITION;
						state->m_transitions = transition;
					}

					//generator
					hkRefPtr<BGSGamebryoSequenceGenerator> generator = new BGSGamebryoSequenceGenerator();
					generator->m_name = sequence.c_str();
					generator->m_userData = 0;
					generator->m_pSequence = (char*)sequence.c_str();
					generator->m_eBlendModeFunction = BGSGamebryoSequenceGenerator::BMF_NONE;
					generator->m_fPercent = 1.0;
					state->m_generator = generator;

					//finish packing the state;
					state->m_name = sequence.c_str();
					state->m_stateId = state_index - 1;
					state->m_probability = 1.000000;
					state->m_enable = true;

					root_fsm.m_states.pushBack(state);
				}

				graph.m_name = "behavior";
				graph.m_userData = 0;
				graph.m_variableMode = hkbBehaviorGraph::VARIABLE_MODE_DISCARD_WHEN_INACTIVE;
				graph.m_rootGenerator = &root_fsm;
				graph.m_data = &root_data;

				hkRootLevelContainer container;
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("hkbBehaviorGraph", &graph, &graph.staticClass()));

				write(container, BEHAVIORS_SUBFOLDER, "behavior");
			}

		public:
			HKXWrapper() {}

			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix)
				: out_name(out_name), out_path(out_path), out_path_abs(out_path_abs), prefix(prefix)
			{

			}

			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const set<string>& sequences_names)
				: out_name(out_name), out_path(out_path), out_path_abs(out_path_abs), prefix(prefix)
			{
				create_project();
				create_character();
				create_skeleton();
				create_behavior(sequences_names);
			}

			string GetPath() { return out_path + "\\" + out_name + "\\" + out_name + ".hkx"; }

			//gives back the ordered bone array as written in the skeleton file
			vector<FbxNode*> create_skeleton(const string& name, const set<FbxNode*>& bones) {
				hkRefPtr<hkaAnimationContainer> anim_container = new hkaAnimationContainer();
				hkRefPtr<hkMemoryResourceContainer> mem_container = new hkMemoryResourceContainer();
				hkRefPtr<hkaSkeleton> skeleton = new hkaSkeleton();

				skeleton->m_name = name.c_str();
				//build ordered set;
				vector<FbxNode*> ordered_bones;
				copy(bones.begin(), bones.end(), back_inserter(ordered_bones));
				skeleton->m_parentIndices.setSize(bones.size());
				skeleton->m_bones.setSize(bones.size());
				skeleton->m_referencePose.setSize(bones.size());
				//build parent_map;
				for (int i=0; i< ordered_bones.size(); i++)
				{
					FbxNode* bone = ordered_bones[i];
					vector<FbxNode*>::iterator parent_it = find(ordered_bones.begin(), ordered_bones.end(), bone->GetParent());
					if (parent_it == ordered_bones.end())
					{
						skeleton->m_parentIndices[i] = -1;
						skeleton->m_bones[i].m_lockTranslation = false;
					}
					else
					{
						skeleton->m_parentIndices[i] = distance(ordered_bones.begin(), parent_it);
						skeleton->m_bones[i].m_lockTranslation = true;
					}
					skeleton->m_bones[i].m_name = bone->GetName();
					skeleton->m_referencePose[i] = setBoneTransform(bone);
				}

				anim_container->m_skeletons.pushBack(skeleton);

				hkRootLevelContainer container;
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Merged Animation Container", anim_container, &anim_container->staticClass()));
				container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Resource Data", mem_container, &mem_container->staticClass()));

				//write(container, ASSETS_SUBFOLDER, "skeleton");
				//not that moving <100 ptrs matters that much, but hey, it's fancy
				out_data[fs::path(ASSETS_SUBFOLDER) / "skeleton"] = container;

				return move(ordered_bones);
			}

			template<typename FbxMatrixType>
			static void convertFbxXMatrixToMatrix4(const FbxMatrixType& fbxMatrix, hkMatrix4& matrix)
			{
				FbxVector4 v0 = fbxMatrix.GetRow(0);
				FbxVector4 v1 = fbxMatrix.GetRow(1);
				FbxVector4 v2 = fbxMatrix.GetRow(2);
				FbxVector4 v3 = fbxMatrix.GetRow(3);

				hkVector4 c0; c0.set((float)v0[0], (float)v0[1], (float)v0[2], (float)v0[3]);
				hkVector4 c1; c1.set((float)v1[0], (float)v1[1], (float)v1[2], (float)v1[3]);
				hkVector4 c2; c2.set((float)v2[0], (float)v2[1], (float)v2[2], (float)v2[3]);
				hkVector4 c3; c3.set((float)v3[0], (float)v3[1], (float)v3[2], (float)v3[3]);

				matrix.setCols(c0, c1, c2, c3);
			}

			

			void create_animations(vector<FbxNode*>& skeleton, set<FbxAnimStack*>& animations, FbxTime::EMode timeMode)
			{
			//	for (FbxAnimStack* stack : animations)
			//	{
			//		hkRefPtr<hkaAnimationContainer> anim_container = new hkaAnimationContainer();
			//		hkRefPtr<hkMemoryResourceContainer> mem_container = new hkMemoryResourceContainer();
			//		hkRefPtr<hkaAnimationBinding> mem_container = new hkaAnimationBinding();
			//		hkRefPtr<hkaInterleavedUncompressedAnimation> tempAnim = new hkaInterleavedUncompressedAnimation();


			//		FbxTimeSpan animTimeSpan = stack->GetLocalTimeSpan();

			//		// Find the time offset (in the "time space" of the FBX file) of the first animation frame
			//		FbxTime timePerFrame; timePerFrame.SetTime(0, 0, 0, 1, 0, timeMode);

			//		const FbxTime startTime = animTimeSpan.GetStart();
			//		const FbxTime endTime = animTimeSpan.GetStop();

			//		const hkReal startTimeSeconds = static_cast<hkReal>(startTime.GetSecondDouble());
			//		const hkReal endTimeSeconds = static_cast<hkReal>(endTime.GetSecondDouble());

			//		hkArray<hkString> annotationStrings;
			//		hkArray<hkReal> annotationTimes;

			//		int numFrames = 0;
			//		bool staticNode = true;



			//		// Sample each animation frame
			//		for (FbxTime time = startTime, priorSampleTime = endTime;
			//			time < endTime;
			//			priorSampleTime = time, time += timePerFrame, ++numFrames)
			//		{
			//			for (FbxNode* bone : skeleton)
			//			{
			//				FbxAMatrix frameMatrix = bone->EvaluateLocalTransform(time);
			//				//staticNode = staticNode && (frameMatrix == bindPoseMatrix);

			//				hkMatrix4 mat;

			//				// Extract this frame's transform
			//				convertFbxXMatrixToMatrix4(frameMatrix, mat);
			//				newChildNode->m_keyFrames.pushBack(mat);
			//			}


			//			

			//		}

			//		
			//		tempAnim->m_duration = duration;
			//		tempAnim->m_numberOfTransformTracks = numTracks;
			//		tempAnim->m_numberOfFloatTracks = 0;//anim->m_numberOfFloatTracks;
			//		tempAnim->m_transforms.setSize(numTracks*nframes, hkQsTransform::getIdentity());
			//		tempAnim->m_floats.setSize(tempAnim->m_numberOfFloatTracks);
			//		tempAnim->m_annotationTracks.setSize(numTracks);
			//	}

			//	for (FbxNode* pNode : unskinned_bones)
			//	{
			//		//FbxNode* pNode = pair.first;
			//		FbxAnimCurve* lXAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			//		FbxAnimCurve* lYAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			//		FbxAnimCurve* lZAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
			//		FbxAnimCurve* lIAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			//		FbxAnimCurve* lJAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
			//		FbxAnimCurve* lKAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

			//		if (lXAnimCurve != NULL || lYAnimCurve != NULL || lZAnimCurve != NULL ||
			//			lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL)
			//		{

			//			NiObjectRef target = conversion_Map[pNode];
			//			targets.insert(target);

			//			NiTransformInterpolatorRef interpolator = new NiTransformInterpolator();
			//			NiQuatTransform trans;
			//			trans.translation = Vector3(0, 0, 0);
			//			trans.rotation = Quaternion(1, 0, 0, 0);
			//			trans.scale = 1;
			//			interpolator->SetTransform(trans);

			//			if (lXAnimCurve != NULL || lYAnimCurve != NULL || lZAnimCurve != NULL) {
			//				addTranslationKeys(interpolator, pNode, lXAnimCurve, lYAnimCurve, lZAnimCurve, last_start);
			//			}

			//			if (lIAnimCurve != NULL || lJAnimCurve != NULL || lKAnimCurve != NULL) {
			//				addRotationKeys(interpolator, pNode, lIAnimCurve, lJAnimCurve, lKAnimCurve, last_start);
			//			}

			//			NiTransformDataRef data = interpolator->GetData();
			//			if (data != NULL) {
			//				KeyGroup<float> scales;
			//				scales.numKeys = 0;
			//				scales.keys = {};
			//				data->SetScales(scales);
			//			}

			//			//interpolator->SetData(new NiTransformData());

			//			ControlledBlock block;
			//			block.interpolator = interpolator;
			//			block.nodeName = DynamicCast<NiAVObject>(conversion_Map[pNode])->GetName();
			//			block.controllerType = "NiTransformController";
			//			block.controller = multiController;

			//			blocks.push_back(block);

			//			vector<FbxTimeSpan> spans(6);

			//			if (lXAnimCurve != NULL)
			//				lXAnimCurve->GetTimeInterval(spans[0]);
			//			if (lYAnimCurve != NULL)
			//				lYAnimCurve->GetTimeInterval(spans[1]);
			//			if (lZAnimCurve != NULL)
			//				lZAnimCurve->GetTimeInterval(spans[2]);
			//			if (lIAnimCurve != NULL)
			//				lIAnimCurve->GetTimeInterval(spans[3]);
			//			if (lJAnimCurve != NULL)
			//				lJAnimCurve->GetTimeInterval(spans[4]);
			//			if (lKAnimCurve != NULL)
			//				lKAnimCurve->GetTimeInterval(spans[5]);

			//			double start = 1e10;
			//			double end = -1e10;

			//			for (const auto& span : spans) {
			//				double span_start = span.GetStart().GetSecondDouble();
			//				double span_stop = span.GetStop().GetSecondDouble();
			//				if (span_start < start)
			//					start = span_start;
			//				if (span_stop > end)
			//					end = span_stop;
			//			}

			//			sequence->SetControlledBlocks(blocks);

			//			sequence->SetStartTime(0.0);
			//			sequence->SetStopTime(end - start);
			//			sequence->SetManager(manager);
			//			sequence->SetAccumRootName(accum_root_name);

			//			NiTextKeyExtraDataRef extra_data = new NiTextKeyExtraData();
			//			extra_data->SetName(string(""));
			//			Key<IndexString> start_key;
			//			start_key.time = 0;
			//			start_key.data = "start";

			//			Key<IndexString> end_key;
			//			end_key.time = end - last_start;
			//			end_key.data = "end";

			//			extra_data->SetTextKeys({ start_key,end_key });
			//			extra_data->SetNextExtraData(NULL);

			//			sequence->SetTextKeys(extra_data);

			//			sequence->SetFrequency(1.0);
			//			sequence->SetCycleType(CYCLE_CLAMP);

			//		}
			//	}
			}

		};

		typedef map<set<string>, HKXWrapper> wrap_map;

		class HKXWrapperCollection {
			wrap_map wrappers;
		public:
			string wrap(const string& out_name, const string& out_path, const string& out_path_root, const string& prefix, const set<string>& sequences_names)
			{

				if (wrappers.find(sequences_names) == wrappers.end()) {
					wrappers[sequences_names] = move(HKXWrapper(out_name, out_path, out_path_root, prefix, sequences_names));
				}
				return wrappers[sequences_names].GetPath();
			}
		};
	}
}
