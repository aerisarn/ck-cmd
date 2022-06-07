#pragma once

#include <src/models/ProjectNode.h>
#include <src/hkx/HkxTableVariant.h>

namespace ckcmd {
    namespace HKX {

        class HkxTreeVariant : public ProjectNode
        {
            hkVariant* _variant;
            int _file_index;

        public:

            virtual std::string name() override;
            virtual hkVariant* variant() const override;
            virtual int file() const override;

            HkxTreeVariant::HkxTreeVariant(int file_index, hkVariant* variant, ProjectNode* parent = nullptr)
                : ProjectNode(ProjectNode::NodeType::hkx_node, {}, parent),
                _file_index(file_index),
                _variant(variant)
            {}
        };
    }
}