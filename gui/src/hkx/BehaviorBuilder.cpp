#include "BehaviorBuilder.h"

#include <src/hkx/HkxItemEvent.h>
#include <src/hkx/HkxItemVar.h>
#include <src/hkx/HkxItemFSMState.h>

#include <hkbBehaviorGraph_1.h>
#include <hkbStateMachineTransitionInfo_1.h>
#include <hkbStateMachineTimeInterval_0.h>
#include <hkbVariableBindingSet_2.h>
#include <hkbBehaviorReferenceGenerator_0.h>
#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>

using namespace ckcmd::HKX;

std::vector<const hkClass*> BehaviorBuilder::getHandledClasses()
{
	return {
		&hkbBehaviorGraphClass,
		&hkbBehaviorGraphDataClass,
		&hkbBehaviorGraphStringDataClass,
		&hkbBehaviorReferenceGeneratorClass,
		&hkbClipGeneratorClass
	};
};

std::vector<member_id_t> BehaviorBuilder::getEventFields() {
	return {
		{&hkbStateMachineTransitionInfoClass, hkbStateMachineTransitionInfoClass.getMemberByName("eventId")},
		{&hkbStateMachineTimeIntervalClass, hkbStateMachineTimeIntervalClass.getMemberByName("enterEventId")},
		{&hkbStateMachineTimeIntervalClass, hkbStateMachineTimeIntervalClass.getMemberByName("exitEventId")}
	};
}

