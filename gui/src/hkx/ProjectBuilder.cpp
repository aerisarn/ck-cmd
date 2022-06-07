#include "ProjectBuilder.h"
#include <src/hkx/TreeBuilder.h>
#include <src/hkx/SkeletonBuilder.h>

#include <src/esp/IDLEBuilder.h>

#include <hkbProjectStringData_1.h>
#include <hkbCharacterStringData_5.h>
#include <hkbBehaviorGraph_1.h>

using namespace ckcmd::HKX;

std::pair<hkbProjectStringData*, size_t> ProjectBuilder::buildProjectFileModel()
{
	auto project_path = fs::path(_name);
	hkVariant* project_root;
	hkbProjectStringData* project_data = loadHkxFile<hkbProjectStringData>(project_path, hkbProjectStringDataClass, project_root);
	auto project_file_index = _resourceManager.index(project_path);
	_parent->appendChild(
		_resourceManager.createHkxNode(
			project_file_index,
			{
				QString("Properties"),
				(unsigned long long)project_root,
				0,
				project_file_index
			},
			_parent
		)
	);
	return { project_data, project_file_index };
}

std::tuple<hkbCharacterStringData*, size_t, ProjectNode*> ProjectBuilder::buildCharacter(const fs::path& character_path, ProjectNode* characters_node)
{
	hkVariant* character_root;
	hkbCharacterStringData* character_data = loadHkxFile<hkbCharacterStringData>(character_path, hkbCharacterStringDataClass, character_root);
	if (character_data == NULL)
		throw std::runtime_error("hkbCharacterStringData variant not found in " + character_path.string());
	auto character_file_index = _resourceManager.index(character_path);
	ProjectNode* character_node = characters_node->appendChild(
		_resourceManager.createHkxNode(
			character_file_index,
			{
				character_data->m_name.cString(),
				(unsigned long long)character_root,
				0,
				character_file_index
			},
			characters_node
		)
	);
	return { character_data, character_file_index, character_node };
}

std::tuple<hkaSkeleton*, hkaSkeleton*, size_t, ProjectNode*> ProjectBuilder::buildSkeleton(const fs::path& rig_path, ProjectNode* character_node)
{
	auto& rig_contents = _resourceManager.get(rig_path);
	auto rig_index = _resourceManager.index(rig_path);
	hkVariant* rig_root;
	hkaSkeleton* skeleton_data = loadHkxFile<hkaSkeleton>(rig_path, hkaSkeletonClass, rig_root);
	hkaSkeleton* ragdoll_data = nullptr;
	for (auto& entry : rig_contents.second)
	{
		if (entry.m_class == &hkaSkeletonClass && entry.m_object != skeleton_data)
		{
			ragdoll_data = reinterpret_cast<hkaSkeleton*>(entry.m_object);
			break;
		}
	}
	ProjectNode* rig_node = character_node->appendChild(
		_resourceManager.createHkxNode(
			rig_index,
			{
				skeleton_data->m_name.cString(),
				(unsigned long long)rig_root,
				0,
				rig_index
			},
			character_node
		)
	);
	return { skeleton_data, ragdoll_data, rig_index, rig_node };
}

typedef std::tuple<hkbBehaviorGraphData*, hkbBehaviorGraphStringData*, size_t, ProjectNode*> BehaviorDescriptor;

std::vector<BehaviorDescriptor> ProjectBuilder::buildBehaviors(const fs::path& behaviors_path, ProjectNode* behaviors_node)
{
	std::vector<BehaviorDescriptor> out;
	fs::directory_iterator end_itr;
	for (fs::directory_iterator itr(behaviors_path);
		itr != end_itr;
		++itr)
	{
		if (!is_directory(itr->status()))
		{
			auto& behavior_path = itr->path();
			auto& behavior_contents = _resourceManager.get(behavior_path);
			auto behavior_index = _resourceManager.index(behavior_path);
			hkVariant* behavior_root;
			hkbBehaviorGraph* behavior_graph = loadHkxFile<hkbBehaviorGraph>(behavior_path, hkbBehaviorGraphClass, behavior_root);
			if (nullptr != behavior_graph)
			{
				hkbBehaviorGraphData* behavior_data = behavior_graph->m_data;
				hkbBehaviorGraphStringData* behavior_string_data = behavior_data->m_stringData;
				ProjectNode* behavior_node = behaviors_node->appendChild(
					_resourceManager.createHkxNode(
						behavior_index,
						{
							behavior_graph->m_name.cString(),
							(unsigned long long)behavior_root,
							0,
							behavior_index
						},
						behaviors_node
					)
				);
				out.push_back(
					{ behavior_data, behavior_string_data, behavior_index, behavior_node }
				);
			}
		}
	}
	return out;
}

