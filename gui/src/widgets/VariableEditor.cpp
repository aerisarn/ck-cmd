#include "VariableEditor.h"

#include <src/items/HkxItemReal.h>
#include <src/items/HkxItemPointer.h>

#include <QSpinBox>
#include <QDoubleSpinBox>

#include <hkbVariableInfo_1.h>
#include <hkbRoleAttribute_0.h>

using namespace ckcmd::HKX;

VariableEditor::VariableEditor(ckcmd::HKX::ProjectModel& model, QWidget* parent)
	: _editor(nullptr),
	ModelWidget(model, parent)
{
	setupUi(this);
}

void VariableEditor::on_ragdollCheckBox_stateChanged(int state)
{
	auto index = _model.index(2, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbRoleAttribute::FLAG_RAGDOLL;
	else
		value &= ~hkbRoleAttribute::FLAG_RAGDOLL;
	_model.setData(index, value);
}

void VariableEditor::on_normalizedCheckBox_stateChanged(int state)
{
	auto index = _model.index(2, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbRoleAttribute::FLAG_NORMALIZED;
	else
		value &= ~hkbRoleAttribute::FLAG_NORMALIZED;
	_model.setData(index, value);
}

void VariableEditor::on_notVariableCheckBox_stateChanged(int state)
{
	auto index = _model.index(2, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbRoleAttribute::FLAG_NOT_VARIABLE;
	else
		value &= ~hkbRoleAttribute::FLAG_NOT_VARIABLE;
	_model.setData(index, value);
}

void VariableEditor::on_hiddenCheckBox_stateChanged(int state)
{
	auto index = _model.index(2, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbRoleAttribute::FLAG_HIDDEN;
	else
		value &= ~hkbRoleAttribute::FLAG_HIDDEN;
	_model.setData(index, value);
}

void VariableEditor::on_outputCheckBox_stateChanged(int state)
{
	auto index = _model.index(2, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbRoleAttribute::FLAG_OUTPUT;
	else
		value &= ~hkbRoleAttribute::FLAG_OUTPUT;
	_model.setData(index, value);
}

void VariableEditor::on_roleComboBox_currentIndexChanged(int index)
{
	auto model_index = _model.index(1, 1, _index);
	_model.setData(model_index, index);
}

void VariableEditor::on_notCharacterPropertyCheckBox_stateChanged(int state)
{
	auto index = _model.index(2, 1, _index);
	auto value = _model.data(index).value<int>();
	if (state == Qt::Checked)
		value |= hkbRoleAttribute::FLAG_NOT_CHARACTER_PROPERTY;
	else
		value &= ~hkbRoleAttribute::FLAG_NOT_CHARACTER_PROPERTY;
	_model.setData(index, value);
}

const char* typeString(int type)
{
	switch (type)
	{
	case hkbVariableInfo::VARIABLE_TYPE_BOOL:
		return "BOOL";
	case hkbVariableInfo::VARIABLE_TYPE_INT8:
		return "INT8";
	case hkbVariableInfo::VARIABLE_TYPE_INT16:
		return "INT16";
	case hkbVariableInfo::VARIABLE_TYPE_INT32:
		return "INT32";
	case hkbVariableInfo::VARIABLE_TYPE_REAL:
		return "REAL";
	case hkbVariableInfo::VARIABLE_TYPE_POINTER:
		return "REFERENCE";
	case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
		return "VECTOR3";
	case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
		return "VECTOR4";
	case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
		return "QUATERNION";
	default:
		break;
	}
	return "INVALID";
}

QWidget* VariableEditor::defaultValueEditor(int type, const QVariant& value)
{
	switch (type)
	{
	case hkbVariableInfo::VARIABLE_TYPE_BOOL:
	{
		QComboBox* trueFalseEditor = new QComboBox(this);
		trueFalseEditor->addItems({ "False", "True" });
		trueFalseEditor->setCurrentIndex(value.toInt() > 0 ? 1 : 0);
		connect(trueFalseEditor, qOverload<int>(&QComboBox::currentIndexChanged), [trueFalseEditor, this](int index)
		{
			_model.setData(_model.index(4, 1, _index), index);
		});
		return trueFalseEditor;
	}
	case hkbVariableInfo::VARIABLE_TYPE_INT8:
	case hkbVariableInfo::VARIABLE_TYPE_INT16:
	case hkbVariableInfo::VARIABLE_TYPE_INT32:
	{
		QSpinBox* valueEditor = new QSpinBox(this);
		valueEditor->setValue(value.toInt());
		connect(valueEditor, qOverload<int>(&QSpinBox::valueChanged), [valueEditor, this](int value)
			{
				_model.setData(_model.index(4, 1, _index), value);
			});
		return valueEditor;
	}
	case hkbVariableInfo::VARIABLE_TYPE_REAL:
	{
		QDoubleSpinBox* valueEditor = new QDoubleSpinBox(this);
		valueEditor->setValue(value.value<float>());
		connect(valueEditor, qOverload<double>(&QDoubleSpinBox::valueChanged), [valueEditor, this](double value)
			{
				_model.setData(_model.index(4, 1, _index), (float)value);
			});
		return valueEditor;
	}
	case hkbVariableInfo::VARIABLE_TYPE_POINTER:
	{
		QLineEdit* ptr = new QLineEdit(this);
		ptr->setText(value.toString());
		return ptr;
	}
	case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
	{
		HkxItemReal real_value = value.value<HkxItemReal>();

		QWidget* container = new QWidget(this);
		QHBoxLayout* layout = new QHBoxLayout(container);

		QLabel* x_label = new QLabel(container);
		x_label->setText("X:");
		QDoubleSpinBox* x_field = new QDoubleSpinBox(container);
		x_field->setValue(real_value.value(0, 0));
		layout->addWidget(x_label);
		layout->addWidget(x_field);

		QLabel* y_label = new QLabel(container);
		y_label->setText("Y:");
		QDoubleSpinBox* y_field = new QDoubleSpinBox(container);
		y_field->setValue(real_value.value(0, 1));
		layout->addWidget(y_label);
		layout->addWidget(y_field);

		QLabel* z_label = new QLabel(container);
		z_label->setText("Z:");
		QDoubleSpinBox* z_field = new QDoubleSpinBox(container);
		z_field->setValue(real_value.value(0, 2));
		layout->addWidget(z_label);
		layout->addWidget(z_field);

		connect(x_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [y_field, z_field, this](double value)
			{
				HkxItemReal val({ {(float)value, (float)y_field->value(), (float)z_field->value(), (float)0. } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		connect(y_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [x_field, z_field, this](double value)
			{
				HkxItemReal val({ {(float)x_field->value(), (float)value, (float)z_field->value(), (float)0. } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		connect(z_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [x_field, y_field, this](double value)
			{
				HkxItemReal val({ {(float)x_field->value(), (float)y_field->value(), (float)value, (float)0. } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		return container;
	}
	case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
	case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
	{
		HkxItemReal real_value = value.value<HkxItemReal>();

		QWidget* container = new QWidget(this);
		QHBoxLayout* layout = new QHBoxLayout(container);

		QLabel* x_label = new QLabel(container);
		x_label->setText("X:");
		QDoubleSpinBox* x_field = new QDoubleSpinBox(container);
		x_field->setValue(real_value.value(0, 0));
		layout->addWidget(x_label);
		layout->addWidget(x_field);

		QLabel* y_label = new QLabel(container);
		y_label->setText("Y:");
		QDoubleSpinBox* y_field = new QDoubleSpinBox(container);
		y_field->setValue(real_value.value(0, 1));
		layout->addWidget(y_label);
		layout->addWidget(y_field);

		QLabel* z_label = new QLabel(container);
		z_label->setText("Z:");
		QDoubleSpinBox* z_field = new QDoubleSpinBox(container);
		z_field->setValue(real_value.value(0, 2));
		layout->addWidget(z_label);
		layout->addWidget(z_field);

		QLabel* w_label = new QLabel(container);
		w_label->setText("W:");
		QDoubleSpinBox* w_field = new QDoubleSpinBox(container);
		w_field->setValue(real_value.value(0, 3));
		layout->addWidget(w_label);
		layout->addWidget(w_field);

		connect(x_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [y_field, z_field, w_field, this](double value)
			{
				HkxItemReal val({ {(float)value, (float)y_field->value(), (float)z_field->value(), (float)w_field->value() } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		connect(y_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [x_field, z_field, w_field, this](double value)
			{
				HkxItemReal val({ {(float)x_field->value(), (float)value, (float)z_field->value(), (float)w_field->value() } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		connect(z_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [x_field, y_field, w_field, this](double value)
			{
				HkxItemReal val({ {(float)x_field->value(), (float)y_field->value(), (float)value, (float)w_field->value() } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		connect(w_field, qOverload<double>(&QDoubleSpinBox::valueChanged), [x_field, y_field, z_field, this](double value)
			{
				HkxItemReal val({ {(float)x_field->value(), (float)y_field->value(), (float)z_field->value(), (float)value } });
				QVariant set_value; set_value.setValue(val);
				_model.setData(_model.index(4, 1, _index), set_value);
			});

		return container;
	}
	}
	return nullptr;
}


void VariableEditor::OnIndexSelected()
{
	auto role = _model.data(_model.index(1, 1, _index)).toInt();
	auto flags = _model.data(_model.index(2, 1, _index)).toInt();
	auto type = _model.data(_model.index(3, 1, _index)).toInt();

	typeLineEdit->setText(typeString(type));
	roleComboBox->setCurrentIndex(role);

	if (flags & hkbRoleAttribute::FLAG_RAGDOLL)
		ragdollCheckBox->setChecked(true);
	else
		ragdollCheckBox->setChecked(false);

	if (flags & hkbRoleAttribute::FLAG_NORMALIZED)
		normalizedCheckBox->setChecked(true);
	else
		normalizedCheckBox->setChecked(false);

	if (flags & hkbRoleAttribute::FLAG_NOT_VARIABLE)
		notVariableCheckBox->setChecked(true);
	else
		notVariableCheckBox->setChecked(false);

	if (flags & hkbRoleAttribute::FLAG_HIDDEN)
		hiddenCheckBox->setChecked(true);
	else
		hiddenCheckBox->setChecked(false);

	if (flags & hkbRoleAttribute::FLAG_OUTPUT)
		outputCheckBox->setChecked(true);
	else
		outputCheckBox->setChecked(false);

	if (flags & hkbRoleAttribute::FLAG_NOT_CHARACTER_PROPERTY)
		notCharacterPropertyCheckBox->setChecked(true);
	else
		notCharacterPropertyCheckBox->setChecked(false);
	
	if (nullptr != _editor)
	{
		verticalLayout_2->removeWidget(_editor);
		delete _editor;
	}
	_editor = defaultValueEditor(type, _model.data(_model.index(4, 1, _index)));
	verticalLayout_2->addWidget(_editor);
}