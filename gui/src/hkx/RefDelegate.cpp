#include "RefDelegate.h"

#include <QLabel>
#include <QComboBox>
#include <QPainter>
#include <QApplication>
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

QString RefDelegate::ObjectText(int object_index, size_t file_index) const {
    QString label = QString("[%1]").arg(object_index);
    if (object_index > 0)
    {
        auto* variant = _manager.at(file_index, object_index);
        auto member = variant->m_class->getMemberByName("name");
        if (HK_NULL != member)
        {
            auto member_ptr = ((char*)variant->m_object) + member->getOffset();
            auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
            label = QString("[%1] \"%2\"").arg(object_index).arg(c_str_ptr);
        }
    }
    return label;
}

void RefDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {

        HkxItemPointer data = index.data().value<HkxItemPointer>();
        auto file_index_ = data.file_index();
        auto* object_address_ = data.get();
        auto object_index = _manager.findIndex(data.file_index(), data.get());
        QString label = ObjectText(object_index, file_index_);

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

QSize RefDelegate::ComboBoxSizeHint(const QStyleOptionViewItem& option, const QStringList& options) const
{
    QStyleOptionComboBox comboBoxOption;
    comboBoxOption.rect = option.rect;
    comboBoxOption.state = option.state;
    comboBoxOption.state |= QStyle::State_Enabled;
    QSize contentsSize = { 0 , 0 };
    qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBoxOption, contentsSize, nullptr);
    for (const auto& label : options)
        contentsSize = contentsSize.expandedTo(qApp->style()->sizeFromContents(
            QStyle::CT_ComboBox,
            &comboBoxOption,
            option.fontMetrics.boundingRect(label).size(),
            nullptr
        ));
    return contentsSize;
}


QSize RefDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {
        HkxItemPointer data = index.data().value<HkxItemPointer>();
        auto object_index = _manager.findIndex(data.file_index(), data.get());
        QString label = ObjectText(object_index, data.file_index());
        auto objects = _manager.findCompatibleNodes(data.file_index(), data.field_class());
        QStringList options;
        for (const auto& object : objects)
            options << ObjectText(object.first, data.file_index());

        return ComboBoxSizeHint(option, options);
    }
    if (index.data().canConvert<HkxItemEnum>()) {
        HkxItemEnum data = index.data().value<HkxItemEnum>();
        QStringList options = data.enumValues();
        return ComboBoxSizeHint(option, options);
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
        return new QComboBox(parent);
    }
    if (index.data().canConvert<HkxItemEnum>()) {
        return new QComboBox(parent);
    }
    if (index.data().canConvert<HkxItemFlags>()) {
        HkxItemFlags flags = index.data().value<HkxItemFlags>();
        return flags.CreateEditor(parent);
    }
    if (index.data().canConvert<HkxItemReal>()) {
        HkxItemReal real = index.data().value<HkxItemReal>();
        return  real.CreateEditor(parent);
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void RefDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        HkxItemPointer ptr = index.data().value<HkxItemPointer>();
        auto objects = _manager.findCompatibleNodes(ptr.file_index(), ptr.field_class());
        QStringList options;
        options << "<clear>";
        int index = 0;
        int i = 0;
        for (const auto& object : objects)
        {
            options << ObjectText(object.first, ptr.file_index());
            if (object.second->m_object == ptr.get())
                index = i;
            i++;
        }
        ptr_editor->addItems(options);
        ptr_editor->setCurrentIndex(index+1);
        return;
    }
    if (index.data().canConvert<HkxItemEnum>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        HkxItemEnum data = index.data().value<HkxItemEnum>();
        QStringList options = data.enumValues();
        ptr_editor->addItems(options);
        ptr_editor->setCurrentIndex(data.value());
        return;
    }
    if (index.data().canConvert<HkxItemFlags>()) {
        QWidget* ptr_editor = dynamic_cast<QWidget*>(editor);
        HkxItemFlags data = index.data().value<HkxItemFlags>();
        data.FillEditor(ptr_editor);
        return;
    }
    if (index.data().canConvert<HkxItemReal>()) {
        QWidget* ptr_editor = dynamic_cast<QWidget*>(editor);
        HkxItemReal data = index.data().value<HkxItemReal>();
        data.FillEditor(ptr_editor);
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void RefDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        HkxItemPointer ptr = index.data().value<HkxItemPointer>();
        auto objects = _manager.findCompatibleNodes(ptr.file_index(), ptr.field_class());
        QVariant new_value;
        if (ptr_editor->currentIndex() == 0)
        {
            new_value.setValue(HkxItemPointer(ptr.file_index(), 0, ptr.field_class()));
        }
        else if (!objects.empty())
        {
            auto selected_object = objects[ptr_editor->currentIndex()-1];
            new_value.setValue(HkxItemPointer(ptr.file_index(), selected_object.second->m_object, ptr.field_class()));
        }
        model->setData(index, new_value, Qt::EditRole);
    }
    else if (index.data().canConvert<HkxItemEnum>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        auto value = index.data().value<HkxItemEnum>();
        value.setValue(ptr_editor->currentIndex());
        QVariant new_value; new_value.setValue(value);
        model->setData(index, new_value, Qt::EditRole);
    }
    else if (index.data().canConvert<HkxItemReal>()) {
        QWidget* ptr_editor = dynamic_cast<QWidget*>(editor);
        HkxItemReal data = index.data().value<HkxItemReal>();
        data.FillFromEditor(ptr_editor);
        QVariant new_value; new_value.setValue(data);
        model->setData(index, new_value, Qt::EditRole);
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void RefDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}