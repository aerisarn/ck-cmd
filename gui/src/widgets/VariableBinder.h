#pragma once

#include <src/widgets/ModelDialog.h>

#include <ui_VariableBinder.h>

class VariableBinder : public ckcmd::ModelDialog, private Ui::VariableBinder
{
	size_t _variable_index;
	QString _binding_path;

	std::map<int, QStringList> _variables_by_type;
	std::map<int, std::vector<int>> _variables_by_type_indices;
	std::map<QString, int> _binding_path_types;

public Q_SLOTS:
	virtual void accept() override;
public:
	explicit VariableBinder(ckcmd::HKX::ProjectModel& model, const QModelIndex& index, QWidget* parent = 0);


	std::pair<size_t, QString> selection() const { return { _variable_index, _binding_path }; }

	static std::pair<size_t, QString> getResult(
		ckcmd::HKX::ProjectModel& model,
		const QModelIndex& index,
		QWidget* parent,
		bool* ok);
};