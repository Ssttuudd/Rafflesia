#include "ClientsHandler.h"

#include <algorithm>
#include <Windows.h>
#include <Psapi.h>
#include <QListWidget>
#include <string>
#include <vector>
#include <TlHelp32.h>

#include "ClientItem.h"
#include "MainWindow.h"
#include "PlayerTab.h"
#include "Injection/Injector.h"
#include "QtWidgets/QRadioButton"
#include "QtUtils.h"

#include <QSettings>

ClientsHandler::ClientsHandler( QWidget* parent )
	: QWidget( parent )
{
	ui.setupUi( this );

	QSettings& settings = getGlobalSettings();
	ui.autoPatchClients->setChecked( settings.value( "autoPatchClients" ).toBool() );
	ui.autoAttachBot->setChecked( settings.value( "autoAttachBot" ).toBool() );

	connect( ui.hideButton, &QPushButton::clicked, this, &ClientsHandler::onHideButtonClicked );
	connect( ui.attachToAllButton, &QPushButton::clicked, this, &ClientsHandler::onAttachToAllClicked );
	connect( ui.attachButton, &QPushButton::clicked, this, &ClientsHandler::onAttachClicked );
	connect( ui.listWidget, &QListWidget::currentItemChanged, this, &ClientsHandler::onSelectedClientChanged );
	connect( ui.autoPatchClients, &QCheckBox::toggled, this, &ClientsHandler::onAutoAttachChanged );
	connect( ui.autoAttachBot, &QCheckBox::toggled, this, &ClientsHandler::onAutoAttachChanged );

	connect( &updateTimer, &QTimer::timeout, [this] () {
		updateClientProcess();
	} );
	updateTimer.start( 1000 );
}

ClientsHandler::~ClientsHandler()
{
}

void ClientsHandler::init( MainWindow* _tabHolder ) {
	tabHolder = _tabHolder;
}

void ClientsHandler::onHideButtonClicked() {
	if( hidden ) {
		ui.widgetContent->show();
		ui.hideButton->setText( "-" );
	}
	else
	{
		ui.widgetContent->hide();
		ui.hideButton->setText( "+" );
	}
	hidden = !ui.widgetContent->isVisible();
}

void ClientsHandler::onAttachToAllClicked() {
	for( int i = 0; i < ui.listWidget->count(); ++i )
	{
		auto* item = ui.listWidget->item( i );
		auto* client = (ClientItem*) ui.listWidget->itemWidget( item );
		tabHolder->attachClient( client->getPid() );
	}
}

void ClientsHandler::onAttachClicked() {
	tabHolder->attachClient( selectedPid );
}

void ClientsHandler::onSelectedClientChanged( QListWidgetItem* current, QListWidgetItem* previous ) {
	bool canAttach = false;
	if( current ) {
		auto* data = (ClientItem*) ui.listWidget->itemWidget( current );
		selectedPid = data->getPid();
		canAttach = tabHolder->isClientAttached( selectedPid );
	}

	ui.attachButton->setEnabled( !canAttach );
}

void ClientsHandler::onAutoAttachChanged( bool value )
{
	for( int i = 0; i < ui.listWidget->count(); ++i )
	{
		auto* item = ui.listWidget->item( i );
		auto* data = (ClientItem*) ui.listWidget->itemWidget( item );
		tryAutoAttachClient( data );
	}

	QSettings& settings = getGlobalSettings();
	settings.setValue( "autoAttachBot", ui.autoAttachBot->isChecked() );
	settings.setValue( "autoPatchClients", ui.autoPatchClients->isChecked());
}

void ClientsHandler::tryAutoAttachClient( ClientItem* itemWidget )
{
	if( itemWidget )
	{
		int pid = itemWidget->getPid();
		if( ui.autoPatchClients->isChecked() && !itemWidget->isAacAttached() && !isDllLoadedInProcess( pid, L"AAD.dll" ) )
			itemWidget->setAacATtached( injectAAD( pid ) );

		if( ui.autoAttachBot->isChecked() && !tabHolder->isClientAttached( pid ) )
			tabHolder->attachClient( pid );
	}
}

