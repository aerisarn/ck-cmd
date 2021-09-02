#pragma once

#include <QMetaType>
#include <QStringList>
#include <Common\Base\hkBase.h>
#include <Common\Base\Reflection\hkClassEnum.h>

namespace ckcmd {
	namespace HKX {
		class HkxItemEnum {
			const hkClassEnum* _enum_class;
			int _value;
		public:
			HkxItemEnum() = default;
			~HkxItemEnum() = default;
			HkxItemEnum(const HkxItemEnum&) = default;
			HkxItemEnum& operator=(const HkxItemEnum&) = default;

			HkxItemEnum(int value, const hkClassEnum* enum_class);
			
			int value();
			void setValue(int value);

			QString value_literal();
			QStringList enumValues();

			
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemEnum);