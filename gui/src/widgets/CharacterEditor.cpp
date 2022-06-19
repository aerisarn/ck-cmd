#include "CharacterEditor.h"


CharacterEditorWidget::CharacterEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) :
    _behavior_file_options(new QStringListModel(this)),
    _behavior_completer(new QCompleter(this)),
    _rig_file_options(new QStringListModel(this)),
    _rig_completer(new QCompleter(this)),
    ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    dataBindingtable.push_back({ behavioComboBox, {}, "behaviorFilename", 0 });
    dataBindingtable.push_back({ rigComboBox, {}, "rigName", 0 });
    dataBindingtable.push_back({ this, {}, "mirrorAxis", 0 });
    //dataBindingtable.push_back({ mirroringYDoubleSpinBox, {}, "mirrorAxis", 1 });
    //dataBindingtable.push_back({ mirroringZDoubleSpinBox, {}, "mirrorAxis", 2 });
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
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
    behavioComboBox->setModel(_behavior_file_options);
    behavioComboBox->setEditable(true);
    behavioComboBox->setInsertPolicy(QComboBox::NoInsert);
    behavioComboBox->setCurrentIndex(new_index);
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
    rigComboBox->setModel(_rig_file_options);
    rigComboBox->setEditable(true);
    rigComboBox->setInsertPolicy(QComboBox::NoInsert);
    rigComboBox->setCurrentIndex(new_index);
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