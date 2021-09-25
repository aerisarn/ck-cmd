#pragma once
#include <core/AnimationCache.h>
#include <src/hkx/BehaviorBuilder.h>

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
			std::map<string, ProjectNode*> _behavior_nodes;
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
				fs::path project_path = node.data(1).value<QString>().toUtf8().constData();
				_behavior_files_indices.push_back(_manager.index(project_path));
				_behavior_nodes[node.data(1).value<QString>().toUtf8().constData()] = &node;
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

			Saver(ResourceManager& manager, ProjectNode* project_root) :
				_saving_character(false),
				_manager(manager)
			{
				if (project_root->isCharacter())
				{
					_saving_character = true;
				}
				project_root->accept(*this);
				save_cache();
			}

			~Saver()
			{
			}
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