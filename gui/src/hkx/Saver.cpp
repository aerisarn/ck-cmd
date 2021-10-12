#include <src/hkx/Saver.h>

//Behavior
#include <hkbBehaviorGraph_1.h>
#include <hkbStateMachineTransitionInfo_1.h>
#include <hkbStateMachineTimeInterval_0.h>
#include <hkbVariableBindingSet_2.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbClipGenerator_2.h>
#include <hkbManualSelectorGenerator_0.h>

// Animation
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Animation/SplineCompressed/hkaSplineCompressedAnimation.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>


using namespace ckcmd::HKX;

std::string Saver::to_bstring(float f)
{
	std::stringstream s;
	s << setprecision(6) << f;
	return s.str();
}

void Saver::save_hkx(int file_index)
{

}

Saver::Saver(ResourceManager& manager, ProjectNode* project_root) :
	_saving_character(false),
	_manager(manager)
{
	if (project_root->isCharacter())
	{
		_saving_character = true;
	}
	project_root->accept(*this);
	NullBuilder builder;
	RecursiveBehaviorVisitor< NullBuilder> r_visitor(builder, _manager, _project_file_index);
	__debugbreak();
	if (!_behavior_nodes.empty())
	{
		_behavior_nodes.at(0)->accept(r_visitor);
	}
	__debugbreak();
	//save_cache();
}

void NestedStateFinder::handle_node(ProjectNode& node)
{
	hkVariant* variant = (hkVariant*)node.data(1).value<unsigned long long>();
	//hkVariant* parent_variant = (hkVariant*)node.data(2).value<unsigned long long>();
	int file_index = node.data(3).value<int>();

	if (variant->m_class == &hkbStateMachineStateInfoClass)
	{
		hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)variant->m_object;
		if (state->m_stateId == _target_id)
			_result = &node;
	}
}

void Saver::handle_clip(hkbClipGenerator* clip, int file_index)
{
	ClipInfo info;
	info.animation_name = clip->m_animationName.cString();
	info.playbackSpeed = clip->m_playbackSpeed;
	info.cropStartTime = clip->m_cropStartAmountLocalTime;
	info.cropEndTime = clip->m_cropEndAmountLocalTime;
	if (NULL != clip->m_triggers)
	{

		BehaviorBuilder* builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
		hkbClipTriggerArray* triggers = clip->m_triggers;
		for (int i = 0; i < triggers->m_triggers.getSize(); i++)
		{
			float time;
			if (triggers->m_triggers[i].m_relativeToEndOfClip == true)
			{
				time = _cache_animation_info[info.animation_name].duration + triggers->m_triggers[i].m_localTime;
			}
			else {
				time = triggers->m_triggers[i].m_localTime;
			}
			std::string event_name = builder->getEvent(triggers->m_triggers[i].m_event.m_id).toUtf8().constData();
			info._events.insert({ time, event_name });
		}
	}
	_cache_clip_info[clip->m_name.cString()] = info;
}

