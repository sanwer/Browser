#include "stdafx.h"
#include "MinHook\MinHook.h"

typedef BOOL (WINAPI *CreateProcessAPtr)(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);

typedef BOOL (WINAPI *CreateProcessWPtr)(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);

CreateProcessWPtr fpCreateProcessW = NULL;
CreateProcessAPtr fpCreateProcessA = NULL;

BOOL WINAPI DetourCreateProcessA(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	std::string strCommandLine = lpCommandLine;

	if (std::string::npos != strCommandLine.find("echo NOT SANDBOXED"))
	{
		//MessageBoxA(GetActiveWindow(), strCommandLine.c_str(), "createprocesA", MB_OK);
		return TRUE;
	}
	else
	{
		return (fpCreateProcessA)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}

}

BOOL WINAPI DetourCreateProcessW(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	std::wstring strCommandLine(lpCommandLine);

	//MessageBoxW(GetActiveWindow(), strCommandLine.c_str(), L"createproceW", MB_OK);
	if (std::string::npos != strCommandLine.find(L"echo NOT SANDBOXED"))
	{
		//MessageBoxW(GetActiveWindow(), strCommandLine.c_str(), L"createproceW", MB_OK);
		return TRUE;
	}
	else
	{
		return (fpCreateProcessW)(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	}
}
// Helper function for MH_CreateHookApi().
template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
    return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

BOOL WINAPI Initialize(VOID)
{
	// Initialize MinHook.
	if (MH_Initialize() != MH_OK)
		return FALSE;

	//if (MH_CreateHook(&CreateProcessA, &DetourCreateProcessA, reinterpret_cast<LPVOID*>(&fpCreateProcessA)) == MH_OK)
    if (MH_CreateHookApiEx(L"kernel32", "CreateProcessA", &DetourCreateProcessA, &fpCreateProcessA) != MH_OK)
		return FALSE;
    //if (MH_CreateHook(&CreateProcessW, &DetourCreateProcessW, reinterpret_cast<LPVOID*>(&fpCreateProcessW)) != MH_OK)
    if (MH_CreateHookApiEx(L"kernel32", "CreateProcessW", &DetourCreateProcessW, &fpCreateProcessW) != MH_OK)
		return FALSE;
	if (MH_EnableHook(&CreateProcessA) != MH_OK)
		return FALSE;
	if (MH_EnableHook(&CreateProcessW) != MH_OK)
		return FALSE;

	return TRUE;
}

BOOL WINAPI Uninitialize(VOID)
{
    // Uninitialize MinHook.
    return MH_Uninitialize() == MH_OK;
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Initialize();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Uninitialize();
		break;
	}
	return TRUE;
}
