#pragma once

#include <vector>
#include "windows.h"
#include "qstring.h"

typedef unsigned long DWORD;

static QString toString(const MEMORY_BASIC_INFORMATION& mbi) {

    QString Result{};
    Result = Result.append("Base Address: %1 | ").arg(-1);
    Result = Result.append("Region Size: %1 | ").arg(mbi.RegionSize);

    // Memory state
    if (mbi.State == MEM_COMMIT) {
        Result.append("State: Committed | ");
    } else if (mbi.State == MEM_RESERVE) {
        Result.append("State: Reserved | ");
    } else if (mbi.State == MEM_FREE) {
        Result.append("State: Free | ");
    }

    // Memory type
    if (mbi.Type == MEM_IMAGE) {
        Result.append("Type: Image | ");
    } else if (mbi.Type == MEM_MAPPED) {
        Result.append("Type: Mapped | ");
    } else if (mbi.Type == MEM_PRIVATE) {
        Result.append("Type: Private | ");
    }

    // Protection
    if (mbi.Protect & PAGE_READONLY) {
        Result.append("Protection: Read-Only |");
    } else if (mbi.Protect & PAGE_READWRITE) {
        Result.append("Protection: Read/Write |");
    } else if (mbi.Protect & PAGE_EXECUTE) {
        Result.append("Protection: Execute |");
    } else if (mbi.Protect & PAGE_EXECUTE_READWRITE) {
        Result.append("Protection: Execute/Read/Write");
    }

    return Result;
}

struct FProcessDesc
{
    FProcessDesc() = default;
    FProcessDesc(const QString& Name, DWORD ProcessID) : 
        ProcessName(Name), ProcessID(ProcessID) 
    {}

    static std::vector<MEMORY_BASIC_INFORMATION> GatherProcessMemoryRegions(DWORD ProcessID);

    QString ProcessName = "None";
    DWORD ProcessID = 0;
};

class FProcessFinder
{
public:
    static std::vector<FProcessDesc> GetProcessList(size_t MaxListSize);
    static bool IsRunningAsAdministrator();
};