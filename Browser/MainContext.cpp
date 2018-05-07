#include "stdafx.h"
#include "MainContext.h"
#include "include/base/cef_logging.h"
#include "include/cef_parser.h"
#include "ClientSwitches.h"
#include <direct.h>
#include <shlobj.h>

namespace Browser
{
	namespace {
		// The default URL to load in a browser window.
		//const WCHAR kDefaultUrl[] = L"https://www.baidu.com";
		const WCHAR kDefaultUrl[] = L"http://www.w3school.com.cn/jsref/met_win_open.asp";

		MainContext* g_main_context = NULL;
	}

	// static
	MainContext* MainContext::Get() {
		DCHECK(g_main_context);
		return g_main_context;
	}
	MainContext::MainContext(CefRefPtr<CefCommandLine> command_line, bool terminate_when_all_windows_closed)
		: m_CommandLine(command_line),
		m_bTerminateWhenAllWindowsClosed(terminate_when_all_windows_closed),
		m_bInitialized(false),
		m_bShutdown(false),
		m_BackgroundColor(CefColorSetARGB(255, 255, 255, 255))
	{
		DCHECK(!g_main_context);
		g_main_context = this;
		DCHECK(m_CommandLine.get());

		// Set the main URL.
		if (m_CommandLine->HasSwitch(Switches::kUrl))
			m_sMainUrl = m_CommandLine->GetSwitchValue(Switches::kUrl);
		if (m_sMainUrl.empty())
			m_sMainUrl = kDefaultUrl;

		if (m_CommandLine->HasSwitch(Switches::kBackgroundColor)) {
			// Parse the background color value.
			CefParseCSSColor(m_CommandLine->GetSwitchValue(Switches::kBackgroundColor),
				false, m_BackgroundColor);
		}
	}

	MainContext::~MainContext() {
		g_main_context = NULL;
		// The context must either not have been initialized, or it must have also
		// been shut down.
		DCHECK(!m_bInitialized || m_bShutdown);
	}

	std::string MainContext::GetConsoleLogPath() {
		return GetAppWorkingDirectory() + "console.log";
	}

	std::string MainContext::GetDownloadPath(const std::string& file_name) {
		TCHAR szFolderPath[MAX_PATH];
		std::string path;

		// Save the file in the user's "My Documents" folder.
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
			NULL, 0, szFolderPath))) {
				path = CefString(szFolderPath);
				path += "\\" + file_name;
		}

		return path;
	}

	std::string MainContext::GetAppWorkingDirectory() {
		char szWorkingDir[MAX_PATH + 1];
		if (_getcwd(szWorkingDir, MAX_PATH) == NULL) {
			szWorkingDir[0] = 0;
		} else {
			// Add trailing path separator.
			size_t len = strlen(szWorkingDir);
			szWorkingDir[len] = '\\';
			szWorkingDir[len + 1] = 0;
		}
		return szWorkingDir;
	}

	std::wstring MainContext::GetMainURL() {
		return m_sMainUrl;
	}

	cef_color_t MainContext::GetBackgroundColor() {
		return m_BackgroundColor;
	}

	void MainContext::PopulateSettings(CefSettings* settings) {
		settings->multi_threaded_message_loop =
			m_CommandLine->HasSwitch(Switches::kMultiThreadedMessageLoop);

		CefString(&settings->cache_path) =
			m_CommandLine->GetSwitchValue(Switches::kCachePath);

		if (m_CommandLine->HasSwitch(Switches::kOffScreenRenderingEnabled))
			settings->windowless_rendering_enabled = true;

		settings->background_color = m_BackgroundColor;
	}

	void MainContext::PopulateBrowserSettings(CefBrowserSettings* settings) {
		if (m_CommandLine->HasSwitch(Switches::kOffScreenFrameRate)) {
			settings->windowless_frame_rate = atoi(m_CommandLine->
				GetSwitchValue(Switches::kOffScreenFrameRate).ToString().c_str());
		}
	}

	BrowserDlgManager* MainContext::GetBrowserDlgManager() {
		DCHECK(InValidState());
		return m_BrowserDlgManager.get();
	}

	bool MainContext::Initialize(const CefMainArgs& args,
		const CefSettings& settings,
		CefRefPtr<CefApp> application,
		void* windows_sandbox_info) {
			DCHECK(m_ThreadChecker.CalledOnValidThread());
			DCHECK(!m_bInitialized);
			DCHECK(!m_bShutdown);

			if (!CefInitialize(args, settings, application, windows_sandbox_info))
				return false;

			m_BrowserDlgManager.reset(new BrowserDlgManager(m_bTerminateWhenAllWindowsClosed));

			m_bInitialized = true;

			return true;
	}

	void MainContext::Shutdown() {
		DCHECK(m_ThreadChecker.CalledOnValidThread());
		DCHECK(m_bInitialized);
		DCHECK(!m_bShutdown);

		m_BrowserDlgManager.reset();

		CefShutdown();

		m_bShutdown = true;
	}
}
