#pragma once

#include <QObject>
#include <QVariant>
#include <map>

#include <src/items/HkxItemReal.h>

//this could have been an inner class if not a Q_OBJECT
class ModelWidgetSignalMapper : public QObject {
	Q_OBJECT

	//
	std::map < QObject*, std::pair<int, int> > _mapping;

public:

	void addMapping(QObject* obj, int model_row, int model_column)
	{
		_mapping.insert({ obj, {model_row, model_column} });
	}

Q_SIGNALS:
	void sendValue(int row, int column, QVariant value);
public slots:

	void receiveValue(QString value) {
		auto dest = _mapping.at(sender());
		emit sendValue(dest.first, dest.second, value);
	}

	void receiveValue(double value) {
		auto dest = _mapping.at(sender());
		emit sendValue(dest.first, dest.second, value);
	}

	void receiveValue(ckcmd::HKX::HkxItemReal value) {
		auto dest = _mapping.at(sender());
		QVariant qvalue; qvalue.setValue(value);
		emit sendValue(dest.first, dest.second, qvalue);
	}
};