#include <QStyledItemDelegate>
#include <src/hkx/ResourceManager.h>

class RefDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    const ckcmd::HKX::ResourceManager& _manager;

    QString ObjectText(int object_index, size_t file_index) const;

public:
    RefDelegate(const ckcmd::HKX::ResourceManager& _manager, QObject* parent = 0);

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