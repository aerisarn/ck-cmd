#pragma once

#include <QMetaType>
#include <QStringList>
#include <QFontMetrics>
#include <QWidget>
#include <QStyleOptionViewItem>
#include <Common\Base\hkBase.h>
#include <Common\Base\Reflection\hkClassEnum.h>

namespace ckcmd {
	namespace HKX {
		class HkxItemFlags {
			const hkClassEnum* _enum_class;
			int _value;
			size_t _storage;

			struct Templates {
				bool initialized = false;
				QWidget* eight_bit_widget;
				QWidget* sixteen_bit_widget;
				QWidget* thirtytwo_bit_widget;

				void InitTemplates();
			};

			static Templates _templates;

			QWidget* getTemplate() const;
			QString flag_literal(size_t bit_index) const;
			void fillTemplate() const ;

		public:
			HkxItemFlags() = default;
			~HkxItemFlags() = default;
			HkxItemFlags(const HkxItemFlags&) = default;
			HkxItemFlags& operator=(const HkxItemFlags&) = default;

			HkxItemFlags(int value, const hkClassEnum* enum_class, size_t storage);
			
			int value();
			void setValue(int value);
			void setFlag(size_t flag_index, bool value);
			
			QString value_literal();

			QStringList enumValues();

			void paint(QPainter* painter, const QStyleOptionViewItem& option) const;
			QSize WidgetSizeHint(const QFontMetrics& metrics) const;

			QWidget* CreateEditor(QWidget* parent);
			void FillEditor(QWidget* editor);
			void FillFromEditor(QWidget* editor);
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemFlags);