#include <src/hkx/CommandManager.h>
#include <QKeySequence>
#include <QAction>

using namespace ckcmd::HKX;

QAction* CommandManager::createUndoAction(QObject* parent) 
{ 
	auto action = _stack.createUndoAction(parent, QObject::tr("&Undo"));
	action->setShortcuts(QKeySequence::Undo);
	return action;
}

QAction* CommandManager::createRedoAction(QObject* parent)
{
	auto action = _stack.createRedoAction(parent, QObject::tr("&Redo"));
	action->setShortcuts(QKeySequence::Redo);
	return action;
}

