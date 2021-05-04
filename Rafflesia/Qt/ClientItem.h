#pragma once

#include <QWidget>
#include "ui_ClientItem.h"

class ClientItem : public QWidget
{
	Q_OBJECT

public:
	ClientItem(QWidget *parent = Q_NULLPTR);
	~ClientItem();

	void setInfos(int pid, std::string name, bool attached);

	int getPid() {
		return pid;
	}

private:
	Ui::ClientItem ui;
	int pid;
	QPixmap validIcon;
	QPixmap invalidIcon;

};
