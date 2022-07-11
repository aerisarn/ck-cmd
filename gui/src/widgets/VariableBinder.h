#pragma once

#include <src/widgets/ModelDialog.h>

#include <src/models/StringListModel.h>

#include <QSortFilterProxyModel>

#include <ui_VariableBinder.h>

class VariableBinder : public ckcmd::ModelDialog, private Ui::VariableBinder
{
	int _variable_index;
	QString _binding_path;

	std::map<int, QStringList> _variables_by_type;
	std::map<int, std::vector<int>> _variables_by_type_indices;
	std::map<QString, int> _binding_path_types;

	void buildVariablesByTypeList();
	void buildBindablesList();

	StringListModel* _variablesModel;
	QSortFilterProxyModel* _variablesProxyModel;

private slots:
	void on_fieldTableView_selectionChanged(const QModelIndex& current, const QModelIndex& previous);
	void on_variableTableView_selectionChanged(const QModelIndex& current, const QModelIndex& previous);

public Q_SLOTS:
	virtual void accept() override;
public:
	explicit VariableBinder(ckcmd::HKX::ProjectModel& model, const QModelIndex& index, QWidget* parent = 0);


	std::pair<int, QString> selection() const { return { _variable_index, _binding_path }; }

	static std::pair<int, QString> getResult(
		ckcmd::HKX::ProjectModel& model,
		const QModelIndex& index,
		QWidget* parent,
		bool* ok);
};