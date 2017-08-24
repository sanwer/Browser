#include "StdAfx.h"
#include "JSHandler.h"
#include <ShellAPI.h>
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")

namespace Browser
{
	bool CJSHandler::Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception)
	{
		if (name == L"GetComputerName") {
			TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH+1];
			memset(szComputerName,0,sizeof(szComputerName));
			DWORD dwNameLen = MAX_COMPUTERNAME_LENGTH;  
			GetComputerName(szComputerName, &dwNameLen);
			retval = CefV8Value::CreateString(szComputerName);
			return true;
		}
		return false;
	}
}
