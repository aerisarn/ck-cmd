#include "CharacterEditor.h"

#include <QInputDialog>
#include <src/widgets/IndexedEntityChooser.h>

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

void CharacterEditorWidget::updateSetComboBox()
{

    QStringList set_files;
    setCurrentComboBox->blockSignals(true);
    setCurrentComboBox->clear();
    int set_file_size = _manager.getAnimationSetsFiles(_project_index);
    for (int i = 0; i < set_file_size; ++i)
    {
        setCurrentComboBox->addItem(_manager.getAnimationSetsFile(_project_index, i));
    }
    setConditionsTableWidget->setColumnCount(3);
    QStringList LIST;
    LIST << "Min" << "Max" << "Variable";
    setConditionsTableWidget->setHorizontalHeaderLabels(LIST);
    setCurrentComboBox->blockSignals(false);
    setCurrentComboBox->setCurrentIndex(0);
    on_setCurrentComboBox_currentIndexChanged(0);

}

void CharacterEditorWidget::setAnimationSetsControls()
{
    if (_manager.isCreatureProject(_project_index))
    {
        updateSetComboBox();
        mainTab->setTabVisible(1, true);
    }
    else {
        setCurrentComboBox->blockSignals(true);
        setCurrentComboBox->clear();
        mainTab->setTabVisible(1, false);
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
    setAnimationSetsControls();
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

void CharacterEditorWidget::updateSetEvents(int set_index)
{
    setTriggetingEventsView->setRowCount(0);
    int events = _manager.getAnimationSetEvents(_project_index, set_index);
    for (int event_index = 0; event_index < events; ++event_index)
    {
        int index = setTriggetingEventsView->rowCount();
        setTriggetingEventsView->insertRow(index);
        QTableWidgetItem* item = new QTableWidgetItem();
        auto name = _manager.getAnimationSetEvent(_project_index, set_index, event_index);
        item->setText(name);
        setTriggetingEventsView->setItem(
            index,
            0,
            item
        );
    }
    if (events > 0 && setTriggetingEventsView->currentIndex().isValid())
    {
        setEventRemoveButton->setEnabled(true);
    }
    else {
        setEventRemoveButton->setEnabled(false);
    }
}

void CharacterEditorWidget::on_setTriggetingEventsView_itemClicked(QTableWidgetItem* item)
{
    if (setTriggetingEventsView->rowCount() > 0 && setTriggetingEventsView->currentIndex().isValid())
    {
        setEventRemoveButton->setEnabled(true);
    }
    else {
        setEventRemoveButton->setEnabled(false);
    }
}

void CharacterEditorWidget::updateSetAttacks(int set_index)
{
    setAttacksTreeWidget->clear();
    int attacks = _manager.getAnimationSetAttacks(_project_index, set_index);
    for (int attack_index = 0; attack_index < attacks; ++attack_index)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(setAttacksTreeWidget);
        item->setText(0, _manager.getAnimationSetAttackEvent(_project_index, set_index, attack_index));
        int attack_clips = _manager.getAnimationSetAttackClips(_project_index, set_index, attack_index);
        for (int j = 0; j < attack_clips; ++j)
        {
            QTreeWidgetItem* clip_item = new QTreeWidgetItem();
            clip_item->setText(0, _manager.getAnimationSetAttackClip(_project_index, set_index, attack_index, j));
            item->addChild(clip_item);
        }
    }
    if (attacks > 0 && setAttacksTreeWidget->currentIndex().isValid())
    {
        setAttacksRemoveButton->setEnabled(true);
    }
    else {
        setAttacksRemoveButton->setEnabled(false);
    }
}

void CharacterEditorWidget::updateSetConditions(int set_index)
{
    setConditionsTableWidget->setRowCount(0);
    int conditions = _manager.getAnimationSetVariables(_project_index, set_index);
    for (int condition_index = 0; condition_index < conditions; ++condition_index)
    {
        int index = setConditionsTableWidget->rowCount();
        setConditionsTableWidget->insertRow(index);
        setConditionsTableWidget->setItem(
            index,
            0,
            new QTableWidgetItem(QString::number(_manager.getAnimationSetVariableMin(_project_index, set_index, condition_index)))
        );
        setConditionsTableWidget->setItem(
            index,
            1,
            new QTableWidgetItem(QString::number(_manager.getAnimationSetVariableMax(_project_index, set_index, condition_index)))
        );
        setConditionsTableWidget->setItem(
            index,
            2,
            new QTableWidgetItem(_manager.getAnimationSetVariable(_project_index, set_index, condition_index))
        );
    }
    if (conditions > 0 && setConditionsTableWidget->currentIndex().isValid())
    {
        setConditionsRemoveButton->setEnabled(true);
    }
    else {
        setConditionsRemoveButton->setEnabled(false);
    }
}

void CharacterEditorWidget::updateSetAnimations(int set_index)
{
    setAnimationsTableWidget->setRowCount(0);
    auto animations = _manager.getAnimationSetAnimation(_project_index, set_index);
    for (const auto& animation : animations)
    {
        int index = setAnimationsTableWidget->rowCount();
        setAnimationsTableWidget->insertRow(index);
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(animation);
        setAnimationsTableWidget->setItem(
            index,
            0,
            item
        );
    }
    if (animations.size() > 0 && setAnimationsTableWidget->currentIndex().isValid())
    {
        setAnimationsRemoveButton->setEnabled(true);
    }
    else {
        setAnimationsRemoveButton->setEnabled(false);
    }
}

void CharacterEditorWidget::updateSetControls(int set_index)
{
    updateSetEvents(set_index);
    updateSetAttacks(set_index);
    updateSetConditions(set_index);
    updateSetAnimations(set_index);

    if (setCurrentComboBox->count() > 1)
    {
        setRemoveButton->setEnabled(true);
    }
    else {
        setRemoveButton->setEnabled(false);
    }
}

void CharacterEditorWidget::on_setCurrentComboBox_currentIndexChanged(int set_index)
{
    updateSetControls(set_index);
}

//Set Panel
void CharacterEditorWidget::on_setRemoveButton_clicked()
{
    if (setCurrentComboBox->count() > 1)
    {
        _manager.deleteAnimationSet(_project_index, setCurrentComboBox->currentIndex());
        setCurrentComboBox->removeItem(setCurrentComboBox->currentIndex());
        setCurrentComboBox->setCurrentIndex(0);
    }
}

void CharacterEditorWidget::on_setAddButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Choose set name"),
        tr("New set name:"), QLineEdit::Normal,
        "", &ok);
    if (ok && !text.isEmpty())
    {
        int new_set_index = _manager.getAnimationSetsFiles(_project_index);
        _manager.createAnimationSet(_project_index, text);
        updateSetComboBox();
        setCurrentComboBox->setCurrentIndex(new_set_index);
    }
}

void CharacterEditorWidget::on_setEventAddButton_clicked()
{
    auto* edit_events_model = _model.editModel(_index, ckcmd::HKX::AssetType::events);
    if (nullptr != edit_events_model)
    {
        bool ok;
        QModelIndex event_index = IndexedEntityChooser::getIndex
        (
            edit_events_model,
            this,
            tr("Add triggering event"),
            tr("Select an existing event:"),
            tr("Create a new one:"),
            &ok
        );
        QString event_name = event_index.data().toString();
        if (ok && !event_name.isEmpty())
        {
            int set_index = setCurrentComboBox->currentIndex();
            _manager.addAnimationSetEvent(_project_index, set_index, event_name);
            updateSetEvents(set_index);
        }
    }
}

void CharacterEditorWidget::on_setEventRemoveButton_clicked()
{
    if (setTriggetingEventsView->currentIndex().isValid())
    {
        int set_index = setCurrentComboBox->currentIndex();
        _manager.deleteAnimationSetEvent(_project_index, set_index, setTriggetingEventsView->currentIndex().row());
        updateSetEvents(set_index);
    }
}