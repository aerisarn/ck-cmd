#pragma once

#include <QString>
#include <QVariant>

namespace ckcmd {
	namespace HKX {

		template <typename Builder>
		class HkxItemReference {
		
		protected:
			Builder* _builder;
			size_t _index;

		public:
			HkxItemReference() = default;
			~HkxItemReference() = default;
			HkxItemReference(const HkxItemReference&) = default;
			HkxItemReference& operator=(const HkxItemReference&) = default;

			HkxItemReference(Builder* builder, size_t index) :
				_builder(builder),
				_index(index)
			{}

			virtual operator QVariant() const = 0;

			virtual QString getValue() const = 0;
			virtual QStringList getValues() const = 0;

			virtual size_t index() { return _index; }

			virtual void setValue(size_t index) { _index = index; }

		};
	}
}