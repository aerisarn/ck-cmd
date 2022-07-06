#pragma once

#include <src/widgets/ModelDialog.h>

#include <QSortFilterProxyModel>

#include "ui_SelectNode.h"

class SelectNode : public ckcmd::ModelDialog, private Ui::SelectNode
{
	const hkClass* _class = nullptr;
	hkVariant* _variant = nullptr;

	ckcmd::HKX::ResourceManager& _manager;
	int file_index = -1;

	ckcmd::HKX::hk_object_list_t _compatible_nodes;
	std::vector<const hkClass*> _compatible_classes;

	QSortFilterProxyModel* _compatible_nodes_proxyModel;
	QSortFilterProxyModel* _compatible_classes_proxyModel;

private slots:

	void createNewTreeViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous);
	void referenceExistingTreeViewCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

public:
	SelectNode(ckcmd::HKX::ProjectModel& model, const QModelIndex& index, QWidget* parent = 0);

	std::pair<std::pair<const hkClass*, QString>, hkVariant*> selected() { return { {_class, newNameLineEdit->text()}, _variant}; }

	static std::pair<std::pair<const hkClass*, QString>, hkVariant*> getNode(
		ckcmd::HKX::ProjectModel& model,
		const QModelIndex& index,
		QWidget* parent,
		bool* ok);
};