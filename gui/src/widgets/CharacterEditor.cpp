#include "CharacterEditor.h"


CharacterEditorWidget::CharacterEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) :
    _behavior_file_options(new QStringListModel(this)),
    _behavior_completer(new QCompleter(this)),
    _rig_file_options(new QStringListModel(this)),
    _rig_completer(new QCompleter(this)),
    _rig_model(nullptr),
    ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    dataBindingtable.push_back({ behavioComboBox, {}, "behaviorFilename", 0 });
    dataBindingtable.push_back({ rigComboBox, {}, "rigName", 0 });
    dataBindingtable.push_back({ this, {}, "mirrorAxis", 0 });
}

QSize CharacterEditorWidget::sizeHint() const
{
    return QSize(784, 302);
}

void CharacterEditorWidget::setBehaviorField()
{
    QString option = behavioComboBox->currentText();
    delete _behavior_file_options;
    _behavior_file_options = static_cast<QStringListModel*>(_model.editModel(_index, ckcmd::HKX::AssetType::behavior));
    int new_index = -1;
    auto options = _behavior_file_options->stringList();
    for (int i = 0; i < options.size(); ++i)
    {
        if (QString::compare(option, options.at(i), Qt::CaseInsensitive) == 0)
        {
            new_index = i;
            break;
        }
    }
    _behavior_file_options->setParent(this); //last gets deleted
    _behavior_completer->setModel(_behavior_file_options);
    _behavior_completer->setCompletionMode(QCompleter::InlineCompletion);
    behavioComboBox->blockSignals(true);
    behavioComboBox->setModel(_behavior_file_options);
    behavioComboBox->setEditable(true);
    behavioComboBox->setInsertPolicy(QComboBox::NoInsert);
    behavioComboBox->setCurrentIndex(new_index);
    behavioComboBox->blockSignals(false);
}

void CharacterEditorWidget::setRigField()
{
    QString option = rigComboBox->currentText();
    delete _rig_file_options;
    _rig_file_options = static_cast<QStringListModel*>(_model.editModel(_index, ckcmd::HKX::AssetType::skeleton));
    int new_index = -1;
    auto options = _rig_file_options->stringList();
    for (int i = 0; i < options.size(); ++i)
    {
        if (QString::compare(option, options.at(i), Qt::CaseInsensitive) == 0)
        {
            new_index = i;
            break;
        }
    }
    _rig_file_options->setParent(this); //last gets deleted
    _rig_completer->setModel(_rig_file_options);
    _rig_completer->setCompletionMode(QCompleter::InlineCompletion);
    rigComboBox->blockSignals(true);
    rigComboBox->setModel(_rig_file_options);
    rigComboBox->setEditable(true);
    rigComboBox->setInsertPolicy(QComboBox::NoInsert);
    rigComboBox->setCurrentIndex(new_index);
    rigComboBox->blockSignals(false);
}

void CharacterEditorWidget::setRigControls()
{
    delete _rig_model;
    _rig_model = _model.editModel(_index, ckcmd::HKX::AssetType::bones);
    if (_rig_model != nullptr)
    {
        _rig_model->setParent(this);

        auto ragdoll_model = _model.editModel(_index, ckcmd::HKX::AssetType::ragdoll_bones);
        if (ragdoll_model != nullptr)
        {
            ragdollBoneCountLineEdit->setText(QString::number(ragdoll_model->rowCount()));
            delete ragdoll_model;
        }
        else {
            ragdollBoneCountLineEdit->setText(QString::number(0));
        }

        rigBoneCountLineEdit->setText(QString::number(_rig_model->rowCount()));

        selectBoneTreeView->setModel(_rig_model);
        selectBoneTreeView->clearSelection();
        selectBoneTreeView->setHeaderHidden(true);
        selectBoneTreeView->setRootIsDecorated(false);
        selectBoneTreeView->setIndentation(0);

        selectPairedBoneTreeView->setModel(_rig_model);
        selectPairedBoneTreeView->clearSelection();
        selectPairedBoneTreeView->setHeaderHidden(true);
        selectPairedBoneTreeView->setRootIsDecorated(false);
        selectPairedBoneTreeView->setIndentation(0);

        selectBoneTreeView->setCurrentIndex(_rig_model->index(0, 0, QModelIndex()));
    }
    else {
        rigBoneCountLineEdit->setText(QString::number(0));
        ragdollBoneCountLineEdit->setText(QString::number(0));
    }
}

ckcmd::HKX::HkxItemReal CharacterEditorWidget::readMirrorAxis()
{
    return ckcmd::HKX::HkxItemReal(
        { {
            (float)mirroringXDoubleSpinBox->value(),
            (float)mirroringYDoubleSpinBox->value(),
            (float)mirroringZDoubleSpinBox->value()
        } }
    );
}

void CharacterEditorWidget::writeMirrorAxis(ckcmd::HKX::HkxItemReal value)
{
    mirroringXDoubleSpinBox->setValue(value.value(0, 0));
    mirroringYDoubleSpinBox->setValue(value.value(0, 1));
    mirroringZDoubleSpinBox->setValue(value.value(0, 2));
}

void CharacterEditorWidget::OnIndexSelected()
{
    setBehaviorField();
    setRigField();
    setRigControls();
}

void CharacterEditorWidget::on_mirroringXDoubleSpinBox_valueChanged(double d)
{
    emit mirrorAxisChanged(readMirrorAxis());
}
void CharacterEditorWidget::on_mirroringYDoubleSpinBox_valueChanged(double d)
{
    emit mirrorAxisChanged(readMirrorAxis());
}
void CharacterEditorWidget::on_mirroringZDoubleSpinBox_valueChanged(double d)
{
    emit mirrorAxisChanged(readMirrorAxis());
}

void CharacterEditorWidget::on_selectBoneTreeView_clicked(const QModelIndex& current)
{
    if (_rig_model != nullptr)
    {
        int initPairedIndex = data("bonePairMap", current.row()).toInt();
        selectPairedBoneTreeView->setCurrentIndex(_rig_model->index(initPairedIndex, 0, QModelIndex()));
    }
}

void CharacterEditorWidget::on_selectPairedBoneTreeView_clicked(const QModelIndex& current)
{
    if (_rig_model != nullptr)
    {
        auto sourceBone = selectBoneTreeView->currentIndex();
        int sourceBoneIndex = sourceBone.row();
        int destBoneIndex = current.row();
        setData("bonePairMap", sourceBoneIndex, destBoneIndex, Qt::EditRole);
    }
}