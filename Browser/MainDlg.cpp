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
#include <windows.h>
#include <objbase.h>

namespace Browser
{
	int RunMain(HINSTANCE hInstance, int nCmdShow)
	{
		int result = 0;
		HRESULT Hr = ::CoInitialize(NULL);
		if(FAILED(Hr)) return 0;

		DuiLib::CPaintManagerUI::SetInstance(hInstance);
		DuiLib::CPaintManagerUI::SetResourceType(DuiLib::UILIB_ZIPRESOURCE);
		DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::CPaintManagerUI::GetInstancePath());
		DuiLib::CPaintManagerUI::SetResourceZip(MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));

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
		

		// Initialize CEF.
		context->Initialize(main_args, settings, app, sandbox_info);

		// Register scheme handlers.
		//ClientRunner::RegisterSchemeHandlers();

		scoped_ptr<MessageLoop> message_loop;
		message_loop.reset(new MessageLoop);

		// Create the first window.
		BrowserDlg* pDlg = context->GetBrowserDlgManager()->CreateBrowserDlg(
			NULL,
			true,             // Show controls.
			CefRect(),        // Use default system size.
			std::wstring());   // Use default URL.

		if(pDlg) {
			pDlg->CenterWindow();
		}

		//DuiLib::CPaintManagerUI::MessageLoop();
		result = message_loop->Run();

		DuiLib::CPaintManagerUI::Term();

		// Shut down CEF.
		context->Shutdown();

		message_loop.reset();

		::CoUninitialize();
		return result;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	return Browser::RunMain(hInstance, nCmdShow);
}