#pragma once

#include <QDialog>

#include "ui_VariableCreator.h"


class VariableCreator : public QDialog, private Ui::VariableCreator
{
public Q_SLOTS:
	virtual void accept() override;

public:

	enum class VariableType {
		type_invalid = -1,
		type_bool = 0,
		type_int8,
		type_int16,
		type_int32,
		type_real,
		type_pointer,
		type_vector3,
		type_vector4,
		type_quaternion
	};

	VariableCreator(QWidget* parent = 0);
	std::pair<QString, VariableType> selected() const { return _results; }

	static std::pair<QString, VariableType> getNewVariable(
		QWidget* parent,
		bool* ok);
private:
	std::pair<QString, VariableType> _results;

};