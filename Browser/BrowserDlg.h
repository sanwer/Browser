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
			virtual CefRefPtr<CefRequestContext> GetRequestContext() = 0;
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
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		void Notify(DuiLib::TNotifyUI& msg);

	private:
		// BrowserWindow::Delegate methods.
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserExit(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnSetAddress(CefRefPtr<CefBrowser> browser, const std::wstring& url) OVERRIDE;
		void OnSetTitle(CefRefPtr<CefBrowser> browser, const std::wstring& title) OVERRIDE;
		void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
		void OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) OVERRIDE;
		void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) OVERRIDE;
		void OnNewPage(const std::wstring& url) OVERRIDE;
		void NotifyDestroyedIfDone();

	private:
		DuiLib::CHorizontalLayoutUI* uiTabs;
		DuiLib::CButtonUI* tabNew;
		DuiLib::CControlUI* uiToolbar;
		DuiLib::CButtonUI* btnBackward;
		DuiLib::CButtonUI* btnForward;
		DuiLib::CEditUI* editUrl;
		DuiLib::CEditUI* editKeyword;
		DuiLib::CDuiString m_sPopup;

	public:
		// BrowserDlg methods.
		void Init(BrowserDlg::Delegate* delegate,const std::wstring& url);
		void InitAsPopup(
			BrowserDlg::Delegate* delegate,
			bool with_controls,
			const CefString& target_url,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings);
		CefRefPtr<CefBrowser> GetBrowser();
		void LoadURL(const CefString& url);

	private:
		void CreateBrowserWindow(CefRefPtr<CefBrowser> browser, const CefBrowserSettings& settings);

		bool m_bWithControls;
		RECT m_rcBrowser;
		bool m_bWithOsr;
		bool m_bIsPopup;
		bool m_bInitialized;
		bool m_bWindowDestroyed;
		bool m_bBrowserDestroyed;
		int m_nBrowserSelectedId;
		Browser::BrowserUI* m_pBrowser;
		BrowserDlg::Delegate* m_Delegate;
		scoped_ptr<Browser::BrowserCtrl> m_BrowserCtrl;
	};
}
#endif