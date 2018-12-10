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

#include <core/EulerAngles.h>
#include <core/MathHelper.h>

using namespace ckcmd::HKX;

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
	for (FbxAnimStack* stack : animations)
	{
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
		sort(temp_track.begin(), temp_track.end(), &annotation_sorter);
		hkaAnnotationTrack& a_track = tempAnim->m_annotationTracks[0];
		for (const auto& ann : temp_track)
			a_track.m_annotations.pushBack(ann);

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
	hkReal incrFrame = animation->m_duration / (hkReal)(FrameNumber - 1);



	if (TrackNumber > numBones)
	{
		Log::Error("\nERROR: More tracks than the actual skeleton loaded! some will be ignored\n");		
	}

	hkLocalArray<float> floatsOut(FloatNumber);
	hkLocalArray<hkQsTransform> transformOut(TrackNumber);
	floatsOut.setSize(FloatNumber);
	transformOut.setSize(TrackNumber);
	hkReal startTime = 0.0;

	hkArray<hkInt16> tracks;
	tracks.setSize(TrackNumber);
	for (int i = 0; i<TrackNumber; ++i) tracks[i] = i;

	hkReal time = startTime;

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

	// loop through keyframes
	for (int iFrame = 0; iFrame<FrameNumber; ++iFrame, time += incrFrame)
	{
		animation->samplePartialTracks(time, TrackNumber, transformOut.begin(), FloatNumber, floatsOut.begin(), NULL);
		hkaSkeletonUtils::normalizeRotations(transformOut.begin(), TrackNumber);

		// assume 1-to-1 transforms
		// loop through animated bones

		// todo support for anything else beside 30 fps?
		lTime.SetTime(0, 0, 0, iFrame, 0, 0, lTime.eFrames30);

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
			const hkQuaternion& anim_rot = transform.getRotation();
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
			FbxProperty this_p = binding->m_floatTrackToFloatSlotIndices.getSize() > 0 ?
				float_tracks[binding->m_floatTrackToFloatSlotIndices[k]] :
				float_tracks[k];
			if (this_p.IsValid())
			{
				FbxAnimCurve* fcurve = this_p.GetCurve(lAnimLayer, this_p.GetNameAsCStr(), true);
				fcurve->KeyModifyBegin();
				lKeyIndex = fcurve->KeyAdd(lTime);
				fcurve->KeySet(lKeyIndex, time, floatsOut[k], FbxAnimCurveDef::eInterpolationConstant);
				fcurve->KeyModifyEnd();
			}
		}
	}
}

vector<FbxNode*> HKXWrapper::add(hkaSkeleton* skeleton, FbxNode* scene_root, vector<FbxProperty>& float_tracks)
{
	
	// get number of bones and apply reference pose
	const int numBones = skeleton->m_bones.getSize();

	map<int, FbxNode*> conversion_map;

	// create base limb objects first
	for (hkInt16 b = 0; b < numBones; b++)
	{
		const hkaBone& bone = skeleton->m_bones[b];

		string b_name = bone.m_name;
		sanitizeString(b_name);

		hkQsTransform localTransform = skeleton->m_referencePose[b];
		const hkVector4& pos = localTransform.getTranslation();
		const hkQuaternion& rot = localTransform.getRotation();

		FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(scene_root->GetScene(), b_name.c_str());

		if ((b == 0))
			lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eRoot);
		else
			lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);

		lSkeletonLimbNodeAttribute1->Size.Set(1.0);
		FbxNode* BaseJoint = FbxNode::Create(scene_root->GetScene(), b_name.c_str());
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
		//else {
		//	skeleton_root = conversion_map[c];
		//}
	}

	//add floats properties
	int float_size = skeleton->m_floatSlots.getSize();
	float_tracks.resize(float_size);
	for (int i = 0; i < float_size; i++)
	{
		//to be sure to preserve 
		float_tracks[i] = FbxProperty::Create(ordered_skeleton[0], FbxFloatDT, skeleton->m_floatSlots[i].cString());
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


string HKXWrapperCollection::wrap(const string& out_name, const string& out_path, const string& out_path_root, const string& prefix, const set<string>& sequences_names)
{

	if (wrappers.find(sequences_names) == wrappers.end()) {
		wrappers[sequences_names] = move(HKXWrapper(out_name, out_path, out_path_root, prefix, sequences_names));
	}
	return wrappers[sequences_names].GetPath();
}


