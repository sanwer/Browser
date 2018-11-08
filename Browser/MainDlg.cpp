#include "stdafx.h"
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "ClientAppBrowser.h"
#include "ClientAppRenderer.h"
#include "MessageLoop.h"
#include "MainContext.h"
#include "BrowserDlgManager.h"
#include "ClientRunner.h"
#include "ClientSwitches.h"
#include <windows.h>
#include <objbase.h>
using namespace Browser;

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

	HMODULE hFlashTools = NULL;
#ifdef _DEBUG
	hFlashTools = LoadLibrary(_T("FlashTools_d.dll"));
#else
	hFlashTools = LoadLibrary(_T("FlashTools.dll"));
#endif

	CefMainArgs main_args(hInstance);
	CefSettings settings;
	void* sandbox_info = NULL;
#ifdef CEF_USE_SANDBOX
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#else
	settings.no_sandbox = true;
#endif
	// Parse command-line arguments.
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());

	CefRefPtr<CefApp> app;
	ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
	if (process_type == ClientApp::BrowserProcess)
		app = new ClientAppBrowser();
	else if (process_type == ClientApp::RendererProcess)
		app = new ClientAppRenderer();
	else if (process_type == ClientApp::OtherProcess)
		app = new ClientAppOther();

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
	if (exit_code >= 0)
		return exit_code;

	// Create the main context object.
	scoped_ptr<MainContext> context(new MainContext(command_line, true));

	// Populate the settings based on command line arguments.
	context->PopulateSettings(&settings);

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
	//settings.single_process = true;

	CefRect rect = CefRect();
	std::string url,width,height;
	url = command_line->GetSwitchValue(Switches::kUrl);
	width = command_line->GetSwitchValue("width");
	height = command_line->GetSwitchValue("height");
	if (url.empty()){
		url = "http://www.baidu.com";
		// Set the main URL.
		command_line->AppendSwitchWithValue(Switches::kUrl, url);
	}
	if (width.empty()){
		rect.width = 1024;
	}else{
		rect.width = atoi(width.c_str());
	}
	if (height.empty()){
		rect.height = 700;
	}else{
		rect.height = atoi(height.c_str());
	}

	// Initialize CEF.
	context->Initialize(main_args, settings, app, sandbox_info);

	// Register scheme handlers.
	//ClientRunner::RegisterSchemeHandlers();

	// Create the first window.
	BrowserDlg* pDlg = context->GetBrowserDlgManager()->CreateBrowserDlg(
		NULL,
		true,		// Show controls.
		rect,		// Use default system size.
		url);		// Use default URL.

	if(pDlg) {
		pDlg->CenterWindow();
	}

	//DuiLib::CPaintManagerUI::MessageLoop();
	CefRunMessageLoop();

	DuiLib::CPaintManagerUI::Term();

	// Shut down CEF.
	context->Shutdown();

	if(hFlashTools){
		CloseHandle(hFlashTools);
		hFlashTools= NULL;
	}
	::CoUninitialize();
	return result;
}
