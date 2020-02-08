#include <core\HKXWrangler.h>

#include <Common\Base\Types\Geometry\hkGeometry.h>

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
#include <hkbClipGenerator_2.h>
#include <hkbProjectData_2.h>
#include <hkbCharacterData_7.h>
#include <hkbClipGenerator_2.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <Common/Base/Container/String/hkStringBuf.h>

#include <Physics\Utilities\Collide\ShapeUtils\ShapeConverter\hkpShapeConverter.h>

//collisions
#include <Physics\Collide\Shape\Misc\Transform\hkpTransformShape.h>
#include <Physics\Collide\Shape\Compound\Collection\List\hkpListShape.h>
#include <Physics\Collide\Shape\Deprecated\ConvexList\hkpConvexListShape.h>

#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h"
#include "Physics/Collide/Shape/Compound/Collection/CompressedMesh/hkpCompressedMeshShapeBuilder.h"
#include "Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h"
#include "Physics/Collide/Util/Welding/hkpMeshWeldingUtility.h"
#include "Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h"
#include <Physics\Collide\Shape\Compound\Collection\CompressedMesh\hkpCompressedMeshShape.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesShape.h>
#include <Physics\Collide\Shape\Convex\ConvexTransform\hkpConvexTransformShape.h>

#include <Physics\Utilities\Collide\ShapeUtils\CreateShape\hkpCreateShapeUtility.h>
#include <Common\Base\Types\Geometry\hkStridedVertices.h>
#include <Common\Internal\ConvexHull\hkGeometryUtility.h>

#include <Common\GeometryUtilities\Misc\hkGeometryUtils.h>
#include <Physics\Collide\Shape\Convex\ConvexVertices\hkpConvexVerticesConnectivity.h>
#include <Physics\Utilities\Collide\ShapeUtils\ShapeConverter\hkpShapeConverter.h>

#include <Animation\Ragdoll\Utils\hkaRagdollUtils.h>

#include <core/EulerAngles.h>
#include <core/MathHelper.h>

#include <algorithm>

#include <VHACD.h>
#include <boundingmesh.h>
#include <core/NifFile.h>


using namespace ckcmd::HKX;
using namespace ckcmd::NIF;

bool isShapeFbxNode(FbxNode* node)
{
	string node_name = node->GetName();
	return (
		ends_with(node_name, "_transform") ||
		ends_with(node_name, "_list") ||
		ends_with(node_name, "_convex_list") ||
		ends_with(node_name, "_mopp") ||
		ends_with(node_name, "_sphere") ||
		ends_with(node_name, "_box") ||
		ends_with(node_name, "_capsule") ||
		ends_with(node_name, "_mesh")
		);
}

bool isConstraintFbxNode(FbxNode* node) {
	string node_name = node->GetName();
	return
		node_name.find("_con_") != string::npos;
		
}

