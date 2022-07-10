#include <src/widgets/VariableBinder.h>

#include <src/items/HkxItemVar.h>
#include <src/items/HkxItemReal.h>
#include <src/hkx/HkxTypeInfo.h>
#include <hkbVariableInfo_1.h>

using namespace ckcmd;
using namespace ckcmd::HKX;

/*
case hkbVariableInfo::VARIABLE_TYPE_BOOL:
	return 2;
case hkbVariableInfo::VARIABLE_TYPE_INT8:
	return 2;
case hkbVariableInfo::VARIABLE_TYPE_INT16:
	return 2;
case hkbVariableInfo::VARIABLE_TYPE_INT32:
	return 2;
case hkbVariableInfo::VARIABLE_TYPE_REAL:
	return 2;
case hkbVariableInfo::VARIABLE_TYPE_POINTER:
	return 2;
case hkbVariableInfo::VARIABLE_TYPE_VECTOR3:
	return 4;
case hkbVariableInfo::VARIABLE_TYPE_VECTOR4:
	return 5;
case hkbVariableInfo::VARIABLE_TYPE_QUATERNION:
	return 5;

*/

int QVariantTypeToVariantType(const TypeInfo& value)
{
	switch (value._type)
	{
	case hkClassMember::Type::TYPE_VOID:
		break;
	case hkClassMember::Type::TYPE_BOOL:
		return hkbVariableInfo::VARIABLE_TYPE_BOOL;
	case hkClassMember::Type::TYPE_CHAR:
	case hkClassMember::Type::TYPE_INT8:
	case hkClassMember::Type::TYPE_UINT8:
		return hkbVariableInfo::VARIABLE_TYPE_INT8;
	case hkClassMember::Type::TYPE_INT16:
	case hkClassMember::Type::TYPE_UINT16:
		return hkbVariableInfo::VARIABLE_TYPE_INT16;
	case hkClassMember::Type::TYPE_INT32:
	case hkClassMember::Type::TYPE_UINT32:
		return hkbVariableInfo::VARIABLE_TYPE_INT32;
	case hkClassMember::Type::TYPE_INT64:
	case hkClassMember::Type::TYPE_UINT64:
		return -1;
	case hkClassMember::Type::TYPE_REAL:
		return hkbVariableInfo::VARIABLE_TYPE_REAL;
	case hkClassMember::Type::TYPE_VECTOR4:
		return hkbVariableInfo::VARIABLE_TYPE_VECTOR4;
	case hkClassMember::Type::TYPE_QUATERNION:
		return hkbVariableInfo::VARIABLE_TYPE_QUATERNION;
	case hkClassMember::Type::TYPE_MATRIX3:
	case hkClassMember::Type::TYPE_ROTATION:
	case hkClassMember::Type::TYPE_QSTRANSFORM:
	case hkClassMember::Type::TYPE_MATRIX4:
	case hkClassMember::Type::TYPE_TRANSFORM:
	case hkClassMember::Type::TYPE_ZERO:
	case hkClassMember::Type::TYPE_POINTER:
	case hkClassMember::Type::TYPE_FUNCTIONPOINTER:
	case hkClassMember::Type::TYPE_ARRAY:
	case hkClassMember::Type::TYPE_INPLACEARRAY:
	case hkClassMember::Type::TYPE_ENUM:
		return -1;
	case hkClassMember::Type::TYPE_STRUCT:
		return hkbVariableInfo::VARIABLE_TYPE_POINTER;
	case hkClassMember::Type::TYPE_SIMPLEARRAY:
	case hkClassMember::Type::TYPE_HOMOGENEOUSARRAY:
	case hkClassMember::Type::TYPE_VARIANT:
	case hkClassMember::Type::TYPE_CSTRING:
	case hkClassMember::Type::TYPE_ULONG:
	case hkClassMember::Type::TYPE_FLAGS:
	case hkClassMember::Type::TYPE_HALF:
	case hkClassMember::Type::TYPE_STRINGPTR:
	case hkClassMember::Type::TYPE_RELARRAY:
	default:
		break;
	}
	return -1;
}


VariableBinder::VariableBinder(ProjectModel& model, const QModelIndex& index, QWidget* parent) : 
	ModelDialog(model, index, parent)
{
	_index = index;
	auto variables_index = _model.variablesIndex(index);
	auto variables_count = _model.rowCount(variables_index);
	for (int r = 0; r < variables_count; r++)
	{
		auto variable_index = _model.index(r, 0, variables_index);
		auto variable_type = _model.index(VARIABLE_TYPE_INDEX, 1, variable_index).data().toInt();
		_variables_by_type[variable_type] << variable_index.data().toString();
		_variables_by_type_indices[variable_type].push_back(r);
	}
	auto start = _model.dataStart(index);
	auto class_rows = _model.rowCount(index);
	QStringList bindables;
	for (int r = start.first; r < class_rows; r++)
	{
		auto row_index = _model.index(r, 0, index);
		auto row_name = row_index.data().toString();
		if (row_name.startsWith("bindings"))
			continue;
		if (row_name.startsWith("variableBindingSet"))
			continue;
		row_name.replace(".", "/");
		int dot_index = row_name.indexOf("/");
		auto type = _model.rowType(row_index);
		auto binding_type = QVariantTypeToVariantType(type);
		if (-1 == binding_type)
			continue;
		if (dot_index != -1)
		{
			bool isRowArray = _model.isArray(row_index);
			if (isRowArray)
			{
				int elements = _model.columnCount(row_index) - start.second;
				for (int e = 0; e < elements; e++)
				{
					QString array_row_name = row_name;
					array_row_name.insert(dot_index, QString(":%1").arg(e));
					_binding_path_types[array_row_name] = binding_type;
					bindables << array_row_name;
				}
			}
			else {
				bindables << row_name;
				_binding_path_types[row_name] = binding_type;
			}
		}
		else {
			bindables << row_name;
			_binding_path_types[row_name] = binding_type;
		}
	}
}

void VariableBinder::accept()
{
	//if (attackEventsListView->currentIndex().isValid() &&
	//	clipListView->currentIndex().isValid())
	//{
	//	_choosen_clip.clear();
	//	_choosen_attack_event = attackEventsListView->currentIndex().data().toString();
	//	auto& clipIndexList = clipListView->selectionModel()->selectedIndexes();
	//	for (auto& index : clipIndexList)
	//		_choosen_clip << index.data().toString();
	//}
	QDialog::accept();
}

std::pair<size_t, QString> VariableBinder::getResult(
	ProjectModel& model,
	const QModelIndex& index,
	QWidget* parent,
	bool* ok)
{
	std::unique_ptr<VariableBinder> dialog = std::make_unique<VariableBinder>(model, index, parent);
	const int ret = dialog->exec();
	if (ok)
		*ok = !!ret;
	if (ret) {
		return dialog->selection();
	}
	else {
		return {};
	}
}