void Saver::handle_transition(ProjectNode& node)
{
	//hkVariant* variant = (hkVariant*)node.data(1).value<unsigned long long>();
	//int file_index = node.data(3).value<int>();
	//hkbStateMachineTransitionInfoArray* info_array = (hkbStateMachineTransitionInfoArray*)variant->m_object;
	//BehaviorBuilder* builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
	//for (int array_index = 0; array_index < info_array->m_transitions.getSize(); array_index++)
	//{
	//	hkbStateMachineTransitionInfo& info = info_array->m_transitions[array_index];
	//	QString event_name = builder->getEvent(info.m_eventId);
	//	if (event_name != "No Event") {
	//		std::string event_ = event_name.toUtf8().constData();
	//		if (_cache_attacks.find(event_) != _cache_attacks.end()) {
	//			int target_state_id = info.m_toStateId;
	//			int nested_state_id = info.m_toNestedStateId;
	//			vector<ProjectNode*> fsm_nodes;
	//			for (int f = 0; f < node.parentCount(); f++)
	//			{
	//				auto parent = node.parentItem(f);
	//				hkVariant* fsm_variant = (hkVariant*)parent->data(1).value<unsigned long long>();
	//				if (fsm_variant->m_class == &hkbStateMachineStateInfoClass)
	//				{
	//					for (int ff = 0; ff < parent->parentCount(); ff++)
	//						fsm_nodes.push_back(parent->parentItem(ff));
	//					
	//				}
	//				else {
	//					fsm_nodes.push_back(parent);
	//				}
	//			}
	//			set<int> branch_valid_attack_styles_set = { -1 };
	//			if (_cache_styles.size() > 1) {
	//				branch_valid_attack_styles_set.clear();
	//			}
	//			for (auto fsm_node : fsm_nodes)
	//			{
	//				if (_cache_styles.size() > 1) {
	//					//check if I'm into the correct branch, we need to match two manual selector generators
	//					std::vector<int> valid_attack_styles;
	//					for (auto& entry : _cache_style_attacks) {
	//						if ((entry.second.find({ event_ , true }) != entry.second.end()) || (entry.second.find({ event_ , false }) != entry.second.end()))
	//						{
	//							valid_attack_styles.push_back(entry.first);
	//						}
	//					}
	//					for (auto& style_index : valid_attack_styles)
	//					{
	//						auto& style_info = _cache_styles[style_index];
	//						ProjectNode* current_node = AnimationStyleReverseWalker(fsm_node, style_info, builder)._root;

	//						if (!current_node->isVariant())
	//						{
	//							//valid Branch, cache clips
	//							//auto style_set = ClipCollector(fsm_node, _cache_styles[style_index], builder)._result;
	//							//_cache_style_clips[style_index].insert(style_set.begin(), style_set.end());
	//							branch_valid_attack_styles_set.insert(style_index);
	//						}
	//					}
	//				}
	//				vector<int> branch_valid_attack_styles;
	//				for (const auto& style : branch_valid_attack_styles_set)
	//				{
	//					branch_valid_attack_styles.push_back(style);
	//				}
	//				if (!branch_valid_attack_styles.empty())
	//				{
	//					for (int fsm_child_row = 0; fsm_child_row < fsm_node->childCount(); fsm_child_row++)
	//					{
	//						ProjectNode* fsm_child = fsm_node->child(fsm_child_row);
	//						hkVariant* fsm_child_variant = (hkVariant*)fsm_child->data(1).value<unsigned long long>();
	//						if (fsm_child_variant->m_class == &hkbStateMachineStateInfoClass) {
	//							hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)fsm_child_variant->m_object;
	//							if (state->m_stateId == target_state_id) {
	//								NestedStateFinder finder(fsm_child, nested_state_id);
	//								if (finder._result != NULL)
	//								{
	//									fsm_child = finder._result;
	//								}
	//								for (int branch_style = 0; branch_style < branch_valid_attack_styles.size(); branch_style++)
	//								{
	//									set<string> clips;
	//									if (branch_valid_attack_styles.size() == 1 && branch_valid_attack_styles[0] == -1)
	//										clips = ClipCollector(fsm_child, {}, builder)._result;
	//									else
	//										clips = ClipCollector(fsm_child, _cache_styles[branch_valid_attack_styles[branch_style]], builder)._result;
	//									for (auto clip : clips)
	//									{
	//										_cache_attack_clips[event_].insert({ branch_valid_attack_styles[branch_style],clip });
	//									}
	//								}
	//								break;
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

void AnimationStyleReverseWalker::handle_node(ProjectNode& node)
{
	bool correct_branch = false;
	_root = &node;
	for (int p = 0; p < node.parentCount(); p++)
	{
		ProjectNode* parent_node = node.parentItem(p);
		if (parent_node == NULL)
			throw 666;
		if (!parent_node->isVariant())
			parent_node->accept(*this);
		else {
			hkVariant* parent_node_variant = (hkVariant*)parent_node->data(1).value<unsigned long long>();
			hkVariant* child_node_variant = (hkVariant*)node.data(1).value<unsigned long long>();
			if (parent_node_variant->m_class == &hkbManualSelectorGeneratorClass) {
				hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)parent_node_variant->m_object;
				int min = -1; int max = -1; int variable_index = -1;
				if (msg->m_variableBindingSet != NULL)
				{
					hkbVariableBindingSet* vbs = msg->m_variableBindingSet;
					for (int b = 0; b < vbs->m_bindings.getSize(); b++)
					{
						string variable_name = _builder->getVariable(vbs->m_bindings[b].m_variableIndex).toUtf8().constData();
						for (int v = 0; v < _style_info.size(); v++) {
							if (_style_info[v].variable == variable_name)
							{
								variable_index = v;
								min = _style_info[v].min;
								max = _style_info[v].max;
								break;
							}
						}
					}
				}
				//binded
				if (max >= msg->m_generators.getSize()) max = msg->m_generators.getSize() - 1;
				if (min >= 0 && max >= 0 && min <= max) {
					//correct variable
					for (int g = min; g <= max; g++)
					{
						if (msg->m_generators[g] == child_node_variant->m_object)
						{
							parent_node->accept(*this);
						}
					}
				}
				else {
					parent_node->accept(*this);
				}
			}
			else if (parent_node_variant->m_class == &hkbStateMachineClass) {
				hkbStateMachine* msg = (hkbStateMachine*)parent_node_variant->m_object;
				int min = -1; int max = -1; int variable_index = -1;
				if (msg->m_variableBindingSet != NULL)
				{
					hkbVariableBindingSet* vbs = msg->m_variableBindingSet;
					for (int b = 0; b < vbs->m_bindings.getSize(); b++)
					{
						string variable_name = _builder->getVariable(vbs->m_bindings[b].m_variableIndex).toUtf8().constData();
						for (int v = 0; v < _style_info.size(); v++) {
							if (_style_info[v].variable == variable_name)
							{
								variable_index = v;
								min = _style_info[v].min;
								max = _style_info[v].max;
								break;
							}
						}
					}
				}
				if (min >= 0 && max >= 0 && min <= max) {
					for (int s = 0; s < msg->m_states.getSize(); s++)
					{
						if (msg->m_states[s] == child_node_variant->m_object &&
							msg->m_states[s]->m_stateId >= min &&
							msg->m_states[s]->m_stateId <= max)
						{
							parent_node->accept(*this);
						}
					}
				}
				else {
					parent_node->accept(*this);
				}
			}
			else {
				parent_node->accept(*this);
			}
		}
		if (!_root->isVariant())
			break;
	}
}

