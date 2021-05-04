#include "QtUtils.h"

void updateBar(QProgressBar* bar, int curValue, int maxValue) {
	if (maxValue == 0) {
		curValue = 1;
		maxValue = 1;
	}
	int percent = 100 * (float)curValue / (float)maxValue;
	bar->setMaximum(maxValue);
	bar->setValue(curValue);
	QString newText = QString::number(curValue) + "/" + QString::number(maxValue) + " - " + QString::number(percent) + "%";
	bar->setFormat(newText);
}

void updateBarFlatValue(QProgressBar* bar, int curValue, int maxValue) {
	QString newText = "";
	if (maxValue == 0) {
		curValue = 1;
		maxValue = 1;
	}
	else {
		newText = QString::number(curValue);
	}
	bar->setMaximum(maxValue);
	bar->setValue(curValue);
	bar->setFormat(newText);
}