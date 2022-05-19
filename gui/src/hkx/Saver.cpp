#include <src/Skyrim/TES5File.h>
#include <src/Collection.h>
#include <src/ModFile.h>

#include <fstream>

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
#include <hkbModifierGenerator_0.h>
#include <BSIsActiveModifier_1.h>
#include <hkbBlenderGeneratorChild_2.h>

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

#undef max

Saver::Saver(ResourceManager& manager, ProjectNode* project_root, ProjectTreeModel* viewmodel) :
	_saving_character(false),
	_manager(manager),
	_viewmodel(viewmodel)
{
	_animation_sets_root_fsm_depth = std::numeric_limits<int>::max();
	if (project_root->isCharacter())
	{
		_saving_character = true;
	}

	fs::path ppath = project_root->data(0).toString().toUtf8().constData();

	std::string to_crc = ppath.filename().replace_extension().string();
	transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);

	CreatureCacheEntry* entry = (CreatureCacheEntry*)_manager.findCacheEntry(to_crc);
	for (int i = 0; i < entry->getProjectSetFiles().size(); i++)
	{
		bool all_zero = true;
		bool has_var = false;
		auto& temp = entry->getProjectAttackBlocks().at(i);
		if (!temp.getHandVariableData().getVariables().empty())
			has_var = true;
		for (int j = 0; j < temp.getHandVariableData().getVariables().size(); j++)
		{
			if (temp.getHandVariableData().getVariables().at(j).value_min != 0 ||
				temp.getHandVariableData().getVariables().at(j).value_max != 0)
			{
				all_zero = false;
			}
		}
		if (all_zero && has_var)
		{
			auto& strings = temp.getCrc32Data().getStrings();
			int k = 0;
			for (auto& string: strings)
			{
				if (k % 3 == 1)
					_crc_to_find[string] = "";
				k++;
			}
			
		}
	}

	_right_current_animation_set.push_front({});
	_left_current_animation_set.push_front({});
	_animation_sets_stack.push_front({});
	_fsm_reacheable_states_stack.push_front({});
	__debugbreak();

	/*
		{0} 'HandToHandMelee',
	{1} 'OneHandSword',
	{2} 'OneHandDagger',
	{3} 'OneHandAxe',
	{4} 'OneHandMace',
	{5} 'TwoHandSword',
	{6} 'TwoHandAxe',
	{7} 'Bow /crossbow',
	{8} 'Staff',
	{9} 'Magic'
	10 Shield
	11 Torch
	12 Player crossbow
	
	*/


	project_root->accept(*this);



	__debugbreak();
//	PathBuilder builder;
//	if (_fsm_root != nullptr)
//	{
//#pragma omp parallel for
//		for (int i=0; i<_clips.size(); i++)
//		{
//			RecursiveClipPathBuilder< PathBuilder> r_visitor(
//				*_clips[i], builder,
//				_manager,
//				_behaviors_references_nodes_map,
//				_behaviors_references_builders_map,
//				_behavior_files_indices.at(0));
//		}
//	}
	//__debugbreak();
	//save_cache();
}

ClipPath PathBuilder::handle(ProjectNode& node, BehaviorBuilder* builder, ProjectNode* child) {
	if (node.isVariant())
	{
		hkVariant* variant = (hkVariant*)node.data(1).value<unsigned long long>();
		if (variant->m_class == &hkbStateMachineStateInfoClass)
		{
			hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)variant->m_object;
			return ClipPath();
		} 
		else if (variant->m_class == &hkbManualSelectorGeneratorClass) 
		{ 
			hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)variant->m_object;
			if (msg->m_variableBindingSet != NULL)
			{
				hkbVariableBindingSet* vbs = msg->m_variableBindingSet;
				for (int b = 0; b < vbs->m_bindings.getSize(); b++)
				{
					if (0 == strcmp(vbs->m_bindings[b].m_memberPath.cString(),"selectedGeneratorIndex"))
					{
						ClipPath p = { 0 };
						auto& indices = node.indicesOf(child);
						p.data.referenceArray.valid_references = indices.size();
						for (int i = 0; i < indices.size(); i++)
							p.data.referenceArray.multipleReferencesIndices[i] = indices[i];
						//std::stringstream out;
						//size_t msg_index = child_index;
						//string variable_name = builder->getVariable(vbs->m_bindings[b].m_variableIndex).toUtf8().constData();
						//out << variable_name  << ":" << msg_index;
						//return out.str();*/
						return p;
					}
				}
			}
		}
		else if (variant->m_class == &hkbStateMachineClass) {

		}
	}
	return ClipPath();
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

/*
0 H2H
1 1HS
2 1HD
3 1HA
4 1HM
5 2HS
6 2HM
7 Bow
8 Staff
9 Speel
10 Shield
11 Torch
12 CBow

*/

std::string set_name(int value)
{
	switch (value)
	{
	case 0: return "H2H";
	case 1: return "1HS";
	case 2: return "1HD";
	case 3: return "1HA";
	case 4: return "1HM";
	case 5: return "2HS";
	case 6: return "2HM";
	case 7: return "Bow";
	case 8: return "Staff";
	case 9: return "Spell";
	case 10: return "Shield";
	case 11: return "Torch";
	case 12: return "CBow";
	default: break;
	}
	return "error value " + std::to_string(value);
}