void HKXWrapper::write(hkRootLevelContainer& rootCont, string subfolder, string name) {
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

hkRootLevelContainer* HKXWrapper::read(const fs::path& path, hkArray<hkVariant>& objects) {
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

hkRootLevelContainer* HKXWrapper::read(const fs::path& path) {
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

hkRootLevelContainer* HKXWrapper::read(const uint8_t* data, const size_t& size, hkArray<hkVariant>& objects) {
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

hkRootLevelContainer* HKXWrapper::read(const uint8_t* data, const size_t& size) {
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

//hkRefPtr<hkbProjectData> HKXWrapper::load_project(const fs::path& path) {
//	hkRootLevelContainer* root;
//	hkRefPtr<hkbProjectData> project = load<hkbProjectData>(path, root);
//}
//
//hkRefPtr<hkbProjectData> HKXWrapper::load_project(const uint8_t* data, const size_t& size) {
//	hkRootLevelContainer* root;
//	hkRefPtr<hkbProjectData> project = load<hkbProjectData>(data, size, root);
//}

void  HKXWrapper::create_project(const set<string>& havok_sequences_names) {
	hkbProjectStringData string_data;
	string_data.m_characterFilenames.pushBack(CHARACTERS_SUBFOLDER"\\character.hkx");
	for (const auto& animation : havok_sequences_names)
	{
		string_data.m_animationFilenames.pushBack((fs::path(ANIMATIONS_SUBFOLDER) / string(animation + ".hkx")).string().c_str());
	}
	hkbProjectData data;
	data.m_worldUpWS = hkVector4(0.000000, 0.000000, 1.000000, 0.000000);
	data.m_stringData = &string_data;

	hkRootLevelContainer container;
	container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("hkbProjectData", &data, &data.staticClass()));

	write(container, "", out_name);
}

void HKXWrapper::create_character(const set<string>& havok_sequences_names) {
	hkbCharacterData data;
	hkbVariableValueSet values;
	hkbCharacterStringData string_data;
	hkbMirroredSkeletonInfo skel_info;

	skel_info.m_mirrorAxis = hkVector4(1.000000, 0.000000, 0.000000, 0.000000);

	// hkbCharacterStringData
	string_data.m_name = "character";
	string_data.m_rigName = ASSETS_SUBFOLDER"\\skeleton.hkx";
	string_data.m_behaviorFilename = BEHAVIORS_SUBFOLDER"\\behavior.hkx";

	for (const auto& animation : havok_sequences_names)
	{
		string_data.m_animationNames.pushBack((fs::path(ANIMATIONS_SUBFOLDER) / string(animation + ".hkx")).string().c_str());
	}

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

void HKXWrapper::create_skeleton() {
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

hkQsTransform getBoneTransform(FbxNode* pNode, FbxTime time) {
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
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform(time);

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

hkTransform getTransform(const FbxVector4& lT, const FbxQuaternion& lR) {
	hkQsTransform hk_trans;

	hk_trans.setTranslation(hkVector4(lT[0], lT[1], lT[2]));
	hk_trans.setRotation(::hkQuaternion(lR[0], lR[1], lR[2], lR[3]));

	hkTransform b;
	hk_trans.copyToTransformNoScale(b);
	return b;
}

hkTransform getTransform(FbxNode* pNode, bool absolute = false, bool inverse = false) {
	hkQsTransform hk_trans;
	FbxAMatrix localMatrix;
	if (absolute)
		localMatrix = pNode->EvaluateGlobalTransform();
	else
		localMatrix = pNode->EvaluateLocalTransform();

	const FbxVector4 lT = localMatrix.GetT();
	FbxQuaternion lR = localMatrix.GetQ(); 
	if (inverse)
		lR.Inverse();

	hk_trans.setTranslation(hkVector4(lT[0], lT[1], lT[2]));
	hk_trans.setRotation(::hkQuaternion(lR[0], lR[1], lR[2], lR[3]));

	hkTransform b;
	hk_trans.copyToTransformNoScale(b);
	return b;
}

hkReal getFloatTrackValue(FbxProperty& property, FbxTime time) {
	hkReal result = 0.0;
	if (property.IsValid())
	{
		property.EvaluateValue(time).Get(&result, EFbxType::eFbxFloat);
	}
	return result;
}

void HKXWrapper::create_behavior(const set<string>& kf_sequences_names, const set<string>& havok_sequences_names) {
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
	for (string event : events)
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

	vector<string> sequences_names(kf_sequences_names.begin(), kf_sequences_names.end());
	sequences_names.insert(sequences_names.end(), havok_sequences_names.begin(), havok_sequences_names.end());

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
		if (state_index <= kf_sequences_names.size())
		{
			hkRefPtr<BGSGamebryoSequenceGenerator> generator = new BGSGamebryoSequenceGenerator();
			generator->m_name = sequence.c_str();
			generator->m_userData = 0;
			generator->m_pSequence = (char*)sequence.c_str();
			generator->m_eBlendModeFunction = BGSGamebryoSequenceGenerator::BMF_NONE;
			generator->m_fPercent = 1.0;
			state->m_generator = generator;
		}
		else {
			hkRefPtr<hkbClipGenerator> generator = new hkbClipGenerator();
			generator->m_name = sequence.c_str();
			generator->m_userData = 0;
			generator->m_animationName = (fs::path(ANIMATIONS_SUBFOLDER) / string(sequence + ".hkx")).string().c_str();
			generator->m_cropStartAmountLocalTime = 0.0;
			generator->m_cropEndAmountLocalTime = 0.0;
			generator->m_startTime = 0.0;
			generator->m_playbackSpeed = 1.0;
			generator->m_enforcedDuration = 0.0;
			generator->m_userControlledTimeFraction = 0.0;
			generator->m_animationBindingIndex = -1;
			generator->m_mode = hkbClipGenerator::PlaybackMode::MODE_LOOPING;
			generator->m_flags = 0;
			state->m_generator = generator;
		}

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

HKXWrapper::HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix)
	: out_name(out_name), out_path(out_path), out_path_abs(out_path_abs), prefix(prefix)
{

}

HKXWrapper::HKXWrapper(const string& out_name, const string& out_path, const string& out_path_abs, const string& prefix, const set<string>& sequences_names)
	: out_name(out_name), out_path(out_path), out_path_abs(out_path_abs), prefix(prefix)
{
	create_project();
	create_character({});
	create_skeleton();
	create_behavior(sequences_names, {});
}

vector<string> HKXWrapper::read_track_list(const fs::path& path, string& skeleton_name, string& root_name, vector<string>& floats) {
	vector<string> ordered_tracks;
	hkArray<hkVariant> objects;
	read(path, objects);
	for (const auto& variant : objects) {
		//WARNING: MUST be the FIRST. There's no good way to separate the animation skeleton from the ragdoll
		if (strcmp(variant.m_class->getName(), "hkaSkeleton") == 0)
		{
			hkRefPtr<hkaSkeleton> skeleton((hkaSkeleton*)variant.m_object);
			if (string(skeleton->m_name.cString()).find("Ragdoll") == string::npos)
			{
				for (int i = 0; i < skeleton->m_bones.getSize(); i++)
					ordered_tracks.push_back(skeleton->m_bones[i].m_name.cString());

				for (int i = 0; i < skeleton->m_floatSlots.getSize(); i++)
					floats.push_back(skeleton->m_floatSlots[i].cString());

				//find root
				for (int i = 0; i < skeleton->m_parentIndices.getSize(); i++) {
					if (skeleton->m_parentIndices[i] == -1 &&
						string(skeleton->m_bones[i].m_name.cString()).find("Camera") == string::npos)
						root_name = skeleton->m_bones[i].m_name.cString();
				}

				skeleton_name = skeleton->m_name.cString();
				return move(ordered_tracks);
			}
		}
	}

	return move(ordered_tracks);
}

vector<FbxNode*> HKXWrapper::create_skeleton(const string& name, const set<FbxNode*>& bones, FbxNode* root) {
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
	for (int i = 0; i < ordered_bones.size(); i++)
	{
		FbxNode* bone = ordered_bones[i];
		vector<FbxNode*>::iterator parent_it = find(ordered_bones.begin(), ordered_bones.end(), bone->GetParent());
		if (parent_it == ordered_bones.end())
		{
			skeleton->m_parentIndices[i] = -1;
			skeleton->m_bones[i].m_lockTranslation = false;
			root = bone;
		}
		else
		{
			skeleton->m_parentIndices[i] = distance(ordered_bones.begin(), parent_it);
			skeleton->m_bones[i].m_lockTranslation = true;
		}
		skeleton->m_bones[i].m_name = bone->GetName();
		skeleton->m_referencePose[i] = getBoneTransform(bone, 0);
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

bool annotation_sorter(hkaAnnotationTrack::Annotation const& lhs, hkaAnnotationTrack::Annotation const& rhs) {
	return lhs.m_time < rhs.m_time;
}

set<string> HKXWrapper::create_animations(
	const string& skeleton_name,
	vector<FbxNode*>& skeleton,
	set<FbxAnimStack*>& animations,
	FbxTime::EMode timeMode,
	const vector<uint32_t>& transform_track_to_bone_indices,
	set<FbxProperty>& annotations,
	vector<FbxProperty>& floats,
	const vector<uint32_t>& transform_track_to_float_indices
	)
{

	set<string> sequences_names;
	FbxAnimStack* starting_stack = (*animations.begin())->GetScene()->GetCurrentAnimationStack();
	for (FbxAnimStack* stack : animations)
	{
		stack->GetScene()->SetCurrentAnimationStack(stack);
		hkRefPtr<hkaAnimationContainer> anim_container = new hkaAnimationContainer();
		hkRefPtr<hkMemoryResourceContainer> mem_container = new hkMemoryResourceContainer();
		hkRefPtr<hkaAnimationBinding> binding = new hkaAnimationBinding();
		hkRefPtr<hkaInterleavedUncompressedAnimation> tempAnim = new hkaInterleavedUncompressedAnimation();


		FbxTimeSpan animTimeSpan = stack->GetLocalTimeSpan();

		// Find the time offset (in the "time space" of the FBX file) of the first animation frame
		FbxTime timePerFrame; timePerFrame.SetTime(0, 0, 0, 1, 0, timeMode);

		const FbxTime startTime = animTimeSpan.GetStart();
		const FbxTime endTime = animTimeSpan.GetStop();

		const hkReal startTimeSeconds = static_cast<hkReal>(startTime.GetSecondDouble());
		const hkReal endTimeSeconds = static_cast<hkReal>(endTime.GetSecondDouble());

		hkArray<hkString> annotationStrings;
		hkArray<hkReal> annotationTimes;

		size_t numTracks = skeleton.size();
		hkReal duration = endTimeSeconds - startTimeSeconds;
		size_t numFrames = 0;
		bool staticNode = true;

		tempAnim->m_duration = endTimeSeconds - startTimeSeconds;
		tempAnim->m_numberOfTransformTracks = skeleton.size();
		tempAnim->m_annotationTracks.setSize(skeleton.size());
		tempAnim->m_numberOfFloatTracks = floats.size();
		tempAnim->m_floats.setSize(tempAnim->m_numberOfFloatTracks);

		//Annotations
		vector<hkaAnnotationTrack::Annotation> temp_track;
		for (FbxProperty annotation : annotations)
		{
			FbxAnimCurveNode* curve_node = annotation.GetCurveNode();
			if (curve_node)
			{
				//conventionally we want annotation on a single enum channel
				FbxAnimCurve* first_curve = curve_node->GetCurve(0);
				if (first_curve) {
					size_t keys = first_curve->KeyGetCount();					
					//hkaAnnotationTrack& a_track = tempAnim->m_annotationTracks[0];
					if (keys > 0)
					{						
						for (int i = 0; i < keys; i++)
						{
							hkaAnnotationTrack::Annotation new_ann;
							new_ann.m_time = first_curve->KeyGet(i).GetTime().GetSecondDouble();
							string text = annotation.GetNameAsCStr();
							//remove "hk"
							text = text.substr(2, text.size());
							//set first part to lowercase
							//std::transform(text.begin(), text.end(), text.begin(), ::tolower);
							string value = annotation.GetEnumValue(first_curve->KeyGet(i).GetValue());
							//set second part to lowercase
							std::transform(value.begin(), value.end(), value.begin(), ::tolower);
							//now first char uppercase
							*value.begin() = ::toupper(*value.begin());
							text += value;
							new_ann.m_text = text.c_str();
							temp_track.push_back(new_ann);
						}
					}
				}
			}
		}

		//sort annotations by time
		if (!temp_track.empty())
		{ 
			sort(temp_track.begin(), temp_track.end(), &annotation_sorter);
			hkaAnnotationTrack& a_track = tempAnim->m_annotationTracks[0];
			for (const auto& ann : temp_track)
				a_track.m_annotations.pushBack(ann);
		}

		// Sample each animation frame
		for (FbxTime time = startTime, priorSampleTime = endTime;
			time <= endTime;
			priorSampleTime = time, time += timePerFrame, ++numFrames)
		{
			for (FbxNode* bone : skeleton)
			{
				tempAnim->m_transforms.pushBack(getBoneTransform(bone, time));
			}
			for (FbxProperty& float_track : floats)
			{
				tempAnim->m_floats.pushBack(getFloatTrackValue(float_track, time));
			}
		}

		if (!transform_track_to_bone_indices.empty()) {
			for (const auto& index : transform_track_to_bone_indices)
				binding->m_transformTrackToBoneIndices.pushBack(index);
		}

		if (!transform_track_to_float_indices.empty()) {
			for (const auto& index : transform_track_to_float_indices)
				binding->m_floatTrackToFloatSlotIndices.pushBack(index);
		}

		hkaSkeletonUtils::normalizeRotations(tempAnim->m_transforms.begin(), tempAnim->m_transforms.getSize());

		// create the animation with default settings
		{
			hkaSplineCompressedAnimation::TrackCompressionParams tparams;
			hkaSplineCompressedAnimation::AnimationCompressionParams aparams;

			tparams.m_rotationTolerance = 0.001f;
			tparams.m_rotationQuantizationType = hkaSplineCompressedAnimation::TrackCompressionParams::THREECOMP40;

			hkRefPtr<hkaSplineCompressedAnimation> outAnim = new hkaSplineCompressedAnimation(*tempAnim.val(), tparams, aparams);
			binding->m_animation = outAnim;
			binding->m_originalSkeletonName = skeleton_name.c_str();

			anim_container->m_bindings.pushBack(binding);
			anim_container->m_animations.pushBack(binding->m_animation);
		}

		hkRootLevelContainer container;


		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Merged Animation Container", anim_container, &anim_container->staticClass()));
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Resource Data", mem_container, &mem_container->staticClass()));

		sequences_names.insert(stack->GetName());
		out_data[fs::path(ANIMATIONS_SUBFOLDER) / stack->GetName()] = container;

	}
	starting_stack->GetScene()->SetCurrentAnimationStack(starting_stack);
	return move(sequences_names);
}

void HKXWrapper::write_animations(const string& out_path, const set<string>& havok_sequences_names)
{
	if (havok_sequences_names.size() == 1)
	{
		this->out_name = fs::path(out_path).filename().replace_extension("").string();
		this->out_path = fs::path(out_path).parent_path().string();
		this->out_path_abs = this->out_path;
		this->prefix = "";

		write(out_data.begin()->second, "", out_name);
	}
	else {
		for (auto& asset : out_data)
		{
			this->out_path = fs::path(out_path).parent_path().string();
			this->out_path_abs = this->out_path;
			this->prefix = "";

			write(out_data.begin()->second, "", asset.first.string());
		}
	}
}

string HKXWrapper::write_project(const string& out_name, const string& out_path, const string& out_path_abs,
	const string& prefix, const set<string>& kf_sequences_names, const set<string>& havok_sequences_names)
{
	this->out_name = out_name;
	this->out_path = out_path;
	this->out_path_abs = out_path_abs;
	this->prefix = prefix;

	create_project(havok_sequences_names);
	create_character(havok_sequences_names);
	for (auto& asset : out_data)
	{
		write(asset.second, "", asset.first.string());
	}
	create_behavior(kf_sequences_names, havok_sequences_names);
	return GetPath();
}

void HKXWrapper::add(const string& name, hkaAnimation* animation, hkaAnimationBinding* binding, vector<FbxNode*>& ordered_skeleton, vector<FbxProperty>& float_tracks)
{
	FbxString lAnimStackName;
	FbxTime lTime;
	int lKeyIndex = 0;

	FbxAnimStack* lAnimStack = FbxAnimStack::Create(ordered_skeleton[0]->GetScene(), name.c_str());

	// The animation nodes can only exist on AnimLayers therefore it is mandatory to
	// add at least one AnimLayer to the AnimStack. And for the purpose of this example,
	// one layer is all we need.
	FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(ordered_skeleton[0]->GetScene(), "Base Layer");
	lAnimStack->AddMember(lAnimLayer);

	// havok related animation stuff now
	const int numBones = ordered_skeleton.size();

	int FrameNumber = animation->getNumOriginalFrames();
	int TrackNumber = animation->m_numberOfTransformTracks;
	int FloatNumber = animation->m_numberOfFloatTracks;

	float AnimDuration = animation->m_duration;
	hkReal incrFrame = animation->m_duration / (hkReal)(FrameNumber -1);
	double framerate = FrameNumber / AnimDuration;

	lTime.SetGlobalTimeMode(FbxTime::EMode::eCustom, framerate);
	FbxTime startTime; startTime.SetSecondDouble(0.0);
	FbxTime stopTime; stopTime.SetSecondDouble(AnimDuration);
	lAnimStack->SetLocalTimeSpan(FbxTimeSpan(startTime, stopTime));

	if (TrackNumber > numBones)
	{
		Log::Error("\nERROR: More tracks than the actual skeleton loaded! some will be ignored\n");		
	}

	hkLocalArray<float> floatsOut(FloatNumber);
	hkLocalArray<hkQsTransform> transformOut(TrackNumber);
	floatsOut.setSize(FloatNumber);
	transformOut.setSize(TrackNumber);
	//hkReal startTime = 0.0;

	hkArray<hkInt16> tracks;
	tracks.setSize(TrackNumber);
	for (int i = 0; i<TrackNumber; ++i) tracks[i] = i;

	//hkReal time = startTime;

	FbxAnimCurve* lCurve_Trans_X;
	FbxAnimCurve* lCurve_Trans_Y;
	FbxAnimCurve* lCurve_Trans_Z;
	FbxAnimCurve* lCurve_Rot_X;
	FbxAnimCurve* lCurve_Rot_Y;
	FbxAnimCurve* lCurve_Rot_Z;
	FbxAnimCurve* lCurve_Scaling_X;
	FbxAnimCurve* lCurve_Scaling_Y;
	FbxAnimCurve* lCurve_Scaling_Z;

	//add annotations
	if (animation->m_annotationTracks.getSize() > 0)
	{
		hkaAnnotationTrack& a_track = animation->m_annotationTracks[0];

		if (a_track.m_annotations.getSize() > 0)
		{
			//on first pass we create the enums
			for (int i = 0; i < a_track.m_annotations.getSize(); i++)
			{
				hkaAnnotationTrack::Annotation& this_hk_ann = a_track.m_annotations[i];
				string hk_value = this_hk_ann.m_text.cString();
				size_t index = 0;
				for (index = hk_value.size()-1; index >= 0; index--)
				{
					if (::isupper(hk_value[index]))
						break;
				}
				if (index > 0)
				{
					string e_type = hk_value.substr(0, index);
					string e_value = hk_value.substr(index, hk_value.size() - index);
					string out_name = "hk" + e_type;
					FbxProperty this_p = ordered_skeleton[0]->FindProperty(out_name.c_str());
					if (!this_p.IsValid()) {
						this_p = FbxProperty::Create(ordered_skeleton[0], FbxEnumDT, out_name.c_str());
						this_p.ModifyFlag(FbxPropertyFlags::eUserDefined, true);
						this_p.ModifyFlag(FbxPropertyFlags::eAnimatable, true);
					}
					//check that the enum already has the value
					bool already_inserted = false;
					size_t enum_values = this_p.GetEnumCount();
					int e_index = 0;
					for (e_index = 0; e_index < enum_values; e_index++) {
						string enum_val = this_p.GetEnumValue(e_index);
						if (enum_val == e_value) {
							already_inserted = true;
							break;
						}
					}
					if (!already_inserted)
					{
						e_index = enum_values;
						this_p.AddEnumValue(e_value.c_str());
					}
					//annotate curve
					FbxAnimCurve* fcurve = this_p.GetCurve(lAnimLayer, out_name.c_str(), true);
					fcurve->KeyModifyBegin();
					FbxTime time; time.SetSecondDouble(this_hk_ann.m_time);
					lKeyIndex = fcurve->KeyAdd(time);
					fcurve->KeySet(lKeyIndex, time, (float)e_index, FbxAnimCurveDef::eInterpolationConstant);
					fcurve->KeyModifyEnd();
				}
			}
		}
	}

	//prepare track vector
	vector<FbxProperty> floats_properties;

	for (int k = 0; k < FloatNumber; k++) {
		FbxProperty this_p = binding->m_floatTrackToFloatSlotIndices.getSize() > 0 ?
			float_tracks[binding->m_floatTrackToFloatSlotIndices[k]] :
			float_tracks[k];
		FbxAnimCurve* curve = this_p.GetCurve(lAnimLayer, true);

		if (this_p.IsValid() && curve != NULL)
		{
			floats_properties.push_back(this_p);
		}
	}

	// loop through keyframes
	for (hkReal time = 0.0; time<AnimDuration; time += incrFrame)
	{
		animation->samplePartialTracks(time, TrackNumber, transformOut.begin(), FloatNumber, floatsOut.begin(), NULL);
		hkaSkeletonUtils::normalizeRotations(transformOut.begin(), TrackNumber);

		// assume 1-to-1 transforms
		// loop through animated bones

		// todo support for anything else beside 30 fps?
		lTime.SetSecondDouble(time);

		for (int i = 0; i<TrackNumber; ++i)
		{
			FbxNode* CurrentJointNode = binding->m_transformTrackToBoneIndices.getSize() > 0 ?
				ordered_skeleton[binding->m_transformTrackToBoneIndices[i]] :
				ordered_skeleton[i];

			// Translation
			lCurve_Trans_X = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			lCurve_Trans_Y = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			lCurve_Trans_Z = CurrentJointNode->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			// Rotation
			lCurve_Rot_X = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			lCurve_Rot_Y = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			lCurve_Rot_Z = CurrentJointNode->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

			lCurve_Scaling_X = CurrentJointNode->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
			lCurve_Scaling_Y = CurrentJointNode->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
			lCurve_Scaling_Z = CurrentJointNode->LclScaling.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);


			hkQsTransform& transform = transformOut[i];
			const hkVector4& anim_pos = transform.getTranslation();
			const ::hkQuaternion& anim_rot = transform.getRotation();
			const hkVector4& anim_scal = transform.getScale();

			// Translation first
			lCurve_Trans_X->KeyModifyBegin();
			lKeyIndex = lCurve_Trans_X->KeyAdd(lTime);
			lCurve_Trans_X->KeySetValue(lKeyIndex, anim_pos.getSimdAt(0));
			lCurve_Trans_X->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Trans_X->KeyModifyEnd();

			lCurve_Trans_Y->KeyModifyBegin();
			lKeyIndex = lCurve_Trans_Y->KeyAdd(lTime);
			lCurve_Trans_Y->KeySetValue(lKeyIndex, anim_pos.getSimdAt(1));
			lCurve_Trans_Y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Trans_Y->KeyModifyEnd();

			lCurve_Trans_Z->KeyModifyBegin();
			lKeyIndex = lCurve_Trans_Z->KeyAdd(lTime);
			lCurve_Trans_Z->KeySetValue(lKeyIndex, anim_pos.getSimdAt(2));
			lCurve_Trans_Z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Trans_Z->KeyModifyEnd();

			// Rotation
			Quat QuatRotNew = { anim_rot.m_vec.getSimdAt(0), anim_rot.m_vec.getSimdAt(1), anim_rot.m_vec.getSimdAt(2), anim_rot.m_vec.getSimdAt(3) };
			EulerAngles inAngs_Animation = Eul_FromQuat(QuatRotNew, EulOrdXYZs);

			lCurve_Rot_X->KeyModifyBegin();
			lKeyIndex = lCurve_Rot_X->KeyAdd(lTime);
			lCurve_Rot_X->KeySetValue(lKeyIndex, float(rad2deg(inAngs_Animation.x)));
			lCurve_Rot_X->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Rot_X->KeyModifyEnd();

			lCurve_Rot_Y->KeyModifyBegin();
			lKeyIndex = lCurve_Rot_Y->KeyAdd(lTime);
			lCurve_Rot_Y->KeySetValue(lKeyIndex, float(rad2deg(inAngs_Animation.y)));
			lCurve_Rot_Y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Rot_Y->KeyModifyEnd();

			lCurve_Rot_Z->KeyModifyBegin();
			lKeyIndex = lCurve_Rot_Z->KeyAdd(lTime);
			lCurve_Rot_Z->KeySetValue(lKeyIndex, float(rad2deg(inAngs_Animation.z)));
			lCurve_Rot_Z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Rot_Z->KeyModifyEnd();

			//Scaling
			lCurve_Scaling_X->KeyModifyBegin();
			lKeyIndex = lCurve_Scaling_X->KeyAdd(lTime);
			lCurve_Scaling_X->KeySetValue(lKeyIndex, anim_scal.getSimdAt(0));
			lCurve_Scaling_X->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Scaling_X->KeyModifyEnd();

			lCurve_Scaling_Y->KeyModifyBegin();
			lKeyIndex = lCurve_Scaling_Y->KeyAdd(lTime);
			lCurve_Scaling_Y->KeySetValue(lKeyIndex, anim_scal.getSimdAt(1));
			lCurve_Scaling_Y->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Scaling_Y->KeyModifyEnd();

			lCurve_Scaling_Z->KeyModifyBegin();
			lKeyIndex = lCurve_Scaling_Z->KeyAdd(lTime);
			lCurve_Scaling_Z->KeySetValue(lKeyIndex, anim_scal.getSimdAt(2));
			lCurve_Scaling_Z->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationCubic);
			lCurve_Scaling_Z->KeyModifyEnd();
		}

		for (int k = 0; k < FloatNumber; k++) {
			FbxAnimCurve* curve = floats_properties[k].GetCurve(lAnimLayer, true);
			if (curve != NULL)
			{
				curve->KeyModifyBegin();
				lKeyIndex = curve->KeyAdd(lTime);
				curve->KeySet(lKeyIndex, time, floatsOut[k], FbxAnimCurveDef::eInterpolationConstant);
				curve->KeyModifyEnd();
			}
		}
	}
}

vector<FbxNode*> HKXWrapper::add(hkaSkeleton* skeleton, FbxNode* scene_root, vector<FbxProperty>& float_tracks)
{
	
	// get number of bones and apply reference pose
	const int numBones = skeleton->m_bones.getSize();

	map<int, FbxNode*> conversion_map;

	// first track is actually a dummy, find the real root


	// create base limb objects first
	for (hkInt16 b = 0; b < numBones; b++)
	{
		const hkaBone& bone = skeleton->m_bones[b];

		string b_name = bone.m_name;
		sanitizeString(b_name);

		hkQsTransform localTransform = skeleton->m_referencePose[b];
		const hkVector4& pos = localTransform.getTranslation();
		const ::hkQuaternion& rot = localTransform.getRotation();

		FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(scene_root->GetScene(), b_name.c_str());

		//TODO: fix with ragdoll mapping
		if (b == 0)
			lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eRoot);
		else
			lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);

		lSkeletonLimbNodeAttribute1->Size.Set(1.0);
		FbxNode* BaseJoint = FbxNode::Create(scene_root->GetScene(), b_name.c_str());
		set_property(BaseJoint, "bone_order", b, FbxShortDT);
		BaseJoint->SetNodeAttribute(lSkeletonLimbNodeAttribute1);

		// Set Translation
		BaseJoint->LclTranslation.Set(FbxVector4(pos.getSimdAt(0), pos.getSimdAt(1), pos.getSimdAt(2)));

		// convert quat to euler
		Quat QuatTest = { rot.m_vec.getSimdAt(0), rot.m_vec.getSimdAt(1), rot.m_vec.getSimdAt(2), rot.m_vec.getSimdAt(3) };
		EulerAngles inAngs = Eul_FromQuat(QuatTest, EulOrdXYZs);
		BaseJoint->LclRotation.Set(FbxVector4(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z)));

		scene_root->GetScene()->GetRootNode()->AddChild(BaseJoint);
		conversion_map[b] = BaseJoint;
	}

	vector<FbxNode*> ordered_skeleton(conversion_map.size());

	// process parenting and transform now
	for (int c = 0; c < numBones; c++)
	{
		const hkInt32& parent = skeleton->m_parentIndices[c];
		ordered_skeleton[c] = conversion_map[c];
		if (parent != -1)
		{
			FbxNode* ParentJointNode = conversion_map[parent];
			FbxNode* CurrentJointNode = conversion_map[c];
			ParentJointNode->AddChild(CurrentJointNode);
		}
	}

	//add floats properties
	int float_size = skeleton->m_floatSlots.getSize();
	float_tracks.resize(float_size);
	for (int i = 0; i < float_size; i++)
	{

		//check if property has a valid parent
		string track_name = skeleton->m_floatSlots[i].cString();
		int index = track_name.find(":");

		if (index != string::npos)
		{
			string parent = track_name.substr(index + 1, track_name.size());
			sanitizeString(parent);
			track_name = track_name.substr(0, index);
			FbxNode* parent_node = ordered_skeleton[0]->GetScene()->FindNodeByName(parent.c_str());
			if (parent_node != NULL)
			{
				float_tracks[i] = FbxProperty::Create(parent_node, FbxFloatDT, track_name.c_str());
				set_property(ordered_skeleton[0], ("float" + parent).c_str(), i, FbxShortDT);
			}
			else {
				float_tracks[i] = FbxProperty::Create(ordered_skeleton[0], FbxFloatDT, track_name.c_str());
			}
		}
		else {
			float_tracks[i] = FbxProperty::Create(ordered_skeleton[0], FbxFloatDT, track_name.c_str());
		}

		//to be sure to preserve 
		
		float_tracks[i].ModifyFlag(FbxPropertyFlags::eUserDefined, true);
		float_tracks[i].ModifyFlag(FbxPropertyFlags::eAnimatable, true);
	}

	return ordered_skeleton;
}

vector<FbxNode*> HKXWrapper::load_skeleton(const fs::path& path, FbxNode* scene_root, vector<FbxProperty>& float_tracks)
{
	vector<string> ordered_tracks;
	hkArray<hkVariant> objects;
	read(path, objects);
	hkaAnimationContainer* anim_container;
	hkpPhysicsData* physics_data;
	hkaRagdollInstance* ragdoll_instance;
	vector<hkaSkeleton*> skeletons;
	vector<hkaSkeletonMapper*> mappers;

	hkaSkeleton* animation_skeleton;
	hkaSkeleton* ragdoll_skeleton;

	for (const auto& variant : objects) {
		//read skeletons
		if (strcmp(variant.m_class->getName(), "hkaSkeleton") == 0)
		{
			skeletons.push_back((hkaSkeleton*)variant.m_object);
		}
		if (strcmp(variant.m_class->getName(), "hkpPhysicsData") == 0)
		{
			physics_data = (hkpPhysicsData*)variant.m_object;
		}
		if (strcmp(variant.m_class->getName(), "hkaRagdollInstance") == 0)
		{
			ragdoll_instance = (hkaRagdollInstance*)variant.m_object;
		}
		if (strcmp(variant.m_class->getName(), "hkaSkeletonMapper") == 0)
		{
			mappers.push_back((hkaSkeletonMapper*)variant.m_object);
		}
	}
	if (skeletons.empty())
		return {};
	if (skeletons.size() > 1)
	{
		if (ragdoll_instance == NULL)
		{
			Log::Warn("Multiple skeleton detected. Only files with one skeleton and one ragdoll (optional) are supported!");
			animation_skeleton = skeletons[0];
		}
		else
		{
			ragdoll_instance->m_skeleton == skeletons[0] ?
				animation_skeleton = skeletons[1], ragdoll_skeleton = skeletons[0] :
				animation_skeleton = skeletons[0], ragdoll_skeleton = skeletons[1];
		}
	}

	Log::Info("Animation Skeleton: %s", animation_skeleton->m_name.cString());
	if (ragdoll_instance != NULL)
		Log::Info("Ragdoll Skeleton: %s", ragdoll_skeleton->m_name.cString());
	return add(animation_skeleton, scene_root, float_tracks);
}

void HKXWrapper::load_animation(const fs::path& path, vector<FbxNode*>& ordered_skeleton, vector<FbxProperty>& float_tracks)
{
	hkArray<hkVariant> objects;
	read(path, objects);

	hkaAnimationContainer* container = NULL;

	hkaAnimation* animation;
	hkaAnimationBinding* binding;

	for (const auto& variant : objects) {
		//read skeletons
		if (strcmp(variant.m_class->getName(), "hkaAnimationContainer") == 0)
		{
			container = (hkaAnimationContainer*)variant.m_object;
		}
	}

	if (container == NULL) {
		Log::Info("Error: cannot find an animation container inside the file %s", path.string().c_str());
		return;
	}
	animation = container->m_animations[0];
	binding = container->m_bindings[0];

	const string debug = path.filename().string();
	const string name = path.filename().replace_extension("").string();
	add(name, animation, binding, ordered_skeleton, float_tracks);

}

struct bmeshinfo
{
	vector<float> points;
	vector<int> triangles;
};

void convert_geometry(shared_ptr<bmeshinfo> bmesh, pair<FbxAMatrix, FbxMesh*> translated_mesh)
{
	FbxMesh* mesh = translated_mesh.second;
	size_t vertices_count = mesh->GetControlPointsCount();
	size_t map_offset = bmesh->points.size() / 3;
	for (int i = 0; i < vertices_count; i++) {
		FbxVector4 vertex = translated_mesh.first.MultT(mesh->GetControlPointAt(i));
		bmesh->points.push_back(vertex[0]);
		bmesh->points.push_back(vertex[1]);
		bmesh->points.push_back(vertex[2]);
	}

	size_t tris_count = mesh->GetPolygonCount();

	for (int i = 0; i < tris_count; i++) {
		int v1 = mesh->GetPolygonVertex(i, 0);
		int v2 = mesh->GetPolygonVertex(i, 1);
		int v3 = mesh->GetPolygonVertex(i, 2);

		bmesh->triangles.push_back(v1 + map_offset);
		bmesh->triangles.push_back(v2 + map_offset);
		bmesh->triangles.push_back(v3 + map_offset);
	}
}

void convert_hkgeometry(hkGeometry& geometry, pair<FbxAMatrix, FbxMesh*> translated_mesh, vector<hkpNamedMeshMaterial>& materials, double scaling)
{
	FbxMesh* mesh = translated_mesh.second;
	size_t vertices_count = mesh->GetControlPointsCount();
	int map_offset = geometry.m_vertices.getSize();
	for (int i = 0; i < vertices_count; i++) {
		FbxVector4 vertex = translated_mesh.first.MultT(mesh->GetControlPointAt(i));
		geometry.m_vertices.pushBack(
			{ (hkReal)(vertex[0] * scaling),  (hkReal)(vertex[1] * scaling), (hkReal)(vertex[2] *scaling) }
		);
	}

	vector<int> materials_map;
	int materials_offset = materials.size();
	FbxNode* parent = mesh->GetNode();
	if (parent)
	{
		int materials_size = parent->GetMaterialCount();
		if (materials_size == 0) materials_size = 1;
		for (int i = 0; i < materials_size; i++)
		{
			FbxSurfaceMaterial* collision_material = parent->GetMaterial(i);
			string name;
			if (collision_material == NULL)
				name = "SKY_HAV_MAT_STONE";
			else
			    name = collision_material->GetName();
			
			FbxProperty layer; 
			if (collision_material != NULL)
				layer = collision_material->FindProperty("CollisionLayer");
			string collision_layer_name;
			if (!layer.IsValid())
				collision_layer_name = "SKYL_STATIC";
			else
				collision_layer_name = layer.Get<FbxString>().Buffer();

			SkyrimLayer layer_value = NifFile::layer_value(collision_layer_name);
			//find it on the materials vector
			int material_index = -1;
			for (int m = 0; m< materials.size(); m++)
			{ 
				string this_material_name = materials[m].m_name;
				SkyrimLayer this_layer_value = (SkyrimLayer)materials[m].m_filterInfo;
				if (name == this_material_name && layer_value == this_layer_value)
				{
					material_index = m;
					materials_map.push_back(m);
					break;
				}
			}
			if (material_index == -1)
			{
				materials_map.push_back(materials.size());
				hkpNamedMeshMaterial new_material;
				new_material.m_name = name.c_str();
				new_material.m_filterInfo = layer_value;
				materials.push_back(new_material);
			}		
		}
	}

	size_t tris_count = mesh->GetPolygonCount();
	const FbxLayerElementMaterial* pPolygonMaterials = mesh->GetElementMaterial();
	for (int i = 0; i < tris_count; i++) {
		int v1 = mesh->GetPolygonVertex(i, 0);
		int v2 = mesh->GetPolygonVertex(i, 1);
		int v3 = mesh->GetPolygonVertex(i, 2);
		int material_index = 0;
		if (pPolygonMaterials)
			material_index = pPolygonMaterials->mIndexArray->GetAt(i);

		geometry.m_triangles.pushBack(
			{ v1 + map_offset, v2 + map_offset , v3 + map_offset, materials_map[material_index] }
		);
	}
}

hkGeometry extract_bounding_geometry(FbxNode* shape_root, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes, vector<hkpNamedMeshMaterial>& materials, hkpMassProperties& properties, double scaling)
{
	hkGeometry out;
	if (shape_root->GetNodeAttribute() != NULL &&
		shape_root->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		convert_hkgeometry(out, {FbxAMatrix(), (FbxMesh*)shape_root->GetNodeAttribute() }, materials, scaling);
	}
	else {
		for (const auto& mesh : geometry_meshes)
		{
			convert_hkgeometry(out, mesh, materials, scaling);
		}
	}
	if (properties.m_mass == 0.0)
	{
		vector<hkGeometry> geometry_by_material(materials.size());
		//calculate approximate mass, split the geometry by materials
		for (int i = 0; i < out.m_triangles.getSize(); i++)
		{
			hkGeometry& this_geometry = geometry_by_material[out.m_triangles[i].m_material];
			hkGeometry::Triangle t;
			t.m_a = this_geometry.m_vertices.getSize(); this_geometry.m_vertices.pushBack(out.m_vertices[out.m_triangles[i].m_a]);
			t.m_b = this_geometry.m_vertices.getSize(); this_geometry.m_vertices.pushBack(out.m_vertices[out.m_triangles[i].m_b]);
			t.m_c = this_geometry.m_vertices.getSize(); this_geometry.m_vertices.pushBack(out.m_vertices[out.m_triangles[i].m_c]);
			this_geometry.m_triangles.pushBack(t);
		}
		for (const auto& geom : geometry_by_material)
		{
			hkReal volume = hkGeometryUtils::computeVolume(geom);
			double density = 1.0; //TODO;
			properties.m_mass += volume * density;
		}
	}

	return out;
}

/// You should avoid using a transform shape as the root shape of a rigid body.
hkpShape* handle_output_transform(hkpCreateShapeUtility::ShapeInfoOutput& output, hkpNamedMeshMaterial* material, hkpMassProperties& properties, FbxNode* shape, FbxNode* body, hkpRigidBodyCinfo& hk_body)
{
	output.m_shape->setUserData((hkUlong)material);
	if (!output.m_extraShapeTransform.isApproximatelyEqual(hkTransform::getIdentity()))
	{
		//see if the transformed shape is a simple shape
		if (shape->GetParent() == body)
		{
			//just use the body transform to avoid the transform shape
			hk_body.setTransform(output.m_extraShapeTransform);
		}
		else {
			hkArray<hkpMassElement> element(1);
			element[0].m_properties = properties;
			element[0].m_transform = output.m_extraShapeTransform;
			hkInertiaTensorComputer::combineMassProperties(element, properties);
			//add a transform
			if (output.m_isConvex)
				return new hkpConvexTransformShape((hkpConvexShape*)output.m_shape, output.m_extraShapeTransform);
			else
				return new hkpTransformShape(output.m_shape, output.m_extraShapeTransform);
		}
	}
	return output.m_shape;
}

FbxNode* create_hulls(FbxManager* manager, VHACD::IVHACD* interfaceVHACD)
{
	unsigned int nConvexHulls = interfaceVHACD->GetNConvexHulls();
	FbxNode* list = NULL;
	if (nConvexHulls > 1)
	{
		list = FbxNode::Create(manager, "_list");
	}
	for (unsigned int p = 0; p < nConvexHulls; ++p) 
	{
		VHACD::IVHACD::ConvexHull ch;

		interfaceVHACD->GetConvexHull(p, ch);

		FbxNode* temp_convex = FbxNode::Create(manager, "_convex");
		if (list != NULL)
			list->AddChild(temp_convex);
		else
			list = temp_convex;
		

		FbxMesh* m = FbxMesh::Create(manager, "_convex_mesh");
		m->InitControlPoints(ch.m_nPoints);
		FbxVector4* points = m->GetControlPoints();

		for (int i = 0; i < ch.m_nPoints; i++) {
			points[i] = FbxVector4(ch.m_points[3 * i], ch.m_points[3 * i + 1], ch.m_points[3 * i + 2]);
		}
		for (int i = 0; i < ch.m_nTriangles; i++) {
			m->BeginPolygon(0);
			m->AddPolygon(ch.m_triangles[3 * i]);
			m->AddPolygon(ch.m_triangles[3 * i + 1]);
			m->AddPolygon(ch.m_triangles[3 * i + 2]);
			m->EndPolygon();
		}

		temp_convex->AddNodeAttribute(m);
	}
	return list;
}

FbxNode* create_mesh(FbxManager* manager, VHACD::IVHACD* interfaceVHACD)
{
	FbxNode* root = FbxNode::Create(manager, "_mesh");
	FbxMesh* m = FbxMesh::Create(manager, "_mesh_tris");
	size_t current_points = 0;
	unsigned int nConvexHulls = interfaceVHACD->GetNConvexHulls();
	for (unsigned int p = 0; p < nConvexHulls; ++p)
	{
		VHACD::IVHACD::ConvexHull ch;
		interfaceVHACD->GetConvexHull(p, ch);
		current_points += ch.m_nPoints;
	}

	m->InitControlPoints(current_points);

	current_points = 0;
	FbxVector4* points = m->GetControlPoints();

	for (unsigned int p = 0; p < nConvexHulls; ++p)
	{
		VHACD::IVHACD::ConvexHull ch;
		interfaceVHACD->GetConvexHull(p, ch);
		for (int i = 0; i < ch.m_nPoints; i++) {
			points[current_points + i] = FbxVector4(ch.m_points[3 * i], ch.m_points[3 * i + 1], ch.m_points[3 * i + 2]);
		}
		
		for (int i = 0; i < ch.m_nTriangles; i++) {
			m->BeginPolygon(0);
			m->AddPolygon(ch.m_triangles[3 * i] + current_points);
			m->AddPolygon(ch.m_triangles[3 * i + 1] + current_points);
			m->AddPolygon(ch.m_triangles[3 * i + 2] + current_points);
			m->EndPolygon();
		}
		current_points += ch.m_nPoints;
	}

	root->AddNodeAttribute(m);
	return root;
}

static inline hkVector4 TOVECTOR4(const Niflib::Vector4& v) {
	return hkVector4(v.x, v.y, v.z, v.w);
}

static inline hkVector4 TOVECTOR4(const FbxVector4& v) {
	return hkVector4(v.mData[0], v.mData[1], v.mData[2], v.mData[3]);
}

static inline hkMatrix4 TOMATRIX4(const Niflib::Matrix44& q, float scale=1.0) {
	hkMatrix4 m4;
	m4.setCols(TOVECTOR4(q.rows[0]), TOVECTOR4(q.rows[1]), TOVECTOR4(q.rows[2]), TOVECTOR4(q.rows[3]));
	m4(3, 0) = m4(3, 0) * scale;
	m4(3, 1) = m4(3, 1) * scale;
	m4(3, 2) = m4(3, 2) * scale;
	return m4;
}


void extract_geometry(vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes, hkGeometry& out)
{
	for (const auto& pair : geometry_meshes)
	{
		hkGeometry this_geom;
		const auto& vertices = pair.second->GetData()->GetVertices();
		this_geom.m_vertices.reserve(vertices.size());
		for (const auto& v : vertices)
			this_geom.m_vertices.pushBack(TOVECTOR4(v));
		const auto& triangles = DynamicCast<NiTriShapeData>(pair.second->GetData())->GetTriangles();
		this_geom.m_triangles.reserve(triangles.size());
		for (const auto& t : triangles)
		{
			hkGeometry::Triangle out;
			out.m_a = t.v1;
			out.m_b = t.v2;
			out.m_c = t.v3;
			this_geom.m_triangles.pushBack(out);
		}
		hkMatrix4 tt; tt.set(pair.first);
		hkGeometryUtils::transformGeometry(tt, this_geom);
		hkGeometryUtils::appendGeometry(this_geom, out);
	}
}

void extract_geometry(const bhkShapeRef shape_root, double scale_factor, hkGeometry& out)
{
	//If shape_root is null, no hints were given on how to handle the collisions
	if (shape_root == NULL)
	{
		//ntd;
		return;
	}
	if (shape_root->IsDerivedType(bhkTransformShape::TYPE))
	{
		hkGeometry sub_geom;
		bhkTransformShapeRef tshape = DynamicCast<bhkTransformShape>(shape_root);
		extract_geometry(tshape->GetShape(), scale_factor, out);
		hkGeometryUtils::transformGeometry(TOMATRIX4(tshape->GetTransform(), scale_factor), sub_geom);
		hkGeometryUtils::appendGeometry(sub_geom, out);
		return;
	}
	if (shape_root->IsDerivedType(bhkListShape::TYPE))
	{
		bhkListShapeRef tshape = DynamicCast<bhkListShape>(shape_root);
		const auto& sub_shapes = tshape->GetSubShapes();
		hkArray<hkpMassElement> sub_elements;
		for (auto& shape : sub_shapes)
		{
			extract_geometry(shape, scale_factor, out);
		}
		return;
	}
	if (shape_root->IsDerivedType(bhkMoppBvTreeShape::TYPE))
	{
		bhkMoppBvTreeShapeRef bv = DynamicCast<bhkMoppBvTreeShape>(shape_root);
		extract_geometry(bv->GetShape(), scale_factor, out);
		return;
	}
	//shapes
	if (shape_root->IsDerivedType(bhkSphereShape::TYPE))
	{
		bhkSphereShapeRef bv = DynamicCast<bhkSphereShape>(shape_root);
		hkpSphereShape hksph(bv->GetRadius()*scale_factor);
		hkGeometry* sphere_geom = hkpShapeConverter::toSingleGeometry(&hksph);
		hkGeometryUtils::appendGeometry(*sphere_geom, out);
		delete sphere_geom;
		return;
	}
	if (shape_root->IsDerivedType(bhkBoxShape::TYPE))
	{
		bhkBoxShapeRef bv = DynamicCast<bhkBoxShape>(shape_root);
		hkpBoxShape hkbx(TOVECTOR4(bv->GetDimensions()*scale_factor), bv->GetRadius()*scale_factor);
		hkGeometry* box_geom = hkpShapeConverter::toSingleGeometry(&hkbx);
		hkGeometryUtils::appendGeometry(*box_geom, out);
		delete box_geom;
		return;
	}
	if (shape_root->IsDerivedType(bhkCapsuleShape::TYPE))
	{
		bhkCapsuleShapeRef bv = DynamicCast<bhkCapsuleShape>(shape_root);
		hkpCapsuleShape hkcp(TOVECTOR4(bv->GetFirstPoint()*scale_factor), TOVECTOR4(bv->GetSecondPoint()*scale_factor), bv->GetRadius()*scale_factor);
		hkGeometry* caps_geom = hkpShapeConverter::toSingleGeometry(&hkcp);
		hkGeometryUtils::appendGeometry(*caps_geom, out);
		delete caps_geom;
		return;
	}
	if (shape_root->IsDerivedType(bhkConvexVerticesShape::TYPE))
	{
		bhkConvexVerticesShapeRef cvs = DynamicCast<bhkConvexVerticesShape>(shape_root);
		hkStridedVertices stridedVertsIn;
		auto& verts = cvs->GetVertices();
		stridedVertsIn.set(verts.data(), verts.size());
		hkGeometry convex;
		hkArray<hkVector4> planeEquationsOut;
		hkGeometryUtility::createConvexGeometry(stridedVertsIn, convex, planeEquationsOut);
		hkStridedVertices stridedVertsOut(convex.m_vertices);
		hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(NifFile::material_name(cvs->GetMaterial().material_sk));
		hkpShape* convex_shape = new hkpConvexVerticesShape(convex.m_vertices, planeEquationsOut);
		//hkInertiaTensorComputer::computeVertexHullVolumeMassProperties(stridedVertsOut.m_vertices, stridedVertsOut.m_striding, stridedVertsOut.m_numVertices, mass, properties);
		convex_shape->setUserData((hkUlong)material);
		return;
	}
	//if (shape_root->IsDerivedType(bhkCompressedMeshShape::TYPE))
	//{
	//	bhkCompressedMeshShapeRef cms = DynamicCast<bhkCompressedMeshShape>(shape_root);
	//	bhkCompressedMeshShapeDataRef cms_data = cms->GetData();
	//	hkpCompressedMeshShapeBuilder			shapeBuilder;
	//	shapeBuilder.m_stripperPasses = 5000;
	//	hkpCompressedMeshShape* pCompMesh = shapeBuilder.createMeshShape(0.001f, hkpCompressedMeshShape::MATERIAL_SINGLE_VALUE_PER_CHUNK);
	//	
	//	//todo
	//	//cms_data->
	//	//pCompMesh->m_namedMaterials.setSize(materials.size());
	//	//for (int i = 0; i < materials.size(); i++)
	//	//	pCompMesh->m_namedMaterials[i] = materials[i];



	//	try {
	//		//  add geometry to shape
	//		int										subPartId(0);
	//		subPartId = shapeBuilder.beginSubpart(pCompMesh);
	//		shapeBuilder.addGeometry(to_bound, hkMatrix4::getIdentity(), pCompMesh);
	//		shapeBuilder.endSubpart(pCompMesh);
	//		shapeBuilder.addInstance(subPartId, hkMatrix4::getIdentity(), pCompMesh);

	//		//add materials to shape
	//		for (int i = 0; i < materials.size(); i++) {
	//			pCompMesh->m_materials.pushBack(i);
	//		}
	//		//check connectivity
	//		hkpConvexVerticesConnectivity connector;
	//		for (const auto& t : to_bound.m_triangles)
	//		{
	//			int tt[3]; tt[0] = t.m_a; tt[1] = t.m_b; tt[2] = t.m_c;
	//			connector.addFace(tt, 3);
	//		}
	//		if (connector.isClosed())
	//		{
	//			hkInertiaTensorComputer::computeGeometryVolumeMassPropertiesChecked(&to_bound, mass, properties);
	//		}
	//		else {
	//			hkInertiaTensorComputer::computeGeometrySurfaceMassProperties(&to_bound, 0.1, true, mass, properties);
	//		}
	//		return pCompMesh;
	//	}
	//	catch (...) {
	//		throw runtime_error("Unable to calculate MOPP code!");
	//	}
	//}
	return;
}

std::map< FbxNode*, hkpRigidBody*> bodies;
std::map< FbxNode*, hkaBone*> boneMap;
std::vector< hkpRigidBody*> rigidBodies;
std::set< hkpConstraintInstance*> constraints;

void HKXWrapper::create_skeleton(FbxNode* bone)
{
	skeleton = new hkaSkeleton();
	string root_name = "NPC Root [Root]";
	sanitizeString(root_name);
	skeleton->m_name = "NPC Root [Root]";
	bone->SetName(root_name.c_str());
	skeleton->m_parentIndices.setSize(1);
	skeleton->m_bones.setSize(1);
	skeleton->m_referencePose.setSize(1);
	skeleton->m_parentIndices[0] = -1;
	skeleton->m_bones[0].m_name = "NPC Root [Root]";
	skeleton->m_bones[0].m_lockTranslation = false;
	auto transform = getTransform(bone);
	skeleton->m_referencePose[0].setTranslation(transform.getTranslation());
	skeleton->m_referencePose[0].setRotation(transform.getRotation());
	skeleton->m_referencePose[0].setScale(hkVector4(1.000000, 1.000000, 1.000000, 0.000000));

	FbxProperty prop = bone->GetFirstProperty();
	while (prop.IsValid()) {
		string name = prop.GetNameAsCStr();
		if (name.rfind("float", 0) == 0) {
			float_map[name.substr(5, name.size() - 5)] = get_property<FbxShort>(bone, name.c_str());
		} else if (prop.GetFlag(FbxPropertyFlags::eUserDefined) == true &&
			prop.GetFlag(FbxPropertyFlags::eAnimatable) == true &&
			prop.GetPropertyDataType() == FbxFloatDT)
		{
			skeleton->m_floatSlots.pushBack((const char*)prop.GetName());
			skeleton->m_referenceFloats.pushBack(getFloatTrackValue(prop,0.0));
		}
		prop = bone->GetNextProperty(prop);
	}
}

void HKXWrapper::add_bone(FbxNode* bone)
{
	//find parent
	string parent_name = bone->GetParent()->GetName();
	parent_name = unsanitizeString(parent_name);
	string bone_name = bone->GetName();
	bone_name = unsanitizeString(bone_name);
	int parent_index = -1;
	for (int b = 0; b < skeleton->m_bones.getSize(); b++)
	{
		if (skeleton->m_bones[b].m_name && strcmp(skeleton->m_bones[b].m_name.cString(), parent_name.c_str()) == 0)
		{
			parent_index = b;
			break;
		}
	}
	if (parent_index == -1)
	{
		Log::Warn("Cannot find parent bone of %s (%s)", bone->GetName(), bone->GetParent()->GetName());
	}
	int bone_index = -1;
	if (bone->FindProperty("bone_order").IsValid())
	{
		bone_index = get_property<FbxShort>(bone, "bone_order");
		Log::Info("Found preset bone order: %d",bone_index);
	}
	else bone_index = skeleton->m_bones.getSize();
	if (bone_index >= skeleton->m_bones.getSize())
	{
		skeleton->m_parentIndices.setSize(bone_index + 1);
		skeleton->m_bones.setSize(bone_index + 1);
		skeleton->m_referencePose.setSize(bone_index + 1);
	}

	skeleton->m_parentIndices[bone_index] = parent_index;
	skeleton->m_bones[bone_index].m_name = bone_name.c_str();
	skeleton->m_bones[bone_index].m_lockTranslation = true;
	auto transform = getTransform(bone);
	skeleton->m_referencePose[bone_index].setTranslation(transform.getTranslation());
	skeleton->m_referencePose[bone_index].setRotation(transform.getRotation());
	skeleton->m_referencePose[bone_index].setScale(hkVector4(1.000000, 1.000000, 1.000000, 0.000000));

	FbxProperty prop = bone->GetFirstProperty();
	while (prop.IsValid()) {
		if (prop.GetFlag(FbxPropertyFlags::eUserDefined) == true &&
			prop.GetFlag(FbxPropertyFlags::eAnimatable) == true &&
			prop.GetPropertyDataType() == FbxFloatDT)
		{
			int float_index = -1;
			if (float_map.find(bone->GetName()) != float_map.end())
			{
				float_index = float_map[bone->GetName()];
				Log::Info("Found preset float order: %d", float_index);
			}
			else float_index = skeleton->m_floatSlots.getSize();
			if (float_index >= skeleton->m_floatSlots.getSize())
			{
				skeleton->m_floatSlots.setSize(float_index + 1);
			}
			skeleton->m_floatSlots[float_index] = (string(prop.GetNameAsCStr()) + ":" + bone_name).c_str();
			skeleton->m_referenceFloats.pushBack(getFloatTrackValue(prop, 0.0));
		}
		prop = bone->GetNextProperty(prop);
	}
}
hkpPhysicsSystem* physic_entities = NULL;
hkaSkeletonMapperData* fromRagdollToSkeletonMapping = NULL;

//		Log::Info("Build Mappings Ragdoll -> Skeleton\n");
//
//		hkaSkeletonMapperData* fromRagdollToSkeletonMapping = new hkaSkeletonMapperData();
//		fromRagdollToSkeletonMapping->m_simpleMappings.setSize(rigidBodies.size());
//		fromRagdollToSkeletonMapping->m_skeletonA = hkRagdollSkeleton;
//		fromRagdollToSkeletonMapping->m_skeletonB = hkSkeleton;
//		set<int> mappedBones;
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			hkaSkeletonMapperData::SimpleMapping& mapping = fromRagdollToSkeletonMapping->m_simpleMappings[i];
//			mapping.m_boneA = i;
//			mapping.m_boneB = ragdollAnimationParentMap[i];
//			mappedBones.insert(ragdollAnimationParentMap[i]);
//
//			//Absolute transform
//			int findroot = ragdollParentMap[i];
//			hkQsTransform ragdollBoneTransform = hkRagdollSkeleton->m_referencePose[i];
//			while (findroot != -1) {
//				ragdollBoneTransform.setMul(hkRagdollSkeleton->m_referencePose[findroot], ragdollBoneTransform);
//				findroot = ragdollParentMap[findroot];
//			}
//
//			NiNodeRef animationBone = bones[ragdollAnimationParentMap[i]];
//
//			hkQsTransform animationBoneTransform;
//			double scale = 1.0;
//			animationBoneTransform.setTranslation(TOVECTOR4(animationBone->GetWorldTransform().GetTranslation()*scale));
//			animationBoneTransform.setRotation(TOQUAT(animationBone->GetWorldTransform().GetRotation().AsQuaternion()));
//			animationBoneTransform.setScale(hkVector4(animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale()));
//
//			mapping.m_aFromBTransform.setMulInverseMul(ragdollBoneTransform, animationBoneTransform);
//		}
//
//		for (int i = 0; i < bones.size(); i++) {
//			if (mappedBones.find(i) == mappedBones.end())
//				fromRagdollToSkeletonMapping->m_unmappedBones.pushBack(i);
//		}
//
//		hkRefPtr<hkaSkeletonMapper> ragdollToAnimationMapper = new hkaSkeletonMapper(*fromRagdollToSkeletonMapping);
//
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("SkeletonMapper", ragdollToAnimationMapper.val(), &ragdollToAnimationMapper->staticClass()));
//
//
//		Log::Info("Build Mappings Skeleton -> Ragdoll\n");
//
//		hkaSkeletonMapperData* fromSkeletonToRagdollMapping = new hkaSkeletonMapperData();
//		fromSkeletonToRagdollMapping->m_simpleMappings.setSize(rigidBodies.size());
//		fromSkeletonToRagdollMapping->m_skeletonA = hkSkeleton;
//		fromSkeletonToRagdollMapping->m_skeletonB = hkRagdollSkeleton;
//
//		for (size_t i = 0; i < rigidBodies.size(); i++) {
//			hkaSkeletonMapperData::SimpleMapping& mapping = fromSkeletonToRagdollMapping->m_simpleMappings[i];
//			mapping.m_boneA = ragdollAnimationParentMap[i];
//			mapping.m_boneB = i;
//			mappedBones.insert(ragdollAnimationParentMap[i]);
//
//			//Absolute transform
//			int findroot = ragdollParentMap[i];
//			hkQsTransform ragdollBoneTransform = hkRagdollSkeleton->m_referencePose[i];
//			while (findroot != -1) {
//				ragdollBoneTransform.setMul(hkRagdollSkeleton->m_referencePose[findroot], ragdollBoneTransform);
//				findroot = ragdollParentMap[findroot];
//			}
//
//			NiNodeRef animationBone = bones[ragdollAnimationParentMap[i]];
//
//			hkQsTransform animationBoneTransform;
//			double scale = 1.0;
//			animationBoneTransform.setTranslation(TOVECTOR4(animationBone->GetWorldTransform().GetTranslation()*scale));
//			animationBoneTransform.setRotation(TOQUAT(animationBone->GetWorldTransform().GetRotation().AsQuaternion()));
//			animationBoneTransform.setScale(hkVector4(animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale(), animationBone->GetWorldTransform().GetScale()));
//
//			mapping.m_aFromBTransform.setMulInverseMul(animationBoneTransform, ragdollBoneTransform);
//		}
//
//		hkRefPtr<hkaSkeletonMapper> animationToRagdollMapper = new hkaSkeletonMapper(*fromSkeletonToRagdollMapping);
//
//		rootCont.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("SkeletonMapper", animationToRagdollMapper.val(), &animationToRagdollMapper->staticClass()));
//


void HKXWrapper::build_skeleton_from_ragdoll()
{
	if (constraints.size() == rigidBodies.size() - 1)
	{
		hkArray<hkpRigidBody*> hkRigidBodies;
		for (const auto body : rigidBodies)
			hkRigidBodies.pushBack(body);
		hkArray<hkpConstraintInstance*> hkConstraints;
		for (const auto con : constraints)
			hkConstraints.pushBack(con); 
		hkaRagdollUtils::reorderAndAlignForRagdoll(hkRigidBodies, hkConstraints);
		hkaSkeleton* ragdoll_skeleton = hkaRagdollUtils::constructSkeletonForRagdoll(hkRigidBodies, hkConstraints);

		auto ragdoll = hkaRagdollUtils::createRagdollInstanceFromSkeleton(ragdoll_skeleton, hkRigidBodies, hkConstraints);

		hkaSkeleton* hkRagdollSkeleton = ragdoll_skeleton;
		hkaSkeleton* hkSkeleton = skeleton;
		Log::Info("Build Mappings Ragdoll -> Skeleton\n");

		
		vector<int> ragdollAnimationParentMap(rigidBodies.size());
		for (int rb = 0; rb < rigidBodies.size(); rb++)
		{
			string rb_name = hkRigidBodies[rb]->getName();
			size_t offset = sizeof("Ragdoll_") - 1;
			string animation_name = rb_name.substr(offset, rb_name.size() - offset);
			for (int ab = 0; ab < hkSkeleton->m_bones.getSize(); ab++)
			{
				auto& bone = hkSkeleton->m_bones[ab];
				if (strcmp(bone.m_name, animation_name.c_str()) == 0)
				{
					ragdollAnimationParentMap[rb] = ab;
					break;
				}
			}
		}

		//vector<int> animationRagdollParentMap;

		hkaSkeletonMapperData* fromRagdollToSkeletonMapping = new hkaSkeletonMapperData();
		fromRagdollToSkeletonMapping->m_simpleMappings.setSize(rigidBodies.size());
		fromRagdollToSkeletonMapping->m_skeletonA = hkRagdollSkeleton;
		fromRagdollToSkeletonMapping->m_skeletonB = hkSkeleton;
		set<int> mappedBones;
		for (size_t i = 0; i < rigidBodies.size(); i++) {
			hkaSkeletonMapperData::SimpleMapping& mapping = fromRagdollToSkeletonMapping->m_simpleMappings[i];
			mapping.m_boneA = i;
			mapping.m_boneB = ragdollAnimationParentMap[i];
			mappedBones.insert(ragdollAnimationParentMap[i]);

			//Absolute transform
			int findroot = ragdoll_skeleton->m_parentIndices[i];
			hkQsTransform ragdollBoneTransform = hkRagdollSkeleton->m_referencePose[i];
			while (findroot != -1) {
				ragdollBoneTransform.setMul(hkRagdollSkeleton->m_referencePose[findroot], ragdollBoneTransform);
				findroot = ragdoll_skeleton->m_parentIndices[findroot];
			}

			int animationBone = ragdollAnimationParentMap[i];
			findroot = skeleton->m_parentIndices[animationBone];
			hkQsTransform animationBoneTransform = skeleton->m_referencePose[animationBone];
			while (findroot != -1) {
				animationBoneTransform.setMul(hkSkeleton->m_referencePose[findroot], animationBoneTransform);
				findroot = skeleton->m_parentIndices[findroot];
			}

			mapping.m_aFromBTransform.setMulInverseMul(ragdollBoneTransform, animationBoneTransform);
		}

		for (int i = 0; i < skeleton->m_bones.getSize(); i++) {
			if (mappedBones.find(i) == mappedBones.end())
				fromRagdollToSkeletonMapping->m_unmappedBones.pushBack(i);
		}


		Log::Info("Build Mappings Skeleton -> Ragdoll\n");

		hkaSkeletonMapperData* fromSkeletonToRagdollMapping = new hkaSkeletonMapperData();
		fromSkeletonToRagdollMapping->m_simpleMappings.setSize(rigidBodies.size());
		fromSkeletonToRagdollMapping->m_skeletonA = hkSkeleton;
		fromSkeletonToRagdollMapping->m_skeletonB = hkRagdollSkeleton;

		for (size_t i = 0; i < rigidBodies.size(); i++) {
			hkaSkeletonMapperData::SimpleMapping& mapping = fromSkeletonToRagdollMapping->m_simpleMappings[i];
			mapping.m_boneA = ragdollAnimationParentMap[i];
			mapping.m_boneB = i;
			mappedBones.insert(ragdollAnimationParentMap[i]);

			//Absolute transform
			int findroot = ragdoll_skeleton->m_parentIndices[i];
			hkQsTransform ragdollBoneTransform = hkRagdollSkeleton->m_referencePose[i];
			while (findroot != -1) {
				ragdollBoneTransform.setMul(hkRagdollSkeleton->m_referencePose[findroot], ragdollBoneTransform);
				findroot = ragdoll_skeleton->m_parentIndices[findroot];
			}

			int animationBone = ragdollAnimationParentMap[i];
			findroot = skeleton->m_parentIndices[animationBone];
			hkQsTransform animationBoneTransform = skeleton->m_referencePose[animationBone];
			while (findroot != -1) {
				animationBoneTransform.setMul(hkSkeleton->m_referencePose[findroot], animationBoneTransform);
				findroot = skeleton->m_parentIndices[findroot];
			}

			mapping.m_aFromBTransform.setMulInverseMul(animationBoneTransform, ragdollBoneTransform);
		}

		hkaAnimationContainer anim_container;
		hkMemoryResourceContainer mem_container;
		
		anim_container.m_skeletons.pushBack(skeleton);
		anim_container.m_skeletons.pushBack(ragdoll_skeleton);

		hkpPhysicsData physics_data;
		physics_data.addPhysicsSystem(physic_entities);

		hkRefPtr<hkaSkeletonMapper> ragdollToAnimationMapper = new hkaSkeletonMapper(*fromRagdollToSkeletonMapping);
		hkRefPtr<hkaSkeletonMapper> animationToRagdollMapper = new hkaSkeletonMapper(*fromSkeletonToRagdollMapping);

		hkRootLevelContainer container;
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Merged Animation Container", &anim_container, &anim_container.staticClass()));
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Resource Data", &mem_container, &mem_container.staticClass()));
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("Physics Data", &physics_data, &physics_data.staticClass()));
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("RagdollInstance", ragdoll, &ragdoll->staticClass()));
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("SkeletonMapper", ragdollToAnimationMapper.val(), &ragdollToAnimationMapper->staticClass()));
		container.m_namedVariants.pushBack(hkRootLevelContainer::NamedVariant("SkeletonMapper", animationToRagdollMapper.val(), &animationToRagdollMapper->staticClass()));

		hkPackFormat pkFormat = HKPF_DEFAULT;
		hkSerializeUtil::SaveOptionBits flags = hkSerializeUtil::SAVE_DEFAULT;
		hkPackfileWriter::Options packFileOptions = GetWriteOptionsFromFormat(pkFormat);
		fs::path final_out_path = "./skeleton.hkx";
		hkOstream stream(final_out_path.string().c_str());
		hkVariant root = { &container, &container.staticClass() };
		hkResult res = hkSerializeUtilSave(pkFormat, root, stream, flags, packFileOptions);
		if (res != HK_SUCCESS)
		{
			Log::Error("Havok reports save failed.");
		}
		hkPackFormat pkFormat2 = HKPF_AMD64;
		fs::path final_out_path2 = "./skeleton_se.hkx";
		hkPackfileWriter::Options packFileOptions2 = GetWriteOptionsFromFormat(pkFormat2);
		hkOstream stream2(final_out_path2.string().c_str());
		res = hkSerializeUtilSave(pkFormat2, root, stream2, flags, packFileOptions2);
		if (res != HK_SUCCESS)
		{
			Log::Error("Havok reports save failed.");
		}
	}
}