struct EnumData
{
	DWORD dwProcessId;
	HWND hWnd;
};

// Application-defined callback for EnumWindows
BOOL CALLBACK enumProc( HWND hWnd, LPARAM lParam ) {
	// Retrieve storage location for communication data
	EnumData& ed = *(EnumData*) lParam;
	DWORD dwProcessId = 0x0;
	// Query process ID for hWnd
	GetWindowThreadProcessId( hWnd, &dwProcessId );
	// Apply filter - if you want to implement additional restrictions,
	// this is the place to do so.
	if( ed.dwProcessId == dwProcessId ) {
		// Found a window matching the process ID
		ed.hWnd = hWnd;
		// Report success
		SetLastError( ERROR_SUCCESS );
		// Stop enumeration
		return FALSE;
	}
	// Continue enumeration
	return TRUE;
}

HWND findWindowFromProcessId( DWORD dwProcessId ) {
	EnumData ed = { dwProcessId };
	if( !EnumWindows( enumProc, (LPARAM) &ed ) &&
		( GetLastError() == ERROR_SUCCESS ) ) {
		return ed.hWnd;
	}
	return NULL;
}

QListWidgetItem* ClientsHandler::getRowItem( int pid ) {
	for( int i = 0; i < ui.listWidget->count(); ++i )
	{
		auto* item = ui.listWidget->item( i );
		auto* data = (ClientItem*) ui.listWidget->itemWidget( item );
		if( data && data->getPid() == pid ) {
			return item;
		}
	}
	return nullptr;
}

void ClientsHandler::updateClientProcess() {
	if( hidden ) {
		return;
	}

	std::vector<int> listedPid;
	for( int i = 0; i < ui.listWidget->count(); ++i )
	{
		auto* item = ui.listWidget->item( i );
		auto* data = (ClientItem*) ui.listWidget->itemWidget( item );
		if( data ) {
			listedPid.push_back( data->getPid() );
		}
	}

	DWORD process_list[516], bytes_needed;
	if( K32EnumProcesses( process_list, sizeof( process_list ), &bytes_needed ) )
	{
		for( size_t index = 0; index < bytes_needed / sizeof( std::uint32_t ); index++ )
		{
			HANDLE handle( OpenProcess( PROCESS_ALL_ACCESS, false, process_list[index] ) );
			char name_buffer[MAX_PATH];
			GetModuleBaseNameA( handle, nullptr, name_buffer, MAX_PATH );
			const std::string name( name_buffer );

			if( name == "L2.bin" || name == "L2.exe" || name == "l2.exe" ) {
				int pid = process_list[index];

				HWND hwnd = findWindowFromProcessId( pid );
				int len = GetWindowTextLength( hwnd ) + 1;
				std::vector<char> buf( len );
				GetWindowTextA( hwnd, &buf[0], len );
				std::string wndName = &buf[0];

				// if the pid is in the list already, remove the pid from the reference list update the infos
				if( auto it = std::find( listedPid.begin(), listedPid.end(), pid ); it != std::end( listedPid ) ) {
					listedPid.erase( it );
					auto item = getRowItem( pid );
					auto* data = (ClientItem*) ui.listWidget->itemWidget( item );
					data->setInfos( pid, wndName, tabHolder->isClientAttached( pid ) );
					continue;
				}
				else {
					ClientItem* itemWidget = new ClientItem();
					itemWidget->setInfos( pid, wndName, tabHolder->isClientAttached( pid ) );
					QListWidgetItem* item = new QListWidgetItem;

					tryAutoAttachClient( itemWidget );

					ui.listWidget->addItem( item );
					ui.listWidget->setItemWidget( item, itemWidget );
				}
			}
		}

		// Remaining pids are dead processes
		for( const auto pid : listedPid ) {
			auto item = getRowItem( pid );
			ui.listWidget->takeItem( ui.listWidget->row( item ) );
			delete item;
		}
	}
}