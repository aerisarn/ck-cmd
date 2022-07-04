#pragma once

#include <QString>
#include <QVariant>

namespace ckcmd {
	namespace HKX {

		class HkxItemReference {
		
		protected:
			size_t _index = -1;

		public:
			HkxItemReference() = default;
			~HkxItemReference() = default;
			HkxItemReference(const HkxItemReference&) = default;
			HkxItemReference& operator=(const HkxItemReference&) = default;

			HkxItemReference(size_t index) :
				_index(index)
			{}

			virtual QString getValue() const { return ""; }
			virtual QStringList getValues() const { return {""}; }

			virtual operator QVariant() const = 0;

			virtual size_t index() { return _index; }

			virtual void setValue(size_t index) { _index = index; }

		};
	}
}