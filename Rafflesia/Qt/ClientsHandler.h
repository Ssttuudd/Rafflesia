#pragma once

#include <QTimer>
#include <QWidget>

#include "ui_ClientsHandler.h"

class MainWindow;
class ClientItem;

class ClientsHandler : public QWidget
{
	Q_OBJECT

public:
	ClientsHandler(QWidget *parent = Q_NULLPTR);
	~ClientsHandler();

	void updateClientProcess();
	void init(MainWindow* playerTab);
	QListWidgetItem* getRowItem(int pid);

public slots:
	void onHideButtonClicked();
	void onAttachToAllClicked();
	void onAttachClicked();
	void onSelectedClientChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void onAutoAttachChanged( bool value );

private:
	void tryAutoAttachClient( ClientItem* item );

	Ui::ClientsHandler ui;
	QTimer updateTimer;
	MainWindow* tabHolder{ nullptr };
	int selectedPid{ 0 };
	bool hidden{ false };
};
