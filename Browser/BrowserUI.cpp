#include "StdAfx.h"
#include <sstream>
#include <string>
#include "BrowserUI.h"
#include "MessageLoop.h"

namespace Browser
{
	BrowserCtrl::BrowserCtrl(Delegate* delegate, const std::wstring& startup_url)
		: m_Delegate(delegate),
		m_IsClosing(false)
	{
		DCHECK(m_Delegate);
		m_ClientHandler = new ClientHandler(this, false, startup_url);
	}

	void BrowserCtrl::CreateBrowser(
		CefWindowHandle parent_handle,
		const CefRect& rect,
		const CefBrowserSettings& settings,
		CefRefPtr<CefRequestContext> request_context) {
			REQUIRE_MAIN_THREAD();

			CefWindowInfo window_info;
			RECT wnd_rect = {rect.x, rect.y, rect.x + rect.width, rect.y + rect.height};
			window_info.SetAsChild(parent_handle, wnd_rect);

			CefBrowserHost::CreateBrowser(window_info, m_ClientHandler,
				m_ClientHandler->StartupUrl(), settings, request_context);
	}

	void BrowserCtrl::GetPopupConfig(CefWindowHandle temp_handle,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings) {
			// Note: This method may be called on any thread.
			// The window will be properly sized after the browser is created.
			windowInfo.SetAsChild(temp_handle, RECT());
			client = m_ClientHandler;
	}

	void BrowserCtrl::ShowPopup(CefWindowHandle parent_handle,
		int x, int y, size_t width, size_t height) {
			REQUIRE_MAIN_THREAD();

			HWND hwnd = GetWindowHandle();
			if (hwnd) {
				SetParent(hwnd, parent_handle);
				SetWindowPos(hwnd, NULL, x, y,static_cast<int>(width), static_cast<int>(height),SWP_NOZORDER);
				::ShowWindow(hwnd, SW_SHOW);
			}
	}

	void BrowserCtrl::Show() {
		REQUIRE_MAIN_THREAD();

		HWND hwnd = GetWindowHandle();
		if (hwnd && !::IsWindowVisible(hwnd))
			::ShowWindow(hwnd, SW_SHOW);
	}

	void BrowserCtrl::Hide() {
		REQUIRE_MAIN_THREAD();

		HWND hwnd = GetWindowHandle();
		if (hwnd) {
			// When the frame window is minimized set the browser window size to 0x0 to
			// reduce resource usage.
			SetWindowPos(hwnd, NULL,
				0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}
	}

	void BrowserCtrl::SetBounds(int x, int y, size_t width, size_t height) {
		REQUIRE_MAIN_THREAD();

		HWND hwnd = GetWindowHandle();
		if (hwnd) {
			// Set the browser window bounds.
			SetWindowPos(hwnd, NULL, x, y,static_cast<int>(width), static_cast<int>(height), SWP_NOZORDER);
		}
	}

	void BrowserCtrl::SetFocus(bool focus) {
		REQUIRE_MAIN_THREAD();

		if (m_Browser)
			m_Browser->GetHost()->SetFocus(focus);
	}

	void BrowserCtrl::SetDeviceScaleFactor(float device_scale_factor) {
	}

	float BrowserCtrl::GetDeviceScaleFactor() const {
		return 1.0f;
	}

	CefWindowHandle BrowserCtrl::GetWindowHandle() const {
		REQUIRE_MAIN_THREAD();

		if (m_Browser)
			return m_Browser->GetHost()->GetWindowHandle();
		return NULL;
	}

	CefRefPtr<CefBrowser> BrowserCtrl::GetBrowser() const {
		REQUIRE_MAIN_THREAD();
		return m_Browser;
	}

	bool BrowserCtrl::IsClosing() const {
		REQUIRE_MAIN_THREAD();
		return m_IsClosing;
	}

	void BrowserCtrl::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
		REQUIRE_MAIN_THREAD();
		//DCHECK(!m_Browser);
		m_Browser = browser;

		m_Delegate->OnBrowserCreated(browser);
	}

	void BrowserCtrl::OnBrowserClosing(CefRefPtr<CefBrowser> browser) {
		REQUIRE_MAIN_THREAD();
		DCHECK_EQ(browser->GetIdentifier(), m_Browser->GetIdentifier());
		m_IsClosing = true;
	}

	void BrowserCtrl::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
		REQUIRE_MAIN_THREAD();
		if (m_Browser.get()) {
			DCHECK_EQ(browser->GetIdentifier(), m_Browser->GetIdentifier());
			m_Browser = NULL;
		}

		m_ClientHandler->DetachDelegate();
		m_ClientHandler = NULL;

		// |this| may be deleted.
		m_Delegate->OnBrowserWindowDestroyed();
	}

	void BrowserCtrl::OnSetAddress(const std::wstring& url) {
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetAddress(url);
	}

	void BrowserCtrl::OnSetTitle(const std::wstring& title) {
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetTitle(title);
	}

	void BrowserCtrl::OnSetFullscreen(bool fullscreen) {
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetFullscreen(fullscreen);
	}

	void BrowserCtrl::OnSetLoadingState(bool isLoading,
		bool canGoBack,
		bool canGoForward) {
			REQUIRE_MAIN_THREAD();
			m_Delegate->OnSetLoadingState(isLoading, canGoBack, canGoForward);
	}

	void BrowserCtrl::OnSetDraggableRegions(
		const std::vector<CefDraggableRegion>& regions) {
			REQUIRE_MAIN_THREAD();
			m_Delegate->OnSetDraggableRegions(regions);
	}

	BrowserUI::BrowserUI()
		: m_pBrowserCtrl(NULL)
	{
		SetBkColor(0xFFFFFFFF);
	}

	LPCTSTR BrowserUI::GetClass() const
	{
		return _T("BrowserUI");
	}

	LPVOID BrowserUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("BrowserUI")) == 0 ) return static_cast<BrowserUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void BrowserUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		DuiLib::CDuiRect rcPos = rc;
		if (m_pBrowserCtrl && !rcPos.IsNull()) {
			CefRefPtr<CefBrowser> browser = m_pBrowserCtrl->GetBrowser();
			if (browser) {
				::SetWindowPos(browser->GetHost()->GetWindowHandle(),NULL, rc.left, rc.top, rc.right - rc.left,rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
		CControlUI::SetPos(rc, bNeedInvalidate);
	}
	
	BrowserCtrl* BrowserUI::CreateBrowserCtrl(BrowserCtrl::Delegate* delegate, const std::wstring& startup_url)
	{
		return m_pBrowserCtrl = new BrowserCtrl(delegate, startup_url);
	}
}