void ClipCollector::handle_node(ProjectNode& node) {
	hkVariant* variant = (hkVariant*)node.data(1).value<unsigned long long>();
	//hkVariant* parent_variant = (hkVariant*)node.data(2).value<unsigned long long>();
	int file_index = node.data(3).value<int>();
	for (int p = 0; p < node.childCount(); p++)
	{
		ProjectNode* child_node = node.child(p);
		if (child_node == NULL)
			throw 666;
		if (!child_node->isVariant())
			continue;
		else {
			hkVariant* parent_node_variant = (hkVariant*)node.data(1).value<unsigned long long>();
			hkVariant* child_node_variant = (hkVariant*)child_node->data(1).value<unsigned long long>();
			if (parent_node_variant->m_class == &hkbManualSelectorGeneratorClass) {
				hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)parent_node_variant->m_object;
				int min = -1; int max = -1; int variable_index = -1;
				if (msg->m_variableBindingSet != NULL)
				{
					hkbVariableBindingSet* vbs = msg->m_variableBindingSet;
					for (int b = 0; b < vbs->m_bindings.getSize(); b++)
					{
						string variable_name = _builder->getVariable(vbs->m_bindings[b].m_variableIndex).toUtf8().constData();
						for (int v = 0; v < _style_info.size(); v++) {
							if (_style_info[v].variable == variable_name)
							{
								variable_index = v;
								min = _style_info[v].min;
								max = _style_info[v].max;
								break;
							}
						}
					}
				}
				//binded
				//TODO: fix this!
				if (max >= msg->m_generators.getSize()) max = msg->m_generators.getSize() - 1;
				if (min >= 0 && max >= 0 && min <= max && msg->m_generators.getSize()> max) {
					//correct variable
					for (int g = min; g <= max; g++)
					{
						child_node->accept(*this);
					}
				}
				else {
					child_node->accept(*this);
				}
			}
			else if (parent_node_variant->m_class == &hkbStateMachineClass) {
				hkbStateMachine* msg = (hkbStateMachine*)parent_node_variant->m_object;
				int min = -1; int max = -1; int variable_index = -1;
				if (msg->m_variableBindingSet != NULL)
				{
					hkbVariableBindingSet* vbs = msg->m_variableBindingSet;
					for (int b = 0; b < vbs->m_bindings.getSize(); b++)
					{
						string variable_name = _builder->getVariable(vbs->m_bindings[b].m_variableIndex).toUtf8().constData();
						for (int v = 0; v < _style_info.size(); v++) {
							if (_style_info[v].variable == variable_name)
							{
								variable_index = v;
								min = _style_info[v].min;
								max = _style_info[v].max;
								break;
							}
						}
					}
				}
				if (min >= 0 && max >= 0 && min <= max) {
					for (int s = 0; s < msg->m_states.getSize(); s++)
					{
						if (msg->m_states[s] == child_node_variant->m_object &&
							msg->m_states[s]->m_stateId >= min &&
							msg->m_states[s]->m_stateId <= max)
						{
							child_node->accept(*this);
						}
					}
				}
				else {
					child_node->accept(*this);
				}
			}
			else {
				if (variant->m_class == &hkbClipGeneratorClass)
				{
					hkbClipGenerator* state = (hkbClipGenerator*)variant->m_object;
					_result.insert(state->m_name.cString());
				}
				child_node->accept(*this);
			}
		}
	}	
}

