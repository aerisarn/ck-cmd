#include "ResourceManager.h"

#include <hkbProjectData_2.h>

using namespace ckcmd::HKX;

ResourceManager::ResourceManager(WorkspaceConfig& workspace) :
	_workspace(workspace),
	_cache(_workspace.getFolder())
{
}

size_t ResourceManager::index(const fs::path& file) const {
	auto index = std::distance(
		_files.begin(),
		std::find(_files.begin(), _files.end(), file)
	);
	return index;
}

fs::path ResourceManager::path(int file_index) const
{
	return _files.at(file_index);
}

int ResourceManager::findIndex(int file_index, const void* object) const
{
	auto& _objects = _contents.at(file_index).second;
	for (int i = 0; i < _objects.size(); i++) {
		if (_objects[i].m_object == object)
			return i;
	}
	return -1;
}

int ResourceManager::findIndex(const fs::path& file, const void* object) const
{
	return findIndex(index(file), object);
}

hkVariant* ResourceManager::at(const fs::path& file, size_t _index) {
	return const_cast<hkVariant*>(static_cast<const ResourceManager&>(*this).at(file, _index));
}

const hkVariant* ResourceManager::at(const fs::path& file, size_t _index) const {
	return &_contents.at(index(file)).second[_index];
}

hkVariant* ResourceManager::at(size_t file_index, size_t _index) {
	return const_cast<hkVariant*>(static_cast<const ResourceManager&>(*this).at(file_index, _index));
}

const hkVariant* ResourceManager::at(size_t file_index, size_t _index) const {
	return &_contents.at(file_index).second[_index];
}

hk_object_list_t ResourceManager::findCompatibleNodes(size_t file_index, const hkClassMember* member_class) const
{
	hk_object_list_t result;
	auto& file_contents = _contents.at(file_index).second;
	for (int i = 0; i < file_contents.size(); i++) {
		const auto& object = file_contents[i];
		if (member_class->getClass() == object.m_class ||
			member_class->getClass()->isSuperClass(*object.m_class)) {
			result.push_back({ i, &object });
		}
	}
	return result;
}

hkx_file_t& ResourceManager::get(size_t index)
{
	return _contents[index];
}

hkx_file_t& ResourceManager::get(const fs::path& file)
{
	if (std::find(_files.begin(), _files.end(), file) == _files.end()) {
		HKXWrapper wrap;
		fs::path xml_path = file;  xml_path.replace_extension(".xml");
		fs::path hkx_path = file;  hkx_path.replace_extension(".hkx");
		
		hkRootLevelContainer* root;
		hkx_file_t new_file;
		hkArray<hkVariant> to_read;
		if (fs::exists(xml_path))
		{
			root = wrap.read(xml_path, to_read);
		}
		else if (fs::exists(hkx_path))
			root = wrap.read(hkx_path, to_read);
		else
		{
			LOG << "Unable to find: " + file.string() << log_endl;
			//throw std::runtime_error("Unable to find: " + file.string());
		}
		new_file.second.resize(to_read.getSize());
		for (int i = 0; i < to_read.getSize(); i++) {
			new_file.second[i] = to_read[i];
		}

		for (int i = 0; i < new_file.second.size(); i++) {
			if (new_file.second[i].m_object == root)
			{
				new_file.first = new_file.second[i];
				break;
			}
		}	
		_contents[_files.size()] = new_file;
		_nodes[_files.size()].reserve(new_file.second.size());
		_files.push_back(file);
	}
	return _contents[index(file)];
}

bool ResourceManager::isHavokProject(const fs::path& file)
{
	HKXWrapper wrap;
	try {
		auto root = wrap.read(file);
		if (root)
			return root->findObjectByType(hkbProjectDataClass.getName()) != NULL;
		return false;
	}
	catch (...) {
		return false;
	}

}

void ResourceManager::setClassHandler(size_t index, ITreeBuilderClassHandler* handler)
{
	_class_handlers[index] = handler;
}

void ResourceManager::setFieldHandler(size_t index, ISpecialFieldsHandler* handler)
{
	_field_handlers[index] = handler;
}

