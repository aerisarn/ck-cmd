#include "RefDelegate.h"

#include <QLabel>
#include <qpainter.h>
#include <src/hkx/HkxItemPointer.h>
#include <src/hkx/HkxItemEnum.h>
#include <src/hkx/HkxItemFlags.h>
#include <src/hkx/HkxItemReal.h>

using namespace ckcmd::HKX;

#define CUSTOM_SIZE_PADDING 3

RefDelegate::RefDelegate(const ResourceManager& manager, QObject* parent)
    : _manager(manager),
    QStyledItemDelegate(parent)
{
}

void RefDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {

        HkxItemPointer data = index.data().value<HkxItemPointer>();
        auto file_index_ = data.file_index();
        auto* object_address_ = data.get();
        auto object_index = _manager.findIndex(data.file_index(), data.get());
        QString label = QString("[%1]").arg(object_index);
        if (object_index > 0)
        {
            auto* variant = _manager.at(data.file_index(), object_index);
            auto member = variant->m_class->getMemberByName("name");
            if (HK_NULL != member)
            {
                auto member_ptr = ((char*)variant->m_object) + member->getOffset();
                auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
                label = QString("[%1] \"%2\"").arg(object_index).arg(c_str_ptr);
            }
        }

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(Qt::white);
        }
        else {
            painter->setPen(Qt::black);
        }

        auto rect = option.rect.adjusted(CUSTOM_SIZE_PADDING, 0, 0, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, label);
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

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(Qt::white);
        }
        else {
            painter->setPen(Qt::black);
        }

        flags.paint(painter, option);
    }
    else if (index.data().canConvert<HkxItemReal>()) {
        HkxItemReal real = index.data().value<HkxItemReal>();

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(Qt::white);
        }
        else {
            painter->setPen(Qt::black);
        }

        real.paint(painter, option);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize RefDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {

        HkxItemPointer data = index.data().value<HkxItemPointer>();
        auto object_index = _manager.findIndex(data.file_index(), data.get());
        QString label = QString("[%1]").arg(object_index);
        if (object_index > 0)
        {
            auto* variant = _manager.at(data.file_index(), object_index);
            auto member = variant->m_class->getMemberByName("name");
            if (HK_NULL != member)
            {
                auto member_ptr = ((char*)variant->m_object) + member->getOffset();
                auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
                label = QString("[%1] \"%2\"").arg(object_index).arg(c_str_ptr);
            }
        }
        auto rect = option.fontMetrics.boundingRect(label);
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
    if (index.data().canConvert<HkxItemReal>()) {
        HkxItemReal real = index.data().value<HkxItemReal>();
        return  real.WidgetSizeHint(option.fontMetrics);
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