set<tuple<FbxNode*, FbxNode*, hkpConstraintInstance*>> constraints_table;

const set<tuple<FbxNode*, FbxNode*, hkpConstraintInstance*>>&  HKXWrapper::get_constraints_table() {
	return constraints_table;
}

hkRefPtr<hkpConstraintInstance> HKXWrapper::build_constraint(FbxNode* body)
{

	int numConstraints = body->RootProperty.GetSrcObjectCount(FbxCriteria::ObjectType(FbxConstraintParent::ClassId));
	if (numConstraints > 0)
	{
		FbxConstraintParent* fbx_constraint = body->RootProperty.GetSrcObject<FbxConstraintParent>(FbxCriteria::ObjectType(FbxConstraintParent::ClassId));
		FbxNode* target = (FbxNode*)fbx_constraint->GetConstrainedObject();
		auto rotation = fbx_constraint->GetRotationOffset(body);
		HMatrix M;
		Eul_ToHMatrix({ deg2rad(rotation[0]), deg2rad(rotation[1]), deg2rad(rotation[2]), EulOrdXYZs }, M);
		FbxQuaternion qrotation; qrotation.ComposeSphericalXYZ(rotation); qrotation.Inverse();
		
		auto translation = fbx_constraint->GetTranslationOffset(body);
		auto source = body->GetParent();

		hkRefPtr<hkpRigidBody> entity_b = bodies[body->GetParent()];
		hkRefPtr<hkpRigidBody> entity_a = bodies[target];
		hkTransform transform_a; transform_a.setIdentity(); //= getTransform(translation, qrotation);
		transform_a(0, 0) = M[0][0]; transform_a(0, 1) = M[1][0]; transform_a(0, 2) = M[2][0]; transform_a(0, 3) = translation[0];
		transform_a(1, 0) = M[0][1]; transform_a(1, 1) = M[1][1]; transform_a(1, 2) = M[2][1]; transform_a(1, 3) = translation[1];
		transform_a(2, 0) = M[0][2]; transform_a(2, 1) = M[1][2]; transform_a(2, 2) = M[2][2]; transform_a(2, 3) = translation[2];

		hkTransform transform_b = getTransform(body, false, true);

		hkpConstraintData* data = NULL;
		//Entity A is the child 
		if (fbx_constraint->AffectRotationZ.Get() == false) {
			//ragdoll
			hkpRagdollConstraintData* temp = new hkpRagdollConstraintData();
			temp->m_atoms.m_transforms.m_transformA = transform_a;
			temp->m_atoms.m_transforms.m_transformB = transform_b;
			data = temp;
		}
		else {
			hkpLimitedHingeConstraintData* temp = new hkpLimitedHingeConstraintData();
			temp->m_atoms.m_transforms.m_transformA = transform_a;
			temp->m_atoms.m_transforms.m_transformB = transform_b;
			data = temp;
		}

		hkRefPtr<hkpConstraintInstance> instance = 
			new hkpConstraintInstance(entity_a, entity_b, data);
		instance->setName(entity_a->getName());
		constraints.insert(instance);
		physic_entities->addConstraint(instance);
		constraints_table.insert({ target, body->GetParent(), instance });
		return instance;
	}
	return NULL;
}



