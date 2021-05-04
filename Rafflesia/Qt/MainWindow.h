#pragma once

#include <QWidget>
#include <QDockWidget>
#include "ui_MainWindow.h"

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	bool isClientAttached(int pid);
	void attachClient(int pid);
	void setTabName(int id, std::string name);

private slots:
	void onTabClosed(int pid);

private:
	Ui::MainWindow ui;
};