std::vector<member_id_t> BehaviorBuilder::getStateIdFields() {
	return {
		{&hkbStateMachineTransitionInfoClass, hkbStateMachineTransitionInfoClass.getMemberByName("toStateId")},
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
	auto stateids = getStateIdFields();
	result.reserve(events.size() + variables.size());
	result.insert(result.end(), events.begin(), events.end());
	result.insert(result.end(), variables.begin(), variables.end());
	result.insert(result.end(), stateids.begin(), stateids.end());
	if (_skeleton_builder != NULL) {
		auto skeleton = _skeleton_builder->getHandledFields();
		result.reserve(events.size() + variables.size() + skeleton.size());
		result.insert(result.end(), skeleton.begin(), skeleton.end());
	}
	return result;
}

BehaviorBuilder::BehaviorBuilder(
	CommandManager& commandManager, 
	ResourceManager& manager, 
	size_t project_file_index,
	size_t character_file_index,
	size_t file_index, ProjectNode* animationsNode,
	bool root_behavior) :
	_command_manager(commandManager),
	_manager(manager),
	_project_file_index(project_file_index),
	_character_file_index(character_file_index),
	_file_index(file_index),
	_animationsNode(animationsNode),
	_root_behavior(root_behavior)
{
}

void BehaviorBuilder::buildEvents(const buildContext& context)
{
	if (context.data->m_eventInfos.getSize() > 0)
	{
		_eventsNode = context.parent->appendChild(
			_manager.createEventsSupport(
				_file_index,
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
			auto event_infos = _manager.getEventsInfo(_project_file_index, event_name.cString());
			if (event_infos.size() == 1 && event_infos.at(0).type == AnimationCache::event_type_t::idle) {
				name = QString("[%1] <IDLE> %2").arg(i).arg(event_name.cString());
			}
			else if (event_infos.size() > 0 && event_infos.at(0).type == AnimationCache::event_type_t::attack) {
				name = QString("[%1] <ATTACK> %2").arg(i).arg(event_name.cString());
			}
			
			auto event_node = _eventsNode->appendChild(
				_manager.createEventNode(
					_file_index,
					{
						name,
						(unsigned long long)_manager.at(_file_index, context.object_index),
						(int)_file_index,
						i,
					},
					_eventsNode)
			);
		}
	}
}
void BehaviorBuilder::buildVariables(const buildContext& context)
{
	if (context.data->m_variableInfos.getSize() > 0)
	{
		_variablesNode = context.parent->appendChild(
			_manager.createVariablesSupport(
				_file_index,
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
			auto variable_node = _variablesNode->appendChild(
				_manager.createEventNode(
					_file_index,
					{
						name,
						(unsigned long long)0,
						(int)_file_index,
						i
					},
					_variablesNode)
			);
		}
	}
}
void BehaviorBuilder::buildProperties(const buildContext& context)
{
	if (context.data->m_variableInfos.getSize() > 0)
	{
		auto properties_node = context.parent->appendChild(
			_manager.createSupport(
				_file_index,
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
				_manager.createPropertyNode(
					_file_index,
					{
						name,
						(unsigned long long)_manager.at(_file_index, context.object_index),
						_file_index,
						i
					},
					properties_node)
			);
		}
	}
}
void BehaviorBuilder::buildAnimationStyles(const buildContext& context)
{
	if (_manager.isCreatureProject(_project_file_index))
	{
		auto cache_animation_entry = (CreatureCacheEntry*)_manager.findCacheEntry(_project_file_index);
		auto cache_animation_sets_data = cache_animation_entry->getProjectAttackBlocks();

		auto animation_styles_node = context.parent->appendChild(
			_manager.createAnimationStylesSupport(
				_file_index,
				{
					"Animation Styles"
				},
				context.parent)
		);
		if (cache_animation_sets_data.size() == 1)
		{
			ProjectNode* parent = animation_styles_node->appendChild(
				_manager.createAnimationStyle(
					_file_index,
					{
						"Unarmed"
					},
					animation_styles_node)
			);
			for (auto& attack : cache_animation_sets_data.at(0).getAttackData().getAttackData()) {
				auto attack_node = parent->appendChild(
					_manager.createAnimationStyle(
						_file_index,
						{
							attack.getEventName().c_str(),
							attack.getUnk1()
						},
						parent)
				);
			}
		}
		else {
			for (auto& data : cache_animation_sets_data) {
				ProjectNode* parent;
				if (!data.getHandVariableData().getVariables().empty()) {

					auto variables = data.getHandVariableData().getVariables();
					QString style_name = QString::fromStdString(variables.at(0).getHandString());
					for (int i = 1; i < variables.size(); i++)
					{
						style_name += "/" + QString::fromStdString(variables.at(i).getHandString());
					}
					QVector<QString> qvariables;
					QVector<int> qmins;
					QVector<int> qmaxs;
					for (int i = 0; i < variables.size(); i++)
					{
						qvariables.push_back(QString::fromStdString(variables.at(i).variable_name));
						qmins.push_back(variables.at(i).value_min);
						qmaxs.push_back(variables.at(i).value_max);
					}
					QVariant value1; value1.setValue(qvariables);
					QVariant value2; value2.setValue(qmins);
					QVariant value3; value3.setValue(qmaxs);

					parent = animation_styles_node->appendChild(
						_manager.createAnimationStyle(
							_file_index,
							{
								style_name,
								value1,
								value2,
								value3
							},
							animation_styles_node)
					);
					for (auto& attack : data.getAttackData().getAttackData()) {
						auto attack_node = parent->appendChild(
							_manager.createAnimationStyle(
								_file_index,
								{
									attack.getEventName().c_str(),
									attack.getUnk1()
								},
								parent)
						);
					}
				}			
			}
		}
	}
}

//TODO: common to all builders
ProjectNode* BehaviorBuilder::buildBranch(hkVariant& variant, ProjectNode* root_node, const fs::path& path) {
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
				(unsigned long long)_manager.at(_file_index, object_index),
				(unsigned long long) root_node->isVariant() ? root_node->data(1) : 0,
				(int)_file_index
			},
			root_node));
}

void BehaviorBuilder::addCacheToClipNode(ProjectNode* animation_node, const hkbClipGenerator* clip)
{
	if (_manager.isCreatureProject(_project_file_index))
	{
		try {
			auto movement = _manager.getMovement(_project_file_index, clip->m_name.cString());
			animation_node->appendData(QString::fromStdString(movement.getBlock()));
		}
		catch (out_of_range e)
		{
			LOG << "Unable to find the movement for " << clip->m_name.cString() << log_endl;
		}
	}
}