hkRefPtr<hkpRigidBody> HKXWrapper::build_body(FbxNode* body, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes)
{
	double bhkScaleFactorInverse = 0.01428; // 1 skyrim unit = 0,01428m
	hkpRigidBodyCinfo body_cinfo;
	body_cinfo.setTransform(getTransform(body, true));
	//search for the mesh children
	FbxNode* mesh_child = NULL;
	vector<FbxNode*> constraint_childs;
	for (int i = 0; i < body->GetChildCount(); i++)
	{
		FbxNode* temp_child = body->GetChild(i);
		if (isShapeFbxNode(temp_child))
		{
			mesh_child = temp_child;
		}
		if (isConstraintFbxNode(temp_child))
		{
			constraint_childs.push_back(temp_child);
		}
	}
	for (int i = 0; i < body->GetNodeAttributeCount(); i++)
	{
		if (body->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)
			mesh_child = body;
	}
	//if (mesh_child == NULL) mesh_child = body->GetChild(0);
	if (mesh_child == NULL && geometry_meshes.empty()) return NULL;
	hkpMassProperties properties;
	body_cinfo.m_shape = HKXWrapper::build_shape(mesh_child, geometry_meshes, properties, bhkScaleFactorInverse, body, body_cinfo);
	body_cinfo.setMassProperties(properties);
	hkRefPtr<hkpRigidBody> hk_body = new hkpRigidBody(body_cinfo);
	string name = body->GetName();
	name = name.substr(0, name.size() - 3);
	if (string(body->GetName()).find("_sp") == string::npos)
		name = "Ragdoll_"+unsanitizeString(name);
	else
	{
		name = unsanitizeString(name);
		hk_body->setAllowedPenetrationDepth(340282001837565597733306976381245063168.000000);
	}
	hk_body->setName(name.c_str());
	hk_body->setShape(body_cinfo.m_shape);
	hk_body->m_npData = 0;

	if (physic_entities == NULL)
	{
		physic_entities = new hkpPhysicsSystem();
		physic_entities->setName("Default Physics System");
		physic_entities->setActive(true);
	}
	//recalculate the rigid bodies with real values, not scaled values for nifs
	hkpRigidBodyCinfo body_cinfo_unscaled;
	body_cinfo_unscaled.setTransform(getTransform(body, true));

	hkpMassProperties unscaled_properties;
	body_cinfo_unscaled.m_shape = HKXWrapper::build_shape(mesh_child, geometry_meshes, unscaled_properties, 1.0, body, body_cinfo_unscaled);
	body_cinfo_unscaled.setMassProperties(unscaled_properties);
	hkRefPtr<hkpRigidBody> hk_body_unscaled = new hkpRigidBody(body_cinfo_unscaled);
	hk_body_unscaled->setName(name.c_str());
	hk_body_unscaled->setShape(body_cinfo_unscaled.m_shape);
	hk_body_unscaled->m_npData = 0;
	if (string(body->GetName()).find("_sp") == string::npos)
	{
		bodies[body] = hk_body_unscaled;
		hk_body_unscaled->addReference();
		rigidBodies.push_back(hk_body_unscaled);
		for (auto con : constraint_childs)
			build_constraint(con);
	}
	else {
		hk_body_unscaled->setAllowedPenetrationDepth(340282001837565597733306976381245063168.000000);
	}
	physic_entities->addRigidBody(hk_body_unscaled);
	return hk_body;
}

