#pragma once

#include <core\hkxpch.h>
#include <vector>
#include <map>
#include <QVariant>

namespace ckcmd {
    namespace HKX {

        typedef std::pair<const hkClass*, const hkClassMember*> member_id_t;

        class ISpecialFieldsHandler {
        public:
            virtual std::vector<member_id_t> getHandledFields() = 0;

            virtual QVariant handle(void* value, const hkClass*, const hkClassMember*, const hkVariant*, const hkVariant* parent_container) = 0;

            template<typename T>
            QVariant value(T& value, const hkClass* hkclass, const hkClassMember* hkmember, const hkVariant* container, const hkVariant* parent_container) {
                return handle(&value, hkclass, hkmember, container, parent_container);
            }
        };

        class SpecialFieldsListener {
        protected:
            std::map<member_id_t, ISpecialFieldsHandler*> _handlers;
        public:
            void registerFieldHandler(ISpecialFieldsHandler* handler) {
                for (auto info : handler->getHandledFields())
                    _handlers[info] = handler;
            }
            void unregisterFieldHandler(ISpecialFieldsHandler* handler) {
                for (auto info : handler->getHandledFields())
                    _handlers[info] = handler;
            }
        };
    }
}