void Saver::handle_hkx_node(ProjectNode& node)
{
	hkVariant* variant = (hkVariant*)node.data(1).value<unsigned long long>();
	//hkVariant* parent_variant = (hkVariant*)node.data(2).value<unsigned long long>();
	int file_index = node.data(3).value<int>();

	if (variant->m_class == &hkbClipGeneratorClass) 
	{
		handle_clip((hkbClipGenerator*)variant->m_object, file_index);
	}
	else if (variant->m_class == &hkbStateMachineTransitionInfoArrayClass) {
		handle_transition(node);
	}
}

void Saver::handle_animation(ProjectNode& node)
{
	string name_ = node.data(0).value<QString>().toUtf8().constData();
	fs::path path_ = node.data(1).value<QString>().toUtf8().constData();
	//we need to get all the events from the animation and also its motion data, appended by either import or behavior Builder
	_animation_relative_files.insert(name_);
	_animation_absolute_files[name_] = path_;
	auto animation_content = _manager.get(path_);
	hkaSplineCompressedAnimation* animation = NULL;
	for (auto& object : animation_content.second)
	{
		if (object.m_class == &hkaSplineCompressedAnimationClass)
		{
			animation = (hkaSplineCompressedAnimation*)object.m_object;
			break;
		}
	}

	AnimationInfo info;

	if (animation)
	{
		info.duration = animation->m_duration;
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
					info._events.insert({ this_hk_ann.m_time,this_hk_ann.m_text.cString() });
				}
			}
		}
	}

	//movements
	std::string movements;
	RootMovement movement;
	if (node.data(2).canConvert< QString>())
		movements = node.data(2).value<QString>().toUtf8().constData();
	else {
		//UGH, there were no movements, create something valid
		AnimData::ClipMovementData data;
		data.setDuration(to_string(movement.duration));
		data.setTraslations(movement.getClipTranslations());
		data.setRotations(movement.getClipRotations());
		movements = data.getBlock();
	}
	info._root_movements = movements;
	_cache_animation_info[name_] = info;
}

