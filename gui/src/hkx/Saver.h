#pragma once
#include <core/AnimationCache.h>
#include <src/hkx/BehaviorBuilder.h>
#include <src/models/ProjectTreeModel.h>

#include <deque>
#include <stack>

class hkbStateMachine;
class hkbStateMachineStateInfo;

namespace ckcmd {
	namespace HKX {


		class NestedStateFinder {

			virtual void recurse(ProjectNode& node)
			{
				for (int i = 0; i < node.childCount(); i++)
					node.child(i)->accept(*this);
			}

		public:
			ProjectNode* _result = NULL;
			ProjectNode* _starting_node;
			int _target_id = NULL;

			void handle_node(ProjectNode& node);

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node)
			{
				if (_result == NULL)
					recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_node>(ProjectNode& node)
			{
				if (&node != _starting_node)
				{
					handle_node(node);
				}
				if (_result == NULL)
					recurse(node);
			}

			NestedStateFinder(ProjectNode* node, int target_id) :
				_target_id(target_id)
			{
				_starting_node = node;
				node->accept(*this);
			}


		};

		

		class Saver {

			ProjectTreeModel* _viewmodel;
			std::set<ProjectNode*> _visited;

			virtual void recurse(ProjectNode& node)
			{
				for (int i = 0; i < node.childCount(); i++)
				{
					//if (_visited.insert(node.child(i)).second)
					node.child(i)->accept(*this);
				}
			}

			ResourceManager& _manager;
		
			bool _saving_character;

			int _project_file_index;
			std::vector<int> _character_files_indices;
			std::vector<int> _behavior_files_indices;
			std::vector<ProjectNode*> _behavior_nodes;
			std::set<std::string> _animation_relative_files;
			std::map<std::string, fs::path> _animation_absolute_files;
			std::string _rig_file;
			CacheEntry* _cache;

			struct AnimationInfo {
				float duration;
				std::string _root_movements;
				std::multimap<float, std::string> _events;
			};

			struct ClipInfo {
				std::string animation_name = "";
				float playbackSpeed = 0.;
				float cropStartTime = 0.;
				float cropEndTime = 0.;
				std::multimap<float, std::string> _events; // = new StringListBlock();
			};

			//Animation Cache temp info;
			std::map<std::string, AnimationInfo> _cache_animation_info;
			std::map<std::string, ClipInfo> _cache_clip_info;
			std::vector<ProjectNode*> _clips;

		public:
			struct StyleInfo {
				std::string variable;
				int min;
				int max;
			};

		private:
			std::vector<std::vector<StyleInfo>> _cache_styles;
			std::map<int, std::set<std::string>> _cache_style_clips;
			std::map<int, std::set<std::pair<std::string, bool>>> _cache_style_attacks;
			std::set<std::string> _cache_attacks;
			std::map<std::string, std::set<std::pair<int,std::string>>> _cache_attack_clips;



			std::string to_bstring(float f);

			void save_hkx(int file_index);
			void save_cache();

			void handle_hkx_node(ProjectNode& node);
			void handle_animation(ProjectNode& node);
			void handle_clip(hkbClipGenerator* clip, int file_index);
			void handle_animation_style(ProjectNode& node);
			void handle_transition(ProjectNode& node);
			void handle_behavior(ProjectNode& node);
			void handle_action(ProjectNode& node);
			void handle_idle(ProjectNode& node);

			void handle_animation_binded_fsm(ProjectNode& fsm, int file_index);

			std::set<std::string> find_animation_driven_transitions(ProjectNode& node, BehaviorBuilder*);

			std::multimap<size_t, ProjectNode*> _behaviors_references_int_map;
			std::multimap<ProjectNode*, ProjectNode*> _behaviors_references_nodes_map;
			std::map<ProjectNode*, BehaviorBuilder*> _behaviors_references_builders_map;

			ProjectNode* _animation_sets_root_fsm = NULL;
			std::set<ProjectNode*> _animation_sets_root_fsm_tagged_states;
			int _animation_sets_root_fsm_depth;

			std::deque<ProjectNode*> _fsm_stack;
			std::deque < std::set<ProjectNode*>> _fsm_reacheable_states_stack;

