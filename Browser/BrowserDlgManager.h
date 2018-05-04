#ifndef __BROWSER_MANAGER_H__
#define __BROWSER_MANAGER_H__
#pragma once
#include <set>
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "BrowserDlg.h"
#include "TempWindow.h"

namespace Browser
{
	class BrowserDlgManager : public BrowserDlg::Delegate {
	public:
		// If |terminate_when_all_windows_closed| is true quit the main message loop
		// after all windows have closed.
		explicit BrowserDlgManager(bool terminate_when_all_windows_closed);

		scoped_refptr<BrowserDlg> CreateBrowserDlg(
			HWND hParent,
			bool with_controls,
			const CefRect& bounds,
			const std::wstring& url);

		scoped_refptr<BrowserDlg> CreateBrowserDlgAsPopup(
			bool with_controls,
			const CefString& target_url,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings);

		// Returns the BrowserDlg associated with the specified browser ID. Must be
		// called on the main thread.
		scoped_refptr<BrowserDlg> GetWindowForBrowser(int browser_id);

		// Close all existing windows. If |force| is true onunload handlers will not
		// be executed.
		void CloseAllWindows(bool force);

	private:
		// Allow deletion via scoped_ptr only.
		friend struct base::DefaultDeleter<BrowserDlgManager>;

		~BrowserDlgManager();

		void OnRootWindowCreated(scoped_refptr<BrowserDlg> pDlg);

		// BrowserDlg::Delegate methods.
		CefRefPtr<CefRequestContext> GetRequestContext() OVERRIDE;
		void OnExit(BrowserDlg* pDlg) OVERRIDE;
		void OnRootWindowDestroyed(BrowserDlg* pDlg) OVERRIDE;

		const bool m_bTerminateWhenAllWindowsClosed;
		bool m_bRequestContextPerBrowser;
		bool m_bRequestContextSharedBrowser;

		// Existing root windows. Only accessed on the main thread.
		typedef std::set<scoped_refptr<BrowserDlg> > BrowserDlgSet;
		BrowserDlgSet m_BrowserDlgSet;

		// Singleton window used as the temporary parent for popup browsers.
		TempWindow m_TempWindow;

		CefRefPtr<CefRequestContext> m_SharedRequestContext;

		DISALLOW_COPY_AND_ASSIGN(BrowserDlgManager);
	};
}

#endif
