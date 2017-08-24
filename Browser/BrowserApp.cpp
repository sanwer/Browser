#include "BrowserApp.h"
#include "JSHandler.h"

namespace Browser
{
	//////////////////////////////////////////////////////////////////////////////////////////
	// CefApp methods.
	void CBrowserApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
	{
		if (process_type.empty()) 
		{
			// 设置子进程路径很关键，如果不设置，可能会触发cef的一个bug
			// cef在LoadUrl建立渲染进程时，会查找子进程的路径，可能会引发一个bug导致IDE在debug状态时卡死
			// 这里指定好子进程路径就可以了

			// 但是使用sandbox的话，不允许使用另外的子进程;不使用sandbox的话，第一次加载flash插件时会弹出个命令提示行，这是cef的bug。flash与子进程二者不可兼得
			//command_line->AppendSwitchWithValue("browser-subprocess-path", "render.exe");

			command_line->AppendSwitchWithValue("ppapi-flash-version", "20.0.0.228");
			command_line->AppendSwitchWithValue("ppapi-flash-path", "PepperFlash\\pepflashplayer.dll");

			//同一个域下的使用同一个渲染进程
			command_line->AppendSwitch("process-per-site");
			command_line->AppendSwitch("enable-caret-browsing");
			command_line->AppendSwitch("auto-positioned-ime-window");

			//禁用一些选项，提升cef性能
			command_line->AppendSwitch("disable-surfaces");
			command_line->AppendSwitch("disable-gpu");
			command_line->AppendSwitch("disable-gpu-compositing");

			//开启离屏渲染
			//command_line->AppendSwitch("enable-begin-frame-scheduling");
		}
	}

	void CBrowserApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar)
	{
		// Default schemes that support cookies.
		cookieable_schemes_.push_back("http");
		cookieable_schemes_.push_back("https");
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// CefBrowserProcessHandler methods.
	void CBrowserApp::OnContextInitialized() 
	{
		// Register cookieable schemes with the global cookie manager.
		CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(NULL);
		manager->SetSupportedSchemes(cookieable_schemes_, NULL);

		// 删除保存的Cooies信息
		// manager->DeleteCookies(L"", L"", nullptr);
	}

	void CBrowserApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
	{
	}

	void CBrowserApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) 
	{
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// CefRenderProcessHandler methods.
	void CBrowserApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) 
	{
	}

	void CBrowserApp::OnWebKitInitialized() 
	{
		//JS扩展代码
		std::string extensionCode =
			"var Client;"
			"if (!Client)"
			"  Client = {};"
			"(function() {"
			"  Client.GetComputerName = function() {"
			"    native function GetComputerName();"
			"      return GetComputerName();"
			"  };"
			"})();";
		CefRegisterExtension( "v8/Client", extensionCode, new CJSHandler() );
	}

	void CBrowserApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
	}

	void CBrowserApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) 
	{
	}

	CefRefPtr<CefLoadHandler> CBrowserApp::GetLoadHandler()
	{
		return NULL;
	}

	bool CBrowserApp::OnBeforeNavigation(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect) 
	{
		return false;
	}

	void CBrowserApp::OnContextCreated(CefRefPtr<CefBrowser> browser,	CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) 
	{
	}

	void CBrowserApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,	CefRefPtr<CefV8Context> context) 
	{
	}

	void CBrowserApp::OnUncaughtException
		(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace) 
	{
	}

	void CBrowserApp::OnFocusedNodeChanged(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) 
	{
	}

	bool CBrowserApp::OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) 
	{
		return false;
	}
}