ITreeBuilderClassHandler* ResourceManager::classHandler(size_t index) const {
	if (_class_handlers.find(index) != _class_handlers.end())
		return _class_handlers.at(index);
	return NULL;
}

ISpecialFieldsHandler* ResourceManager::fieldsHandler(size_t index) const {
	if (_field_handlers.find(index) != _field_handlers.end())
		return _field_handlers.find(index)->second;
	return NULL;
}

ProjectNode* ResourceManager::createStatic(const QVector<QVariant>& data, ProjectNode* parentItem)
{
	return new ProjectNode(ProjectNode::NodeType::fixed, data, parentItem);
}

ProjectNode* ResourceManager::createStatic(const QVector<QVariant>& data, ProjectNode* parentItem, ProjectNode::NodeType type)
{
	return new ProjectNode(type, data, parentItem);
}

ProjectNode* ResourceManager::createSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::support, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createEventsSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::events_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createVariablesSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::variables_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createAnimationStylesSupport(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::animation_styles_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createAnimationStyle(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::animation_style_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createProject(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::project_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createCharacter(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::character_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createBehavior(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::behavior_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createSkeleton(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::skeleton_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createAnimation(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::animation_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createMisc(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::misc_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createHkxProject(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::hkx_project_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createHkxCharacter(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::hkx_character_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createHkxNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::hkx_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createEventNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::event_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createWeaponSetNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::weapon_set_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createClipEventNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::clip_event_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createVariableNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::variable_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::createPropertyNode(size_t file_index, const QVector<QVariant>& data, ProjectNode* parentItem) {
	auto node = new ProjectNode(ProjectNode::NodeType::property_node, data, parentItem);
	_nodes[file_index].push_back(node);
	return node;
}

ProjectNode* ResourceManager::findNode(int file, const hkVariant* variant) const
{
	const auto& nodes = _nodes.at(file);
	auto node_it = std::find_if(nodes.begin(), nodes.end(),
		[&variant](const ProjectNode* element) { return element->isVariant() && variant == (hkVariant*)element->data(1).value<unsigned long long>(); });
	if (node_it != nodes.end())
		return *node_it;
	return nullptr;
}

const std::string& ResourceManager::get_sanitized_name(int file_index) {
	if (_sanitized_names.find(file_index) == _sanitized_names.end())
	{
		auto p = _files.at(file_index);
		string sanitized_project_name = p.filename().replace_extension("").string();
		_sanitized_names[file_index] = sanitized_project_name;
	}
	return _sanitized_names.at(file_index);
}

bool ResourceManager::isCreatureProject(int file_index) {
	auto entry = findCacheEntry(file_index);
	if (entry->hasCache())
		return true;
	return false;
}

CacheEntry* ResourceManager::findCacheEntry(const std::string& sanitized_name)
{
	return _cache.find(sanitized_name);
}

CacheEntry* ResourceManager::findOrCreateCacheEntry(size_t file_index, bool character)
{
	return _cache.findOrCreate(get_sanitized_name(file_index), character);
}

CacheEntry* ResourceManager::findCacheEntry(size_t file_index)
{
	return _cache.find(get_sanitized_name(file_index));
}

const AnimData::ClipMovementData& ResourceManager::getMovement(size_t file_index, string clip) {
	return _cache.getMovement(get_sanitized_name(file_index), clip );
}

vector<AnimationCache::event_info_t> ResourceManager::getEventsInfo(size_t file_index, string anim_event) 
{
	return _cache.getEventsInfo(get_sanitized_name(file_index), anim_event);
}

void ResourceManager::save_cache(int file_index)
{
	string project = get_sanitized_name(file_index);
	CacheEntry* project_entry = findCacheEntry(project);
	fs::path animationDataPath = "animationdatasinglefile.txt";
	fs::path animationSetDataPath = "animationsetdatasinglefile.txt";
	fs::create_directories(_workspace.getFolder() / "test");
	_cache.save_creature(
		project, project_entry, animationDataPath, animationSetDataPath, _workspace.getFolder() / "test"
	);
}