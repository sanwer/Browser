#include "stdafx.h"
#include "BrowserDlgManager.h"
#include <sstream>
#include "include/base/cef_bind.h"
#include "include/base/cef_logging.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "MainContext.h"
#include "ClientSwitches.h"

namespace Browser
{
	namespace
	{
		class ClientRequestContextHandler : public CefRequestContextHandler
		{
		public:
			ClientRequestContextHandler() {}
			bool OnBeforePluginLoad(
				const CefString& mime_type,
				const CefString& plugin_url,
				const CefString& top_origin_url,
				CefRefPtr<CefWebPluginInfo> plugin_info,
				PluginPolicy* plugin_policy) OVERRIDE
			{
				// Always allow the PDF plugin to load.
				if (*plugin_policy != PLUGIN_POLICY_ALLOW && mime_type == "application/pdf") {
					*plugin_policy = PLUGIN_POLICY_ALLOW;
					return true;
				}
				return false;
			}
		private:
			IMPLEMENT_REFCOUNTING(ClientRequestContextHandler);
		};
	}

	BrowserDlgManager::BrowserDlgManager(bool terminate_when_all_windows_closed)
		: m_bTerminateWhenAllWindowsClosed(terminate_when_all_windows_closed)
	{
		CefRefPtr<CefCommandLine> command_line = CefCommandLine::GetGlobalCommandLine();
		DCHECK(command_line.get());
		m_bRequestContextPerBrowser = command_line->HasSwitch(Switches::kRequestContextPerBrowser);
		m_bRequestContextSharedBrowser = command_line->HasSwitch(Switches::kRequestContextSharedCache);
	}

	BrowserDlgManager::~BrowserDlgManager()
	{
		// All root windows should already have been destroyed.
		DCHECK(m_BrowserDlgSet.empty());
	}

	scoped_refptr<BrowserDlg> BrowserDlgManager::CreateBrowserDlg(
		HWND hParent,
		bool with_controls,
		const CefRect& bounds,
		const CefString& url)
	{
		CefBrowserSettings settings;
		MainContext::Get()->PopulateBrowserSettings(&settings);
		scoped_refptr<BrowserDlg> pDlg = new BrowserDlg();
		if(pDlg){
			pDlg->Init(this, hParent, with_controls, bounds, settings, url.empty() ? MainContext::Get()->GetMainURL() : url);

			// Store a reference to the root window on the main thread.
			OnBrowserDlgCreated(pDlg);
		}

		return pDlg;
	}

	scoped_refptr<BrowserDlg> BrowserDlgManager::CreateBrowserDlgAsPopup(
		bool with_controls,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings)
	{
		MainContext::Get()->PopulateBrowserSettings(&settings);
		scoped_refptr<BrowserDlg> pDlg = new Browser::BrowserDlg();
		if(pDlg){
			pDlg->InitAsPopup(this, with_controls, popupFeatures, windowInfo, client, settings);

			// Store a reference to the root window on the main thread.
			OnBrowserDlgCreated(pDlg);
		}

		return pDlg;
	}

	scoped_refptr<BrowserDlg> BrowserDlgManager::GetWindowForBrowser(int browser_id)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		BrowserDlgSet::const_iterator it = m_BrowserDlgSet.begin();
		for (; it != m_BrowserDlgSet.end(); ++it) {
			CefRefPtr<CefBrowser> browser = (*it)->GetBrowser();
			if (browser.get() && browser->GetIdentifier() == browser_id)
				return *it;
		}
		return NULL;
	}

	void BrowserDlgManager::CloseAllWindows(bool force)
	{
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute this method on the main thread.
			CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&BrowserDlgManager::CloseAllWindows, base::Unretained(this), force)));
			return;
		}

		if (m_BrowserDlgSet.empty())
			return;

		BrowserDlgSet::const_iterator it = m_BrowserDlgSet.begin();
		for (; it != m_BrowserDlgSet.end(); ++it)
			(*it)->Close(force);
	}

	void BrowserDlgManager::OnBrowserDlgCreated(scoped_refptr<BrowserDlg> pDlg)
	{
		if (!CefCurrentlyOn(TID_UI)) {
			// Execute this method on the main thread.
			CefPostTask(TID_UI, CefCreateClosureTask(base::Bind(&BrowserDlgManager::OnBrowserDlgCreated, base::Unretained(this), pDlg)));
			return;
		}

		m_BrowserDlgSet.insert(pDlg);
	}

	CefRefPtr<CefRequestContext> BrowserDlgManager::GetRequestContext()
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		if (m_bRequestContextPerBrowser) {
			// Create a new request context for each browser.
			CefRequestContextSettings settings;

			CefRefPtr<CefCommandLine> command_line =
				CefCommandLine::GetGlobalCommandLine();
			if (command_line->HasSwitch(Switches::kCachePath)) {
				if (m_bRequestContextSharedBrowser) {
					// Give each browser the same cache path. The resulting context objects
					// will share the same storage internally.
					CefString(&settings.cache_path) =
						command_line->GetSwitchValue(Switches::kCachePath);
				} else {
					// Give each browser a unique cache path. This will create completely
					// isolated context objects.
					std::stringstream ss;
					ss << command_line->GetSwitchValue(Switches::kCachePath).ToString() <<
						time(NULL);
					CefString(&settings.cache_path) = ss.str();
				}
			}

			return CefRequestContext::CreateContext(settings, new ClientRequestContextHandler);
		}

		// All browsers will share the global request context.
		if (!m_SharedRequestContext.get()) {
			m_SharedRequestContext = CefRequestContext::CreateContext(CefRequestContext::GetGlobalContext(), new ClientRequestContextHandler);
		}
		return m_SharedRequestContext;
	}

	void BrowserDlgManager::OnBrowserDlgDestroyed(BrowserDlg* pDlg)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		BrowserDlgSet::iterator it = m_BrowserDlgSet.find(pDlg);
		if (it != m_BrowserDlgSet.end())
			m_BrowserDlgSet.erase(it);

		if (m_bTerminateWhenAllWindowsClosed && m_BrowserDlgSet.empty()) {
			// Quit the main message loop after all windows have closed.
			CefQuitMessageLoop();
			//PostQuitMessage(0);
		}
	}
}
