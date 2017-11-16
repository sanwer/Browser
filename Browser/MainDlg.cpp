#include "stdafx.h"
#include "BrowserManager.h"
#include <windows.h>
#include <objbase.h>

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	int result = 0;
	HRESULT Hr = ::CoInitialize(NULL);
	if(FAILED(Hr)) return 0;

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	DuiLib::CPaintManagerUI::SetResourceType(DuiLib::UILIB_ZIPRESOURCE);
	DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::CPaintManagerUI::GetInstancePath());
	DuiLib::CPaintManagerUI::SetResourceZip(MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
	
	CefMainArgs main_args(hInstance);
	CefRefPtr<CefApp> app;
	CefSettings settings;// Specify CEF global settings here.
	void* sandbox_info = NULL;
#ifdef CEF_USE_SANDBOX
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#else
	settings.no_sandbox = true;
#endif
	//CefRefPtr<Browser::ClientAppBrowser> app(new Browser::ClientAppBrowser);
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    app = new Browser::ClientAppBrowser();
	command_line->InitFromString(::GetCommandLineW());
	const std::string& process_type = command_line->GetSwitchValue("type");

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	if (exit_code >= 0)
		return exit_code;

	WCHAR szBuffer[MAX_PATH];
	::ZeroMemory(&szBuffer, sizeof(szBuffer));
	GetTempPathW(MAX_PATH,szBuffer);
	std::wstring sBuffer = szBuffer;
	sBuffer += L"Browser";
	CefString(&settings.cache_path).FromWString(sBuffer);

	settings.ignore_certificate_errors = true;

	//settings.command_line_args_disabled = true;

	CefString(&settings.locale).FromASCII("zh-CN");

#ifndef _DEBUG
	settings.log_severity = LOGSEVERITY_DISABLE;
#endif
	//settings.multi_threaded_message_loop = true;

	// Initialize CEF.
	CefInitialize(main_args, settings, app, sandbox_info);

	scoped_ptr<Browser::BrowserManager> pBrowserManager(new Browser::BrowserManager(true));
	scoped_ptr<Browser::MessageLoop> message_loop;
	message_loop.reset(new Browser::MessageLoop);

	pBrowserManager->CreateRootWindow();

	//DuiLib::CPaintManagerUI::MessageLoop();
	result = message_loop->Run();

	DuiLib::CPaintManagerUI::Term();

	// Shut down CEF.
	CefShutdown();
	
	message_loop.reset();

	::CoUninitialize();
	return result;
}
