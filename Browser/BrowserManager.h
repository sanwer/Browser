#ifndef __BROWSER_MANAGER_H__
#define __BROWSER_MANAGER_H__
#pragma once
#include <set>
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "BrowserDlg.h"
#ifdef CEF_USE_SANDBOX
#pragma comment(lib, "cef_sandbox.lib")
#endif

namespace Browser
{
	class TempWindow
	{
	public:
		static CefWindowHandle GetWindowHandle();

	private:
		friend class BrowserManager;

		TempWindow();
		~TempWindow();

		CefWindowHandle m_hWnd;

		DISALLOW_COPY_AND_ASSIGN(TempWindow);
	};

	class BrowserManager : public BrowserDlg::Delegate {
	public:
		// If |terminate_when_all_windows_closed| is true quit the main message loop
		// after all windows have closed.
		explicit BrowserManager(bool terminate_when_all_windows_closed);

		// Returns the singleton instance of this object.
		static BrowserManager* Get();

		// Returns the full path to |file_name|.
		std::string GetDownloadPath(const std::string& file_name);

		// Returns the main application URL.
		std::string GetMainURL();

		// Returns the background color.
		cef_color_t GetBackgroundColor();

		scoped_refptr<BrowserDlg> CreateRootWindow(
			bool with_controls,
			bool with_osr,
			const CefRect& bounds,
			const std::wstring& url);

		scoped_refptr<BrowserDlg> CreateRootWindowAsPopup(
			bool with_controls,
			bool with_osr,
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
		friend struct base::DefaultDeleter<BrowserManager>;

		~BrowserManager();

		void OnRootWindowCreated(scoped_refptr<BrowserDlg> pDlg);

		// BrowserDlg::Delegate methods.
		CefRefPtr<CefRequestContext> GetRequestContext(BrowserDlg* pDlg) OVERRIDE;
		void OnExit(BrowserDlg* pDlg) OVERRIDE;
		void OnRootWindowDestroyed(BrowserDlg* pDlg) OVERRIDE;

		const bool m_bAllWindowsClosed;
		bool m_bRequestContextPerBrowser;
		bool m_bRequestContextSharedBrowser;

		// Existing root windows. Only accessed on the main thread.
		typedef std::set<scoped_refptr<BrowserDlg> > BrowserWindowSet;
		BrowserWindowSet m_BrowserWindowSet;

		// Singleton window used as the temporary parent for popup browsers.
		TempWindow m_TempWindow;

		CefRefPtr<CefRequestContext> m_SharedRequestContext;

		DISALLOW_COPY_AND_ASSIGN(BrowserManager);
	};
}

#endif
