#include <src/models/ModelEdgeRegistry.h>

#include <src/edges/BehaviorModel.h>
#include <src/edges/BehaviorReferenceModel.h>
#include <src/edges/CharacterModel.h>
#include <src/edges/SkeletonModel.h>
#include <src/edges/RagdollModel.h>
#include <src/edges/StateMachineModel.h>
#include <src/edges/FileModel.h>
#include <src/edges/ClipModel.h>
#include <src/edges/EvaluateExpressionModifierModel.h>
#include <src/edges/StateMachineStateModel.h>

using namespace ckcmd::HKX;

ModelEdgeRegistry::ModelEdgeRegistry()
{
	addEdgeHandler(new BehaviorModel());
	addEdgeHandler(new BehaviorReferenceModel());
	addEdgeHandler(new CharacterModel());
	addEdgeHandler(new SkeletonModel());
	addEdgeHandler(new RagdollModel());
	addEdgeHandler(new StateMachineModel());
	addEdgeHandler(new StateMachineStateModel());
	addEdgeHandler(new FileModel());
	addEdgeHandler(new ClipModel());
	addEdgeHandler(new EvaluateExpressionModifierModel());
	_default_handler = new MultipleVariantsEdge();
}

ModelEdgeRegistry::~ModelEdgeRegistry()
{

}

void ModelEdgeRegistry::addEdgeHandler(IEdge* edge)
{
	if (!edge->handled_types().empty())
	{
		for (auto type : edge->handled_types())
		{
			_node_handlers[type] = edge;
		}
	}
	if (!edge->handled_hkclasses().empty())
	{
		for (auto* type : edge->handled_hkclasses())
		{
			_type_handlers[type] = edge;
		}
	}
}

IEdge* ModelEdgeRegistry::handler(const ModelEdge& edge)
{
	if (_node_handlers.find(edge.childType()) != _node_handlers.end())
	{
		return _node_handlers[edge.childType()];
	}
	if (edge.childItem<hkVariant>() != nullptr && _type_handlers.find(edge.childItem<hkVariant>()->m_class) != _type_handlers.end())
	{
		return _type_handlers[edge.childItem<hkVariant>()->m_class];
	}
	return _default_handler;
}
