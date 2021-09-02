#include "SkeletonBuilder.h"

#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>
#include <hkbBoneIndexArray_0.h>
#include <hkbKeyframeBonesModifier_3.h>
#include <Common/Serialize/ResourceDatabase/hkResourceHandle.h>

using namespace ckcmd::HKX;

SkeletonBuilder::SkeletonBuilder(ResourceManager& manager, size_t file_index) :
	_skeleton(NULL),
	_ragdoll(NULL),
	_manager(manager),
	_file_index(file_index)
{
}

std::vector<const hkClass*> SkeletonBuilder::getHandledClasses()
{
	return {
		&hkaAnimationContainerClass,
		&hkaRagdollInstanceClass,
		&hkMemoryResourceContainerClass
	};
};

std::vector<member_id_t> SkeletonBuilder::getSkeletonBoneFields() {
	return {
		{&hkaSkeletonMapperData::SimpleMapping::staticClass(), hkaSkeletonMapperData::SimpleMapping::staticClass().getMemberByName("boneA")},
		{&hkaSkeletonMapperData::SimpleMapping::staticClass(), hkaSkeletonMapperData::SimpleMapping::staticClass().getMemberByName("boneB")},
	};
}

std::vector<member_id_t> SkeletonBuilder::getRagdollBoneFields()
{
	return {
		{ &hkbBoneIndexArrayClass, hkbBoneIndexArrayClass.getMemberByName("boneIndices") }
	};
}


std::vector<member_id_t> SkeletonBuilder::getHandledFields() {
	std::vector<member_id_t> result;
	auto skeleton = getSkeletonBoneFields();
	auto ragdoll = getRagdollBoneFields();
	result.reserve(skeleton.size() + ragdoll.size());
	result.insert(result.end(), skeleton.begin(), skeleton.end());
	result.insert(result.end(), ragdoll.begin(), ragdoll.end());
	return result;
}

void SkeletonBuilder::buildSkeleton(const buildContext& context)
{
}

//TODO: common to all builders
ProjectNode* SkeletonBuilder::buildBranch(hkVariant& variant, ProjectNode* root_node, const fs::path& path) {
	QString display_name = variant.m_class->getName();
	//check if the object has a name we can display
	auto member = variant.m_class->getMemberByName("name");
	if (HK_NULL != member)
	{
		auto member_ptr = ((char*)variant.m_object) + member->getOffset();
		auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
		display_name = QString("%1 \"%2\"").arg(display_name).arg(c_str_ptr);
	}

	auto object_index = _manager.findIndex(path, variant.m_object);
	QString name = QString("[%1] %2").arg(object_index).arg(display_name);
	return root_node->appendChild(
		_manager.createHkxNode(
			_file_index,
			{
				name,
				(unsigned long long)_manager.at(path, object_index),
				(unsigned long long) root_node->isVariant() ? root_node->data(1) : 0,
				(int)_manager.index(path)
			},
			root_node));
}

ProjectNode* SkeletonBuilder::visit(
	const fs::path& _file,
	int object_index,
	ProjectNode* parent)
{
	auto* variant = _manager.at(_file, object_index);
	if (variant->m_class == &hkaAnimationContainerClass)
	{
		hkaAnimationContainer* container = (hkaAnimationContainer*)variant->m_object;
		_skeleton = container->m_skeletons[0];

		buildContext context =
		{
			_file,
			object_index,
			parent,
		};

		if (container->m_skeletons.getSize() > 1)
			_ragdoll = container->m_skeletons[1];
		else
			buildSkeleton(context);

	}
	else if (variant->m_class == &hkaRagdollInstanceClass) {

		hkaRagdollInstance* container = (hkaRagdollInstance*)variant->m_object;
		if (container->m_skeleton != _ragdoll)
		{
			auto real_ragdoll = _skeleton;
			_skeleton = _ragdoll;
			_ragdoll = real_ragdoll;
		}
		buildContext context =
		{
			_file,
			object_index,
			parent
		};

		buildSkeleton(context);
	}

	return buildBranch(*variant, parent, _file);
}

