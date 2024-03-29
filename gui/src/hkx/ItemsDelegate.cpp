#include "ItemsDelegate.h"

#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>
#include <QApplication>
#include <src/items/HkxItemPointer.h>
#include <src/items/HkxItemEnum.h>
#include <src/items/HkxItemFlags.h>
#include <src/items/HkxItemReal.h>
#include <src/items/HkxItemEvent.h>
#include <src/items/HkxItemVar.h>
#include <src/items/HkxItemBone.h>
#include <src/items/HkxItemRagdollBone.h>
#include <src/items/HkxItemFSMState.h>
#include <src/items/HkxItemEventPayload.h>

#include <src/hkx/HkxVariant.h>

using namespace ckcmd::HKX;

#define CUSTOM_SIZE_PADDING 3

ItemsDelegate::ItemsDelegate(ckcmd::HKX::ValuesProxyModel& model, QObject* parent)
    : _model(model),
    _manager(_model.getResourceManager()),
    QStyledItemDelegate(parent)
{
}

//QString ItemsDelegate::ObjectText(int object_index, size_t file_index) const {
//    QString label = QString("[%1]").arg(object_index);
//    if (object_index > 0)
//    {
//        auto* variant = _manager.at(file_index, object_index);
//        auto member = variant->m_class->getMemberByName("name");
//        if (HK_NULL != member)
//        {
//            auto member_ptr = ((char*)variant->m_object) + member->getOffset();
//            auto c_str_ptr = (char*)*(uintptr_t*)(member_ptr);
//            label = QString("[%1] \"%2\"").arg(object_index).arg(c_str_ptr);
//        }
//    }
//    return label;
//}


