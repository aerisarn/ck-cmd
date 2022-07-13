#pragma once

#include <src/widgets/ModelDialog.h>

#include <QSortFilterProxyModel>

#include "ui_TransitionCreator.h"

struct NewTransition {
	int fromStateId;
	int toStateId;
	int eventIndex;
};

class TransitionCreator : public ckcmd::ModelDialog, private Ui::TransitionCreator
{
	NewTransition _result;
	QSortFilterProxyModel* event_proxy_model;

public Q_SLOTS:
	virtual void accept() override;

public:
	TransitionCreator(ckcmd::HKX::ProjectModel& model, QModelIndex index, QWidget* parent = 0);
	NewTransition selected() const { return _result; }

	static NewTransition getTransition(
		ckcmd::HKX::ProjectModel& model, QModelIndex index,
		QWidget* parent,
		bool* ok);

};