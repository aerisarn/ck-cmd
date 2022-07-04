#pragma once

#include <QMetaType>
#include <QStringList>
#include <QFontMetrics>
#include <QWidget>
#include <QStyleOptionViewItem>
#include <Common\Base\hkBase.h>
#include <Common\Base\Reflection\hkClassEnum.h>

#include <map>
#include <vector>

namespace ckcmd {
	namespace HKX {

		class HkxItemReal {

			class HkxItemRealTemplateHolder
			{
				std::map<std::pair<size_t, size_t>, QWidget*> _templates;

				HkxItemRealTemplateHolder() {}
				~HkxItemRealTemplateHolder();

				// Delete copy/move so extra instances can't be created/moved.
				HkxItemRealTemplateHolder(const HkxItemRealTemplateHolder&) = delete;
				HkxItemRealTemplateHolder& operator=(const HkxItemRealTemplateHolder&) = delete;
				HkxItemRealTemplateHolder(HkxItemRealTemplateHolder&&) = delete;
				HkxItemRealTemplateHolder& operator=(HkxItemRealTemplateHolder&&) = delete;

			public:
				static HkxItemRealTemplateHolder& GetInstance() {
					static HkxItemRealTemplateHolder singleton;
					return singleton;
				}

				QWidget* getTemplate(size_t rows, size_t columns);

			};

			QWidget* fillTemplate() const;

			std::vector<std::vector<float>> _value;

		public:

			HkxItemReal() = default;
			~HkxItemReal() = default;
			HkxItemReal(const HkxItemReal&) = default;
			HkxItemReal& operator=(const HkxItemReal&) = default;

			HkxItemReal(const std::vector<std::vector<float>>& value);


			
			float value(size_t row, size_t column);
			void setValue(size_t row, size_t column, float value);

			void paint(QPainter* painter, const QStyleOptionViewItem& option) const;
			QSize WidgetSizeHint(const QFontMetrics& metrics) const;

			QWidget* CreateEditor(QWidget* parent) const;
			void FillEditor(QWidget* editor) const;
			void FillFromEditor(QWidget* editor);
		};
	}
}

Q_DECLARE_METATYPE(ckcmd::HKX::HkxItemReal);