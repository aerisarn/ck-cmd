#pragma once

#include <src/edges/SupportEnhancedEdge.h>

#include <hkbBehaviorGraph_1.h>
#include <hkbBehaviorGraphData_2.h>
#include <hkbBehaviorGraphStringData_1.h>

namespace ckcmd {
	namespace HKX {

		class BehaviorModel : public SupportEnhancedEdge
		{
			hkbBehaviorGraph* variant(const ModelEdge& edge) const;
			hkbBehaviorGraphData* data_variant(const ModelEdge& edge) const;
			hkbBehaviorGraphStringData* string_variant(const ModelEdge& edge) const;
			virtual std::vector<std::function<hkVariant* (const ModelEdge&, ResourceManager& manager, hkVariant*)>> additional_variants() const override;

		protected:

			virtual int supports() const override;
			virtual const char* supportName(int support_index) const override;
			virtual NodeType supportType(int support_index) const override;

		public:

			virtual int rows(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int columns(int row, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childCount(const ModelEdge& edge, ResourceManager& manager) const override;
			virtual std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
			virtual QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager) const override;
		};

		//struct BehaviorModel
		//{
		//	static const char* DataListsName(NodeType type)
		//	{
		//		switch (type) {
		//		case NodeType::behaviorEventNames:
		//			return "Events";
		//		case NodeType::behaviorVariableNames:
		//			return "Variables";
		//		case NodeType::behaviorCharacterPropertyNames:
		//			return "Properties";
		//		default:
		//			break;
		//		}
		//		return "Invalid Behavior Entry";
		//	};

		//	//rows with lists to be shown
		//	static const QStringList children_rows;

		//	static int rows(const ModelEdge& edge, ResourceManager& manager);
		//	static int columns(int row, const ModelEdge& edge, ResourceManager& manager);
		//	static int childCount(const ModelEdge& edge, ResourceManager& manager);
		//	static std::pair<int, int> child(int index, const ModelEdge& edge, ResourceManager& manager);
		//	static int childIndex(int row, int column, const ModelEdge& edge, ResourceManager& manager);
		//	static ModelEdge child(int row, int column, const ModelEdge& edge, ResourceManager& manager);

		//	static QVariant data(int row, int column, const ModelEdge& edge, ResourceManager& manager);
		//	static bool setData(int row, int column, const ModelEdge& edge, const QVariant& data, ResourceManager& manager);
		//	static bool addRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
		//	static bool removeRows(int row_start, int count, const ModelEdge& edge, ResourceManager& manager);
		//	static bool changeColumns(int row, int column_start, int delta, const ModelEdge& edge, ResourceManager& manager);
		//	
		//};
	}
}