			//std::deque<std::string> _fsm_name_stack;
			std::map<std::string, int> _variables_values;
			std::deque<std::set<std::string>> _events;
			std::deque<std::set<std::string>> _animation_sets_stack;
			fs::path variables_path;
			std::map<std::string, std::set<std::string>>  _animation_sets;

			std::set<std::string> _animation_styles_control_variables_msg = { "iRightHandType"/*, "iLeftHandType"*/ };
			std::set<std::string> _animation_styles_control_variables_fsm = { "iEquippedItemState" };
			std::set<std::string> _avoid_variables_set = { "RootBoneSwitch_MSG", "DualWieldShieldBoneRootMSG" };
			fs::path _nodes_stack;
			bool _first_behavior = true;
			std::deque<ProjectNode*> _behavior_stack;

			std::map<std::pair<int, int>, std::map<std::string, std::string>> _weapon_animation_sets;
			std::map<std::pair<int, int>, std::set<std::string>> _weapon_events_sets;

			std::deque<std::pair<std::string,std::set<std::string>>> _attack_animations;

			std::map<std::string, std::string> _crc_to_find;


			std::map<std::pair<std::string,std::string>, std::set<std::string>> _new_animation_sets;
			std::deque<std::string> _right_current_animation_set;
			std::deque<std::string> _left_current_animation_set;
			std::set<std::string> _right_animation_set_binding_variable = { "iRightHandType"};
			std::set<std::string> _left_animation_set_binding_variable = { "iLeftHandType", "iEquippedItemState"};
			std::deque<std::string> _current_attack_set;
			std::deque<std::set<std::string>> _current_attack_set_animations;
			std::map<std::tuple<std::string, std::string,std::string>, std::set<std::string>> _new_attack_sets;

			bool _animation_driven = false;
			bool _blending = false;

			bool _equip_action = false;
			std::map<std::pair<std::string, std::string>, std::set<std::string>> _equip_event_sets;

			std::set<std::string> _cache_set_events;

			int isSetBinded(hkbBindable* bindable, BehaviorBuilder* builder, const std::string& path);

			std::set<std::string> already_red;


			bool _main_equip_state = false;

