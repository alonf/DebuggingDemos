// BasicDebugger.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <memory>

using namespace std;

void ExitOnError(bool bResult = false)
{
    if (bResult)
        return;
    //else
    
    const DWORD error = GetLastError();
    cerr << "Error creating process: " << error << endl;
    ExitProcess(error);
}

void PrintDebugString(DWORD processId, LPVOID textAddress, SIZE_T length, bool isUnicode);
void PrintFilePathFromHandle(HANDLE hFile);

int main()
{
    STARTUPINFOW startupInfo;
    memset(&startupInfo, 0, sizeof startupInfo);
    startupInfo.cb = sizeof startupInfo;

    PROCESS_INFORMATION processInformation;


    //Create the process (debugge) under the debugger
    BOOL bResult = ::CreateProcess(L"DebugEvents.exe", nullptr, nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS, nullptr, nullptr, &startupInfo, &processInformation);
    ExitOnError(bResult);

    ::CloseHandle(processInformation.hThread);
    ::CloseHandle(processInformation.hProcess);

    while(true)
    {
        DWORD ContinueStatus = DBG_CONTINUE;
        DEBUG_EVENT debugEvent;
        bResult = ::WaitForDebugEventEx(&debugEvent, INFINITE);
        ExitOnError(bResult);

        switch (debugEvent.dwDebugEventCode)
        {
        case CREATE_PROCESS_DEBUG_EVENT:
            cout << L"New process.";
            PrintFilePathFromHandle(debugEvent.u.CreateProcessInfo.hFile);
            
            break;

        case CREATE_THREAD_DEBUG_EVENT:
            cout << "New thread. Start Address: " <<
                hex << debugEvent.u.CreateThread.lpStartAddress << endl;
            break;

        case EXCEPTION_DEBUG_EVENT:
            ContinueStatus = DBG_EXCEPTION_NOT_HANDLED;

            cout << "Exception has occured. Exception code: " << 
                hex << debugEvent.u.Exception.ExceptionRecord.ExceptionCode << " ";
            switch (debugEvent.u.Exception.ExceptionRecord.ExceptionCode)
            {
            case EXCEPTION_BREAKPOINT:
                cout << " ==> Breakpoint!!!";
                ContinueStatus = DBG_CONTINUE;
                break;

            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                cout << " ==> Divide By Zero!!!";
                break;
            default: ;
            }
            cout << endl;
            break;

        case EXIT_PROCESS_DEBUG_EVENT:
            //Exit also the debugger
            cout << "The debugee process has exited." << endl;
            cin.get();
            ExitProcess(0);

        case EXIT_THREAD_DEBUG_EVENT:
            cout << "Thread exit. Exit code: " <<
                hex << debugEvent.u.ExitThread.dwExitCode << endl;
            break;

        case LOAD_DLL_DEBUG_EVENT:
            cout << "DLL Loaded. ";
            PrintFilePathFromHandle(debugEvent.u.LoadDll.hFile);
            break;

        case UNLOAD_DLL_DEBUG_EVENT:
            cout << "Dll unloaded. " << endl;
            break;

        case OUTPUT_DEBUG_STRING_EVENT:
            PrintDebugString(debugEvent.dwProcessId, debugEvent.u.DebugString.lpDebugStringData, debugEvent.u.DebugString.nDebugStringLength, debugEvent.u.DebugString.fUnicode);
            break;
        case RIP_EVENT:

            break;

        default: ;
        }

        bResult = ::ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, ContinueStatus);
        ExitOnError(bResult);

    }
}

void PrintFilePathFromHandle(HANDLE hFile)
{
    if ((hFile != nullptr) && (hFile != INVALID_HANDLE_VALUE))
    {
        wchar_t fileName[MAX_PATH];
        GetFinalPathNameByHandle(hFile, fileName, MAX_PATH, FILE_NAME_NORMALIZED);
        wcout << " Path: " << fileName;
        ::CloseHandle(hFile);
    }
    cout << endl;
}

void PrintDebugString(DWORD processId, LPVOID textAddress, SIZE_T length, bool isUnicode)
{
    const HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, processId);
    if (hProcess == nullptr)
    {
        ExitOnError();
    }

    const auto buffer = make_unique<char[]>(length);
    
    SIZE_T nSize;
    ReadProcessMemory(hProcess, textAddress, buffer.get(), length, &nSize);

    if (isUnicode)
        wcout << reinterpret_cast<wchar_t *>(buffer.get()) << endl;
    else
        cout << buffer.get() << endl;

    ::CloseHandle(hProcess);
    
}



