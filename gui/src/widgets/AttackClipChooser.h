#pragma once

#include <QDialog>

#include "ui_AttackClipChooser.h"


class AttackClipChooser : public QDialog, private Ui::AttackClipChooser
{
	QString _choosen_attack_event;
	QStringList _choosen_clip;

public Q_SLOTS:
	virtual void accept() override;

public:
	AttackClipChooser(QAbstractItemModel* attackEventModel, QAbstractItemModel* clipModel, QWidget* parent = 0);
	std::pair<QString, QStringList> selection() const { return { _choosen_attack_event, _choosen_clip }; }

	static std::pair<QString, QStringList> getResult(
		QAbstractItemModel* attackEventModel,
		QAbstractItemModel* clipModel,
		QWidget* parent,
		bool* ok);

};