void Saver::handle_behavior(ProjectNode& node)
{
	fs::path behavior_path = node.data(1).value<QString>().toUtf8().constData();
	size_t file_index = _manager.index(behavior_path);
	_manager.save(file_index);


	
	//_behavior_files_indices.push_back(behavior_index);
	//_behavior_nodes[behavior_index] = &node;


	//BehaviorBuilder* builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));

	//auto& behavior_contents = _manager.get(file_index);
	//bool found = false;
	//for (const auto& content : behavior_contents.second)
	//{
	//	if (content.m_class == &hkbClipGeneratorClass)
	//	{
	//		hkbClipGenerator* clip = (hkbClipGenerator*)content.m_object;
	//		_new_animation_sets[""].insert(clip->m_animationName.cString());
	//	}
	//	else if (content.m_class == &hkbManualSelectorGeneratorClass)
	//	{
	//		bool recurse = false;
	//		hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)content.m_object;
	//		if (msg->m_variableBindingSet != NULL) {
	//			for (int binding = 0; binding < msg->m_variableBindingSet->m_bindings.getSize(); ++binding)
	//			{
	//				auto& value = msg->m_variableBindingSet->m_bindings[binding];
	//				string variable_name = builder->getVariable(value.m_variableIndex).toUtf8().constData();
	//				if (variable_name == _animation_set_binding_variable)
	//				{
	//					recurse = true;
	//					break;
	//				}
	//			}
	//		}
	//		if (recurse)
	//		{
	//			for (int generator = 0; generator < msg->m_generators.getSize(); ++generator)
	//			{
	//				_current_animation_set = set_name(generator);
	//				auto* node = _manager.findNode(file_index, (void*)msg->m_generators[generator]);
	//				if (NULL == node)
	//					__debugbreak();
	//				node->accept(*this);
	//				_current_animation_set = "";
	//			}
	//		}
	//	}
	//	else if (content.m_class == &hkbStateMachineClass)
	//	{
	//	
	//	}
	//}


	//recurse(node);
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

std::set<std::string> Saver::find_animation_driven_transitions(ProjectNode& node, BehaviorBuilder* behavior_builder)
{
	////how I got here?
	ProjectNode* parent_state_node = NULL;
	hkbStateMachineStateInfo* parent_state = NULL;
	ProjectNode* parent_fsm_node = NULL;
	hkbStateMachine* parent_fsm = NULL;
	hkbStateMachineStateInfo* grandparent_state = NULL;
	hkbStateMachine* grandparent_fsm = NULL;
	ProjectNode* grandparent_fsm_node = NULL;

	for (int i = 0; i < _fsm_stack.size(); ++i)
	{
		hkVariant* variant = (hkVariant*)_fsm_stack.at(i)->data(1).value<unsigned long long>();
		if (NULL != variant)
		{
			if (variant->m_class == &hkbStateMachineStateInfoClass)
			{
				if (parent_state == NULL)
				{
					parent_state = (hkbStateMachineStateInfo*)variant->m_object;
					parent_state_node = _fsm_stack.at(i);
				}
				else if (grandparent_state == NULL)
				{
					grandparent_state = (hkbStateMachineStateInfo*)variant->m_object;
				}
				else
					continue;
			}
			else if (variant->m_class == &hkbStateMachineClass && parent_state!= NULL)
			{
				if (parent_fsm == NULL)
				{
					parent_fsm = (hkbStateMachine*)variant->m_object;
					parent_fsm_node = _fsm_stack.at(i);
				}
				else if (grandparent_fsm == NULL)
				{
					grandparent_fsm = (hkbStateMachine*)variant->m_object;
					grandparent_fsm_node = _fsm_stack.at(i);
					break;
				}
				else
					continue;
			}
		}
	}
	return {};
}


void Saver::handle_animation_binded_fsm(ProjectNode& node, int file_index)
{
	ProjectNode* child_fsm = &node;
	hkVariant* child_fsm_variant = (hkVariant*)child_fsm->data(1).value<unsigned long long>();
	hkbStateMachine* hk_fsm = (hkbStateMachine*)child_fsm_variant->m_object;

	map<int, ProjectNode*> states;
	//build state indexes
	for (int fsm_child_index = 0; fsm_child_index < child_fsm->childCount(); ++fsm_child_index)
	{
		hkVariant* child_fsm_variant = (hkVariant*)child_fsm->child(fsm_child_index)->data(1).value<unsigned long long>();
		if (child_fsm_variant->m_class == &hkbStateMachineStateInfoClass)
		{
			hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)child_fsm_variant->m_object;
			states[state->m_stateId] = child_fsm->child(fsm_child_index);
		}
	}

	std::map<ProjectNode*, std::set<string>> state_events;
	std::map<ProjectNode*, std::set<ProjectNode*>> relations;

	//Visit FSM by wildcard transitions
	if (NULL != hk_fsm->m_wildcardTransitions)
	{
		auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
		auto& transitions = hk_fsm->m_wildcardTransitions->m_transitions;
		for (int t = 0; t < transitions.getSize(); t++)
		{
			auto& transition = transitions[t];
			//_animation_sets_stack.push_front({});
			auto* state = states.at(transition.m_toStateId);
			std::string set_event = behavior_builder->getEvent(transition.m_eventId).toUtf8().constData();
			state_events[state].insert(set_event);
			//state->accept(*this);
			//Also visit its transitions
			hkVariant* hk_state = (hkVariant*)state->data(1).value<unsigned long long>();
			hkbStateMachineStateInfo* fsm_hk_state = (hkbStateMachineStateInfo*)hk_state->m_object;
			if (NULL != fsm_hk_state->m_transitions)
			{
				auto& state_transitions = fsm_hk_state->m_transitions->m_transitions;
				for (int ct = 0; ct < state_transitions.getSize(); ct++)
				{
					auto& state_transition = state_transitions[ct];
					auto* trans_state = states.at(state_transition.m_toStateId);
					relations[state].insert(trans_state);
				}
			}
		}
	}
	//visit also parent fsm, but only if more than one state?
	if (_fsm_stack.size() >= 2)
	{
		ProjectNode* parent_fsm = _fsm_stack.at(1);
		hkVariant* parent_fsm_variant = (hkVariant*)parent_fsm->data(1).value<unsigned long long>();
		hkbStateMachine* hk_fsm = (hkbStateMachine*)parent_fsm_variant->m_object;

		ProjectNode* parent_state = _fsm_stack.at(0);
		hkVariant* parent_state_variant = (hkVariant*)parent_state->data(1).value<unsigned long long>();
		hkbStateMachineStateInfo* hk_state = (hkbStateMachineStateInfo*)parent_state_variant->m_object;

		map<int, ProjectNode*> parent_states;
		//build parent fsm indexes
		for (int fsm_child_index = 0; fsm_child_index < parent_fsm->childCount(); ++fsm_child_index)
		{
			hkVariant* child_fsm_variant = (hkVariant*)parent_fsm->child(fsm_child_index)->data(1).value<unsigned long long>();
			if (child_fsm_variant->m_class == &hkbStateMachineStateInfoClass)
			{
				hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)child_fsm_variant->m_object;
				parent_states[state->m_stateId] = parent_fsm->child(fsm_child_index);
			}
		}

		auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));

		//Visit parent FSM wildcard transitions
		if (NULL != hk_fsm->m_wildcardTransitions)
		{
			//_animation_sets_stack.push_front({});

			auto& transitions = hk_fsm->m_wildcardTransitions->m_transitions;
			int last_state = -1;
			for (int t = 0; t < transitions.getSize(); t++)
			{
				auto& transition = transitions[t];
				if (transition.m_toStateId == hk_state->m_stateId)
				{
					std::string set_event = behavior_builder->getEvent(transition.m_eventId).toUtf8().constData();
					if (states.find(transition.m_toNestedStateId) == states.end())
					{
						//TODO
						continue;
					}
					auto* dest_state = states.at(transition.m_toNestedStateId);
					state_events[dest_state].insert(set_event);
				}
			}
		}

		//finally visit parent FSM states
		for (int fsm_child_index = 0; fsm_child_index < parent_fsm->childCount(); ++fsm_child_index)
		{
			hkVariant* child_fsm_variant = (hkVariant*)parent_fsm->child(fsm_child_index)->data(1).value<unsigned long long>();
			if (child_fsm_variant->m_class == &hkbStateMachineStateInfoClass)
			{
				hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)child_fsm_variant->m_object;
				if (NULL != state->m_transitions)
				{
					auto& transitions = state->m_transitions->m_transitions;
					int last_state = -1;
					for (int t = 0; t < transitions.getSize(); t++)
					{
						auto& transition = transitions[t];
						if (transition.m_toStateId == hk_state->m_stateId)
						{
							std::string set_event = behavior_builder->getEvent(transition.m_eventId).toUtf8().constData();
							//nested states are not accurate on riekling
							//if (transition.m_toNestedStateId == 0 && states.find(0) == states.end())
							//{
							//	transition.m_toNestedStateId = 1;
							//}
							if (states.find(transition.m_toNestedStateId) == states.end())
							{
								//TODO
								continue;
							}
							auto* dest_state = states.at(transition.m_toNestedStateId);
							state_events[dest_state].insert(set_event);
						}
					}
				}
			}
		}
	}

	bool binded = false;
	//for (const auto& binding_var : _animation_styles_control_variables)
	//{
	//	if (_variables_values.find(binding_var) != _variables_values.end())
	//	{
	//		binded = true;
	//		break;
	//	}
	//}

	if (!binded)
	{
		for (const auto& entry : state_events)
		{
			_animation_sets_stack.push_front({});
			entry.first->accept(*this);
			for (const auto& related_state : relations[entry.first])
			{
				related_state->accept(*this);
			}
			std::string events_string;
			for (const auto& event_string : entry.second)
			{
				events_string += event_string + ",";
			}
			for (const auto& item : _animation_sets_stack.front())
			{
				_animation_sets[events_string].insert(item);
			}
			_animation_sets_stack.pop_front();
		}
	}
	else {
		_animation_sets_stack.push_front({});
		std::string events_string;
		for (const auto& entry : state_events)
		{

			entry.first->accept(*this);
			for (const auto& related_state : relations[entry.first])
			{
				related_state->accept(*this);
			}
			for (const auto& event_string : entry.second)
			{
				events_string += event_string + ",";
			}
		}
		for (const auto& item : _animation_sets_stack.front())
		{
			_animation_sets[events_string].insert(item);
		}
		_animation_sets_stack.pop_front();
	}
}

