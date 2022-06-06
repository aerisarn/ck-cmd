#pragma once

#include <src/models/ProjectNode.h>
#include <src/hkx/HkxTableVariant.h>

namespace ckcmd {
    namespace HKX {

        class HkxTreeVariant : public ProjectNode
        {
            hkVariant& _variant;

        public:

            HkxTreeVariant::HkxTreeVariant(NodeType type, hkVariant& variant, const QVector<QVariant>& data, ProjectNode* parent = nullptr)
                : ProjectNode(type, data, parent),
                _variant(variant)
            {}

            virtual ProjectNode* child(int row) override;
            virtual int childCount() const override;
            virtual int columnCount() const override;
            virtual QVariant data(int column) const override;
            virtual int row() const override;
            virtual ProjectNode* parentItem() override;
        };
    }
}