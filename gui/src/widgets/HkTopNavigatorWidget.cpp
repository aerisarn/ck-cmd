#include "HkTopNavigatorWidget.h"

#include <Common/Base/hkBase.h>
#include <hkbNode_1.h>


TopInfoWidget::TopInfoWidget(ckcmd::HKX::ProjectModel& model, QWidget* parent) : ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
}

QSize TopInfoWidget::sizeHint() const
{
    return QSize(472, 30);
}

void TopInfoWidget::OnIndexSelected()
{
    if (_model.isVariant(_index))
    {
        nameLineEdit->setText(_model.data(_index, Qt::DisplayRole).toString());
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