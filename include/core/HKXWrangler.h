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

			HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const set<string>& sequences_names)
				: out_name(out_name), out_path(out_path), out_path_abs(out_path_abs), prefix(prefix)
			{
				create_project();
				create_character();
				create_skeleton();
				create_behavior(sequences_names);
			}

			string GetPath() { return out_path + "\\" + out_name + "\\" + out_name + ".hkx"; }

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