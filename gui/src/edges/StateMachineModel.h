#pragma once

#include <src/edges/SupportEnhancedEdge.h>

#include <hkbStateMachine_4.h>
#include <hkbStateMachineStateInfo_4.h>
#include <hkbStateMachineTransitionInfoArray_0.h>

namespace ckcmd {
	namespace HKX {

		class StateMachineModel : public SupportEnhancedEdge
		{

			hkbStateMachine* variant(const ModelEdge& edge) const;
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> additional_variants() const override { return {}; }

		protected:

			virtual int supports() const override;
			virtual const char* supportName(int support_index) const override;
			virtual NodeType supportType(int support_index) const override;

		public:
			virtual std::vector<const hkClass*> handled_hkclasses() const override;
			virtual std::vector<NodeType> handled_types() const override;

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;

			virtual TypeInfo StateMachineModel::rowClass(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual bool isArray(int row, const ModelEdge& edge, ResourceManager& manager) const override;

			virtual bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager) override;
			virtual bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
			virtual bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager) override;
		};
	}
}
