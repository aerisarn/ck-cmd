#include "ProjectBuilder.h"
#include <src/hkx/SkeletonBuilder.h>

#include <src/esp/IDLEBuilder.h>

#include <hkbProjectStringData_1.h>
#include <hkbCharacterData_7.h>
#include <hkbCharacterStringData_5.h>
#include <hkbBehaviorGraph_1.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>

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

std::tuple<hkbCharacterData*, size_t, ProjectNode*> ProjectBuilder::buildCharacter(const fs::path& character_path, ProjectNode* characters_node, int project_file_index)
{
	hkVariant* character_root;
	hkbCharacterData* character_data = loadHkxFile<hkbCharacterData>(character_path, hkbCharacterDataClass, character_root);
	if (character_data == NULL)
		throw std::runtime_error("hkbCharacterStringData variant not found in " + character_path.string());
	auto character_file_index = _resourceManager.index(character_path);
	ProjectNode* character_node = characters_node->appendChild(
		_resourceManager.createHkxNode(
			character_file_index,
			{
				character_data->m_stringData->m_name.cString(),
				(unsigned long long)character_root,
				project_file_index,
				character_file_index
			},
			characters_node
		)
	);
	return { character_data, character_file_index, character_node };
}

void ProjectBuilder::buildSkeleton(const fs::path& rig_path, ProjectNode* character_node, bool ragdoll, int project_file_index)
{
	auto& rig_contents = _resourceManager.get(rig_path);
	auto rig_index = _resourceManager.index(rig_path);
	bool fileHasRagdoll = false;
	hkRefPtr<const hkaSkeleton> ragdoll_skeleton = nullptr;
	for (const auto& content : rig_contents.second)
	{
		if (content.m_class == &hkaRagdollInstanceClass)
		{
			fileHasRagdoll = true;
			hkaRagdollInstance* ragdoll = (hkaRagdollInstance*)content.m_object;
			ragdoll_skeleton = ragdoll->m_skeleton;
			break;
		}
	}

	hkVariant* skeleton_data = nullptr;
	hkVariant* ragdoll_data = nullptr;
	for (auto& entry : rig_contents.second)
	{
		if (entry.m_class == &hkaSkeletonClass)
		{
			if (!fileHasRagdoll || ragdoll_skeleton.val() != entry.m_object)
			{
				skeleton_data = &entry;
			}
		}
		if (entry.m_class == &hkaRagdollInstanceClass)
		{
			ragdoll_data = &entry;
		}
	}
	if (nullptr != skeleton_data)
	{
		ProjectNode* rig_node = character_node->appendChild(
			_resourceManager.createHkxNode(
				rig_index,
				{
					"Skeleton",
					(unsigned long long)skeleton_data,
					project_file_index,
					rig_index
				},
				character_node
			)
		);
	}
	if (ragdoll_data != skeleton_data)
	{
		ProjectNode* rig_node = character_node->appendChild(
			_resourceManager.createHkxNode(
				rig_index,
				{
					"Ragdoll",
					(unsigned long long)ragdoll_data,
					project_file_index,
					rig_index
				},
				character_node
			)
		);
	}
}

typedef std::tuple<hkbBehaviorGraphData*, hkbBehaviorGraphStringData*, size_t, ProjectNode*> BehaviorDescriptor;

std::vector<BehaviorDescriptor> ProjectBuilder::buildBehaviors(const fs::path& behaviors_path, ProjectNode* behaviors_node, int project_file_index)
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
							project_file_index,
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
	LOGINFO << "Loading: " << name << log_endl;
	auto project_data = buildProjectFileModel();
	if (project_data.first != nullptr)
	{
		hkbProjectStringData* data = project_data.first;
		size_t project_file_index = project_data.second;
		ProjectNode* characters_node = 
			_parent->appendChild(
				_resourceManager.createSupport(project_file_index, { "Characters", "", project_file_index, -1 }, _parent)
			);

		for (int c = 0; c < data->m_characterFilenames.getSize(); c++)
		{
			auto character_path = project_folder / data->m_characterFilenames[c].cString();
			LOGINFO << "Loading character: " << character_path << log_endl;
			auto character_result = buildCharacter(character_path, characters_node, project_file_index);

			auto character_data = std::get<0>(character_result);
			auto character_file_index = std::get<1>(character_result);
			auto character_node = std::get<2>(character_result);

			if (!std::string(character_data->m_stringData->m_rigName).empty())
			{
				auto rig_path = project_folder / character_data->m_stringData->m_rigName.cString();
				LOGINFO << "Loading rig: " << rig_path << log_endl;
				buildSkeleton(rig_path, character_node, true, project_file_index);
			}

			auto behavior_path = project_folder / character_data->m_stringData->m_behaviorFilename.cString();
			behavior_path = behavior_path.parent_path();

			ProjectNode* behaviors_node =
				character_node->appendChild(
					_resourceManager.createSupport(project_file_index, { "Behaviors", "", project_file_index, -1 }, character_node)
				);

			buildBehaviors(behavior_path, behaviors_node, project_file_index);
		}
	}
}