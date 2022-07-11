#include "HkTopNavigatorWidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QCompleter>
#include <QComboBox>
#include <QStringListModel>

#include <src/items/HkxItemEnum.h>
#include <src/items/HkxItemVar.h>
#include <src/items/HkxItemPointer.h>

#include <src/widgets/VariableBinder.h>

#include <Common/Base/hkBase.h>
#include <hkbNode_1.h>

using namespace ckcmd::HKX;

TopInfoWidget::TopInfoWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) : ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    bindingsTableWidget->setColumnCount(3);
    bindingsTableWidget->setHorizontalHeaderLabels(QStringList() << "Type" << "Variable" << "Binded Member");
    removeBindingToolButton->setEnabled(false);
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
}

QSize TopInfoWidget::sizeHint() const
{
    return QSize(472, 30);
}

//bindings.bindingType
//bindings.bitIndex
//bindings.memberPath
//bindings.variableIndex

//void clearLayout(QLayout* layout)
//{
//    QLayoutItem* child;
//    while ((child = layout->takeAt(0)) != 0) {
//        if (child->layout() != 0)
//            clearLayout(child->layout());
//        else if (child->widget() != 0)
//            delete child->widget();
//
//        delete child;
//    }
//}

void TopInfoWidget::refreshBindings()
{
    bindingsTableWidget->clear();
    bindingsTableWidget->setHorizontalHeaderLabels(QStringList() << "Type" << "Variable" << "Binded Member");
    if (_members.find("bindings.variableIndex") != _members.end())
    {
        bindingsTableWidget->setVisible(true);
        auto bindings = _members.at("bindings.variableIndex");
        auto bindings_path = _members.at("bindings.memberPath");
        auto bindings_type = _members.at("bindings.bindingType");
        if (bindings.second > 1)
        {
            removeBindingToolButton->setEnabled(true);
            int bindings_rows = bindings.second - 1;
            auto variables_model = _model.editModel(_index, AssetType::variables);
            auto bindable_rows = _model.rowNames(_index, "bindings");
            auto bindable_model = new QStringListModel(bindable_rows);
            auto temp_type_index = _model.index(bindings_type.first, 1, _index);
            auto available_types = temp_type_index.data().value<HkxItemEnum>().enumValues();
            auto type_model = new QStringListModel(available_types);

            bindingsTableWidget->setRowCount(bindings_rows);

            for (int i = 0; i < bindings_rows; i++)
            {

                auto index = _model.index(bindings.first, i + 1, _index);
                auto path_index = _model.index(bindings_path.first, i + 1, _index);
                auto type_index = _model.index(bindings_type.first, i + 1, _index);

                auto type = tr("%1").arg(available_types.at(type_index.data().value<HkxItemEnum>().value()));

                QTableWidgetItem* newType = new QTableWidgetItem(type);

                bindingsTableWidget->setItem(i, 0, newType);

                auto index_data = index.data().value<HkxItemVar>().index();
                auto variable = variables_model->data(variables_model->index(index_data, 0)).toString();
                QTableWidgetItem* newVar = new QTableWidgetItem(tr("%1").arg(variable));

                bindingsTableWidget->setItem(i, 1, newVar);

                auto path = tr("%1").arg(path_index.data().toString());

                QTableWidgetItem* newMember = new QTableWidgetItem(path);

                bindingsTableWidget->setItem(i, 2, newMember);
            }
        }
        else {
            bindingsTableWidget->clearContents();
            bindingsTableWidget->setVisible(false);
            removeBindingToolButton->setEnabled(false);
        }
    }
    else {
        bindingsTableWidget->clearContents();
        bindingsTableWidget->setVisible(false);
        removeBindingToolButton->setEnabled(false);
    }
}

void TopInfoWidget::on_addBindingToolButton_clicked()
{
    bool ok = false;
    auto result = VariableBinder::getResult(_model, _index, nullptr, &ok);
    if (ok && result.first != -1 && !result.second.isEmpty())
    {
        auto binding_set = _members.at("variableBindingSet");
        auto binding_set_row_index = _model.index(binding_set.first, 0, _index);
        if (binding_set_row_index.data().value<HkxItemPointer>().get() == nullptr)
        {
            auto set = _model.getResourceManager().createObject<hkbVariableBindingSet>(_model.getFileIndex(binding_set_row_index), &hkbVariableBindingSetClass);
            HkxItemPointer ptr(set); QVariant value; value.setValue(ptr);
            auto binding_set_data_index = _model.index(binding_set.first, 1, _index);
            bool new_set_data = _model.setData(binding_set_data_index, value);
            buildReflectionTable(true);
        }

        auto bindings = _members.at("bindings.variableIndex");
        auto bindings_path = _members.at("bindings.memberPath");
        auto bindings_type = _members.at("bindings.bindingType");

        auto row_index = _model.index(bindings.first, 0, _index);
        auto current_columns = _model.columnCount(row_index);
        bool insert = _model.insertColumns(bindings.first, current_columns, 1, _index);

        auto variable_index = _model.index(bindings.first, current_columns, _index);
        auto path_index = _model.index(bindings_path.first, current_columns, _index);
        auto type_index = _model.index(bindings_type.first, current_columns, _index);

        bool set_variable_index = _model.setData(variable_index, result.first);
        bool set_path_index = _model.setData(path_index, result.second);
        bool set_type_index = _model.setData(type_index, 0);

        buildReflectionTable(true);
        refreshBindings();
    }
}

void TopInfoWidget::on_removeBindingToolButton_clicked()
{
    auto bindings = _members.at("bindings.variableIndex");
    auto row_index = _model.index(bindings.first, 0, _index);
    auto current_columns = _model.columnCount(row_index);
    if (current_columns > 0)
    {
        int selected_row = current_columns - 1;
        if (bindingsTableWidget->currentIndex().isValid())
        {
            selected_row = bindingsTableWidget->currentIndex().row();
        }
        _model.removeColumns(bindings.first, selected_row, 1, _index);
        buildReflectionTable(true);
        refreshBindings();
    }
}

void TopInfoWidget::OnIndexSelected()
{
    if (_model.isVariant(_index))
    {
        buildReflectionTable(true);
        nameLineEdit->setText(_model.data(_index, Qt::DisplayRole).toString());
        refreshBindings();
    }
}

void TopInfoWidget::on_nameLineEdit_textChanged(const QString& text)
{
    _model.setData(_index, text, Qt::EditRole);
}

//void TopInfoWidget::resizeNameToContent(const QString& text)
//{
//    QFontMetrics fm(nameLineEdit->font());
//    int pixelsWide = fm.width(text) + 20;
//    nameLineEdit->setFixedWidth(pixelsWide);
//    adjustSize();
//}