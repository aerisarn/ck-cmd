#pragma once

#include <src/edges/IEdge.h>

namespace ckcmd {
    namespace HKX {

        struct ModelEdgeRegistry {

            static ModelEdgeRegistry& instance() {
                static ModelEdgeRegistry instance;
                return instance;
            }

            ModelEdgeRegistry(const ModelEdgeRegistry&) = delete;
            ModelEdgeRegistry& operator = (const ModelEdgeRegistry&) = delete;

        private:

            std::map<NodeType, IEdge*> _node_handlers;
            std::map<const hkClass*, IEdge*> _type_handlers;
            IEdge* _default_handler;

            ModelEdgeRegistry();
            ~ModelEdgeRegistry();

            void addEdgeHandler(IEdge*);

        public:

            IEdge* handler(const ModelEdge& edge);
        };
    }
}