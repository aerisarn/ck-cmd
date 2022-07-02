#pragma once

#include <QWidget>
#include <src/models/ProjectModel.h>
#include <QAbstractItemDelegate>
#include <src/widgets/ModelWidgetSignalMapper.h>
#include <src/hkx/ResourceManager.h>

#include <map>

namespace ckcmd {

	class ModelWidget : public QWidget
	{
		Q_OBJECT
	private:
		void populate();
		void populate(int row_index, int column_index);
		void populate(int row_index, int column_index, QWidget* widget, const QByteArray& propertyName);

		void bind(QWidget* widget, size_t row_index, size_t column_index);
		void bind(QWidget* widget, const QByteArray& property_name, size_t row_index, size_t column_index);

		size_t memberModelRow(const QString& memberName);

		bool bindings_done = false;
		ModelWidgetSignalMapper _widget_signal_map;

	protected:
		HKX::ProjectModel& _model;
		QModelIndex _index;
		int _project_index;
		int _file_index;
		HKX::ResourceManager& _manager;
		QAbstractItemDelegate* _delegate;
	
		virtual void OnIndexSelected() = 0;

		virtual size_t dataBindingRowStart() const { return 1; }
		virtual size_t dataBindingColumnStart() const { return 1; }

		virtual bool doDataBinding() const { return !bindingTable().empty(); }

		//row_index, columns
		typedef std::pair<size_t, size_t> data_info;

		std::map<QString, data_info> _members;
		void buildReflectionTable();
		void doBindings();

		std::multimap<std::pair<size_t, size_t>, std::pair<QWidget*, QByteArray>> _bindings;

		typedef std::vector<std::tuple<QWidget*, QByteArray, QString, int>> StaticBindingTable;

		virtual const StaticBindingTable& bindingTable() const = 0;
		
		void bind(QWidget* widget, const QString& memberName);
		void bind(QWidget* widget, const QByteArray& property_name, const QString& memberName, int column_index);

		QVariant data(const char* memberName, int column);
		bool setData(const char* memberName, int column, const QVariant& value, int role);

	public:
		ModelWidget(HKX::ProjectModel& model, QWidget* parent = 0);
		virtual ~ModelWidget() {};

		void setIndex(const QModelIndex& index);

	private slots:
		void doPropertyChange(int row, int column, QVariant value);
	};
}