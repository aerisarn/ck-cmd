#pragma once

#include <QUndoCommand>
#include <QVariant>

namespace ckcmd {
	namespace HKX {

		class CommandManager {

			QUndoStack _stack;

		public:

			void pushCommand(QUndoCommand* command) {
				_stack.push(command);
			}

			QAction* createUndoAction(QObject* parent);
			QAction* createRedoAction(QObject* parent);
		};
	}
}