QVariant SkeletonBuilder::handle(void* value, const hkClass* hkclass, const hkClassMember* hkmember, const hkVariant* hkcontainer, const hkVariant* parent_container)
{
	auto skeletons = getSkeletonBoneFields();
	auto ragdolls = getRagdollBoneFields();
	//special handling that need parent variant
	if (&hkbBoneIndexArrayClass == hkclass &&
		hkbBoneIndexArrayClass.getMemberByName("boneIndices") == hkmember
	) {
		auto p_class = parent_container->m_class;
		auto int_value = *(short*)value;
		if (p_class == &hkbKeyframeBonesModifierClass)
		{
			//skeleton
			if (int_value >= 0 && int_value < _skeleton->m_bones.getSize())
				return _skeleton->m_bones[int_value].m_name.cString();
			return "Invalid Skeleton Bone";
		}
		else {
			if (int_value >= 0 && int_value < _skeleton->m_bones.getSize())
				return _skeleton->m_bones[int_value].m_name.cString();
			return "Invalid Skeleton Bone";
		}
		return "Invalid boneIndices Index";
	}

	//special handling that needs parent
	if (&hkaSkeletonMapperData::SimpleMapping::staticClass() == hkclass &&
		hkaSkeletonMapperData::SimpleMapping::staticClass().getMemberByName("boneA") == hkmember
	) {
		//nearest object
		auto& object = ((hkaSkeletonMapper*)hkcontainer->m_object)->m_mapping;
		if (object.m_skeletonA == _skeleton) {
			auto int_value = *(short*)value;
			if (int_value >= 0 && int_value < _skeleton->m_bones.getSize())
				return _skeleton->m_bones[int_value].m_name.cString();
			return "Invalid Skeleton Bone";
		}
		else {
			auto int_value = *(short*)value;
			if (int_value >= 0 && int_value < _ragdoll->m_bones.getSize())
				return _ragdoll->m_bones[int_value].m_name.cString();
			return "Invalid Ragdoll Bone";
		}
	}
	if (&hkaSkeletonMapperData::SimpleMapping::staticClass() == hkclass &&
		hkaSkeletonMapperData::SimpleMapping::staticClass().getMemberByName("boneB") == hkmember
	) {
		auto& object = ((hkaSkeletonMapper*)hkcontainer->m_object)->m_mapping;
		if (object.m_skeletonB == _skeleton) {
			auto int_value = *(short*)value;
			if (int_value >= 0 && int_value < _skeleton->m_bones.getSize())
				return _skeleton->m_bones[int_value].m_name.cString();
			return "Invalid Skeleton Bone";
		}
		else {
			auto int_value = *(short*)value;
			if (int_value >= 0 && int_value < _ragdoll->m_bones.getSize())
				return _ragdoll->m_bones[int_value].m_name.cString();
			return "Invalid Ragdoll Bone";
		}
	}
	if (std::find_if(skeletons.begin(), skeletons.end(),
		[&hkclass, &hkmember](const member_id_t& element){ return element.first == hkclass && element.second == hkmember; }) != skeletons.end())
	{
		auto int_value = *(short*)value;
		if (int_value >= 0 && int_value < _skeleton->m_bones.getSize())
			return _skeleton->m_bones[int_value].m_name.cString();
		return "Invalid Skeleton Bone";
	}
	if (std::find_if(ragdolls.begin(), ragdolls.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != ragdolls.end())
	{
		auto int_value = *(short*)value;
		if (int_value >= 0 && int_value < _ragdoll->m_bones.getSize())
			return _ragdoll->m_bones[int_value].m_name.cString();
		return "Invalid Ragdoll Bone";
	}
	return "BehaviorBuilder - Not set";
}