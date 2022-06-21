#pragma once

#include <QDialog>

#include "ui_AnimationChooser.h"


class AnimationChooser : public QDialog, private Ui::AnimationChooser
{
	QStringList _results;

public Q_SLOTS:
	virtual void accept() override;

public:
	AnimationChooser(QAbstractItemModel* viewModel, QWidget* parent = 0);
	QStringList selected() const { return _results; }

	static QStringList getIndex(
		QAbstractItemModel* viewModel,
		QWidget* parent,
		bool* ok);

};