void Saver::save_cache() {
	fs::path project_folder = _manager.path(_project_file_index).parent_path();
	_cache->block.setHasProjectFiles(
		!_character_files_indices.empty() || !_behavior_files_indices.empty() || !_rig_file.empty()
	);
	std::vector<std::string> project_files;
	for (const auto& behavior_file_index : _behavior_files_indices)
	{
		fs::path file_path = _manager.path(behavior_file_index);
		file_path = fs::relative(file_path, project_folder);
		project_files.push_back(file_path.string());
	}
	for (const auto& character_file_index : _character_files_indices)
	{
		fs::path file_path = _manager.path(character_file_index);
		file_path = fs::relative(file_path, project_folder);
		project_files.push_back(file_path.string());
	}
	project_files.push_back(fs::relative(_rig_file, project_folder).string());
	AnimData::StringListBlock files_block; files_block.setStrings(project_files);
	_cache->block.setProjectFiles(files_block);
	if (_saving_character) {
		_cache->block.setHasAnimationCache(true);
		//create movement indices
		std::map<string, int> mapping;
		std::vector<AnimData::ClipMovementData> movementData;
		for (auto& entry : _cache_animation_info)
		{
			AnimData::ClipMovementData data; data.parseBlock(scannerpp::Scanner(entry.second._root_movements));
			data.setCacheIndex(movementData.size());
			//Broken animations
			if (entry.second.duration == 0.)
			{
				LOG << "WARNING: " << entry.first << " has duration 0." << log_endl;
			}
			data.setDuration(to_bstring(entry.second.duration));
			if (data.getTraslations().getStrings().empty())
			{
				LOG << "WARNING: " << entry.first << " has no translations. inserting default cache line" << log_endl;
				std::vector<std::string> def; def.push_back(to_bstring(entry.second.duration) + " 0 0 0");
				AnimData::StringListBlock b; b.setStrings(def);
				data.setTraslations(b);
			}
			if (data.getRotations().getStrings().empty()) 
			{
				LOG << "WARNING: " << entry.first << " has no rotations. Inserting default cache line" << log_endl;
				std::vector<std::string> def; def.push_back(to_bstring(entry.second.duration) + " 0 0 0 1");
				AnimData::StringListBlock b; b.setStrings(def);
				data.setRotations(b);
			}
			movementData.push_back(data);
			mapping[entry.first] = data.getCacheIndex();
		}
		_cache->movements.setMovementData(movementData);
		//add clips
		std::list<AnimData::ClipGeneratorBlock> clips;
		for (auto& entry : _cache_clip_info)
		{
			AnimData::ClipGeneratorBlock clip;
			clip.setCacheIndex(mapping[entry.second.animation_name]);
			clip.setPlaybackSpeed(to_bstring(entry.second.playbackSpeed));
			clip.setCropStartTime(to_bstring(entry.second.cropStartTime));
			clip.setCropEndTime(to_bstring(entry.second.cropEndTime));
			clip.setName(entry.first);
			std::multimap<float, std::string> animations_events = _cache_animation_info[entry.second.animation_name]._events;
			animations_events.insert(entry.second._events.begin(), entry.second._events.end());
			std::vector<string> event_lines;
			for (auto& event : animations_events)
			{
				string line = event.second + ":" + to_bstring(event.first);
				event_lines.push_back(line);
			}
			AnimData::StringListBlock events_block; events_block.setStrings(event_lines);
			clip.setEvents(events_block);
			clips.push_back(clip);
		}
		_cache->block.setClips(clips);

		//Animation sets
		CreatureCacheEntry* _creature_entry = dynamic_cast<CreatureCacheEntry*>(_cache);
		AnimData::StringListBlock projectFiles;
		std::vector<AnimData::ProjectAttackBlock> projectAttackBlocks;
		//Base set
		AnimData::ProjectAttackBlock base_block;

		int style_index = -1;
		if (!_cache_styles.empty())
		{

			auto& style = _cache_styles.at(0);
			for (const auto& info : style) {
				base_block.getHandVariableData().addVariable(
					{ info.variable, (AnimData::HandVariableData::EquipType)info.min, (AnimData::HandVariableData::EquipType)info.max });
			}
			style_index = 0;
		}
		if (!_cache_style_attacks.empty())
		{
			auto& attacks = _cache_style_attacks.at(style_index);
			AnimData::ClipAttacksBlock attacks_block;
			for (auto& attack : attacks) {
				AnimData::AttackDataBlock block;
				block.setEventName(attack.first);
				block.setUnk1(attack.second);
				auto clips = _cache_attack_clips[attack.first];
				for (auto& clip : clips) {
					if (clip.first == style_index)
						block.addClip(clip.second);
				}
				attacks_block.attackData.push_back(block);
			}
			attacks_block.setBlocks(attacks_block.attackData.size());
			base_block.setAttackData(attacks_block);
		}

		AnimData::ClipFilesCRC32Block crc32s;
		auto& crc_strings = crc32s.getStrings();
		for (auto& file : _animation_absolute_files)
		{
			//meshes\actors\dragon\animations
			fs::path folder = "meshes" / fs::relative(file.second.parent_path(), _manager.workspace().getFolder());
			std::string path_to_crc = folder.string();
			transform(path_to_crc.begin(), path_to_crc.end(), path_to_crc.begin(), ::tolower);
			std::string to_crc = fs::path(file.second).filename().replace_extension("").string();
			transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);

			long long pathcrc = stoll(HkCRC::compute(path_to_crc), NULL, 16);
			string path_crc_str = to_string(pathcrc);

			long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
			string crc_str = to_string(crc);

			crc_strings.push_back(path_crc_str);
			crc_strings.push_back(crc_str);
			crc_strings.push_back("7891816");
		}
		base_block.setCrc32Data(crc32s);
		projectFiles.append("base_set.txt");
		projectAttackBlocks.push_back(base_block);

		//additional styles
		if (_cache_styles.size() > 1) {
			for (int style_index = 1; style_index < _cache_styles.size(); style_index++)
			{
				AnimData::ProjectAttackBlock style_block;

				auto& style = _cache_styles.at(style_index);
				for (const auto& info : style) {
					style_block.getHandVariableData().addVariable(
						{ info.variable, (AnimData::HandVariableData::EquipType)info.min, (AnimData::HandVariableData::EquipType)info.max });
				}

				if (_cache_style_attacks.find(style_index) != _cache_style_attacks.end())
				{
					auto& attacks = _cache_style_attacks.at(style_index);
					AnimData::ClipAttacksBlock attacks_block;
					//AnimData::ClipFilesCRC32Block crc32s;
					auto& crc_strings = crc32s.getStrings();
					for (auto& attack : attacks) {
						AnimData::AttackDataBlock block;
						block.setEventName(attack.first);
						block.setUnk1(attack.second);
						auto clips = _cache_attack_clips[attack.first];
						for (auto& clip : clips) {
							if (clip.first == style_index)
							{
								block.addClip(clip.second);
								/*auto file = _animation_absolute_files[_cache_clip_info[clip.second].animation_name];

								fs::path folder = "meshes" / fs::relative(file.parent_path(), _manager.workspace().getFolder());
								std::string path_to_crc = folder.string();
								transform(path_to_crc.begin(), path_to_crc.end(), path_to_crc.begin(), ::tolower);
								std::string to_crc = fs::path(file).filename().replace_extension("").string();
								transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);

								long long pathcrc = stoll(HkCRC::compute(path_to_crc), NULL, 16);
								string path_crc_str = to_string(pathcrc);

								long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
								string crc_str = to_string(crc);

								crc_strings.push_back(path_crc_str);
								crc_strings.push_back(crc_str);
								crc_strings.push_back("7891816");*/
							}
						}
						attacks_block.attackData.push_back(block);
					}
					attacks_block.setBlocks(attacks_block.attackData.size());
					style_block.setAttackData(attacks_block);

				}
				else {
					//the engine is stupid enough to not load this? however works ingame
				}
				style_block.setCrc32Data(crc32s);
				projectFiles.append(std::string("style")+to_string(style_index)+".txt");
				projectAttackBlocks.push_back(style_block);
			}
		}

		_creature_entry->sets.setProjectFiles(projectFiles);
		_creature_entry->sets.setProjectAttackBlocks(projectAttackBlocks);
	}

	_manager.save_cache(_project_file_index);
}

