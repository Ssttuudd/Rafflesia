#include "MainWindow.h"

#include "PlayerTab.h"
#include "Injection/Injector.h"

#include "Bot/Bot.h"
#include "Game/Game.h"
#include "Game/Network/PacketHandler.h"

MainWindow::MainWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.clientsHandler->init(this);

	checkFiles();
}

void MainWindow::attachClient( int pid ) {
	if( pid > 0 && !isClientAttached( pid ) )
	{
		auto newTab = new PlayerTab( pid, ui.tabWidget->count(), this );
		connect( newTab, &PlayerTab::tabClose, this, &MainWindow::onTabClosed );
		int tabId = ui.tabWidget->addTab( newTab, std::to_string( pid ).c_str() );

		if( !injectRoot( pid ) )
		{
			ui.tabWidget->removeTab( tabId );
			delete newTab;
		}
	}
}

MainWindow::~MainWindow()
{
	for (int i = ui.tabWidget->count(); i > 0; i--)
	{
		delete ui.tabWidget->widget(i - 1);
		ui.tabWidget->removeTab(i - 1);
	}
}

bool MainWindow::isClientAttached(int pid) {
	for (int i = 0; i < ui.tabWidget->count(); ++i)
	{
		PlayerTab* tab = (PlayerTab*)ui.tabWidget->widget(i);
		if (tab->getPid() == pid) {
			return true;
		}
	}
	return false;
}

void MainWindow::setTabName(int id, std::string name) {
	ui.tabWidget->setTabText(id, name.c_str());
}

void MainWindow::onTabClosed(int pid) {
	for (int i = 0; i < ui.tabWidget->count(); ++i)
	{
		PlayerTab* tab = (PlayerTab*)ui.tabWidget->widget(i);
		if (tab->getPid() == pid) {
			tab->stop();
			ui.tabWidget->removeTab(i);
			delete tab;
			return;
		}
	}
}