#include "ClientItem.h"

ClientItem::ClientItem(QWidget *parent)
	: QWidget(parent),
	validIcon(":/validIcon.png"),
	invalidIcon(":/invalidIcon.png")
{
	ui.setupUi(this);
	if (validIcon.isNull()) {
		int a = 0;
		a++;
	}
}

ClientItem::~ClientItem()
{
}

void ClientItem::setInfos(int _pid, std::string name, bool attached) {
	static QString attachedStylesheet = "QLabel { color : #5db01e; }";
	static QString notAttachedStylesheet = "QLabel { color : #000000; }";
	pid = _pid;
	QString number = QStringLiteral("%1").arg(_pid, 5, 10, QLatin1Char(' '));
	ui.pidLabel->setText(number);
	ui.nameLabel->setText(name.c_str());
	if (attached) {
		ui.nameLabel->setStyleSheet(attachedStylesheet);
		ui.pidLabel->setStyleSheet(attachedStylesheet);
		ui.label->setStyleSheet(attachedStylesheet);
		ui.iconLabel->setPixmap(QPixmap(":/icons/validIcon.png"));
	}
	else {
		ui.nameLabel->setStyleSheet(notAttachedStylesheet);
		ui.pidLabel->setStyleSheet(notAttachedStylesheet);
		ui.label->setStyleSheet(notAttachedStylesheet);
		ui.iconLabel->setPixmap(QPixmap(":/icons/invalidIcon.png"));
	}
}