#pragma once

#include <src/edges/SupportEnhancedEdge.h>

#include <hkbCharacterData_7.h>

namespace ckcmd {
	namespace HKX {

		class CharacterModel : public SupportEnhancedEdge
		{
			hkbCharacterData* variant(const ModelEdge& edge) const;
			hkbCharacterStringData* string_variant(const ModelEdge& edge) const;
			hkbMirroredSkeletonInfo* mirror_variant(const ModelEdge& edge) const;
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
	}
}
