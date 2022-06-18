#pragma once

#include <QWidget>
#include <src/models/ProjectModel.h>
#include <QAbstractItemDelegate>

#include <map>

namespace ckcmd {

	class ModelWidget : public QWidget
	{
		Q_OBJECT
	private:
		void populate();
		void populate(int index);
		void populate(int index, QWidget* widget, const QByteArray& propertyName);

		void bind(QWidget* widget, size_t row_index);
		void bind(QWidget* widget, const QByteArray& property_name, size_t row_index);

		size_t memberModelRow(const QString& memberName);

		bool bindings_done = false;

	protected:
		HKX::ProjectModel& _model;
		QModelIndex _index;
		QAbstractItemDelegate* _delegate;
	
		virtual void OnIndexSelected() = 0;

		virtual size_t dataBindingRowStart() const { return 0; }
		virtual size_t dataBindingColumnStart() const { return 0; }

		virtual bool doDataBinding() const { return !bindingTable().empty(); }

		//row_index, columns
		typedef std::pair<size_t, size_t> data_info;

		std::map<QString, data_info> _members;
		void buildReflectionTable();
		void doBindings();

		std::multimap<size_t, std::pair<QWidget*, QByteArray>> _bindings;

		typedef std::vector<std::tuple<QWidget*, QByteArray, QString>> StaticBindingTable;

		virtual const StaticBindingTable& bindingTable() const = 0;
		
		void bind(QWidget* widget, const QString& memberName);
		void bind(QWidget* widget, const QByteArray& property_name, const QString& memberName);

	public:
		ModelWidget(HKX::ProjectModel& model, QWidget* parent = 0);

		void setIndex(const QModelIndex& index);
	};
}