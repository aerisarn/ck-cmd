#include <QStyledItemDelegate>

#include <src/models/ValuesProxyModel.h>
#include <src/hkx/ResourceManager.h>

class ItemsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    ckcmd::HKX::ValuesProxyModel& _model;
    ckcmd::HKX::ResourceManager& _manager;

public:
    ItemsDelegate(ckcmd::HKX::ValuesProxyModel& model, QObject* parent = 0);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};