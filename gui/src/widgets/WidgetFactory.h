#pragma once
#include <src/models/ProjectTreeModel.h>

#include <type_traits>

#include <QWidget>
#include <QStackedLayout>

namespace ckcmd
{

	enum class widgetType
	{
		invalid = 0,
		CharacterEditor,
		SampleWidget,
		allEditors
	};

	template <typename T>
	constexpr auto operator*(T e) noexcept
		-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
	{
		return static_cast<std::underlying_type_t<T>>(e);
	}

	class WidgetFactory {
	public:
		static QWidget* getWidget(widgetType type, HKX::ProjectModel& model, QWidget* parent = nullptr);
		static void loadAddWidgets(QStackedLayout* stack, HKX::ProjectModel& model, QWidget* parent = nullptr);
	};

}