template <typename T>
void paintReference(QPainter* painter, const QStyleOptionViewItem& option, ValuesProxyModel& model, const QModelIndex& index)
{
    auto value = index.data().value<T>();
    int data_index = value.index();
    QAbstractItemModel* data_model;
    if (typeid(T) == typeid(HkxItemFSMState))
    {
        data_model = model.editModel(index, value.assetType());
    }
    else {
        data_model = model.editModel(index, value.assetType(), "<Not Set>", Qt::EditRole);
    }
    QString data = data_model->index(data_index + 1, 0).data().toString();

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


void ItemsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemEventPayload>()) 
    {
        QString label = "";
        HkxItemEventPayload data = index.data().value<HkxItemEventPayload>();

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
            painter->setPen(Qt::white);
        }
        else {
            painter->setPen(Qt::black);
        }

        label = data.get();
        auto rect = option.rect.adjusted(CUSTOM_SIZE_PADDING, 0, 0, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, label);

    }
    else if (index.data().canConvert<HkxItemPointer>()) {

        QString label = "";
        HkxItemPointer data = index.data().value<HkxItemPointer>();
        auto file_index_ = _model.getFileIndex(index);
        auto* object_address_ = data.get();
        auto variant = _manager.findVariant(file_index_, object_address_);
        if (variant != nullptr)
            label = HkxVariant(*variant).name();

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
    else if (index.data().canConvert<HkxItemEvent>()) {
        paintReference<HkxItemEvent>(painter, option, _model, index);
    }
    else if (index.data().canConvert<HkxItemVar>()) {
        paintReference<HkxItemVar>(painter, option, _model, index);
    }
    else if (index.data().canConvert<HkxItemBone>()) {
        paintReference<HkxItemBone>(painter, option, _model, index);
    }
    else if (index.data().canConvert<HkxItemRagdollBone>()) {
        paintReference<HkxItemRagdollBone>(painter, option, _model, index);
    }
    else if (index.data().canConvert<HkxItemFSMState>()) {
        paintReference<HkxItemFSMState>(painter, option, _model, index);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ComboBoxSizeHint(const QStyleOptionViewItem& option, const QStringList& options)
{
    QStyleOptionComboBox comboBoxOption;
    comboBoxOption.rect = option.rect;
    comboBoxOption.state = option.state;
    comboBoxOption.state |= QStyle::State_Enabled;
    QSize contentsSize = { 0 , 0 };
    qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBoxOption, contentsSize, nullptr);
    for (const auto& _option : options)
        contentsSize = contentsSize.expandedTo(qApp->style()->sizeFromContents(
            QStyle::CT_ComboBox,
            &comboBoxOption,
            option.fontMetrics.boundingRect(_option).size(),
            nullptr
        ));
    return contentsSize;
}

QSize ComboBoxSizeHint(const QStyleOptionViewItem& option, QAbstractItemModel* options)
{
    QStyleOptionComboBox comboBoxOption;
    comboBoxOption.rect = option.rect;
    comboBoxOption.state = option.state;
    comboBoxOption.state |= QStyle::State_Enabled;
    QSize contentsSize = { 0 , 0 };
    qApp->style()->sizeFromContents(QStyle::CT_ComboBox, &comboBoxOption, contentsSize, nullptr);
    int rows = options->rowCount();
    for (int i=0; i<rows; ++i)
        contentsSize = contentsSize.expandedTo(qApp->style()->sizeFromContents(
            QStyle::CT_ComboBox,
            &comboBoxOption,
            option.fontMetrics.boundingRect(options->index(i,0).data().toString()).size(),
            nullptr
        ));
    return contentsSize;
}

template <typename T>
QSize sizeHintReference(const QStyleOptionViewItem& option,
    ValuesProxyModel& model,
    const QModelIndex& index)
{
    auto value = index.data().value<T>();
    int data_index = value.index();
    QAbstractItemModel* data_model;
    if (typeid(T) == typeid(HkxItemFSMState))
    {
        data_model = model.editModel(index, value.assetType());
    }
    else {
        data_model = model.editModel(index, value.assetType(), "<Not Set>", Qt::EditRole);
    }

    return ComboBoxSizeHint(option, data_model);
}


QSize ItemsDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemPointer>()) {
        HkxItemPointer data = index.data().value<HkxItemPointer>();
        //auto object_index = _manager.findIndex(data.file_index(), data.get());
        //QString label = ObjectText(object_index, data.file_index());
        //auto objects = _manager.findCompatibleNodes(data.file_index(), data.field_class());
        QStringList options;
        //for (const auto& object : objects)
        //    options << ObjectText(object.first, data.file_index());

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
    if (index.data().canConvert<HkxItemEvent>()) {
        return sizeHintReference<HkxItemEvent>(option, _model, index);
    }
    if (index.data().canConvert<HkxItemVar>()) {
        return sizeHintReference<HkxItemVar>(option, _model, index);
    }
    if (index.data().canConvert<HkxItemBone>()) {
        return sizeHintReference<HkxItemBone>(option, _model, index);
    }
    if (index.data().canConvert<HkxItemRagdollBone>()) {
        return sizeHintReference<HkxItemRagdollBone>(option, _model, index);
    }
    if (index.data().canConvert<HkxItemFSMState>()) {
        return sizeHintReference<HkxItemFSMState>(option, _model, index);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* ItemsDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option ,
    const QModelIndex& index ) const
{
    if (index.data().canConvert<HkxItemEventPayload>()) {
        return new QLineEdit(parent);
    }
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
    if (index.data().canConvert<HkxItemEvent>()) {
        return new QComboBox(parent);
    }
    if (index.data().canConvert<HkxItemVar>()) {
        return new QComboBox(parent);
    }
    if (index.data().canConvert<HkxItemBone>()) {
        return new QComboBox(parent);
    }
    if (index.data().canConvert<HkxItemRagdollBone>()) {
        return new QComboBox(parent);
    }
    if (index.data().canConvert<HkxItemFSMState>()) {
        return new QComboBox(parent);
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

template <typename T>
void setEditorDataReference(QWidget* editor, ValuesProxyModel& model,
    const QModelIndex& index)
{
    QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
    auto value = index.data().value<T>();
    int data_index = value.index();
    QAbstractItemModel* data_model;
    if (typeid(T) == typeid(HkxItemFSMState))
    {
        data_model = model.editModel(index, value.assetType(), "");
    }
    else {
        data_model = model.editModel(index, value.assetType(), "<Not Set>", Qt::EditRole);
    }

    QStringList options;

    //debug
    int rows = data_model->rowCount();
    for (int i = 0; i < rows; i++)
    {
        options << data_model->index(i, 0).data().toString();
    }

    QCompleter* data_completer = new QCompleter(editor);
    data_completer->setModel(data_model);
    data_completer->setCompletionMode(QCompleter::PopupCompletion);
    ptr_editor->setEditable(true);
    ptr_editor->setInsertPolicy(QComboBox::NoInsert);
    ptr_editor->setModel(data_model);
    ptr_editor->setCompleter(data_completer);
    if (typeid(T) == typeid(HkxItemFSMState))
        ptr_editor->setCurrentIndex(data_index);
    else
        ptr_editor->setCurrentIndex(data_index + 1);
}


void ItemsDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemEventPayload>()) {
        HkxItemEventPayload ptr = index.data().value<HkxItemEventPayload>();
        QLineEdit* ptr_editor = dynamic_cast<QLineEdit*>(editor);
        ptr_editor->setText(ptr.get());
        return;
    }
    if (index.data().canConvert<HkxItemPointer>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        HkxItemPointer ptr = index.data().value<HkxItemPointer>();
        //auto objects = _manager.findCompatibleNodes(ptr.file_index(), ptr.field_class());
        QStringList options;
        options << "<clear>";
        int index = 0;
        //int i = 0;
        //for (const auto& object : objects)
        //{
        //    options << ObjectText(object.first, ptr.file_index());
        //    if (object.second->m_object == ptr.get())
        //        index = i;
        //    i++;
        //}
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
    if (index.data().canConvert<HkxItemEvent>()) {
        setEditorDataReference<HkxItemEvent>(editor, _model, index);
        return;
    }
    if (index.data().canConvert<HkxItemVar>()) {
        setEditorDataReference<HkxItemVar>(editor, _model, index);
        return;
    }
    if (index.data().canConvert<HkxItemBone>()) {
        setEditorDataReference<HkxItemBone>(editor, _model, index);
        return;
    }
    if (index.data().canConvert<HkxItemRagdollBone>()) {
        setEditorDataReference<HkxItemRagdollBone>(editor, _model, index);
        return;
    }
    if (index.data().canConvert<HkxItemFSMState>()) {
        setEditorDataReference<HkxItemFSMState>(editor, _model, index);
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

template <typename T>
void setModelDataReference(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index)
{
    QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
    model->setData(index, ptr_editor->currentIndex() - 1, Qt::EditRole);
}


void ItemsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const
{
    if (index.data().canConvert<HkxItemEventPayload>()) {
        QLineEdit* ptr_editor = dynamic_cast<QLineEdit*>(editor);
        HkxItemEventPayload ptr = index.data().value<HkxItemEventPayload>();
        hkbStringEventPayload* value = ptr.data();
        QString text_value = ptr_editor->text();
        auto file = _model.getFileIndex(index);
        if (text_value.isEmpty())
        {
            value = nullptr;
        }
        else {
            if (value == nullptr)
            {
                value = _model.getResourceManager().createObject<hkbStringEventPayload>(file, &hkbStringEventPayloadClass);
            }
            value->m_data = text_value.toUtf8().constData();
        }
        QVariant new_value; new_value.setValue(HkxItemPointer(value));
        model->setData(index, new_value, Qt::EditRole);
    }
    else if (index.data().canConvert<HkxItemPointer>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        HkxItemPointer ptr = index.data().value<HkxItemPointer>();
        //auto objects = _manager.findCompatibleNodes(ptr.file_index(), ptr.field_class());
        QVariant new_value;
        if (ptr_editor->currentIndex() == 0)
        {
            //new_value.setValue(HkxItemPointer(ptr.file_index(), 0, ptr.field_class()));
        }
        //else if (!objects.empty())
        //{
        //    auto selected_object = objects[ptr_editor->currentIndex()-1];
        //    //new_value.setValue(HkxItemPointer(ptr.file_index(), selected_object.second->m_object, ptr.field_class()));
        //}
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
    else if (index.data().canConvert<HkxItemFlags>()) {
        QWidget* ptr_editor = dynamic_cast<QWidget*>(editor);
        HkxItemFlags data = index.data().value<HkxItemFlags>();
        data.FillFromEditor(ptr_editor);
        QVariant new_value; new_value.setValue(data);
        model->setData(index, new_value, Qt::EditRole);
    }
    else if (index.data().canConvert<HkxItemEvent>()) {
        setModelDataReference<HkxItemEvent>(editor, model, index);
    }
    else if (index.data().canConvert<HkxItemVar>()) {
        setModelDataReference<HkxItemVar>(editor, model, index);
    }
    else if (index.data().canConvert<HkxItemBone>()) {
        setModelDataReference<HkxItemBone>(editor, model, index);
    }
    else if (index.data().canConvert<HkxItemRagdollBone>()) {
        setModelDataReference<HkxItemRagdollBone>(editor, model, index);
    }
    else if (index.data().canConvert<HkxItemFSMState>()) {
        QComboBox* ptr_editor = dynamic_cast<QComboBox*>(editor);
        auto value = index.data().value<HkxItemFSMState>();
        model->setData(index, ptr_editor->currentIndex(), Qt::EditRole);
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ItemsDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}