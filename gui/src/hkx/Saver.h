#pragma once
#include <src/hkx/BehaviorBuilder.h>

namespace ckcmd {
	namespace HKX {
		class Saver {
			ResourceManager _manager;

			fs::path _project_file;
			std::vector<fs::path> _character_files;
			std::vector<fs::path> _behavior_files;
			std::vector<fs::path> _animation_files;

			void recurse(ProjectNode& node)
			{
				for (int i = 0; i < node.childCount(); i++)
					node.child(i)->accept(*this);
			}

			void save_hkx(const fs::path& file);

		public: 

			template<ProjectNode::NodeType>
			void visit(ProjectNode& node) 
			{
				recurse(node);
			}

			template<>
			void visit<ProjectNode::NodeType::project_node>(ProjectNode& node)
			{
				_project_file = node.data(1).value<QString>().toUtf8().constData();
				save_hkx(_project_file);
			}

			template<>
			void visit<ProjectNode::NodeType::hkx_character_node>(ProjectNode& node)
			{
				fs::path project_path = node.data(1).value<QString>().toUtf8().constData();
				_character_files.push_back(project_path);
				save_hkx(project_path);
			}

			template<>
			void visit<ProjectNode::NodeType::behavior_node>(ProjectNode& node)
			{
				fs::path project_path = node.data(1).value<QString>().toUtf8().constData();
				_behavior_files.push_back(project_path);
				save_hkx(project_path);
			}

			Saver(ResourceManager& manager) :
				_manager(manager)
			{
			}

			~Saver()
			{

			}
		};
	}
}