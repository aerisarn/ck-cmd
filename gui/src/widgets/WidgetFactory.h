#pragma once
#include <src/models/ModelEdge.h>

#include <QWidget>

namespace ckcmd
{
	namespace HKX {
		class WidgetFactory {
		public:
			static QWidget* getWidget(NodeType type);
		};
	}
}