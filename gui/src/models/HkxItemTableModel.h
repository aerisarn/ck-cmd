#pragma once

#include <QAbstractTableModel>
#include <QUndoStack>
#include <core/HKXWrangler.h>
#include <src/hkx/ISpecialFieldsHandler.h>
#include <src/hkx/HkxItemPointer.h>
#include <src/hkx/CommandManager.h>

namespace ckcmd {
    namespace HKX {

        class HkxItemTableModel : public QAbstractTableModel, public SpecialFieldsListener {
            Q_OBJECT

            
            friend class ChangeValue;

            CommandManager& _command_manager;
            hkVariant* _variant;
            hkVariant* _parent;
            int _file;
            hkVariant* getObject(const QModelIndex& index) const;

            bool indexValid(const QModelIndex& index) const;

            class ChangeValue : public QUndoCommand {
                QVariant _old_value;
                QVariant _new_value;
                QModelIndex _index;
                int _file;
                hkVariant* _variant;
                HkxItemTableModel& _model;
            public:
                ChangeValue(HkxItemTableModel& model, const QModelIndex& index, const QVariant& new_value) :
                    _new_value(new_value),
                    _index(index),
                    _file(model.file()),
                    _variant(model.variant()),
                    _model(model)
                {}

                virtual void undo() override {
                    _new_value = _model.internalSetData(_file, _variant, _index, _old_value, Qt::EditRole);
                    emit _model.dataChanged(_index, _index, { Qt::DisplayRole, Qt::EditRole });
                }
                virtual void redo() override {
                    _old_value = _model.internalSetData(_file, _variant, _index, _new_value, Qt::EditRole);
                    emit _model.dataChanged(_index, _index, { Qt::DisplayRole, Qt::EditRole });
                }

            };

            QVariant internalSetData(int file, hkVariant* variant, const QModelIndex& index, const QVariant& value,
                int role = Qt::EditRole);

        public:

            HkxItemTableModel(
                CommandManager& command_manager,
                hkVariant* variant, 
                int file, hkVariant* 
                variant_parent, 
                QObject* parent = 0);
            ~HkxItemTableModel() {}

            QVariant headerData(int section, Qt::Orientation orientation,
                int role = Qt::DisplayRole) const override;


            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
            bool setData(const QModelIndex& index, const QVariant& value,
                int role = Qt::EditRole) override;

            int rowCount(const QModelIndex& parent = QModelIndex()) const override;
            int columnCount(const QModelIndex& parent = QModelIndex()) const override;

            Qt::ItemFlags flags(const QModelIndex& index) const override;

            hkVariant* variant() const { return _variant; }
            void setVariant(int file, hkVariant* variant) { beginResetModel(); _file = file, _variant = variant; endResetModel(); }
            int file() const { return _file; }

        signals:

            void HkxItemPointerChanged(HkxItemPointer old_value, HkxItemPointer new_value, int file, hkVariant* variant);

        };
    }
}