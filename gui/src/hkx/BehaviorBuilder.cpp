#include "BehaviorBuilder.h"

#include <hkbBehaviorGraph_1.h>

#include <hkbStateMachineTransitionInfo_1.h>
#include <hkbStateMachineTimeInterval_0.h>
#include <hkbVariableBindingSet_2.h>
#include <hkbBehaviorReferenceGenerator_0.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> BehaviorBuilder::getHandledClasses()
{
	return {
		&hkbBehaviorGraphClass,
		&hkbBehaviorGraphDataClass,
		&hkbBehaviorGraphStringDataClass,
		&hkbBehaviorReferenceGeneratorClass
	};
};

std::vector<member_id_t> BehaviorBuilder::getEventFields() {
	return {
		{&hkbStateMachineTransitionInfoClass, hkbStateMachineTransitionInfoClass.getMemberByName("eventId")},
		{&hkbStateMachineTimeIntervalClass, hkbStateMachineTimeIntervalClass.getMemberByName("enterEventId")},
		{&hkbStateMachineTimeIntervalClass, hkbStateMachineTimeIntervalClass.getMemberByName("exitEventId")}
	};
}

std::vector<member_id_t> BehaviorBuilder::getVariableFields()
{
	return {
		{ &hkbVariableBindingSetBindingClass, hkbVariableBindingSetBindingClass.getMemberByName("variableIndex") }
	};
}


std::vector<member_id_t> BehaviorBuilder::getHandledFields() {
	std::vector<member_id_t> result;
	auto events = getEventFields();
	auto variables = getVariableFields();
	result.reserve(events.size() + variables.size());
	result.insert(result.end(), events.begin(), events.end());
	result.insert(result.end(), variables.begin(), variables.end());
	if (_skeleton_builder != NULL) {
		auto skeleton = _skeleton_builder->getHandledFields();
		result.reserve(events.size() + variables.size() + skeleton.size());
		result.insert(result.end(), skeleton.begin(), skeleton.end());
	}
	return result;
}

void BehaviorBuilder::buildEvents(const buildContext& context)
{
	if (context.data->m_eventInfos.getSize() > 0)
	{
		auto events_node = context.parent->appendChild(
			ProjectNode::createSupport(
				{
					"Events"
				},
				context.parent)
		);

		for (int i = 0; i < context.data->m_eventInfos.getSize(); i++)
		{
			auto event_name = context.string_data->m_eventNames[i];
			auto event_info = context.data->m_eventInfos[i];

			QString name = QString("[%1] %2").arg(i).arg(event_name.cString());
			auto event_node = events_node->appendChild(
				ProjectNode::createEventNode(
					{
						name,
						(unsigned long long)context.resourceManager.at(context._file, context.object_index),
						(int)context.resourceManager.index(context._file),
						i,
					},
					events_node)
			);
		}
	}
}
void BehaviorBuilder::buildVariables(const buildContext& context)
{
	if (context.data->m_variableInfos.getSize() > 0)
	{
		auto variables_node = context.parent->appendChild(
			ProjectNode::createSupport(
				{
					"Variables"
				},
				context.parent)
		);

		for (int i = 0; i < context.data->m_variableInfos.getSize(); i++)
		{
			auto variable_name = context.string_data->m_variableNames[i];
			auto variable_info = context.data->m_variableInfos[i];

			QString name = QString("[%1] %2").arg(i).arg(variable_name.cString());
			auto variable_node = variables_node->appendChild(
				ProjectNode::createEventNode(
					{
						name,
						(unsigned long long)context.resourceManager.at(context._file, context.object_index),
						(int)context.resourceManager.index(context._file),
						i
					},
					variables_node)
			);
		}
	}
}
void BehaviorBuilder::buildProperties(const buildContext& context)
{
	if (context.data->m_variableInfos.getSize() > 0)
	{
		auto properties_node = context.parent->appendChild(
			ProjectNode::createSupport(
				{
					"Properties"
				},
				context.parent)
		);

		for (int i = 0; i < context.data->m_characterPropertyInfos.getSize(); i++)
		{
			auto property_name = context.string_data->m_characterPropertyNames[i];
			auto property_info = context.data->m_characterPropertyInfos[i];

			QString name = QString("[%1] %2").arg(i).arg(property_name.cString());
			auto variable_node = properties_node->appendChild(
				ProjectNode::createPropertyNode(
					{
						name,
						(unsigned long long)context.resourceManager.at(context._file, context.object_index),
						(int)context.resourceManager.index(context._file),
						i
					},
					properties_node)
			);
		}
	}
}


