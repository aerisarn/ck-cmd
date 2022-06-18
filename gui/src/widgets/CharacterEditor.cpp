#include "CharacterEditor.h"


CharacterEditorWidget::CharacterEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) : ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    dataBindingtable.push_back({ behavioComboBox, {}, "behaviorFilename" });
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
}

QSize CharacterEditorWidget::sizeHint() const
{
    return QSize(784, 302);
}

void CharacterEditorWidget::OnIndexSelected()
{
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