void Saver::handle_animation_style(ProjectNode& node)
{
	int style_index = -1;
	if (node.data(1).canConvert<QVector<QString>>()) {
		QVector<QString> variables = node.data(1).value<QVector<QString>>();
		QVector<int> mins = node.data(2).value<QVector<int>>();
		QVector<int> maxs = node.data(3).value<QVector<int>>();
		std::vector<StyleInfo> infos;
		for (int i = 0; i < variables.count(); i++)
		{
			StyleInfo info;
			info.variable = variables[i].toUtf8().constData();
			info.min = mins[i];
			info.max = maxs[i];
			infos.push_back(info);
		}
		style_index = _cache_styles.size();
		_cache_styles.push_back(infos);
	}
	for (int n = 0; n < node.childCount(); n++)
	{
		auto attack_node = node.child(n);
		_cache_style_attacks[style_index].insert(
			{
				attack_node->data(0).value<QString>().toUtf8().constData(),
				attack_node->data(1).value<int>()
			}
		);
		_cache_attacks.insert(attack_node->data(0).value<QString>().toUtf8().constData());
	}
}

bool AnimationSetBuilder::handle(ProjectNode& node, BehaviorBuilder* builder) {
	_branch.push_front(node.data(0).value<QString>().toUtf8().constData());
	if (node.isVariant())
	{
		hkVariant* variant = (hkVariant*)node.data(1).value<unsigned long long>();
		
		if (variant->m_class == &hkbManualSelectorGeneratorClass) {
			hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)variant->m_object;
			int binding = isVariableBinded(msg, builder);
			if (binding >= 0 && binding < _variables.size())
			{

			}
		}
		else if (variant->m_class == &hkbStateMachineClass) {
			hkbStateMachine* fsm = (hkbStateMachine*)variant->m_object;
			_states_stack.push_front({ &node , -1 });
			//check if we find an attack
			if (fsm->m_wildcardTransitions != NULL) {
				for (int wt = 0; wt < fsm->m_wildcardTransitions->m_transitions.getSize(); wt++)
				{
					auto& transition = fsm->m_wildcardTransitions->m_transitions[wt];
					auto event_name = builder->getEvent(transition.m_eventId);
					if ((event_name.startsWith("attackStart", Qt::CaseInsensitive)) ||
						(event_name.startsWith("attackPowerStart", Qt::CaseInsensitive)) ||
						(event_name.startsWith("bashStart", Qt::CaseInsensitive)))
					{
						//it is an attack, check if we already have it (multiple transitions over the same attack from different states)
						attack_state_id_t attack_id = { &node, event_name.toUtf8().constData(), transition.m_toStateId, transition.m_toNestedStateId };
						auto id_it = std::find(_attack_state.begin(), _attack_state.end(), attack_id);
						if (id_it == _attack_state.end())
							_attack_state.push_back(attack_id);
					}
				}
			}
			//check into state transitions too
			for (int state_id = 0; state_id < fsm->m_states.getSize(); ++state_id)
			{
				auto* state = fsm->m_states[state_id];
				if (state->m_transitions != NULL) {
					for (int t = 0; t < state->m_transitions->m_transitions.getSize(); ++t)
					{
						auto& transition = state->m_transitions->m_transitions[t];
						auto event_name = builder->getEvent(transition.m_eventId);
						if ((event_name.startsWith("attackStart", Qt::CaseInsensitive)) ||
							(event_name.startsWith("attackPowerStart", Qt::CaseInsensitive)) ||
							(event_name.startsWith("bashStart", Qt::CaseInsensitive)))
						{
							//it is an attack, check if we already have it (multiple transitions over the same attack from different states)
							attack_state_id_t attack_id = { &node, event_name.toUtf8().constData(), transition.m_toStateId, transition.m_toNestedStateId };
							auto id_it = std::find(_attack_state.begin(), _attack_state.end(), attack_id);
							if (id_it == _attack_state.end())
								_attack_state.push_back(attack_id);
						}
					}
				}
			}
		}
		else if (variant->m_class == &hkbStateMachineStateInfoClass)
		{
			hkbStateMachineStateInfo* info = (hkbStateMachineStateInfo*)variant->m_object;
			_states_stack.push_front({ &node , info->m_stateId });
			if (!_attack_state.empty()) {			
				if (_states_stack.size() > 3)
				{
					for (std::vector<attack_state_id_t>::iterator it = _attack_state.begin();
						it != _attack_state.end();) {
						auto& attack_transition = *it;
						auto& fsm_node = get<0>(attack_transition);
						auto& state = get<2>(attack_transition);
						auto& nested_state = get<3>(attack_transition);
						if (_states_stack.at(3).first == fsm_node &&
							_states_stack.at(2).second == state && 
							/*unimportant FSM for nested state*/
							_states_stack.at(0).second == nested_state)
						{
							auto& current_set = get<2>(sets_stack.front().second);
							//found the nested state, initialize the set
							auto attack_name = get<1>(attack_transition);
							_attacks_nodes.push_front({ &node, attack_name });
							_attack_state.erase(it);
							break;
						}
						it++;
					}
				}
			}
		}
		else if (variant->m_class == &hkbClipGeneratorClass)
		{
			hkbClipGenerator* info = (hkbClipGenerator*)variant->m_object;
			if (!_attacks_nodes.empty()) {
				auto& current_set = get<2>(sets_stack.front().second);
				auto attack_name = _attacks_nodes.front().second;
				current_set[attack_name].insert(info->m_name.cString());
			}
		}
	}
	return true;
}

void AnimationSetBuilder::end_branch(ProjectNode& node, BehaviorBuilder* builder) 
{
	if (!_attacks_nodes.empty() && _attacks_nodes.front().first == &node) {
		_attacks_nodes.pop_front();
	}
	if (!_states_stack.empty() && _states_stack.front().first == &node)
	{
		_states_stack.pop_front();
	}
	_branch.pop_front();
}