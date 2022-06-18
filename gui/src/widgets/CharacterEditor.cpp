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
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
}

QSize CharacterEditorWidget::sizeHint() const
{
    return QSize(784, 302);
}

void CharacterEditorWidget::OnIndexSelected()
{
    //use a submodel;
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

    //int rows = _model.rowCount(_index);
    //int columns = _model.columnCount(_index);

    //std::vector<QVariant> datas;

    //for (int i = 0; i < rows; ++i)
    //{
    //    for (int j = 0; j < columns; ++j)
    //    {
    //        datas.push_back(_model.data(_model.index(i, j, _index)));
    //    }
    //}
}