		public: 

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node)
			{
				recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::project_node>(ProjectNode& node)
			{
				_project_file_index = _manager.index(node.data(1).value<QString>().toUtf8().constData());
				_cache = _manager.findOrCreateCacheEntry(_project_file_index, _saving_character);
				//save_hkx(_project_file_index);
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_character_node>(ProjectNode& node)
			{
				fs::path project_path = node.data(1).value<QString>().toUtf8().constData();
				_character_files_indices.push_back(_manager.index(project_path));
				//save_hkx(project_path);
				recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::behavior_node>(ProjectNode& node)
			{
				_behavior_stack.push_back(&node);
				recurse(node);
				handle_behavior(node);
				_behavior_stack.pop_front();
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_node>(ProjectNode& node)
			{
				//_nodes_stack = _nodes_stack / node.data(0).value<QString>().toUtf8().constData();
				handle_hkx_node(node);
				//_nodes_stack = _nodes_stack.parent_path();
			}

			template<>
			void visit<ProjectNode::NodeType::animation_node>(ProjectNode& node)
			{
				handle_animation(node);
				recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::skeleton_node>(ProjectNode& node)
			{
				_rig_file = node.data(1).value<QString>().toUtf8().constData();
			}

			template<>
			void visit<ProjectNode::NodeType::animation_style_node>(ProjectNode& node)
			{
				handle_animation_style(node);
			}

			//template<>
			//void visit<ProjectNode::NodeType::action_node>(ProjectNode& node)
			//{
			//	handle_action(node);
			//}

			//template<>
			//void visit<ProjectNode::NodeType::idle_node>(ProjectNode& node)
			//{
			//	handle_idle(node);
			//}

			Saver(ResourceManager& manager, ProjectNode* project_root, ProjectTreeModel* viewmodel);

			~Saver()
			{
			}
		};

		struct ClipPath {
			union
			{
				int stateId;
				struct {
					BehaviorBuilder* _builder;
					int referenceIndex;
					int value;
				} referenceIndex;
				struct {
					BehaviorBuilder* _builder;
					size_t valid_references;
					int multipleReferencesIndices[64];
				} referenceArray;
			} data;
			int type;
		};

		template<typename Delegate>
		class RecursiveClipPathBuilder
		{
			ProjectNode& _clip;
			Delegate& _delegate;
			ResourceManager& _manager;
			std::multimap<ProjectNode*, ProjectNode*> _behaviors_references_nodes_map;
			std::map<ProjectNode*, BehaviorBuilder*> _behaviors_references_builders_map;
			std::deque<BehaviorBuilder*> _builders;
			std::vector<ClipPath> _path_stack_container;
			std::stack<ClipPath, std::vector<ClipPath>> _current_path;


		public:
			RecursiveClipPathBuilder(
				ProjectNode& clip,
				Delegate& node_delegate,
				ResourceManager& manager,
				std::multimap<ProjectNode*, ProjectNode*>& behaviors_references_nodes_map,
				std::map<ProjectNode*, BehaviorBuilder*>& behaviors_references_builders_map,
				size_t behavior_index) :
				_clip(clip),
				_delegate(node_delegate),
				_manager(manager),
				_behaviors_references_nodes_map(behaviors_references_nodes_map),
				_behaviors_references_builders_map(behaviors_references_builders_map),
				_path_stack_container(1000),
				_current_path(_path_stack_container)
			{
				BehaviorBuilder* builder = (BehaviorBuilder*)_manager.classHandler(behavior_index);
				_builders.push_front(builder);
				_clip.accept(*this, &_clip);
			}

			void recurse(ProjectNode& node, ProjectNode* child)
			{	
				for (size_t c = 0; c < node.parentCount(); c++)
				{
					node.parentItem(c)->accept(*this, &node);
				}
			}

			void handle_node(ProjectNode& node, ProjectNode* child)
			{
				_current_path.push(_delegate.handle(node, _builders.front(), child));
				recurse(node, child);
				_current_path.pop();
			}

			void handle_behavior_node(ProjectNode& node, ProjectNode* child)
			{
				if (!_builders.empty() || _first_be)
				_builders.push_front(_behaviors_references_builders_map[parent_it->second]);
				parent_it->second->accept(*this, child);
				_builders.pop_front();			
			}

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node, ProjectNode* child)
			{
				handle_node(node, child);
			}

			template<>
			void visit<ProjectNode::NodeType::behavior_node>(ProjectNode& node, ProjectNode* child)
			{
				handle_behavior_node(node, child);
			}
		};

		template<typename Delegate>
		class RecursiveBehaviorVisitor 
		{
			ProjectNode& _root;
			Delegate& _delegate;
			ResourceManager& _manager;
			std::deque<BehaviorBuilder*> _builders;
			size_t _project_file_index;

			std::set<ProjectNode*> _current_branch;

		public:

			RecursiveBehaviorVisitor(
				ProjectNode& root, 
				Delegate& node_delegate, 
				ResourceManager& manager,
				size_t behavior_index,
				size_t project_file_index) :
				_root(root),
				_delegate(node_delegate),
				_manager(manager),
				_project_file_index(project_file_index)
			{
				BehaviorBuilder* builder = (BehaviorBuilder*)_manager.classHandler(behavior_index);
				_builders.push_front(builder);
				_root.accept(*this, {-1});
			}

			void handle_node(ProjectNode& node)
			{
				if (_current_branch.insert(&node).second)
				{
					hkVariant* variant = nullptr;
					if (node.isVariant())
					{
						variant = (hkVariant*)node.data(1).value<unsigned long long>();
						//if (node.data(0).value<QString>() == "MT_H2H_State")
						//	__debugbreak();
					}
					if (nullptr != variant && variant->m_class == &hkbBehaviorReferenceGeneratorClass)
					{
						fs::path project_folder = _manager.path(_project_file_index).parent_path();
						hkbBehaviorReferenceGenerator* reference = (hkbBehaviorReferenceGenerator*)variant->m_object;
						fs::path behavior_path = project_folder / reference->m_behaviorName.cString();
						auto behavior_index = _manager.index(behavior_path);
						auto& behavior_contents = _manager.get(behavior_path);
						bool found = false;
						for (const auto& content : behavior_contents.second)
						{
							if (content.m_class == &hkbBehaviorGraphClass) {
								ProjectNode* behavior_root = _manager.findNode(behavior_index, &content);
								BehaviorBuilder* builder = dynamic_cast<BehaviorBuilder*>(_manager.classHandler(behavior_index));
								_builders.push_front(builder);
								//behavior_root->accept(*this);
								_builders.pop_front();
								found = true;
								break;
							}
						}
						if (!found)
							throw 666;

					}
					else {
						bool result = _delegate.handle(node, _builders.front());
						if (result) {
							for (size_t child_index = 0; child_index < node.childCount(); child_index++)
							{
								node.child(child_index)->accept(*this);
							}
						}
						_delegate.end_branch(node, _builders.front());
					}
					_current_branch.erase(&node);
				}
			}

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node)
			{
				handle_node(node);
			}
		};

		struct NullBuilder {

			size_t handled = 1;

			bool handle(ProjectNode& node, BehaviorBuilder* builder) { 
				handled = ++handled;
				return true;
			}

			void end_branch(ProjectNode& node, BehaviorBuilder* builder) {}

		};

		struct PathBuilder {

			size_t handled = 1;
			std::set<std::string> paths;

			ClipPath handle(ProjectNode& node, BehaviorBuilder* builder, ProjectNode* child);

			void end_branch(const std::stack<ClipPath, std::vector<ClipPath>>& path) {
				//std::string spath = "/";
				//for (const auto& entry : path) {
				//	spath += entry + "/";
				//}
				//paths.insert(spath);
			}

			void end_branch(ProjectNode& node, BehaviorBuilder* builder) {}

		};

		class AnimationSetBuilder {

			/*it is unclear wether this actually has any effect on caching
			in some cases, like draugr, it lists entering effects and all the animations until
			we get back to the initial state machine. Which is unlikely something done automatically*/
			typedef std::set<std::string> events_list_t;

			/*each file is either a weapon set with these valorized (variable name/ value pairs)
			or this is empty and the file is just a list of animations. My guess is that these just get merged in a single map*/
			typedef std::map<std::string, std::vector<int>> variable_list_t;
			
			/*lists all the clips that can be played in response of an attackStart or bashStart event,
			ck uses this to determine attack entries and movements*/
			typedef std::map<std::string, std::set<string>> attack_list_t;

			/*crc32 of the clips*/
			typedef std::set<std::string> clips_list_t;

			typedef std::tuple< events_list_t, variable_list_t, attack_list_t, clips_list_t> animation_set_t;

			/*variables to consider for the set switch*/
			std::vector<std::string> _variables;

			/*current set stack*/
			std::deque<std::pair<ProjectNode*, animation_set_t>> sets_stack;

			typedef std::tuple<ProjectNode*, std::string, int, int> attack_state_id_t;

			/*attack event, state id, nested state id*/
			std::vector<attack_state_id_t> _attack_state;
			std::deque<std::pair<ProjectNode*, std::string>> _attacks_nodes;
			std::deque<std::pair<ProjectNode*, int>> _states_stack;


			/*nodes stack*/
			std::deque<std::string> _branch;

			template<typename HkBindable>
			int isVariableBinded(HkBindable* bindable, BehaviorBuilder* _builder)
			{
				if (bindable->m_variableBindingSet != NULL)
				{
					hkbVariableBindingSet* vbs = bindable->m_variableBindingSet;
					for (int b = 0; b < vbs->m_bindings.getSize(); b++)
					{
						string variable_name = _builder->getVariable(vbs->m_bindings[b].m_variableIndex).toUtf8().constData();
						for (int v = 0; v < _variables.size(); v++) {
							if (_variables[v] == variable_name)
							{
								return v;
							}
						}
					}
				}
				return -1;
			}



		public:

			AnimationSetBuilder() {
				sets_stack.push_front({});
			}

			bool handle(ProjectNode& node, BehaviorBuilder* builder);

			void end_branch(ProjectNode& node, BehaviorBuilder* builder);

		};
	}
}