hkRefPtr<hkpRigidBody> check_body(bhkRigidBodyRef body, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes)
{
	double bhkScaleFactorInverse = 0.01428; // 1 skyrim unit = 0,01428m
	hkpRigidBodyCinfo body_cinfo;
	hkpMassProperties properties;
	hkGeometry total_geometry;
	//create the whole geometry collisioned by this body to check it against the existing collision
	extract_geometry(geometry_meshes, total_geometry);

	body_cinfo.m_shape = HKXWrapper::check_shape(body->GetShape(), body, geometry_meshes, properties, bhkScaleFactorInverse, body_cinfo);
	body_cinfo.setMassProperties(properties);
	hkRefPtr<hkpRigidBody> hk_body = new hkpRigidBody(body_cinfo);
	hk_body->setShape(body_cinfo.m_shape);
	return hk_body;
}

hkRefPtr<hkpShape> HKXWrapper::build_shape(FbxNode* shape_root, set<pair<FbxAMatrix, FbxMesh*>>& geometry_meshes, hkpMassProperties& properties, double scale_factor, FbxNode* body, hkpRigidBodyCinfo& hk_body)
{
	//If shape_root is null, no hints were given on how to handle the collisions
	if (shape_root == NULL)
	{
		//we'll create a fbxnode hierachy to handle this and then recurse
		//calculate geometry
		VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
		shared_ptr<bmeshinfo> cmesh = make_shared<bmeshinfo>();
		for (const auto& mesh : geometry_meshes)
		{
			convert_geometry(cmesh, mesh);
		}
		VHACD::IVHACD::Parameters params;
		bool res = interfaceVHACD->Compute(&cmesh->points[0], (unsigned int)cmesh->points.size() / 3,
			(const uint32_t *)&cmesh->triangles[0], (unsigned int)cmesh->triangles.size() / 3, params);

		if (res) {
			unsigned int nConvexHulls = interfaceVHACD->GetNConvexHulls();
			if (nConvexHulls <= 10)
			{
				FbxManager* temp_manager = FbxManager::Create();
				FbxNode* temp_root = NULL;

	
				temp_root = create_hulls(temp_manager, interfaceVHACD);

				if (nConvexHulls >= 4)
				{
					FbxNode* mopp = FbxNode::Create(temp_manager, "_mopp");
					mopp->AddChild(temp_root);
					temp_root = mopp;
				}

				hkpShape* shape = build_shape(temp_root, geometry_meshes, properties, scale_factor, body, hk_body);
				temp_manager->Destroy();
				return shape;
			}
		}

		//convex optimization failed, we need a bounding mesh
		boundingmesh::Mesh bmesh;
		shared_ptr<bmeshinfo> bbmesh = cmesh;

		for (int i = 0; i < bbmesh->points.size() / 3; i++)
		{
			bmesh.addVertex({ bbmesh->points[i * 3], bbmesh->points[i * 3 + 1], bbmesh->points[i * 3 + 2] });
		}

		for (int i = 0; i < bbmesh->triangles.size() / 3; i++)
		{
			bmesh.addTriangle(bbmesh->triangles[i * 3], bbmesh->triangles[i * 3 + 1], bbmesh->triangles[i * 3 + 2]);
		}

		boundingmesh::Decimator decimator;
		decimator.setMesh(bmesh);
		decimator.setMetric(boundingmesh::Average);
		decimator.setMaximumError(0.1);

		std::shared_ptr<boundingmesh::Mesh> result = decimator.compute();
		FbxManager* temp_manager = FbxManager::Create();
		FbxNode* root = FbxNode::Create(temp_manager, "_mesh");
		FbxMesh* m = FbxMesh::Create(temp_manager, "_mesh_tris");
		m->InitControlPoints(result->nVertices());
		FbxVector4* points = m->GetControlPoints();
		for (int i = 0; i < result->nVertices(); i++)
		{
			boundingmesh::Vector3 v = result->vertex(i).position();
			points[i] = FbxVector4(v[0], v[1], v[2]);
		}

		for (int i = 0; i < result->nTriangles(); i++)
		{
			boundingmesh::Triangle v = result->triangle(i);
			m->BeginPolygon(0);
			m->AddPolygon(v.vertex(0));
			m->AddPolygon(v.vertex(1));
			m->AddPolygon(v.vertex(2));
			m->EndPolygon();
		}
		root->AddNodeAttribute(m);
		FbxNode* mopp = FbxNode::Create(temp_manager, "_mopp");
		mopp->AddChild(root);
		hkpShape* shape = build_shape(mopp, geometry_meshes, properties, scale_factor, body, hk_body);
		temp_manager->Destroy();
		return shape;
	}
	string name = shape_root->GetName();
	//	//Containers
	if (ends_with(name, "_transform"))
	{
		hkpShape* childShape = build_shape(shape_root->GetChild(0), geometry_meshes, properties, scale_factor, body, hk_body);
		FbxAMatrix fbx_transform = shape_root->EvaluateLocalTransform();
		FbxVector4 translation = fbx_transform.GetT();
		FbxQuaternion rotation = fbx_transform.GetQ();
		FbxVector4 scale = fbx_transform.GetS();
		hkQsTransform stransform(
			{ (hkReal)translation[0], (hkReal)translation[0] ,(hkReal)translation[0] },
			{ (hkReal)rotation[0], (hkReal)rotation [1],(hkReal)rotation [2], (hkReal)rotation [3]},
			{ (hkReal)scale [0], (hkReal)scale [1], (hkReal)scale [2]}
		);
		hkTransform transform; stransform.copyToTransform(transform);
		return new hkpTransformShape(childShape, transform);
	}
	if (ends_with(name, "_list"))
	{
		size_t num_children = shape_root->GetChildCount();
		vector<hkRefPtr<hkpShape>> sub_shapes;
		hkArray<hkpMassElement> sub_elements;
		for (int i = 0; i < num_children; i++)
		{
			hkpMassProperties sub_properties;
			hkpMassElement sub_element;
			hkRefPtr<hkpShape> sub_shape = build_shape(shape_root->GetChild(i), geometry_meshes, sub_properties, scale_factor, body, hk_body);
			if (sub_shape != NULL)
			{
				if (sub_shape->getType() == HK_SHAPE_CONVEX_TRANSFORM || sub_shape->getType() == HK_SHAPE_TRANSFORM)
				{
					hkpTransformShape* transform_shape = (hkpTransformShape *)&*sub_shape;
					sub_element.m_transform = transform_shape->getTransform();
				}
				sub_element.m_properties = sub_properties;			
				sub_shapes.push_back(sub_shape);
				sub_elements.pushBack(sub_element);
			}
		}
		hkInertiaTensorComputer::combineMassProperties(sub_elements, properties);
		return new hkpListShape((const hkpShape*const*)sub_shapes.data(), sub_shapes.size());
	}
	if (ends_with(name, "_convex_list"))
	{
		size_t num_children = shape_root->GetChildCount();
		vector<hkRefPtr<hkpConvexShape>> sub_shapes;
		for (int i = 0; i < num_children; i++)
		{
			//GOSH!!!!, TODO: fix
			hkRefPtr<hkpConvexShape> sub_shape = (hkpConvexShape*)&*build_shape(shape_root->GetChild(i), geometry_meshes, properties, scale_factor, body, hk_body);
			if (sub_shape != NULL)
				sub_shapes.push_back(sub_shape);
		}
		return new hkpConvexListShape((const hkpConvexShape*const*)sub_shapes.data(), sub_shapes.size());
	}
	if (ends_with(name, "_mopp"))
	{
		hkpMoppCode*							pMoppCode(NULL);

		hkpMoppCompilerInput					mci;
		hkpShape* childShape = build_shape(shape_root->GetChild(0), geometry_meshes, properties, scale_factor, body, hk_body);
		hkpShapeCollection* collection;
		hkpShapeType result_type = childShape->getType();
		if (result_type != HK_SHAPE_LIST && result_type != HK_SHAPE_COMPRESSED_MESH)
			throw runtime_error("Invalid Mopp Shape type detected: " + to_string(result_type));
		collection = dynamic_cast<hkpShapeCollection*>(childShape);
		//create welding info
		mci.m_enableChunkSubdivision = false;  //  PC version

		pMoppCode = hkpMoppUtility::buildCode(collection->getContainer(), mci);
		hkRefPtr<hkpMoppBvTreeShape> pMoppBvTree = new hkpMoppBvTreeShape(collection, pMoppCode);
		hkpMeshWeldingUtility::computeWeldingInfo(collection, pMoppBvTree, hkpWeldingUtility::WELDING_TYPE_TWO_SIDED);
		return pMoppBvTree;
	}
	//shapes
	vector<hkpNamedMeshMaterial> materials;
	hkGeometry to_bound = extract_bounding_geometry(shape_root, geometry_meshes, materials, properties, scale_factor);
	hkReal mass = properties.m_mass;
	if (ends_with(name, "_sphere"))
	{		
		hkpCreateShapeUtility::CreateShapeInput input;
		hkpCreateShapeUtility::ShapeInfoOutput output;
		input.m_vertices = to_bound.m_vertices;
		hkpCreateShapeUtility::createSphereShape(input, output);
		hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
		hkpSphereShape* shape = (hkpSphereShape*)output.m_shape;
		hkInertiaTensorComputer::computeSphereVolumeMassProperties(shape->getRadius(), mass, properties);
		return handle_output_transform(output, material, properties, shape_root, body, hk_body);
	}
	if (ends_with(name, "_box"))
	{
		hkpCreateShapeUtility::CreateShapeInput input;
		hkpCreateShapeUtility::ShapeInfoOutput output;
		input.m_vertices = to_bound.m_vertices;
		hkpCreateShapeUtility::createBoxShape(input, output);
		hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
		hkpBoxShape* shape = (hkpBoxShape*)output.m_shape;
		hkInertiaTensorComputer::computeBoxVolumeMassProperties(shape->getHalfExtents(), mass, properties);
		return handle_output_transform(output, material, properties, shape_root, body, hk_body);
	}
	if (ends_with(name, "_capsule"))
	{
		hkpCreateShapeUtility::CreateShapeInput input;
		hkpCreateShapeUtility::ShapeInfoOutput output;
		input.m_vertices = to_bound.m_vertices;
		hkpCreateShapeUtility::createCapsuleShape(input, output);
		hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
		hkpCapsuleShape* shape = (hkpCapsuleShape*)output.m_shape;
		hkInertiaTensorComputer::computeCapsuleVolumeMassProperties(shape->getVertex(0), shape->getVertex(1), shape->getRadius(), mass, properties);
		return handle_output_transform(output, material, properties, shape_root, body, hk_body);
	}
	if (ends_with(name, "_convex"))
	{
		hkStridedVertices stridedVertsIn(to_bound.m_vertices);
		hkGeometry convex;
		hkArray<hkVector4> planeEquationsOut;
		hkGeometryUtility::createConvexGeometry(stridedVertsIn, convex, planeEquationsOut);
		hkStridedVertices stridedVertsOut(convex.m_vertices);
		hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
		hkpShape* convex_shape = new hkpConvexVerticesShape(convex.m_vertices, planeEquationsOut);
		hkInertiaTensorComputer::computeVertexHullVolumeMassProperties(stridedVertsOut.m_vertices, stridedVertsOut.m_striding, stridedVertsOut.m_numVertices, mass, properties);
		convex_shape->setUserData((hkUlong)material);
		return convex_shape;
	}
	if (ends_with(name, "_mesh"))
	{
		hkpCompressedMeshShapeBuilder			shapeBuilder;
		shapeBuilder.m_stripperPasses = 5000;
		hkpCompressedMeshShape* pCompMesh = shapeBuilder.createMeshShape(0.001f, hkpCompressedMeshShape::MATERIAL_SINGLE_VALUE_PER_CHUNK);
		pCompMesh->m_namedMaterials.setSize(materials.size());
		for (int i = 0; i < materials.size(); i++)
			pCompMesh->m_namedMaterials[i] = materials[i];
		try {
			//  add geometry to shape
			int										subPartId(0);
			subPartId = shapeBuilder.beginSubpart(pCompMesh);
			shapeBuilder.addGeometry(to_bound, hkMatrix4::getIdentity(), pCompMesh);
			shapeBuilder.endSubpart(pCompMesh);
			shapeBuilder.addInstance(subPartId, hkMatrix4::getIdentity(), pCompMesh);

			//add materials to shape
			for (int i = 0; i < materials.size(); i++) {
				pCompMesh->m_materials.pushBack(i);
			}
			//check connectivity
			hkpConvexVerticesConnectivity connector;
			for (const auto& t : to_bound.m_triangles)
			{
				int tt[3]; tt[0] = t.m_a; tt[1] = t.m_b; tt[2] = t.m_c;
				connector.addFace(tt, 3);
			}
			if (connector.isClosed())
			{
				hkInertiaTensorComputer::computeGeometryVolumeMassPropertiesChecked(&to_bound, mass, properties);
			}
			else {
				hkInertiaTensorComputer::computeGeometrySurfaceMassProperties(&to_bound, 0.1, true, mass, properties);
			}
			return pCompMesh;
		}
		catch (...) {
			throw runtime_error("Unable to calculate MOPP code!");
		}
	}
	return NULL;
}




