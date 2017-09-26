#ifndef __BROWSER_WND_H__
#define __BROWSER_WND_H__
#pragma once
#include "MessageLoop.h"
#include "ClientApp.h"
#include "BrowserUI.h"

namespace Browser
{
	class BrowserDlg : public DuiLib::WindowImplBase, public Browser::BrowserCtrl::Delegate, public base::RefCountedThreadSafe<BrowserDlg, Browser::DeleteOnMainThread>
	{
	public:
		class Delegate {
		public:
			virtual CefRefPtr<CefRequestContext> GetRequestContext(BrowserDlg* pDlg) = 0;
			virtual void OnExit(BrowserDlg* pDlg) = 0;
			virtual void OnRootWindowDestroyed(BrowserDlg* pDlg) = 0;
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

	protected:
		void Notify(DuiLib::TNotifyUI& msg);

	private:
		// BrowserWindow::Delegate methods.
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserWindowDestroyed() OVERRIDE;
		void OnSetAddress(const std::wstring& url) OVERRIDE;
		void OnSetTitle(const std::wstring& title) OVERRIDE;
		void OnSetFullscreen(bool fullscreen) OVERRIDE;
		void OnSetLoadingState(bool isLoading,bool canGoBack,bool canGoForward) OVERRIDE;
		void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) OVERRIDE;
		void NotifyDestroyedIfDone();

	private:
		DuiLib::CLabelUI* labTitle;
		DuiLib::CControlUI* uiToolbar;
		DuiLib::CButtonUI* btnBackward;
		DuiLib::CButtonUI* btnForward;
		DuiLib::CEditUI* editUrl;
		DuiLib::CEditUI* editKeyword;
		DuiLib::CDuiString sHomepage;
		DuiLib::CDuiString m_sUrl;
		DuiLib::CDuiString m_sPopup;

	public:
		// BrowserDlg methods.
		void Init(
			BrowserDlg::Delegate* delegate,
			bool with_controls,
			bool with_osr,
			const CefRect& bounds,
			const CefBrowserSettings& settings,
			const std::wstring& url);
		void InitAsPopup(
			BrowserDlg::Delegate* delegate,
			bool with_controls,
			bool with_osr,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings);
		CefRefPtr<CefBrowser> GetBrowser();
		void LoadURL(const CefString& url);

	private:
		void CreateBrowserCtrl(const std::wstring& startup_url);
		void CreateBrowserWindow(const CefBrowserSettings& settings);

		bool m_bWithControls;
		RECT m_rcBrowser;
		bool m_bWithOsr;
		bool m_bIsPopup;
		bool m_bInitialized;
		bool m_bWindowDestroyed;
		bool m_bBrowserDestroyed;
		Browser::BrowserUI* m_pBrowser;
		CefRefPtr<CefBrowser> m_Browser;
		BrowserDlg::Delegate* m_Delegate;
		scoped_ptr<Browser::BrowserCtrl> m_BrowserCtrl;
	};
}
#endif