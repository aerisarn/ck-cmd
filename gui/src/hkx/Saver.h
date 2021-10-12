#pragma once
#include <core/AnimationCache.h>
#include <src/hkx/BehaviorBuilder.h>

#include <deque>

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

			virtual void recurse(ProjectNode& node)
			{
				for (int i = 0; i < node.childCount(); i++)
					node.child(i)->accept(*this);
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

			void handle_root_behavior(ProjectNode& node);

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
				fs::path behavior_path = node.data(1).value<QString>().toUtf8().constData();
				size_t behavior_index = _manager.index(behavior_path);
				_behavior_files_indices.push_back(behavior_index);
				_behavior_nodes.push_back(&node);
				recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_node>(ProjectNode& node)
			{
				handle_hkx_node(node);
				recurse(node);
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

			Saver(ResourceManager& manager, ProjectNode* project_root);

			~Saver()
			{
			}
		};

		template<typename Delegate>
		class RecursiveBehaviorVisitor 
		{
			Delegate& _delegate;
			ResourceManager& _manager;
			std::deque<BehaviorBuilder*> _builders;
			size_t _project_file_index;
		
		public:

			RecursiveBehaviorVisitor(Delegate& node_delegate, ResourceManager& manager, size_t project_file_index) :
				_delegate(node_delegate),
				_manager(manager),
				_project_file_index(project_file_index)
			{
			}

			void handle_node(ProjectNode& node)
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
							behavior_root->accept(*this);
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
			}

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node)
			{
				handle_node(node);
			}

			template<>
			void visit<ProjectNode::NodeType::behavior_node>(ProjectNode& node)
			{
				fs::path behavior_path = node.data(1).value<QString>().toUtf8().constData();
				size_t behavior_index = _manager.index(behavior_path);
				BehaviorBuilder* builder = (BehaviorBuilder*)_manager.classHandler(behavior_index);
				_builders.push_front(builder);
				handle_node(node);
			}
		};

		struct NullBuilder {

			bool handle(ProjectNode& node, BehaviorBuilder* builder) { return true; }

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

		class ClipCollector {

			const vector<Saver::StyleInfo>& _style_info;
			BehaviorBuilder* _builder;

			virtual void recurse(ProjectNode& node)
			{
				for (int i = 0; i < node.childCount(); i++)
					node.child(i)->accept(*this);
			}

			void handle_node(ProjectNode& node);

		public:
			std::set<std::string> _result;

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node)
			{
				recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_node>(ProjectNode& node)
			{
				handle_node(node);
			}

			ClipCollector(ProjectNode* node, const vector<Saver::StyleInfo>& style_info, BehaviorBuilder* builder) :
				_style_info(style_info),
				_builder(builder)
			{
				node->accept(*this);
			}
		};

		class AnimationStyleReverseWalker {


			const vector<Saver::StyleInfo>& _style_info;
			BehaviorBuilder* _builder;

			virtual void recurse(ProjectNode& node)
			{
				for (int i = 0; i < node.parentCount(); i++)
					node.parentItem(i)->accept(*this);
			}

			void handle_node(ProjectNode& node);

		public:

			ProjectNode* _root;

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node)
			{
				_root = &node;
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_node>(ProjectNode& node)
			{
				handle_node(node);
			}

			AnimationStyleReverseWalker(ProjectNode* node, const vector<Saver::StyleInfo>& info, BehaviorBuilder* builder) :
				_style_info(info),
				_builder(builder)
			{
				node->accept(*this);
			}
		};
	}
}