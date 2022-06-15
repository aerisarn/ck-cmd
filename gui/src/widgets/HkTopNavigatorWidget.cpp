#include "HkTopNavigatorWidget.h"

#include <Common/Base/hkBase.h>
#include <hkbNode_1.h>


TopInfoWidget::TopInfoWidget(ckcmd::HKX::ProjectTreeModel& model, QWidget* parent) : ckcmd::ModelWidget(model, parent)
{
    setupUi(this);
    //connect(nameLineEdit, &QLineEdit::textChanged, this, &TopInfoWidget::resizeNameToContent);
}

QSize TopInfoWidget::sizeHint() const
{
    return QSize(472, 30);
}

void TopInfoWidget::setIndex(const QModelIndex& index)
{
    _selected = index;
    if (_model.isVariant(index))
    {
        nameLineEdit->setText(_model.data(index, Qt::DisplayRole).toString());
    }
}

void TopInfoWidget::on_nameLineEdit_textChanged(const QString& text)
{
    _model.setData(_selected, text, Qt::EditRole);
}

//void TopInfoWidget::resizeNameToContent(const QString& text)
//{
//    QFontMetrics fm(nameLineEdit->font());
//    int pixelsWide = fm.width(text) + 20;
//    nameLineEdit->setFixedWidth(pixelsWide);
//    adjustSize();
//}