hkRefPtr<hkpShape> HKXWrapper::check_shape(bhkShapeRef shape_root, bhkRigidBodyRef bhkBody, vector<pair<hkTransform, NiTriShapeRef>>& geometry_meshes, hkpMassProperties& properties, double scale_factor, hkpRigidBodyCinfo& hk_body)
{
	////If shape_root is null, no hints were given on how to handle the collisions
	//if (shape_root == NULL)
	//{
	//	//we'll create a fbxnode hierachy to handle this and then recurse
	//	//calculate geometry
	//	VHACD::IVHACD* interfaceVHACD = VHACD::CreateVHACD();
	//	shared_ptr<bmeshinfo> cmesh = make_shared<bmeshinfo>();
	//	for (const auto& mesh : geometry_meshes)
	//	{
	//		convert_geometry(cmesh, mesh);
	//	}
	//	VHACD::IVHACD::Parameters params;
	//	bool res = interfaceVHACD->Compute(&cmesh->points[0], (unsigned int)cmesh->points.size() / 3,
	//		(const uint32_t *)&cmesh->triangles[0], (unsigned int)cmesh->triangles.size() / 3, params);

	//	if (res) {
	//		unsigned int nConvexHulls = interfaceVHACD->GetNConvexHulls();
	//		if (nConvexHulls <= 10)
	//		{
	//			FbxManager* temp_manager = FbxManager::Create();
	//			FbxNode* temp_root = NULL;


	//			temp_root = create_hulls(temp_manager, interfaceVHACD);

	//			if (nConvexHulls >= 4)
	//			{
	//				FbxNode* mopp = FbxNode::Create(temp_manager, "_mopp");
	//				mopp->AddChild(temp_root);
	//				temp_root = mopp;
	//			}

	//			hkpShape* shape = build_shape(temp_root, geometry_meshes, properties, scale_factor, body, hk_body);
	//			temp_manager->Destroy();
	//			return shape;
	//		}
	//	}

	//	//convex optimization failed, we need a bounding mesh
	//	boundingmesh::Mesh bmesh;
	//	shared_ptr<bmeshinfo> bbmesh = cmesh;

	//	for (int i = 0; i < bbmesh->points.size() / 3; i++)
	//	{
	//		bmesh.addVertex({ bbmesh->points[i * 3], bbmesh->points[i * 3 + 1], bbmesh->points[i * 3 + 2] });
	//	}

	//	for (int i = 0; i < bbmesh->triangles.size() / 3; i++)
	//	{
	//		bmesh.addTriangle(bbmesh->triangles[i * 3], bbmesh->triangles[i * 3 + 1], bbmesh->triangles[i * 3 + 2]);
	//	}

	//	boundingmesh::Decimator decimator;
	//	decimator.setMesh(bmesh);
	//	decimator.setMetric(boundingmesh::Average);
	//	decimator.setMaximumError(0.1);

	//	std::shared_ptr<boundingmesh::Mesh> result = decimator.compute();
	//	FbxManager* temp_manager = FbxManager::Create();
	//	FbxNode* root = FbxNode::Create(temp_manager, "_mesh");
	//	FbxMesh* m = FbxMesh::Create(temp_manager, "_mesh_tris");
	//	m->InitControlPoints(result->nVertices());
	//	FbxVector4* points = m->GetControlPoints();
	//	for (int i = 0; i < result->nVertices(); i++)
	//	{
	//		boundingmesh::Vector3 v = result->vertex(i).position();
	//		points[i] = FbxVector4(v[0], v[1], v[2]);
	//	}

	//	for (int i = 0; i < result->nTriangles(); i++)
	//	{
	//		boundingmesh::Triangle v = result->triangle(i);
	//		m->BeginPolygon(0);
	//		m->AddPolygon(v.vertex(0));
	//		m->AddPolygon(v.vertex(1));
	//		m->AddPolygon(v.vertex(2));
	//		m->EndPolygon();
	//	}
	//	root->AddNodeAttribute(m);
	//	FbxNode* mopp = FbxNode::Create(temp_manager, "_mopp");
	//	mopp->AddChild(root);
	//	hkpShape* shape = build_shape(mopp, geometry_meshes, properties, scale_factor, body, hk_body);
	//	temp_manager->Destroy();
	//	return shape;
	//}
	//string name = shape_root->GetName();
	////	//Containers
	//if (ends_with(name, "_transform"))
	//{
	//	hkpShape* childShape = build_shape(shape_root->GetChild(0), geometry_meshes, properties, scale_factor, body, hk_body);
	//	FbxAMatrix fbx_transform = shape_root->EvaluateLocalTransform();
	//	FbxVector4 translation = fbx_transform.GetT();
	//	FbxQuaternion rotation = fbx_transform.GetQ();
	//	FbxVector4 scale = fbx_transform.GetS();
	//	hkQsTransform stransform(
	//		{ (hkReal)translation[0], (hkReal)translation[0] ,(hkReal)translation[0] },
	//		{ (hkReal)rotation[0], (hkReal)rotation[1],(hkReal)rotation[2], (hkReal)rotation[3] },
	//		{ (hkReal)scale[0], (hkReal)scale[1], (hkReal)scale[2] }
	//	);
	//	hkTransform transform; stransform.copyToTransform(transform);
	//	return new hkpTransformShape(childShape, transform);
	//}
	//if (ends_with(name, "_list"))
	//{
	//	size_t num_children = shape_root->GetChildCount();
	//	vector<hkRefPtr<hkpShape>> sub_shapes;
	//	hkArray<hkpMassElement> sub_elements;
	//	for (int i = 0; i < num_children; i++)
	//	{
	//		hkpMassProperties sub_properties;
	//		hkpMassElement sub_element;
	//		hkRefPtr<hkpShape> sub_shape = build_shape(shape_root->GetChild(i), geometry_meshes, sub_properties, scale_factor, body, hk_body);
	//		if (sub_shape != NULL)
	//		{
	//			if (sub_shape->getType() == HK_SHAPE_CONVEX_TRANSFORM || sub_shape->getType() == HK_SHAPE_TRANSFORM)
	//			{
	//				hkpTransformShape* transform_shape = (hkpTransformShape *)&*sub_shape;
	//				sub_element.m_transform = transform_shape->getTransform();
	//			}
	//			sub_element.m_properties = sub_properties;
	//			sub_shapes.push_back(sub_shape);
	//			sub_elements.pushBack(sub_element);
	//		}
	//	}
	//	hkInertiaTensorComputer::combineMassProperties(sub_elements, properties);
	//	return new hkpListShape((const hkpShape*const*)sub_shapes.data(), sub_shapes.size());
	//}
	//if (ends_with(name, "_convex_list"))
	//{
	//	size_t num_children = shape_root->GetChildCount();
	//	vector<hkRefPtr<hkpConvexShape>> sub_shapes;
	//	for (int i = 0; i < num_children; i++)
	//	{
	//		//GOSH!!!!, TODO: fix
	//		hkRefPtr<hkpConvexShape> sub_shape = (hkpConvexShape*)&*build_shape(shape_root->GetChild(i), geometry_meshes, properties, scale_factor, body, hk_body);
	//		if (sub_shape != NULL)
	//			sub_shapes.push_back(sub_shape);
	//	}
	//	return new hkpConvexListShape((const hkpConvexShape*const*)sub_shapes.data(), sub_shapes.size());
	//}
	//if (ends_with(name, "_mopp"))
	//{
	//	hkpMoppCode*							pMoppCode(NULL);

	//	hkpMoppCompilerInput					mci;
	//	hkpShape* childShape = build_shape(shape_root->GetChild(0), geometry_meshes, properties, scale_factor, body, hk_body);
	//	hkpShapeCollection* collection;
	//	hkpShapeType result_type = childShape->getType();
	//	if (result_type != HK_SHAPE_LIST && result_type != HK_SHAPE_COMPRESSED_MESH)
	//		throw runtime_error("Invalid Mopp Shape type detected: " + to_string(result_type));
	//	collection = dynamic_cast<hkpShapeCollection*>(childShape);
	//	//create welding info
	//	mci.m_enableChunkSubdivision = false;  //  PC version

	//	pMoppCode = hkpMoppUtility::buildCode(collection->getContainer(), mci);
	//	hkRefPtr<hkpMoppBvTreeShape> pMoppBvTree = new hkpMoppBvTreeShape(collection, pMoppCode);
	//	hkpMeshWeldingUtility::computeWeldingInfo(collection, pMoppBvTree, hkpWeldingUtility::WELDING_TYPE_TWO_SIDED);
	//	return pMoppBvTree;
	//}
	////shapes
	//vector<hkpNamedMeshMaterial> materials;
	//hkGeometry to_bound = extract_bounding_geometry(shape_root, geometry_meshes, materials, properties, scale_factor);
	//hkReal mass = properties.m_mass;
	//if (ends_with(name, "_sphere"))
	//{
	//	hkpCreateShapeUtility::CreateShapeInput input;
	//	hkpCreateShapeUtility::ShapeInfoOutput output;
	//	input.m_vertices = to_bound.m_vertices;
	//	hkpCreateShapeUtility::createSphereShape(input, output);
	//	hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
	//	hkpSphereShape* shape = (hkpSphereShape*)output.m_shape;
	//	hkInertiaTensorComputer::computeSphereVolumeMassProperties(shape->getRadius(), mass, properties);
	//	return handle_output_transform(output, material, properties, shape_root, body, hk_body);
	//}
	//if (ends_with(name, "_box"))
	//{
	//	hkpCreateShapeUtility::CreateShapeInput input;
	//	hkpCreateShapeUtility::ShapeInfoOutput output;
	//	input.m_vertices = to_bound.m_vertices;
	//	hkpCreateShapeUtility::createBoxShape(input, output);
	//	hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
	//	hkpBoxShape* shape = (hkpBoxShape*)output.m_shape;
	//	hkInertiaTensorComputer::computeBoxVolumeMassProperties(shape->getHalfExtents(), mass, properties);
	//	return handle_output_transform(output, material, properties, shape_root, body, hk_body);
	//}
	//if (ends_with(name, "_capsule"))
	//{
	//	hkpCreateShapeUtility::CreateShapeInput input;
	//	hkpCreateShapeUtility::ShapeInfoOutput output;
	//	input.m_vertices = to_bound.m_vertices;
	//	hkpCreateShapeUtility::createCapsuleShape(input, output);
	//	hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
	//	hkpCapsuleShape* shape = (hkpCapsuleShape*)output.m_shape;
	//	hkInertiaTensorComputer::computeCapsuleVolumeMassProperties(shape->getVertex(0), shape->getVertex(1), shape->getRadius(), mass, properties);
	//	return handle_output_transform(output, material, properties, shape_root, body, hk_body);
	//}
	//if (ends_with(name, "_convex"))
	//{
	//	hkStridedVertices stridedVertsIn(to_bound.m_vertices);
	//	hkGeometry convex;
	//	hkArray<hkVector4> planeEquationsOut;
	//	hkGeometryUtility::createConvexGeometry(stridedVertsIn, convex, planeEquationsOut);
	//	hkStridedVertices stridedVertsOut(convex.m_vertices);
	//	hkpNamedMeshMaterial* material = new hkpNamedMeshMaterial(materials[0]);
	//	hkpShape* convex_shape = new hkpConvexVerticesShape(convex.m_vertices, planeEquationsOut);
	//	hkInertiaTensorComputer::computeVertexHullVolumeMassProperties(stridedVertsOut.m_vertices, stridedVertsOut.m_striding, stridedVertsOut.m_numVertices, mass, properties);
	//	convex_shape->setUserData((hkUlong)material);
	//	return convex_shape;
	//}
	//if (ends_with(name, "_mesh"))
	//{
	//	hkpCompressedMeshShapeBuilder			shapeBuilder;
	//	shapeBuilder.m_stripperPasses = 5000;
	//	hkpCompressedMeshShape* pCompMesh = shapeBuilder.createMeshShape(0.001f, hkpCompressedMeshShape::MATERIAL_SINGLE_VALUE_PER_CHUNK);
	//	pCompMesh->m_namedMaterials.setSize(materials.size());
	//	for (int i = 0; i < materials.size(); i++)
	//		pCompMesh->m_namedMaterials[i] = materials[i];
	//	try {
	//		//  add geometry to shape
	//		int										subPartId(0);
	//		subPartId = shapeBuilder.beginSubpart(pCompMesh);
	//		shapeBuilder.addGeometry(to_bound, hkMatrix4::getIdentity(), pCompMesh);
	//		shapeBuilder.endSubpart(pCompMesh);
	//		shapeBuilder.addInstance(subPartId, hkMatrix4::getIdentity(), pCompMesh);

	//		//add materials to shape
	//		for (int i = 0; i < materials.size(); i++) {
	//			pCompMesh->m_materials.pushBack(i);
	//		}
	//		//check connectivity
	//		hkpConvexVerticesConnectivity connector;
	//		for (const auto& t : to_bound.m_triangles)
	//		{
	//			int tt[3]; tt[0] = t.m_a; tt[1] = t.m_b; tt[2] = t.m_c;
	//			connector.addFace(tt, 3);
	//		}
	//		if (connector.isClosed())
	//		{
	//			hkInertiaTensorComputer::computeGeometryVolumeMassPropertiesChecked(&to_bound, mass, properties);
	//		}
	//		else {
	//			hkInertiaTensorComputer::computeGeometrySurfaceMassProperties(&to_bound, 0.1, true, mass, properties);
	//		}
	//		return pCompMesh;
	//	}
	//	catch (...) {
	//		throw runtime_error("Unable to calculate MOPP code!");
	//	}
	//}
	return NULL;
}

string HKXWrapperCollection::wrap(const string& out_name, const string& out_path, const string& out_path_root, const string& prefix, const set<string>& sequences_names)
{

	if (wrappers.find(sequences_names) == wrappers.end()) {
		wrappers[sequences_names] = move(HKXWrapper(out_name, out_path, out_path_root, prefix, sequences_names));
	}
	return wrappers[sequences_names].GetPath();
}


