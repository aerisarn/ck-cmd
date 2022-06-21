#pragma once

#include <QStringListModel>

class StringListModel : public QStringListModel {
	Q_OBJECT

	QStringList _headerData;

public:
	explicit StringListModel(const QStringList& strings, QObject* parent = nullptr);

	Q_INVOKABLE virtual QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
	virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value,
		int role = Qt::EditRole) override;

};