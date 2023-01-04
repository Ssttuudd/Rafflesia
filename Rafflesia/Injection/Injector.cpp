
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>

#include "Injector.h"
#include <psapi.h>

std::wstring getExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName( NULL, buffer, MAX_PATH );
	std::wstring::size_type pos = std::wstring( buffer ).find_last_of( L"\\/" );
	return std::wstring( buffer ).substr( 0, pos );
}

inline bool fileExists( const std::wstring& src ) {
	return ( _waccess( src.c_str(), 0 ) == 0 );
}

void checkFiles()
{
	auto exePath = getExePath() + L"\\";
	if( !fileExists( exePath + INJECTOR_EXE_PATH ) )
		MessageBox( nullptr, L"Couldn't find the injector executable alongside Rafflesia.exe.", L"ThreadHijacking.exe is missing!", MB_ICONERROR | MB_OK );
	if( !fileExists( exePath + ROOT_DLL_PATH ) )
		MessageBox( nullptr, L"Couldn't find the injected dll alongside Rafflesia.exe.", L"Root.dll is missing!", MB_ICONERROR | MB_OK );
}

DWORD GetProcessIdFromName( const wchar_t* targetProcess )
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof( PROCESSENTRY32 );

	HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
	if( Process32First( snapshot, &entry ) == TRUE )
	{
		while( Process32Next( snapshot, &entry ) == TRUE )
		{
			if( _wcsicmp( entry.szExeFile, targetProcess ) == 0 )
			{
				CloseHandle( snapshot );
				return entry.th32ProcessID;
			}
		}
	}
	CloseHandle( snapshot );
	return 0;
}

bool isDllLoadedInProcess( DWORD pid, const wchar_t* dllName )
{
	HANDLE processHandle = OpenProcess( PROCESS_ALL_ACCESS, false, pid );
	if( !processHandle ) {
		return false;
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	if( EnumProcessModules( processHandle, hMods, sizeof( hMods ), &cbNeeded ) )
	{
		for( unsigned int i = 0; i < ( cbNeeded / sizeof( HMODULE ) ); i++ )
		{
			TCHAR moduleName[MAX_PATH];

			if( GetModuleFileNameEx( processHandle, hMods[i], moduleName,
				sizeof( moduleName ) / sizeof( TCHAR ) ) )
			{
				if( _wcsicmp( dllName, moduleName ) == 0 )
				{
					CloseHandle( processHandle );
					return true;
				}
			}
		}
	}

	CloseHandle( processHandle );
	return false;
}

bool injectAAD( int targetProcess )
{
	auto exePath = getExePath() + L"\\";
	if( !fileExists( exePath + INJECTOR_EXE_PATH ) )
	{
		MessageBox( nullptr, L"Couldn't find ThreadHijacking.exe alongside Rafflesia.exe.", L"ThreadHijacking.exe is missing!", MB_ICONERROR | MB_OK );
		return false;
	}

	if( !fileExists( exePath + AAD_DLL_PATH ) )
	{
		MessageBox( nullptr, L"Couldn't find AAD.dll alongside Rafflesia.exe.", L"AAD.dll is missing!", MB_ICONERROR | MB_OK );
		return false;
	}

	if( deleguateInjection( targetProcess, exePath + AAD_DLL_PATH ) != ERROR_SUCCESS )
	{
		MessageBox( nullptr, L"Couldn't inject anti anti-cheat dll.", L"Injection failed", MB_ICONERROR | MB_OK );
		return false;
	}

	return true;
}

bool injectRoot( int targetProcess )
{
	auto exePath = getExePath() + L"\\";
	if( !fileExists( exePath + INJECTOR_EXE_PATH ) )
	{
		MessageBox( nullptr, L"Couldn't find ThreadHijacking.exe alongside Rafflesia.exe.", L"ThreadHijacking.exe is missing!", MB_ICONERROR | MB_OK );
		return false;
	}

	if( !fileExists( exePath + ROOT_DLL_PATH ) )
	{
		MessageBox( nullptr, L"Couldn't find AAD.dll alongside Rafflesia.exe.", L"AAD.dll is missing!", MB_ICONERROR | MB_OK );
		return false;
	}

	if( deleguateInjection( targetProcess, exePath + ROOT_DLL_PATH ) != ERROR_SUCCESS )
	{
		MessageBox( nullptr, L"Couldn't inject root dll.", L"Injection failed", MB_ICONERROR | MB_OK );
		return false;
	}
	return true;
}

DWORD deleguateInjection( int targetProcess, const std::wstring& dllName )
{
	// Open svchost.exe
	auto PID = GetProcessIdFromName( PPID_SPOOFED );
	if( !PID ) {
		return -1;
	}
	HANDLE processHandle = OpenProcess( PROCESS_ALL_ACCESS, false, PID );
	if( !processHandle ) {
		return -1;
	}

	/// Initialize the startupinfo and PPID
	STARTUPINFOEXW sInfo;
	PROCESS_INFORMATION pInfo;
	SIZE_T size = 0;
	ZeroMemory( &sInfo, sizeof( sInfo ) );
	ZeroMemory( &pInfo, sizeof( pInfo ) );

	// Get needed space for attribute list
	bool bRet = InitializeProcThreadAttributeList( NULL, 1, 0, &size );
	if( bRet || size == 0 ) {
		return -1;
	}
	// Allocate memory
	sInfo.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, size );
	if( !sInfo.lpAttributeList ) {
		return -1;
	}
	// Initialize attribute list
	bRet = InitializeProcThreadAttributeList( sInfo.lpAttributeList, 1, 0, &size );
	if( !bRet || !sInfo.lpAttributeList ) {
		return -1;
	}
	// Put in the parent process attribute
	bRet = UpdateProcThreadAttribute( sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &processHandle, sizeof( processHandle ), NULL, NULL );
	if( !bRet ) {
		return -1;
	}
	sInfo.StartupInfo.cb = sizeof( sInfo );

	// Initialize temp folder and command line
	WCHAR tempFolder[MAX_PATH];
	GetTempPathW( MAX_PATH, tempFolder );

	LPWSTR swCmdLine = (LPWSTR) malloc( MAX_PATH * 4 );
	if( !swCmdLine ) {
		return -1;
	}
	memset( swCmdLine, 0, MAX_PATH * 4 );

	lstrcatW( swCmdLine, INJECTOR_EXE_PATH );
	lstrcatW( swCmdLine, L" " );

	LPWSTR swPID = (LPWSTR) malloc( MAX_PATH );
	if( !swPID ) {
		return -1;
	}
	memset( swPID, 0, MAX_PATH );
	_itow( targetProcess, swPID, 10 );
	lstrcatW( swCmdLine, swPID );

	lstrcatW( swCmdLine, L" " );

	lstrcatW( swCmdLine, dllName.c_str() );

	int ret = 0;
	// Spawn the process
	bRet = CreateProcessW( NULL, swCmdLine, NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, tempFolder, (LPSTARTUPINFOW) &sInfo, &pInfo );
	if( !bRet ) {
		ret = GetLastError();
	}

	free( swPID );
	free( swCmdLine );

	CloseHandle( pInfo.hProcess );
	CloseHandle( pInfo.hThread );

	return ret;
}