ProjectBuilder::ProjectBuilder(
	ProjectNode* parent,
	CommandManager& commandManager,
	ResourceManager& resourceManager,
	const std::string& name
) :
	_parent(parent),
	_commandManager(commandManager),
	_resourceManager(resourceManager),
	_name(name)
{

	auto project_folder = fs::path(name).parent_path();
	auto project_data = buildProjectFileModel();
	if (project_data.first != nullptr)
	{
		hkbProjectStringData* data = project_data.first;
		size_t project_file_index = project_data.second;
		ProjectNode* characters_node = 
			_parent->appendChild(
				_resourceManager.createSupport(project_file_index, { "Characters" }, _parent)
			);

		for (int c = 0; c < data->m_characterFilenames.getSize(); c++)
		{
			auto character_path = project_folder / data->m_characterFilenames[c].cString();
			auto character_result = buildCharacter(character_path, characters_node);

			auto character_data = std::get<0>(character_result);
			auto character_file_index = std::get<1>(character_result);
			auto character_node = std::get<2>(character_result);

			if (!std::string(character_data->m_rigName).empty())
			{
				auto rig_path = project_folder / character_data->m_rigName.cString();
				auto rig_result = buildSkeleton(rig_path, character_node);
			}

			auto behavior_path = project_folder / character_data->m_behaviorFilename.cString();
			behavior_path = behavior_path.parent_path();

			ProjectNode* behaviors_node =
				character_node->appendChild(
					_resourceManager.createSupport(project_file_index, { "Behaviors" }, character_node)
				);

			buildBehaviors(behavior_path, behaviors_node);
		}
	}

	//auto project_path = fs::path(_name);
	//auto project_folder = project_path.parent_path();
	//hkVariant project_root;
	//hkbProjectStringData* project_data = loadHkxFile<hkbProjectStringData>(project_path, hkbProjectStringDataClass, project_root);
	//auto project_file_index = _resourceManager.index(project_path);
	//ProjectNode* project_node = _resourceManager.createProject(project_file_index, {"Project", project_path.string().c_str() }, _parent);
	//_parent->appendChild(project_node);

	//buildBranch(project_root, project_node, project_path);
	////auto project_cache = _resourceManager.findCacheEntry(project_file_index);
	//
	//if (project_data)
	//{
	//	ProjectNode* characters_node = _resourceManager.createSupport(project_file_index, { "Characters" }, _parent);
	//	_parent->appendChild(characters_node);
	//	//Load characters
	//	for (int c = 0; c < project_data->m_characterFilenames.getSize(); c++)
	//	{
	//		//character data
	//		auto character_path = project_folder / project_data->m_characterFilenames[c].cString();
	//		hkVariant character_root;
	//		hkbCharacterStringData* character_data = loadHkxFile<hkbCharacterStringData>(character_path, hkbCharacterStringDataClass, character_root);
	//		if (character_data == NULL)
	//			throw std::runtime_error("hkbCharacterStringData variant not found in " + character_path.string());
	//		auto character_file_index = _resourceManager.index(character_path);
	//		ProjectNode* character_node = _resourceManager.createHkxCharacter(character_file_index, { character_data->m_name.cString(), character_path.string().c_str() }, characters_node);
	//		characters_node->appendChild(character_node);
	//		buildBranch(character_root, character_node, character_path);

	//		//skeleton
	//		SkeletonBuilder* skeleton_builder = nullptr;
	//		if (!std::string(character_data->m_rigName).empty())
	//		{
	//			auto rig_path = project_folder / character_data->m_rigName.cString();
	//			auto& rig_contents = _resourceManager.get(rig_path);
	//			auto rig_index = _resourceManager.index(rig_path);
	//			skeleton_builder = new SkeletonBuilder(_resourceManager, rig_index);
	//			ProjectNode* rig_node = _resourceManager.createSkeleton(rig_index, { "Skeleton", rig_path.string().c_str() }, character_node);
	//			character_node->appendChild(rig_node);
	//			_resourceManager.setClassHandler(
	//				rig_index,
	//				static_cast<ITreeBuilderClassHandler*>(skeleton_builder)
	//			);
	//			_resourceManager.setFieldHandler(
	//				rig_index,
	//				static_cast<ISpecialFieldsHandler*>(skeleton_builder)
	//			);
	//			buildBranch(rig_contents.first, rig_node, rig_path);
	//		}

	//		//animations
	//		ProjectNode* animations_node = nullptr;
	//		if (character_data->m_animationNames.getSize() > 0)
	//		{
	//			animations_node = _resourceManager.createSupport(character_file_index, { "Animations" }, character_node);
	//			character_node->appendChild(animations_node);
	//			//check cache;
	//			//auto movements = project_cache->getMovements();
	//			//if (project_cache->hasCache() && movements.size() != character_data->m_animationNames.getSize())
	//			//{
	//			//	LOG << "Cached movements number is different from animation number!" << log_endl;
	//			//}

	//			//ProjectNode* idle = nullptr;
	//			//if (project_cache->hasCache()) {
	//			//	CreatureCacheEntry* creature_cache = dynamic_cast<CreatureCacheEntry*>(project_cache);
	//			//	if (creature_cache->getProjectSetFiles().size() > 1) {
	//			//		idle = _resourceManager.createSupport(
	//			//			character_file_index,
	//			//			{
	//			//				"Idles"
	//			//			},
	//			//			animations_node);
	//			//		animations_node->appendChild(idle);
	//			//	}
	//			//}
	//			//map<string, ProjectNode*> sets;
	//			//ProjectNode* base = _resourceManager.createSupport(
	//			//	character_file_index,
	//			//	{
	//			//		"Base"
	//			//	},
	//			//	animations_node);
	//			//animations_node->appendChild(base);

	//			//NO! Bethesda left all kind of unused animations here, which are referred by other projects.
	//			//Instead we just create nodes for the clips actually present into the behaviors
	//			//for (int a = 0; a < character_data->m_animationNames.getSize(); a++)
	//			//{
	//			//	auto animation_path = project_folder / character_data->m_animationNames[a].cString();
	//			//	if (fs::exists(animation_path))
	//			//	{
	//			//		ProjectNode* animation_node = _resourceManager.createAnimation(
	//			//			character_file_index, {
	//			//				character_data->m_animationNames[a].cString(),
	//			//				animation_path.string().c_str()
	//			//			},
	//			//			animations_node);
	//			//		animations_node->appendChild(animation_node);

	//			//		//kinda pointless to open all the animations
	//			//		//buildBranch(animation_contents.first, animation_node, animation_path);
	//			//	}
	//			//}
	//		}

	//		//behavior
	//		auto behavior_path = project_folder / character_data->m_behaviorFilename.cString();
	//		hkVariant behavior_root;
	//		hkbBehaviorGraph* behavior_data = loadHkxFile<hkbBehaviorGraph>(behavior_path, hkbBehaviorGraphClass, behavior_root);
	//		if (behavior_data == NULL)
	//			throw std::runtime_error("hkbBehaviorGraph variant not found in " + behavior_path.string());
	//		auto behavior_index = _resourceManager.index(behavior_path);
	//		ProjectNode* behavior_node = _resourceManager.createBehavior(behavior_index, { behavior_data->m_name.cString(), behavior_path.string().c_str() }, character_node);
	//		
	//		auto behavior_handler = new BehaviorBuilder(_commandManager, _resourceManager, project_file_index, character_file_index, behavior_index, animations_node, true);
	//		behavior_handler->setSkeleton(skeleton_builder);
	//		_resourceManager.setClassHandler(
	//			behavior_index,
	//			static_cast<ITreeBuilderClassHandler*>(behavior_handler)
	//		);
	//		_resourceManager.setFieldHandler(
	//			behavior_index,
	//			static_cast<ISpecialFieldsHandler*>(behavior_handler)
	//		);
	//		buildBranch(behavior_root, behavior_node, behavior_path);
	//		//additional behaviors
	//		std::set<std::string> _referenced_behaviors = behavior_handler->referenced_behaviors();
	//		std::vector<std::string> iterating_behaviors;
	//		iterating_behaviors.insert(iterating_behaviors.end(), _referenced_behaviors.begin(), _referenced_behaviors.end());
	//		std::set<ProjectNode*> sub_behaviors;
	//		for (size_t bb = 0; bb < iterating_behaviors.size(); bb++)
	//		{
	//			auto behavior_path = project_folder / iterating_behaviors[bb];
	//			hkVariant behavior_root;
	//			hkbBehaviorGraph* behavior_data = loadHkxFile<hkbBehaviorGraph>(behavior_path, hkbBehaviorGraphClass, behavior_root);
	//			if (behavior_data == NULL)
	//				throw std::runtime_error("hkbBehaviorGraph variant not found in " + behavior_path.string());
	//			auto behavior_index = _resourceManager.index(behavior_path);
	//			ProjectNode* behavior_node = _resourceManager.createBehavior(behavior_index, { behavior_data->m_name.cString(), behavior_path.string().c_str(), iterating_behaviors[bb].c_str() }, character_node);
	//			//character_node->appendChild(behavior_node);
	//			sub_behaviors.insert(behavior_node);
	//			
	//			auto behavior_handler = new BehaviorBuilder(_commandManager, _resourceManager, project_file_index, character_file_index, behavior_index, animations_node, false);
	//			behavior_handler->setSkeleton(skeleton_builder);
	//			_resourceManager.setClassHandler(
	//				behavior_index,
	//				static_cast<ITreeBuilderClassHandler*>(behavior_handler)
	//			);
	//			_resourceManager.setFieldHandler(
	//				behavior_index,
	//				static_cast<ISpecialFieldsHandler*>(behavior_handler)
	//			);
	//			buildBranch(behavior_root, behavior_node, behavior_path);
	//			for (const auto& additional : behavior_handler->referenced_behaviors())
	//			{
	//				if (_referenced_behaviors.insert(additional).second)
	//				{
	//					iterating_behaviors.push_back(additional);
	//				}
	//			}
	//		}	
	//	

	//		auto actions_node = _resourceManager.createSupport(behavior_index, { "Actions" }, character_node);
	//		character_node->appendChild(actions_node);

	//		auto idle_builder = new IDLEBuilder(behavior_index, actions_node, _resourceManager);

	//		character_node->appendChild(behavior_node);
	//		for (const auto& sub : sub_behaviors)
	//			character_node->appendChild(sub);
	//	}
	//}
}