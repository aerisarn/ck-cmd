#include "RefDelegate.h"

#include <QLabel>
#include <qpainter.h>
#include <src/hkx/HkxItemPointer.h>
#include <src/hkx/HkxItemEnum.h>
#include <src/hkx/HkxItemFlags.h>

using namespace ckcmd::HKX;

#define CUSTOM_SIZE_PADDING 3

RefDelegate::RefDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void RefDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {

        uintptr_t data = (uintptr_t)index.data().value<HkxItemPointer>().get();

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(Qt::white);
        }
        else {
            painter->setPen(Qt::black);
        }

        auto rect = option.rect.adjusted(CUSTOM_SIZE_PADDING, 0, 0, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, QString::number(data));
    }
    else if (index.data().canConvert<HkxItemEnum>()) {

        QString data = index.data().value<HkxItemEnum>().value_literal();

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(Qt::white);
        }
        else {
            painter->setPen(Qt::black);
        }

        auto rect = option.rect.adjusted(CUSTOM_SIZE_PADDING, 0, 0, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, data);
    }
    else if (index.data().canConvert<HkxItemFlags>()) {
        HkxItemFlags flags = index.data().value<HkxItemFlags>();
        flags.paint(painter, option);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize RefDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {

        uintptr_t data = (uintptr_t)index.data().value<HkxItemPointer>().get();
        auto rect = option.fontMetrics.boundingRect(QString::number(data));
        return {rect.width() + 2 * CUSTOM_SIZE_PADDING, rect.height()};
    }
    if (index.data().canConvert<HkxItemEnum>()) {

        QString data = index.data().value<HkxItemEnum>().value_literal();
        auto rect = option.fontMetrics.boundingRect(data);
        return { rect.width() + 2 * CUSTOM_SIZE_PADDING, rect.height() };
    }
    if (index.data().canConvert<HkxItemFlags>()) {
        HkxItemFlags flags = index.data().value<HkxItemFlags>();
        return  flags.WidgetSizeHint(option.fontMetrics);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* RefDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option ,
    const QModelIndex& index ) const
{
    if (index.data().canConvert<HkxItemPointer>()) {
        return new QLabel(parent);
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void RefDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();

    //QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
    //spinBox->setValue(value);

    if (index.data().canConvert<HkxItemPointer>()) {
        QLabel* editor = qobject_cast<QLabel*>(editor);
        HkxItemPointer ptr = index.data().value<HkxItemPointer>();
        editor->setText(QString((uintptr_t)ptr.get()));
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void RefDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

void RefDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    //editor->setGeometry(option.rect);
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}