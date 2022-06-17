#include "CharacterEditor.h"


CharacterEditorWidget::CharacterEditorWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) : ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
}

QSize CharacterEditorWidget::sizeHint() const
{
    return QSize(784, 302);
}

void CharacterEditorWidget::OnIndexSelected()
{

}