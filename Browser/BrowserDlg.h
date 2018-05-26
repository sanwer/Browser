#ifndef __BROWSER_WND_H__
#define __BROWSER_WND_H__
#pragma once
#include "MessageLoop.h"
#include "BrowserWindow.h"
#include "ClientApp.h"
#include "BrowserUI.h"

namespace Browser
{
	class BrowserDlg : public DuiLib::WindowImplBase, public Browser::BrowserWindow::Delegate, public base::RefCountedThreadSafe<BrowserDlg, Browser::DeleteOnMainThread>
	{
	public:
		class Delegate {
		public:
			virtual CefRefPtr<CefRequestContext> GetRequestContext() = 0;
			virtual void OnBrowserDlgDestroyed(BrowserDlg* pDlg) = 0;
		protected:
			virtual ~Delegate() {}
		};
		BrowserDlg();
		~BrowserDlg();

	public:
		LPCTSTR GetWindowClassName() const;
		virtual void InitWindow();
		virtual void OnFinalMessage(HWND hWnd);
		virtual DuiLib::CDuiString GetSkinFile();
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
		virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		void Notify(DuiLib::TNotifyUI& msg);

	private:
		// BrowserWindow::Delegate methods.
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserAllClosed() OVERRIDE;
		void OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url) OVERRIDE;
		void OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
		void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
		void OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) OVERRIDE;
		void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) OVERRIDE;
		void OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) OVERRIDE;
		void NotifyDestroyedIfDone();

	private:
		DuiLib::CHorizontalLayoutUI* uiTabs;
		DuiLib::CLabelUI* pTitle;
		DuiLib::CButtonUI* tabNew;
		DuiLib::CControlUI* uiToolbar;
		DuiLib::CButtonUI* btnBackward;
		DuiLib::CButtonUI* btnForward;
		DuiLib::CEditUI* editUrl;
		DuiLib::CEditUI* editKeyword;
		DuiLib::CDuiString m_sPopup;

	public:
		// BrowserDlg methods.
		void Init(
			BrowserDlg::Delegate* delegate,
			HWND hParent,
			bool with_controls,
			const CefRect& bounds,
			const CefBrowserSettings& settings,
			const CefString& url);

		void InitAsPopup(
			BrowserDlg::Delegate* delegate,
			bool with_controls,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings);

		void NewTab(const CefString& url);

		CefRefPtr<CefBrowser> GetBrowser();
		CefWindowHandle GetWindowHandle();
		void LoadURL(const CefString& url);

	private:
		void CreateBrowserWindow(const CefString& startup_url);
		void CreateBrowserDlg(const CefBrowserSettings& settings);

		HWND m_hParent;
		bool m_bWithControls;
		bool m_bIsPopup;
		RECT m_rcStart;
		bool m_bInitialized;
		bool m_bWindowDestroyed;
		bool m_bBrowserDestroyed;
		int m_nCurBrowserId;
		BrowserDlg::Delegate* m_Delegate;
		Browser::BrowserUI* m_pBrowserUI;
		scoped_ptr<Browser::BrowserWindow> m_BrowserCtrl;
	};
}
#endif