void reach_states(
	const std::map<int, ProjectNode*>& fsm_states,
	std::set<ProjectNode*>& reacheable_states, ProjectNode* state)
{
	if (reacheable_states.insert(state).second)
	{
		hkVariant* variant = (hkVariant*)state->data(1).value<unsigned long long>();
		hkbStateMachineStateInfo* hk_state = (hkbStateMachineStateInfo*)variant->m_object;
		if (NULL != hk_state->m_transitions)
		{
			auto& transitions = hk_state->m_transitions->m_transitions;
			for (int t = 0; t < transitions.getSize(); t++)
			{
				if (fsm_states.find(transitions[t].m_toStateId) != fsm_states.end())
				{
					reach_states(fsm_states, reacheable_states, fsm_states.at(transitions[t].m_toStateId));
				}
			}
		}
	}
}

// 1 right, 2 left
int Saver::isSetBinded(hkbBindable* bindable, BehaviorBuilder* builder, const std::string& path)
{
	if (bindable->m_variableBindingSet != NULL) {
		for (int binding = 0; binding < bindable->m_variableBindingSet->m_bindings.getSize(); ++binding)
		{
			auto& value = bindable->m_variableBindingSet->m_bindings[binding];
			string variable_name = builder->getVariable(value.m_variableIndex).toUtf8().constData();
			if (path == value.m_memberPath.cString())
			{
				if (_right_animation_set_binding_variable.find(variable_name) != _right_animation_set_binding_variable.end())
					return 1;
				if (_left_animation_set_binding_variable.find(variable_name) != _left_animation_set_binding_variable.end())
					return 2;
			}
		}
	}
	return 0;
}

void Saver::handle_action(ProjectNode& node)
{
	std::string edid = node.data(0).value<QString>().toUtf8().constData();
	bool equip_action = false;
	if (edid == "ActionDraw" || edid == "ActionForceEquip")
		equip_action = true;

	if (equip_action)
		_equip_action = true;
	recurse(node);
	if (equip_action)
		_equip_action = false;
}

