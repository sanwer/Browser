#include "stdafx.h"
#include "ClientAppBrowser.h"
#include "ClientSwitches.h"
#include "include/base/cef_logging.h"
#include "include/cef_cookie.h"

namespace Browser
{
	ClientAppBrowser::ClientAppBrowser()
	{
		CreateDelegates(m_delegates);
	}

	void ClientAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
	{
		if (process_type.empty())
		{
			//Single process mode
			command_line->AppendSwitch("single-process");

			//Disable Same-Origin Policy
			command_line->AppendSwitch("--disable-web-security");

			//Using the same rendering process in the same domain
			command_line->AppendSwitch("process-per-site");
			command_line->AppendSwitch("enable-caret-browsing");
			command_line->AppendSwitch("auto-positioned-ime-window");

			// Pass additional command-line flags when off-screen rendering is enabled.
			if (command_line->HasSwitch("off-screen-rendering-enabled")) {
				// If the PDF extension is enabled then cc Surfaces must be disabled for
				// PDFs to render correctly.
				// See https://bitbucket.org/chromiumembedded/cef/issues/1689 for details.
				if (!command_line->HasSwitch("disable-extensions") &&
					!command_line->HasSwitch("disable-pdf-extension")) {
						command_line->AppendSwitch("disable-surfaces");
				}

				// Use software rendering and compositing (disable GPU) for increased FPS
				// and decreased CPU usage. This will also disable WebGL so remove these
				// switches if you need that capability.
				// See https://bitbucket.org/chromiumembedded/cef/issues/1257 for details.
				if (!command_line->HasSwitch("enable-gpu")) {
					command_line->AppendSwitch("disable-gpu");
					command_line->AppendSwitch("disable-gpu-compositing");
				}

				// Synchronize the frame rate between all processes. This results in
				// decreased CPU usage by avoiding the generation of extra frames that
				// would otherwise be discarded. The frame rate can be set at browser
				// creation time via CefBrowserSettings.windowless_frame_rate or changed
				// dynamically using CefBrowserHost::SetWindowlessFrameRate. In cefclient
				// it can be set via the command-line using `--off-screen-frame-rate=XX`.
				// See https://bitbucket.org/chromiumembedded/cef/issues/1368 for details.
				command_line->AppendSwitch("enable-begin-frame-scheduling");
			}
		}
		
		for (auto it = m_delegates.begin(); it != m_delegates.end(); ++it) {
			(*it)->OnBeforeCommandLineProcessing(this, command_line);
		}
	}

	void ClientAppBrowser::OnRegisterCustomSchemes(
#if CHROME_VERSION_BUILD >= 2924
		CefRawPtr<CefSchemeRegistrar> registrar
#else
		CefRefPtr<CefSchemeRegistrar> registrar
#endif
		)
	{
		// Default schemes that support cookies.
		cookie_schemes.push_back("http");
		cookie_schemes.push_back("https");
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// CefBrowserProcessHandler methods.
	void ClientAppBrowser::OnContextInitialized()
	{
		CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
		DCHECK(manager.get());

		for (auto it = m_delegates.begin(); it != m_delegates.end(); ++it) {
			(*it)->OnContextInitialized(this);
		}
	}

	void ClientAppBrowser::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
	{
		for (auto it = m_delegates.begin(); it != m_delegates.end(); ++it) {
			(*it)->OnBeforeChildProcessLaunch(this, command_line);
		}
	}

	// static
	void ClientAppBrowser::CreateDelegates(DelegateSet& delegates) {
	}
}