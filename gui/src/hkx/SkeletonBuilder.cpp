#include "SkeletonBuilder.h"

#include <src/hkx/HkxItemBone.h>
#include <src/hkx/HkxItemRagdollBone.h>

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

QVariant SkeletonBuilder::handle(void* value, const hkClass* hkclass, const hkClassMember* hkmember, const hkVariant* hkcontainer)
{
	return handle(_file_index, value, hkclass, hkmember, hkcontainer);
}

QVariant SkeletonBuilder::handle(size_t file_index, void* value, const hkClass* hkclass, const hkClassMember* hkmember, const hkVariant* hkcontainer)
{
	auto skeletons = getSkeletonBoneFields();
	auto ragdolls = getRagdollBoneFields();
	//special handling that need parent variant
	if (&hkbBoneIndexArrayClass == hkclass &&
		hkbBoneIndexArrayClass.getMemberByName("boneIndices") == hkmember
	) {
		auto node = _manager.findNode(file_index, hkcontainer);
		auto parent_node = node->parentItem();
		auto parent_container = (hkVariant*)parent_node->data(1).value<unsigned long long>();
		auto p_class = parent_container->m_class;
		auto int_value = *(short*)value;
		if (p_class == &hkbKeyframeBonesModifierClass)
		{
			return HkxItemBone(this, int_value);
		}
		return HkxItemRagdollBone(this, int_value);
	}

	//special handling that needs parent
	if (&hkaSkeletonMapperData::SimpleMapping::staticClass() == hkclass &&
		hkaSkeletonMapperData::SimpleMapping::staticClass().getMemberByName("boneA") == hkmember
	) {
		//nearest object
		auto& object = ((hkaSkeletonMapper*)hkcontainer->m_object)->m_mapping;
		auto int_value = *(short*)value;
		if (object.m_skeletonA == _skeleton) 
		{
			return HkxItemBone(this, int_value);
		}
		return HkxItemRagdollBone(this, int_value);
	}
	if (&hkaSkeletonMapperData::SimpleMapping::staticClass() == hkclass &&
		hkaSkeletonMapperData::SimpleMapping::staticClass().getMemberByName("boneB") == hkmember
	) {
		auto& object = ((hkaSkeletonMapper*)hkcontainer->m_object)->m_mapping;
		auto int_value = *(short*)value;
		if (object.m_skeletonB == _skeleton) 
		{
			return HkxItemBone(this, int_value);
		}
		return HkxItemRagdollBone(this, int_value);
	}
	if (std::find_if(skeletons.begin(), skeletons.end(),
		[&hkclass, &hkmember](const member_id_t& element){ return element.first == hkclass && element.second == hkmember; }) != skeletons.end())
	{
		auto int_value = *(short*)value;
		return HkxItemBone(this, int_value);
	}
	if (std::find_if(ragdolls.begin(), ragdolls.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != ragdolls.end())
	{
		auto int_value = *(short*)value;
		return HkxItemRagdollBone(this, int_value);
	}
	return "BehaviorBuilder - Not set";
}

QStringList SkeletonBuilder::getSkeletonBones() const
{
	QStringList out;
	for (int i = 0; i < _skeleton->m_bones.getSize(); i++)
	{
		out << _skeleton->m_bones[i].m_name.cString();
	}
	return out;
}

QString SkeletonBuilder::getSkeletonBone(size_t index) const
{
	if (index < _skeleton->m_bones.getSize())
		return _skeleton->m_bones[index].m_name.cString();
	return "Invalid Skeleton Bone";
}

QStringList SkeletonBuilder::getRagdollBones() const
{
	QStringList out;
	for (int i = 0; i < _ragdoll->m_bones.getSize(); i++)
	{
		out << _ragdoll->m_bones[i].m_name.cString();
	}
	return out;
}

QString SkeletonBuilder::getRagdollBone(size_t index) const
{
	if (index < _ragdoll->m_bones.getSize())
		return _ragdoll->m_bones[index].m_name.cString();
	return "Invalid Ragdoll Bone";
}