void Saver::handle_idle(ProjectNode& node)
{
	if (_equip_action)
	{
		Sk::IDLERecord* idle = (Sk::IDLERecord*)node.data(1).value<unsigned long long>();
		for (int left = 0; left <= 12; left++)
		{
			for (int right = 0; right <= 12; right++)
			{
				if (!idle->CTDA.value.empty())
				{
					bool do_recurse = true;
					for (auto& condition : idle->CTDA.value)
					{
						auto function = condition->CTDA.value.ifunc;
						//Index: 597; Name: 'GetEquippedItemType
						if (function == 597) {
							auto source = condition->CTDA.value.param1;

							float value = (float)condition->CTDA.value.compValue;
							/*
								enum operTypeType
								{
									eEqual = 0<<5,
									eNotEqual = 1<<5,
									eGreater = 2<<5,
									eGreaterOrEqual = 3<<5,
									eLess = 4<<5,
									eLessOrEqual = 5<<5,
									eOperTypeMask = 0xE0 // First 3 bits
								};
							*/
							switch (condition->CTDA.value.operType) {
							case Sk::SKCTDA::operTypeType::eEqual:
								switch (source) {
								case Sk::SKCTDA::paramCastingSourceType::Left:
									if (left == value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								case Sk::SKCTDA::paramCastingSourceType::Right:
									if (right == value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								}
								break;
							case Sk::SKCTDA::operTypeType::eNotEqual:
								switch (source) {
								case Sk::SKCTDA::paramCastingSourceType::Left:
									if (left != value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								case Sk::SKCTDA::paramCastingSourceType::Right:
									if (right != value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								}
								break;
							case Sk::SKCTDA::operTypeType::eGreater:
								switch (source) {
								case Sk::SKCTDA::paramCastingSourceType::Left:
									if (left > value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								case Sk::SKCTDA::paramCastingSourceType::Right:
									if (right > value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								}
								break;
							case Sk::SKCTDA::operTypeType::eGreaterOrEqual:
								switch (source) {
								case Sk::SKCTDA::paramCastingSourceType::Left:
									if (left >= value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								case Sk::SKCTDA::paramCastingSourceType::Right:
									if (right >= value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								}
								break;
							case Sk::SKCTDA::operTypeType::eLess:
								switch (source) {
								case Sk::SKCTDA::paramCastingSourceType::Left:
									if (left < value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								case Sk::SKCTDA::paramCastingSourceType::Right:
									if (right < value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								}
								break;
							case Sk::SKCTDA::operTypeType::eLessOrEqual:
								switch (source) {
								case Sk::SKCTDA::paramCastingSourceType::Left:
									if (left <= value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								case Sk::SKCTDA::paramCastingSourceType::Right:
									if (right <= value)
									{
										//NTD
									}
									else {
										do_recurse = false;
									}
									break;
								}
								break;
							}
						}
					}
				
					if (do_recurse) {
						if (idle->ENAM.value != NULL)
							_equip_event_sets[{set_name(left), set_name(right)}].insert(idle->ENAM.value);
						else
						{
							for (int i = 0; i < node.childCount(); i++)
							{
								size_t event_size = _equip_event_sets.size();
								node.child(i)->accept(*this);
								if (event_size != _equip_event_sets.size())
									break;
							}
						}
					}
				}
				else {
					//no condition
					if (idle->ENAM.value != NULL)
						_equip_event_sets[{set_name(left), set_name(right)}].insert(idle->ENAM.value);
					else
						for (int i = 0; i < node.childCount(); i++)
						{
							size_t event_size = _equip_event_sets.size();
							node.child(i)->accept(*this);
							if (event_size != _equip_event_sets.size())
								break;
						}
				}
			}
		}
	}
	else {
		//no interest?
		for (int i = 0; i < node.childCount(); i++)
		{
			size_t event_size = _equip_event_sets.size();
			node.child(i)->accept(*this);
			if (event_size != _equip_event_sets.size())
				break;
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
		//std::ofstream log("logfile.txt", std::ios_base::app | std::ios_base::out);

		//if (std::find(_clips.begin(), _clips.end(), &node) == _clips.end())
		//	_clips.push_back(&node);
		//handle_clip((hkbClipGenerator*)variant->m_object, file_index);
		hkbClipGenerator* clip = (hkbClipGenerator*)variant->m_object;
		if (!_animation_driven)
		{
			if (_new_animation_sets[{"", ""}].find(clip->m_animationName.cString()) == _new_animation_sets[{"", ""}].end())
				_new_animation_sets[{_left_current_animation_set.front(), _right_current_animation_set.front()}].insert(clip->m_animationName.cString());
		}
		else {
			_new_animation_sets[{"", ""}].insert(clip->m_animationName.cString());
		}
		if (!_current_attack_set.empty())
		{
			_new_attack_sets[{_left_current_animation_set.front(), _right_current_animation_set.front(), _current_attack_set.front()}].insert(clip->m_animationName.cString());
		}


		//std::string clip_name = fs::path(clip->m_animationName.cString()).filename().replace_extension("").string();

		//std::string to_crc = clip_name;
		//transform(to_crc.begin(), to_crc.end(), to_crc.begin(), ::tolower);

		//long long crc = stoll(HkCRC::compute(to_crc), NULL, 16);
		//string crc_str = to_string(crc);

		////fs::path p = _nodes_stack / clip->m_animationName.cString();
		//int right = -1;
		//int left = -1;
		//if (_variables_values.find("iLeftHandType") != _variables_values.end())
		//{
		//	left = _variables_values["iLeftHandType"];
		//}
		//else if (_variables_values.find("iLeftHandEquipped") != _variables_values.end())
		//{
		//	left = _variables_values["iLeftHandEquipped"];
		//}
		//if (_variables_values.find("iRightHandType") != _variables_values.end())
		//{
		//	right = _variables_values["iRightHandType"];
		//}
		//else if (_variables_values.find("iRightHandEquipped") != _variables_values.end())
		//{
		//	right = _variables_values["iRightHandEquipped"];
		//}
		//if (left != -1)
		//{
		//	/*if (right != -1)
		//	{*/
		//		_weapon_animation_sets[{left, right}].insert({ clip->m_animationName.cString(), _nodes_stack.string() });
		//		//log << "[" << left << "," << right << "] " << clip->m_animationName.cString() << " from " << _nodes_stack.string() << endl;
		//	//}
		//	//else {
		//	//	log << "[" << left << ",*] " << clip->m_animationName.cString() << " from " << _nodes_stack.string() << endl;
		//	//	if (left < 5)
		//	//	{
		//	//		//for (int i = 0; i < 5; i++)
		//	//		//{
		//	//			_weapon_animation_sets[{left, 0}].insert({ clip->m_animationName.cString(), _nodes_stack.string() });
		//	//		//}
		//	//	}
		//	//	else {
		//	//		_weapon_animation_sets[{left, left}].insert({ clip->m_animationName.cString(), _nodes_stack.string() });
		//	//	}
		//	//}
		//}
		//else if (right != -1)
		//{
		//	/*if (left != -1)
		//	{*/
		//		_weapon_animation_sets[{left, right}].insert({ clip->m_animationName.cString(), _nodes_stack.string() });
		//		//log << "[" << left << "," << right << "] " << clip->m_animationName.cString() << " from " << _nodes_stack.string() << endl;
		//	//}
		//	//else {
		//	//	log << "[*," << right << "] " << clip->m_animationName.cString() << " from " << _nodes_stack.string() << endl;
		//	//	//if (right == 5 || )
		//	//	//{
		//	//	//	//for (int i = 0; i < 5; i++)
		//	//	//	//{
		//	//	//		_weapon_animation_sets[{-1, right}].insert({ clip->m_animationName.cString(), _nodes_stack.string() });
		//	//	//	//}
		//	//	//}
		//	//	//else {
		//	//		_weapon_animation_sets[{left, right}].insert({ clip->m_animationName.cString(), _nodes_stack.string() });
		//	//	//}
		//	//}
		//}
		//else {
		//	_animation_sets_stack.front().insert(clip->m_animationName.cString());
		//}
		////else
		////{
		////	_animation_sets_stack.front().insert(clip->m_animationName.cString());
		////}
		////if (_attack_animations.size() > 0)
		////{
		////	if (_attack_animations.size() > 1)
		////	{
		////		__debugbreak();
		////	}
		////	_attack_animations.front().second.insert(clip->m_animationName.cString());
		////}

		//if (_crc_to_find.find(crc_str) != _crc_to_find.end())
		//{
		//	/*if (_weapon_animation_sets[{0, 0}].find(clip->m_animationName.cString()) == _weapon_animation_sets[{0, 0}].end())
		//		log << clip->m_animationName.cString() << " from " << _nodes_stack.string() << endl;*/
		//	_viewmodel->setData(_viewmodel->getIndex(&node), QColor(Qt::red), Qt::BackgroundRole);
		//}

		//recurse(node);
	}
	else if (variant->m_class == &hkbManualSelectorGeneratorClass)
	{
		auto builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
		hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)variant->m_object;
		int binded = isSetBinded(msg, builder, "selectedGeneratorIndex");
		if (binded)
		{
			for (int generator = 0; generator < msg->m_generators.getSize(); ++generator)
			{
				if (binded == 1)
					_right_current_animation_set.push_front(set_name(generator));
				else
					_left_current_animation_set.push_front(set_name(generator));
				auto* node = _manager.findNode(file_index, (void*)msg->m_generators[generator]);
				if (NULL == node)
					__debugbreak();
				node->accept(*this);
				if (binded == 1)
					_right_current_animation_set.pop_front();
				else
					_left_current_animation_set.pop_front();
			}
		}
		else {
			recurse(node);
		}
	}
	else if (variant->m_class == &hkbStateMachineClass) {
		auto builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
		hkbStateMachine* fsm = (hkbStateMachine*)variant->m_object;
		int binded = isSetBinded(fsm, builder, "currentStateId");
		if (!binded)
		{
			binded = isSetBinded(fsm, builder, "startStateId");
		}
		_fsm_stack.push_front(&node);
		if (binded)
		{
			std::map<int, std::pair<ProjectNode*, hkbStateMachineStateInfo*>> fsm_states;
			for (int n = 0; n < node.childCount(); n++)
			{
				auto* state_node = node.child(n);
				hkVariant* child_variant = (hkVariant*)state_node->data(1).value<unsigned long long>();
				
				if (child_variant->m_class == &hkbStateMachineStateInfoClass)
				{
					hkbStateMachineStateInfo* state = (hkbStateMachineStateInfo*)child_variant->m_object;
					fsm_states[state->m_stateId] = { state_node, state };
				}
				else {
					node.child(n)->accept(*this);
				}
			}
			for (auto& entry : fsm_states)
			{
				if (binded == 1)
					_right_current_animation_set.push_front(set_name(entry.second.second->m_stateId));
				else
					_left_current_animation_set.push_front(set_name(entry.second.second->m_stateId));

				entry.second.first->accept(*this);

				if (binded == 1)
					_right_current_animation_set.pop_front();
				else
					_left_current_animation_set.pop_front();
			}
		}
		else {
			recurse(node);
		}
		_fsm_stack.pop_front();
	}
	else if (variant->m_class == &hkbBlenderGeneratorChildClass)
	{
		_blending = true;
		recurse(node);
		_blending = false;
	}
	else if (variant->m_class == &hkbStateMachineStateInfoClass) {
		
		//Current fsm
		auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
		hkbStateMachineStateInfo* this_state = (hkbStateMachineStateInfo*)variant->m_object;
		std::set<std::string> attack_name;

		hkVariant* variant = (hkVariant*)_fsm_stack.front()->data(1).value<unsigned long long>();
		hkbStateMachine* fsm = ((hkbStateMachine*)variant->m_object);
		//if (NULL != fsm->m_wildcardTransitions)
		//{
		//	auto& transitions = fsm->m_wildcardTransitions->m_transitions;
		//	for (int t = 0; t < transitions.getSize(); t++)
		//	{
		//		auto& trans = transitions[t];
		//		if (trans.m_toStateId == this_state->m_stateId)
		//		{
		//			std::string event_name = behavior_builder->getEvent(trans.m_eventId).toUtf8().constData();
		//			if (event_name.find("attackStart") == 0 || event_name.find("bashStart") == 0 ||
		//				event_name.find("attackPowerStart") == 0 || event_name.find("bashPowerStart") == 0)
		//			{
		//				attack_name = event_name;
		//			}
		//		}
		//	}
		//}

		//for (int i = 0; i < fsm->m_states.getSize(); i++)
		//{
		//	auto* another_state = fsm->m_states[i];
		//	if (NULL != another_state->m_transitions)
		//	{
		//		auto& transitions = another_state->m_transitions->m_transitions;
		//		for (int t = 0; t < transitions.getSize(); t++)
		//		{
		//			auto& trans = transitions[t];
		//			if (trans.m_toStateId == this_state->m_stateId)
		//			{
		//				std::string event_name = behavior_builder->getEvent(trans.m_eventId).toUtf8().constData();
		//				if (event_name.find("attackStart") == 0 || event_name.find("bashStart") == 0)
		//				{
		//					attack_name = event_name;
		//				}
		//			}
		//		}
		//	}
		//}

		if (_fsm_stack.size() > 2)
		{
			//parent fsm
			hkVariant* variant = (hkVariant*)_fsm_stack.at(2)->data(1).value<unsigned long long>();
			hkbStateMachine* fsm = ((hkbStateMachine*)variant->m_object);
			hkVariant* state_variant = (hkVariant*)_fsm_stack.at(1)->data(1).value<unsigned long long>();
			hkbStateMachineStateInfo* parent_state = ((hkbStateMachineStateInfo*)state_variant->m_object);

			if (NULL != fsm->m_wildcardTransitions)
			{
				auto& transitions = fsm->m_wildcardTransitions->m_transitions;
				for (int t = 0; t < transitions.getSize(); t++)
				{
					auto& trans = transitions[t];
					if (trans.m_toNestedStateId == this_state->m_stateId
						&& trans.m_toStateId == parent_state->m_stateId)
					{
						std::string event_name = behavior_builder->getEvent(trans.m_eventId).toUtf8().constData();
						if (event_name.find("attackStart") == 0 || event_name.find("bashStart") == 0 ||
							event_name.find("attackPowerStart") == 0 || event_name.find("bashPowerStart") == 0)
						{
							attack_name.insert(event_name);
						}
					}
				}
			}

			//falmer transition from 1HM_Block_State to Power Bash is wrong, check nestedstateid 
			for (int i = 0; i < fsm->m_states.getSize(); i++)
			{
				auto* another_state = fsm->m_states[i];
				if (NULL != another_state->m_transitions)
				{
					auto& transitions = another_state->m_transitions->m_transitions;
					for (int t = 0; t < transitions.getSize(); t++)
					{
						auto& trans = transitions[t];
						if (trans.m_toNestedStateId == this_state->m_stateId
							&& trans.m_toStateId == parent_state->m_stateId)
						{
							std::string event_name = behavior_builder->getEvent(trans.m_eventId).toUtf8().constData();
							if (event_name.find("attackStart") == 0 || event_name.find("bashStart") == 0 ||
								event_name.find("attackPowerStart") == 0 || event_name.find("bashPowerStart") == 0)
							{
								attack_name.insert(event_name);
							}
						}
					}
				}
			}
		}


		if (!attack_name.empty())
		{
			for (const auto& event : attack_name)
			{
				_current_attack_set.push_back(event);
				_fsm_stack.push_front(&node);
				//_fsm_name_stack.push_front(node.data(0).value<QString>().toUtf8().constData());
				//_events.push_front(events);
				recurse(node);
				//_events.pop_front();
				//_fsm_name_stack.pop_front();
				_fsm_stack.pop_front();
				_current_attack_set.pop_front();
			}

		}
		else {
			_fsm_stack.push_front(&node);
			//_fsm_name_stack.push_front(node.data(0).value<QString>().toUtf8().constData());
			//_events.push_front(events);
			recurse(node);
			//_events.pop_front();
			//_fsm_name_stack.pop_front();
			_fsm_stack.pop_front();
		}
	}
	//else if (variant->m_class == &hkbStateMachineClass) {
	//	//if (_fsm_root == nullptr)
	//	//	_fsm_root = &node;
	//	//handle_transition(node);
	//	std::string state_bind = "";
	//	bool animation_driven = false;
	//	hkbStateMachine* fsm = (hkbStateMachine*)variant->m_object;
	//	if (NULL != fsm)
	//	{
	//		if (NULL != fsm->m_variableBindingSet)
	//		{
	//			auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
	//			auto& bindings = fsm->m_variableBindingSet->m_bindings;
	//			for (int i = 0; i < bindings.getSize(); ++i)
	//			{
	//				auto& binding = bindings[i];
	//				auto variable_name = behavior_builder->getVariable(binding.m_variableIndex);
	//				if (binding.m_memberPath == "startStateId")
	//				{
	//					state_bind = variable_name.toUtf8().constData();
	//				}
	//				if (variable_name == "bAnimationDriven")
	//				{
	//					animation_driven = true;
	//				}
	//			}
	//		}
	//	}

	//	if (animation_driven)
	//	{
	//		//handle_animation_binded_fsm(node, file_index);
	//	}


	//	//events
	//	bool control_undefined = _variables_values.find(state_bind) == _variables_values.end();
	//	bool control = _animation_styles_control_variables.find(state_bind) != _animation_styles_control_variables.end();
	//	//if (!state_bind.empty() && control && control_undefined)
	//	//{
	//	//	auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
	//	//	//how I got here?
	//	//	events = find_animation_driven_transitions(node, behavior_builder);
	//	//}

	//	//_fsm_name_stack.push_front(node.data(0).value<QString>().toUtf8().constData());

	//	//Find reacheable states
	//	//std::map<int, ProjectNode*> fsm_states;
	//	//
	//	//std::set<ProjectNode*> reacheable_states;
	//	//for (int i = 0; i < node.childCount(); ++i)
	//	//{
	//	//	hkVariant* variant = (hkVariant*)node.child(i)->data(1).value<unsigned long long>();
	//	//	if (variant->m_class == &hkbStateMachineStateInfoClass)
	//	//	{
	//	//		fsm_states[((hkbStateMachineStateInfo*)variant->m_object)->m_stateId] = node.child(i);
	//	//	}
	//	//}
	//	////current wildcard
	//	//if (NULL != fsm->m_wildcardTransitions)
	//	//{
	//	//	auto& transitions = fsm->m_wildcardTransitions->m_transitions;
	//	//	for (int t = 0; t < transitions.getSize(); t++)
	//	//	{
	//	//		if (fsm_states.find(transitions[t].m_toStateId) != fsm_states.end())
	//	//		{
	//	//			auto* state = fsm_states.at(transitions[t].m_toStateId);
	//	//			reach_states(fsm_states, reacheable_states, state);
	//	//		}
	//	//	}
	//	//}
	//	//if (fsm_states.find(fsm->m_startStateId) != fsm_states.end())
	//	//{
	//	//	reach_states(fsm_states, reacheable_states, fsm_states.at(fsm->m_startStateId));
	//	//}
	//	////parent wildcard
	//	//if (_fsm_stack.size() >= 2)
	//	//{
	//	//	hkVariant* variant = (hkVariant*)_fsm_stack.at(1)->data(1).value<unsigned long long>();
	//	//	hkbStateMachine* parent_fsm = ((hkbStateMachine*)variant->m_object);
	//	//	if (NULL != parent_fsm->m_wildcardTransitions)
	//	//	{
	//	//		auto& transitions = parent_fsm->m_wildcardTransitions->m_transitions;
	//	//		for (int t = 0; t < transitions.getSize(); t++)
	//	//		{
	//	//			if (fsm_states.find(transitions[t].m_toNestedStateId) != fsm_states.end())
	//	//			{
	//	//				auto* state = fsm_states.at(transitions[t].m_toNestedStateId);
	//	//				reach_states(fsm_states, reacheable_states, state);
	//	//			}
	//	//		}
	//	//	}
	//	//	//parent reacheable
	//	//	for (const auto* parent_state : _fsm_reacheable_states_stack.front())
	//	//	{
	//	//		hkVariant* variant = (hkVariant*)parent_state->data(1).value<unsigned long long>();
	//	//		hkbStateMachineStateInfo* hk_state = (hkbStateMachineStateInfo*)variant->m_object;
	//	//		if (NULL != hk_state->m_transitions)
	//	//		{
	//	//			auto& transitions = hk_state->m_transitions->m_transitions;
	//	//			for (int t = 0; t < transitions.getSize(); t++)
	//	//			{
	//	//				if (fsm_states.find(transitions[t].m_toNestedStateId) != fsm_states.end())
	//	//				{
	//	//					reach_states(fsm_states, reacheable_states, fsm_states.at(transitions[t].m_toNestedStateId));
	//	//				}
	//	//			}
	//	//		}
	//	//	}
	//	//}

	//	//update stack
	//	_fsm_stack.push_front(&node);
	//	//_fsm_reacheable_states_stack.push_front(reacheable_states);

	//	//if (!state_bind.empty())
	//	//{
	//	//	if (control_undefined)
	//	//	{
	//	//		for (int n=0; n<node.childCount(); n++)
	//	//		{
	//	//			auto* state = node.child(n);
	//	//			hkVariant* variant = (hkVariant*)state->data(1).value<unsigned long long>();
	//	//			if (variant->m_class == &hkbStateMachineStateInfoClass)
	//	//			{

	//	//				//variables
	//	//				_variables_values[state_bind] = ((hkbStateMachineStateInfo*)variant->m_object)->m_stateId;
	//	//			
	//	//				//if (control)
	//	//				//{
	//	//				//	_animation_sets_stack.push_front({});
	//	//				//	//for (auto& event : events)
	//	//				//	//	variables_path /= event;
	//	//				//	variables_path = variables_path / state_bind / std::to_string(_variables_values[state_bind]);

	//	//				//}
	//	//				state->accept(*this);
	//	//				//if (control)
	//	//				//{
	//	//				//	for (const auto& item : _animation_sets_stack.front())
	//	//				//	{
	//	//				//		_animation_sets[variables_path.string()].insert(item);
	//	//				//	}
	//	//				//	//for (auto& event : events)
	//	//				//	//	variables_path = variables_path.parent_path();
	//	//				//	variables_path = variables_path.parent_path().parent_path();
	//	//				//	_animation_sets_stack.pop_front();
	//	//				//}

	//	//				_variables_values.erase(state_bind);
	//	//			}
	//	//			else {
	//	//				state->accept(*this);
	//	//			}
	//	//		}
	//	//	}
	//	//	else {
	//	//		int current_starting_state = _variables_values[state_bind];
	//	//		for (int n = 0; n < node.childCount(); n++)
	//	//		{
	//	//			auto* state = node.child(n);
	//	//			//hkVariant* variant = (hkVariant*)state->data(1).value<unsigned long long>();
	//	//			//if (variant->m_class == &hkbStateMachineStateInfoClass)
	//	//			//{				
	//	//			//	if (current_starting_state == ((hkbStateMachineStateInfo*)variant->m_object)->m_stateId)
	//	//			//	{
	//	//			//		state->accept(*this);
	//	//			//	}
	//	//			//}
	//	//			//else {
	//	//				state->accept(*this);
	//	//			//}
	//	//		}
	//	//	}
	//	//}
	//	//else {
	//		recurse(node);
	//	//}
	//	//_fsm_stack.pop_front();
	//	//_fsm_reacheable_states_stack.pop_front();
	//}
	//else if (variant->m_class == &hkbManualSelectorGeneratorClass) 
	//{
	//	hkbManualSelectorGenerator* msg = (hkbManualSelectorGenerator*)variant->m_object;
	//	std::string selector_bind = "";
	//	if (NULL != msg->m_variableBindingSet)
	//	{
	//		auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
	//		auto& bindings = msg->m_variableBindingSet->m_bindings;
	//		for (int i = 0; i < bindings.getSize(); ++i)
	//		{
	//			auto& binding = bindings[i];
	//			auto variable_name = behavior_builder->getVariable(binding.m_variableIndex);
	//			if (binding.m_memberPath == "selectedGeneratorIndex")
	//			{
	//				selector_bind = variable_name.toUtf8().constData();
	//			}
	//		}
	//	}
	//	bool banned = _avoid_variables_set.find(msg->m_name.cString()) != _avoid_variables_set.end();
	//	if (!selector_bind.empty() && !banned)
	//	{
	//		bool control_undefined = _variables_values.find(selector_bind) == _variables_values.end();
	//		bool control = _animation_styles_control_variables.find(selector_bind) != _animation_styles_control_variables.end();

	//		if (control_undefined )
	//		{
	//			size_t generator_index = 0;
	//			for (int i = 0; i < node.childCount(); ++i)
	//			{
	//				hkVariant* variant = (hkVariant*)node.child(i)->data(1).value<unsigned long long>();
	//				if (hkbGeneratorClass.isSuperClass(*variant->m_class))
	//				{
	//					_variables_values[selector_bind] = generator_index++;					
	//					node.child(i)->accept(*this);
	//					_variables_values.erase(selector_bind);
	//				}
	//				else {
	//					node.child(i)->accept(*this);
	//				}
	//			}
	//		}
	//		else {
	//			int active_generator = _variables_values[selector_bind];
	//			size_t generator_index = 0;
	//			for (int i = 0; i < node.childCount(); ++i)
	//			{
	//				hkVariant* variant = (hkVariant*)node.child(i)->data(1).value<unsigned long long>();
	//				if (hkbGeneratorClass.isSuperClass(*variant->m_class))
	//				{
	//					if (generator_index == active_generator)
	//					{
	//						node.child(i)->accept(*this);
	//					}
	//					generator_index++;
	//				}
	//				else {
	//					node.child(i)->accept(*this);
	//				}
	//			}
	//		}
	//	}
	//	else {
	//		recurse(node);
	//	}
	//}
	else if (variant->m_class == &hkbModifierGeneratorClass) 
	{
		hkbModifierGenerator* mg = (hkbModifierGenerator*)variant->m_object;
		bool old_animation_driven = _animation_driven;
		if (NULL != mg->m_modifier)
		{
			if (NULL != dynamic_cast<BSIsActiveModifier*>(mg->m_modifier.val()))
			{
				auto BSI = dynamic_cast<BSIsActiveModifier*>(mg->m_modifier.val());
				auto bindingset = mg->m_modifier->m_variableBindingSet;
				if(NULL != bindingset)
				{
					auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
					auto& bindings = bindingset->m_bindings;
					for (int i = 0; i < bindings.getSize(); ++i)
					{
						auto& binding = bindings[i];
						auto variable_name = behavior_builder->getVariable(binding.m_variableIndex);
						if (variable_name == "bAnimationDriven")
						{
							switch (i)
							{
							case 0: _animation_driven = BSI->m_bIsActive0; break;
							case 1: _animation_driven = BSI->m_bIsActive1; break;
							case 2: _animation_driven = BSI->m_bIsActive2; break;
							case 3: _animation_driven = BSI->m_bIsActive3; break;
							case 4: _animation_driven = BSI->m_bIsActive4; break;
							default: _animation_driven = true; break;
							}
						}
					}
				}
			}
		}

			recurse(node);
			_animation_driven = old_animation_driven;
	}
	//else if (variant->m_class == &hkbBehaviorReferenceGeneratorClass) {

	//	/*fs::path project_folder = _manager.path(_project_file_index).parent_path();
	//	hkbBehaviorReferenceGenerator* reference = (hkbBehaviorReferenceGenerator*)variant->m_object;
	//	fs::path behavior_path = project_folder / reference->m_behaviorName.cString();
	//	auto behavior_index = _manager.index(behavior_path);
	//	auto& behavior_contents = _manager.get(behavior_path);
	//	bool found = false;
	//	for (const auto& content : behavior_contents.second)
	//	{
	//		if (content.m_class == &hkbBehaviorGraphClass) {
	//			ProjectNode* behavior_root = _manager.findNode(behavior_index, &content);
	//			BehaviorBuilder* builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(behavior_index));
	//			behavior_root->accept(*this);
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (!found)
	//		throw 666;*/
	//}

	//else if (variant->m_class == &hkbStateMachineClass) 
	//{
	//	hkbStateMachine* fsm = (hkbStateMachine*)variant->m_object;
	//	if (NULL != fsm)
	//	{
	//		if (NULL != fsm->m_variableBindingSet)
	//		{
	//			auto behavior_builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(file_index));
	//			auto& bindings = fsm->m_variableBindingSet->m_bindings;
	//			for (int i = 0; i < bindings.getSize(); ++i)
	//			{
	//				auto& binding = bindings[i];
	//				auto variable_name = behavior_builder->getVariable(binding.m_variableIndex);
	//				if (variable_name == "bAnimationDriven")
	//				{
	//					_animation_driven = true;
	//				}
	//			}
	//		}
	//	}
	//	recurse(node);
	//	_animation_driven = false;
	//}
	else {
		recurse(node);
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