ProjectNode* BehaviorBuilder::visit(
	const fs::path& _file,
	int object_index,
	ProjectNode* parent)
{
	auto* variant = _manager.at(_file, object_index);
	if (variant->m_class == &hkbBehaviorGraphClass)
	{
		auto _graph = (hkbBehaviorGraph*)variant->m_object;
		_data = _graph->m_data;
		_strings = _graph->m_data->m_stringData;

		buildContext context =
		{
			_data,
			_strings,
			_file,
			object_index,
			parent,
		};

		buildEvents(context);
		buildVariables(context);
		buildProperties(context);
		if (_root_behavior)
			buildAnimationStyles(context);
		return buildBranch(*variant, parent, _file);
	}
	else if (variant->m_class == &hkbBehaviorReferenceGeneratorClass) {
		hkbBehaviorReferenceGenerator* reference = (hkbBehaviorReferenceGenerator*)variant->m_object;
		_referenced_behaviors.insert(reference->m_behaviorName.cString());
		return buildBranch(*variant, parent, _file);
	}
	else if (variant->m_class == &hkbClipGeneratorClass) {
		auto clip_node = buildBranch(*variant, parent, _file);
		hkbClipGenerator* clip = (hkbClipGenerator*)variant->m_object;

		fs::path project_folder = _manager.path(_project_file_index).parent_path();
		auto animation_path = project_folder / clip->m_animationName.cString();
		if (fs::exists(animation_path))
		{
			bool found = false;
			if (_animationsNode != nullptr) {
				for (int i = 0; i < _animationsNode->childCount(); i++)
				{
					if (_animationsNode->child(i)->data(0).value<QString>() == clip->m_animationName.cString()) {
						found = true;
						break;
					}
				}
			}

			if (!found)
			{
				ProjectNode* animation_node = _manager.createAnimation(
					_character_file_index, {
						clip->m_animationName.cString(),
						animation_path.string().c_str()
					},
					_animationsNode);
				_animationsNode->appendChild(animation_node);
				addCacheToClipNode(animation_node, clip);
			}

			
		}
		return clip_node;
	}
	return parent;
}