//TODO: common to all builders
ProjectNode* BehaviorBuilder::buildBranch(hkVariant& variant, ProjectNode* root_node, const fs::path& path, ResourceManager& _resourceManager) {
	QString display_name = variant.m_class->getName();
	//check if the object has a name we can display
	auto member = variant.m_class->getMemberByName("name");
	if (HK_NULL != member)
	{
		auto member_ptr = ((char*)variant.m_object) + member->getOffset();
		auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
		display_name = QString("%1 \"%2\"").arg(display_name).arg(c_str_ptr);
	}

	auto object_index = _resourceManager.findIndex(path, variant.m_object);
	QString name = QString("[%1] %2").arg(object_index).arg(display_name);
	return root_node->appendChild(
		ProjectNode::createHkxNode(
			{
				name,
				(unsigned long long)_resourceManager.at(path, object_index),
				(unsigned long long) root_node->isVariant() ? root_node->data(1) : 0,
				(int)_resourceManager.index(path)
			},
			root_node));
}

ProjectNode* BehaviorBuilder::visit(
	const fs::path& _file,
	int object_index,
	ProjectNode* parent,
	ResourceManager& resourceManager)
{
	auto* variant = resourceManager.at(_file, object_index);
	if (variant->m_class == &hkbBehaviorGraphClass)
	{
		hkbBehaviorGraph* graph = (hkbBehaviorGraph*)variant->m_object;
		auto data = graph->m_data;
		_strings = graph->m_data->m_stringData;

		buildContext context =
		{
			data,
			_strings,
			_file,
			object_index,
			parent,
			resourceManager
		};

		buildEvents(context);
		buildVariables(context);
		buildProperties(context);
	}
	else if (variant->m_class == &hkbBehaviorReferenceGeneratorClass) {
		hkbBehaviorReferenceGenerator* reference = (hkbBehaviorReferenceGenerator*)variant->m_object;
		_referenced_behaviors.insert(reference->m_behaviorName.cString());
		return buildBranch(*variant, parent, _file, resourceManager);
	}
	return parent;
}

QVariant BehaviorBuilder::handle(void* value, const hkClass* hkclass, const hkClassMember* hkmember, const hkVariant* container, const hkVariant* parent_container)
{
	auto events = getEventFields();
	if (std::find_if(events.begin(), events.end(), 
		[&hkclass, &hkmember](const member_id_t& element){ return element.first == hkclass && element.second == hkmember; }) != events.end())
	{
		auto int_value = *(int*)value;
		if (int_value >= 0 && int_value < _strings->m_eventNames.getSize())
			return _strings->m_eventNames[*(int*)value].cString();
		return "Invalid Event";
	}
	auto variables = getVariableFields();
	if (std::find_if(variables.begin(), variables.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != variables.end())
	{
		auto int_value = *(int*)value;
		if (int_value >= 0 && int_value < _strings->m_variableNames.getSize())
			return _strings->m_variableNames[*(int*)value].cString();
		return "Invalid Variable";
	}
	auto bones = _skeleton_builder->getHandledFields();
	if (std::find_if(bones.begin(), bones.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != bones.end())
	{
		return _skeleton_builder->handle(value, hkclass, hkmember, container, parent_container);
	}
	return "BehaviorBuilder - Not set";
}