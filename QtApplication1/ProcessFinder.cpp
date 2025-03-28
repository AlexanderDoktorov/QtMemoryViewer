#include "ProcessFinder.h"
#include <windows.h>
#include <psapi.h>
#include <tchar.h> // For TCHAR and related macros
#include <iostream>

std::vector<FProcessDesc> FProcessFinder::GetProcessList(size_t MaxListSize)
{
    std::vector<DWORD> ProcessesIDs{};
    ProcessesIDs.resize(MaxListSize);

    DWORD cbNeeded = DWORD();
    DWORD cProcesses = DWORD();

    if ( !EnumProcesses( ProcessesIDs.data(), ProcessesIDs.size() * sizeof(DWORD), &cbNeeded))
        return {};

    size_t NumProcesses = cbNeeded / sizeof(DWORD);

    std::vector<FProcessDesc> ReturnList{};
    ReturnList.reserve(NumProcesses);

    for (size_t i = 0; i < NumProcesses; ++i)
    {
        const auto& ID = ProcessesIDs[i];

        // Get a handle to the process
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ID);

        // Check if we have a valid handle
        if (hProcess != NULL) {
            HMODULE hMod;
            DWORD cbNeeded;

            TCHAR processName[MAX_PATH] = TEXT("<unknown>");

            // Get the first module in the process (which is the executable)
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                if (GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR)) >= 0)
                {
                    ReturnList.emplace_back(QString::fromWCharArray(processName), ID);
                }
            }

            CloseHandle(hProcess);
        }
    }
    return ReturnList;
}

bool FProcessFinder::IsRunningAsAdministrator()
{
    BOOL isElevated = FALSE;
    HANDLE tokenHandle = NULL;
    TOKEN_ELEVATION tokenElevation{};
    DWORD returnLength = 0;

    // Open the access token of the current process
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &tokenHandle)) {
        // Get the elevation information for the token
        if (GetTokenInformation(tokenHandle, TokenElevation, &tokenElevation, sizeof(tokenElevation), &returnLength)) {
            isElevated = tokenElevation.TokenIsElevated;
        }
        CloseHandle(tokenHandle); // Close the token handle when done
    } else {
        std::cerr << "Failed to open process token." << std::endl;
    }

    return isElevated;
}

std::vector<MEMORY_BASIC_INFORMATION> FProcessDesc::GatherProcessMemoryRegions(DWORD ProcessID)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcessID);

    if (hProcess == NULL) {
        std::cerr << "Failed to open process." << std::endl;
        return {};
    }

    // Initialize memory address pointer
    unsigned char* address = nullptr;

    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T bytesRead;

    std::vector<MEMORY_BASIC_INFORMATION> ReturnValue{};

    // Iterate through the virtual memory regions
    while ((bytesRead = VirtualQueryEx(hProcess, address, &mbi, sizeof(mbi))) != 0) {
        // Print information about the memory region
        ReturnValue.emplace_back(mbi);

        // Move to the next memory region
        address += mbi.RegionSize;
    }

    // Close the process handle
    CloseHandle(hProcess);

    return ReturnValue;
}
