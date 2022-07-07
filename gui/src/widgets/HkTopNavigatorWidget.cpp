#include "HkTopNavigatorWidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QCompleter>
#include <QComboBox>
#include <QStringListModel>

#include <src/items/HkxItemEnum.h>

#include <Common/Base/hkBase.h>
#include <hkbNode_1.h>

using namespace ckcmd::HKX;

TopInfoWidget::TopInfoWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) : ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    bindingsTableWidget->setColumnCount(3);
    bindingsTableWidget->setHorizontalHeaderLabels(QStringList() << "Type" << "Variable" << "Binded Member");
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

void clearLayout(QLayout* layout)
{
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != 0) {
        if (child->layout() != 0)
            clearLayout(child->layout());
        else if (child->widget() != 0)
            delete child->widget();

        delete child;
    }
}

void TopInfoWidget::OnIndexSelected()
{
    if (_model.isVariant(_index))
    {
        buildReflectionTable(true);
        nameLineEdit->setText(_model.data(_index, Qt::DisplayRole).toString());
        //clearLayout(bindingsVerticalLayout);
        if (_members.find("bindings.variableIndex") != _members.end())
        {
            bindingsTableWidget->setVisible(true);
            auto bindings = _members.at("bindings.variableIndex");
            auto bindings_path = _members.at("bindings.memberPath");
            auto bindings_type = _members.at("bindings.bindingType");
            if (bindings.second > 1)
            {
                int bindings_rows = bindings.second - 1;
                auto variables_model = _model.editModel(_index, AssetType::variables);
                auto bindable_rows = _model.rowNames(_index, "bindings");
                auto bindable_model = new QStringListModel(bindable_rows);
                auto temp_type_index = _model.index(bindings_type.first, 1, _index);
                auto available_types = temp_type_index.data().value<HkxItemEnum>().enumValues(;
                auto type_model = new QStringListModel(available_types);

                bindingsTableWidget->setRowCount(bindings_rows);

                for (int i = 0; i < bindings_rows; i++)
                {


                    auto index = _model.index(bindings.first, i + 1, _index);
                    auto path_index = _model.index(bindings_path.first, i + 1, _index);
                    auto type_index = _model.index(bindings_type.first, i + 1, _index);

                    QTableWidgetItem* newType = new QTableWidgetItem(tr("%1").arg(available_types.at(type_index.data().value<HkxItemEnum>().value())));

                    bindingsTableWidget->setItem(i, 0, newType);
                   
                    QString variable = variables_model->data(variables_model->index(index.data().toInt(), 0)).toString();
                    QTableWidgetItem* newVar = new QTableWidgetItem(tr("%1").arg(variable));

                    bindingsTableWidget->setItem(i, 1, newVar);

                    QTableWidgetItem* newMember = new QTableWidgetItem(tr("%1").arg(path_index.data().toString()));

                    bindingsTableWidget->setItem(i, 2, newMember);



                    //int binded_member = bindable_rows.indexOf(path_index.data().toString());

                    //QCompleter* path_completer = new QCompleter();
                    //path_completer->setModel(bindable_model);
                    //path_completer->setCompletionMode(QCompleter::PopupCompletion);
                    //QComboBox* path_binding_box = new QComboBox();
                    //path_binding_box->blockSignals(true);
                    //path_binding_box->setModel(bindable_model);
                    //path_binding_box->setEditable(true);
                    //path_binding_box->setInsertPolicy(QComboBox::NoInsert);
                    //path_binding_box->setCurrentIndex(binded_member);
                    //path_binding_box->blockSignals(false);

                    //bindingsVerticalLayout->addWidget(path_binding_box, i, 0);

                    //QCompleter* _variable_completer = new QCompleter();
                    //_variable_completer->setModel(variables_model);
                    //_variable_completer->setCompletionMode(QCompleter::PopupCompletion);
                    //QComboBox* variable_binding_box = new QComboBox();
                    //variable_binding_box->blockSignals(true);
                    //variable_binding_box->setModel(variables_model);
                    //variable_binding_box->setEditable(true);
                    //variable_binding_box->setInsertPolicy(QComboBox::NoInsert);
                    //int variable_index = index.data().toInt();
                    //variable_binding_box->setCurrentIndex(variable_index);
                    //variable_binding_box->blockSignals(false);

                    //bindingsVerticalLayout->addWidget(variable_binding_box, i, 1);

                    //auto type = type_index.data().value<HkxItemEnum>();
                    //QCompleter* type_completer = new QCompleter();
                    //type_completer->setModel(type_model);
                    //type_completer->setCompletionMode(QCompleter::PopupCompletion);
                    //QComboBox* type_binding_box = new QComboBox();
                    //type_binding_box->blockSignals(true);
                    //type_binding_box->setModel(type_model);
                    //type_binding_box->setEditable(true);
                    //type_binding_box->setInsertPolicy(QComboBox::NoInsert);
                    //type_binding_box->setCurrentIndex(type.value());
                    //type_binding_box->blockSignals(false);

                    //bindingsVerticalLayout->addWidget(type_binding_box, i, 2);

                }
            }
        }
        else {
            bindingsTableWidget->clearContents();
            bindingsTableWidget->setVisible(false);
        }
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