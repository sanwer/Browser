#ifndef __BROWSERUI_H__
#define __BROWSERUI_H__
#pragma once
#include "ClientHandler.h"
#include <UIlib.h>
using namespace DuiLib;

namespace Browser
{
	class BrowserUI;
	class BrowserDlg;

	class BrowserCtrl : public ClientHandler::Delegate
	{
	public:
		class Delegate {
		public:
			virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) = 0;
			virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) = 0;
			virtual void OnBrowserExit(CefRefPtr<CefBrowser> browser) = 0;
			virtual void OnSetAddress(CefRefPtr<CefBrowser> browser, const std::wstring& url) = 0;
			virtual void OnSetTitle(CefRefPtr<CefBrowser> browser, const std::wstring& title) = 0;
			virtual void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) = 0;
			virtual void OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) = 0;
			virtual void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) = 0;
			virtual void OnNewPage(const std::wstring& url) = 0;

		protected:
			virtual ~Delegate() {}
		};

		// Constructor may be called on any thread.
		// |delegate| must outlive this object.
		explicit BrowserCtrl(Delegate* delegate);


		// Create a new browser and native window.
		virtual void CreateBrowser(
			CefWindowHandle parent_handle,
			const std::wstring& url,
			const CefRect& rect,
			const CefBrowserSettings& settings,
			CefRefPtr<CefRequestContext> request_context);

		// Retrieve the configuration that will be used when creating a popup window.
		// The popup browser will initially be parented to |temp_handle| which should
		// be a pre-existing hidden window. The native window will be created later
		// after the browser has been created. This method may be called on any
		// thread.
		virtual void GetPopupConfig(
			CefWindowHandle temp_handle,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings);

		// Show the popup window with correct parent and bounds in parent coordinates.
		virtual void ShowPopup(CefRefPtr<CefBrowser> browser, CefWindowHandle hParentWnd, int x, int y, size_t width, size_t height);

		// Show the window.
		virtual void Show(int nBrowserId, int x, int y, size_t width, size_t height);

		// Set focus to the window.
		virtual void SetFocus(int nBrowserId, bool focus);

		// Returns the window handle.
		virtual CefWindowHandle GetWindowHandle(int nBrowserId) const;

		// Returns the browser owned by the window.
		CefRefPtr<CefBrowser> GetBrowser(int nBrowserId) const;

	protected:
		// Allow deletion via scoped_ptr only.
		friend struct base::DefaultDeleter<BrowserCtrl>;

		// ClientHandler::Delegate methods.
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosing(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnSetAddress(CefRefPtr<CefBrowser> browser, const std::wstring& url) OVERRIDE;
		void OnSetTitle(CefRefPtr<CefBrowser> browser, const std::wstring& title) OVERRIDE;
		void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
		void OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) OVERRIDE;
		void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) OVERRIDE;
		void OnNewPage(const std::wstring& url) OVERRIDE;

		Delegate* m_Delegate;
		CefRefPtr<ClientHandler> m_ClientHandler;

	private:
		DISALLOW_COPY_AND_ASSIGN(BrowserCtrl);
	};

	class BrowserUI : public DuiLib::CControlUI
	{
		DECLARE_DUICONTROL(BrowserUI)
	public:
		BrowserUI(BrowserDlg* pParent, HWND hParentWnd);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void SetCtrl(BrowserCtrl* pCtrl);
		void NewPage(const std::wstring& url, bool bPopup = false);
		void ShowPage(int nBrowserId);
		void DelPage(int nBrowserId);
		CefRefPtr<CefBrowser> GetBrowser() const;

	protected:
		HWND m_hParentWnd;
		BrowserDlg* m_pParent;
		BrowserCtrl* m_pCtrl;
		int m_nSelectedId;

		DISALLOW_COPY_AND_ASSIGN(BrowserUI);
	};

	class TitleUI : public CHorizontalLayoutUI
	{
		DECLARE_DUICONTROL(TitleUI)
	public:
		TitleUI();
		~TitleUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);

		bool IsSelected() const;
		virtual void Selected(bool bSelected = true);

		bool Activate();
		void DoEvent(TEventUI& event);
		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();
		void PaintBkColor(HDC hDC);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		void PaintText(HDC hDC);

	protected:
		bool		m_bSelected;
		UINT		m_uButtonState;
		DWORD		m_dwSelectedBkColor;
		CDuiString	m_sGroupName;

		int			m_iFont;
		UINT		m_uTextStyle;
		RECT		m_rcTextPadding;
		DWORD		m_dwTextColor;
		DWORD		m_dwDisabledTextColor;
	};
}

#endif
