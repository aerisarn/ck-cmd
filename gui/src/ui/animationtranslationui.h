#ifndef ANIMATIONTRANSLATIONUI_H
#define ANIMATIONTRANSLATIONUI_H

#include <QGroupBox>

#include "src/animData/skyrimanimdata.h"

class QGridLayout;
class QPushButton;
class DoubleSpinBox;
class TableWidget;

namespace UI {

	class AnimationTranslationUI final : public QGroupBox
	{
		Q_OBJECT
	public:
		AnimationTranslationUI();
		AnimationTranslationUI& operator=(const AnimationTranslationUI&) = delete;
		AnimationTranslationUI(const AnimationTranslationUI &) = delete;
		~AnimationTranslationUI() = default;
	public:
		void loadData(SkyrimAnimationTranslation *trans, qreal maxtime);
	signals:
		void returnToParent();
	private slots:
		void setLocalTime();
		void setX();
		void setY();
		void setZ();
	private:
		void toggleSignals(bool toggleconnections);
	private:
		static const QStringList headerLabels;
		SkyrimAnimationTranslation *bsData;
		QGridLayout *topLyt;
		QPushButton *returnPB;
		TableWidget *table;
		DoubleSpinBox *localTime;
		DoubleSpinBox *x;
		DoubleSpinBox *y;
		DoubleSpinBox *z;
	};

}

#endif // ANIMATIONTRANSLATIONUI_H
