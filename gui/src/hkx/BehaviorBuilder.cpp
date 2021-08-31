#include "BehaviorBuilder.h"

#include <hkbBehaviorGraph_1.h>

#include <hkbStateMachineTransitionInfo_1.h>
#include <hkbStateMachineTimeInterval_0.h>
#include <hkbVariableBindingSet_2.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> BehaviorBuilder::getHandledClasses()
{
	return {
		&hkbBehaviorGraphClass,
		&hkbBehaviorGraphDataClass,
		&hkbBehaviorGraphStringDataClass
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
	return parent;
}

QVariant BehaviorBuilder::handle(void* value, const hkClass* hkclass, const hkClassMember* hkmember)
{
	auto events = getEventFields();
	auto variables = getVariableFields();
	if (std::find_if(events.begin(), events.end(), 
		[&hkclass, &hkmember](const member_id_t& element){ return element.first == hkclass && element.second == hkmember; }) != events.end())
	{
		auto int_value = *(int*)value;
		if (int_value >= 0 && int_value < _strings->m_eventNames.getSize())
			return _strings->m_eventNames[*(int*)value].cString();
		return "Invalid Event";
	}
	if (std::find_if(variables.begin(), variables.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != variables.end())
	{
		auto int_value = *(int*)value;
		if (int_value >= 0 && int_value < _strings->m_variableNames.getSize())
			return _strings->m_variableNames[*(int*)value].cString();
		return "Invalid Variable";
	}
	return "BehaviorBuilder - Not set";
}