QVariant BehaviorBuilder::handle(void* value, const hkClass* hkclass, const hkClassMember* hkmember, const hkVariant* container)
{
	auto events = getEventFields();
	if (std::find_if(events.begin(), events.end(), 
		[&hkclass, &hkmember](const member_id_t& element){ return element.first == hkclass && element.second == hkmember; }) != events.end())
	{
		return HkxItemEvent(this, *(int*)value);
	}
	auto variables = getVariableFields();
	if (std::find_if(variables.begin(), variables.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != variables.end())
	{
		return HkxItemVar(this, *(int*)value);
	}
	auto stateids = getStateIdFields();
	if (std::find_if(stateids.begin(), stateids.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != stateids.end())
	{
		size_t fsm_index;
		auto node = _manager.findNode(_file_index, container);
		auto parent_variant = (hkVariant*)(node->parentItem()->data(1).value<unsigned long long>());
		if (parent_variant->m_class == &hkbStateMachineStateInfoClass) {
			auto grandnode = node->parentItem();
			auto grandparent_variant = (hkVariant*)(grandnode->parentItem()->data(1).value<unsigned long long>());
			fsm_index = _manager.findIndex(_file_index, grandparent_variant->m_object);
		}
		else if (parent_variant->m_class == &hkbStateMachineClass) {
			fsm_index = _manager.findIndex(_file_index, parent_variant->m_object);
		}
		else {
			throw std::runtime_error("Unknown parent for state Id");
		}
		return HkxItemFSMState(this, fsm_index , *(int*)value);
	}
	auto bones = _skeleton_builder->getHandledFields();
	if (std::find_if(bones.begin(), bones.end(),
		[&hkclass, &hkmember](const member_id_t& element) { return element.first == hkclass && element.second == hkmember; }) != bones.end())
	{
		return _skeleton_builder->handle(_file_index, value, hkclass, hkmember, container);
	}
	return "BehaviorBuilder - Not set";
}

QStringList BehaviorBuilder::getEvents() const
{
	QStringList out;
	for (int i = 0; i < _strings->m_eventNames.getSize(); i++)
	{
		out << _strings->m_eventNames[i].cString();
	}
	return out;
}

QString BehaviorBuilder::getEvent(size_t index) const
{
	if (index < _strings->m_eventNames.getSize())
		return _strings->m_eventNames[index].cString();
	return "No Event";
}

QStringList BehaviorBuilder::getVariables() const
{
	QStringList out;
	for (int i = 0; i < _strings->m_variableNames.getSize(); i++)
	{
		out << _strings->m_variableNames[i].cString();
	}
	return out;
}

QString BehaviorBuilder::getVariable(size_t index) const
{
	if (index < _strings->m_variableNames.getSize())
		return _strings->m_variableNames[index].cString();
	return "No Variable";
}

QStringList BehaviorBuilder::getFSMStates(size_t fsm_index) const
{
	QStringList out;
	auto fsm_variant = _manager.at(_file_index, fsm_index);
	if (fsm_variant->m_class = &hkbStateMachineClass) {
		hkbStateMachine* fsm = (hkbStateMachine*)fsm_variant->m_object;
		for (int i = 0; i < fsm->m_states.getSize(); i++)
		{
			out << fsm->m_states[i]->m_name.cString();
		}
		return out;
	}
	return { "Wrong Reference!" };
}

QString BehaviorBuilder::getFSMState(size_t fsm_index, size_t index) const
{
	auto fsm_variant = _manager.at(_file_index, fsm_index);
	if (fsm_variant->m_class = &hkbStateMachineClass) {
		hkbStateMachine* fsm = (hkbStateMachine*)fsm_variant->m_object;
		for (int i = 0; i < fsm->m_states.getSize(); i++)
		{
			if (fsm->m_states[i]->m_stateId == index)
				return fsm->m_states[i]->m_name.cString();
		}
	}
	return "Invalid State";
}

size_t BehaviorBuilder::getFSMStateId(size_t fsm_index, size_t combo_index) const
{
	auto fsm_variant = _manager.at(_file_index, fsm_index);
	if (fsm_variant->m_class = &hkbStateMachineClass) {
		hkbStateMachine* fsm = (hkbStateMachine*)fsm_variant->m_object;
		return fsm->m_states[combo_index]->m_stateId;
	}
	return -1;
}

size_t BehaviorBuilder::addEvent(const QString& event_name)
{
	for (int i = 0; i < _strings->m_eventNames.getSize(); i++)
	{
		if (event_name == _strings->m_eventNames[i].cString()) {
			return i;
		}
	}
	size_t new_index = _strings->m_eventNames.getSize();
	_strings->m_eventNames.pushBack(event_name.toUtf8().data());
	_data->m_eventInfos.pushBack(hkbEventInfo());

	QString name = QString("[%1] %2").arg(new_index).arg(event_name);
	auto event_node = _eventsNode->appendChild(
		_manager.createEventNode(
			_file_index,
			{
				name,
				(unsigned long long)0,
				(int)_file_index,
				new_index,
			},
			_eventsNode
		)
	);
	return new_index;
}

bool BehaviorBuilder::renameEvent(size_t index, const QString& name)
{
	if (index < _strings->m_eventNames.getSize())
	{
		_strings->m_eventNames[index] = name.toUtf8().data();
		return true;
	}
	return false;
}

size_t BehaviorBuilder::removeEvent(const QString& event_name)
{
	return 0;
}

size_t BehaviorBuilder::addVariable(const QString& variable_name)
{
	for (int i = 0; i < _strings->m_variableNames.getSize(); i++)
	{
		if (variable_name == _strings->m_variableNames[i].cString()) {
			return i;
		}
	}
	size_t new_index = _strings->m_variableNames.getSize();
	_strings->m_variableNames.pushBack(variable_name.toUtf8().data());
	_data->m_eventInfos.pushBack(hkbEventInfo());

	QString name = QString("[%1] %2").arg(new_index).arg(variable_name);
	auto event_node = _variablesNode->appendChild(
		_manager.createVariableNode(
			_file_index,
			{
				name,
				(unsigned long long)0,
				(int)_file_index,
				new_index,
			},
			_variablesNode
			)
	);
	return new_index;
}

size_t BehaviorBuilder::removeVariable(const QString& variable_name)
{
	return 0;
}