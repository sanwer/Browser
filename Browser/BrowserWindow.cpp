#include "stdafx.h"
#include "MainContext.h"
#include "BrowserWindow.h"
#include "include/base/cef_bind.h"

namespace Browser
{
	BrowserWindow::BrowserWindow(Delegate* delegate, const std::string& startup_url)
		: m_Delegate(delegate)
	{
		DCHECK(m_Delegate);
		m_ClientHandler = new ClientHandler(this, startup_url);
	}

	void BrowserWindow::CreateBrowser(
		CefWindowHandle parent_handle,
		const std::wstring& url,
		const CefRect& rect,
		const CefBrowserSettings& settings,
		CefRefPtr<CefRequestContext> request_context)
	{
		REQUIRE_MAIN_THREAD();

		CefWindowInfo window_info;
		RECT wnd_rect = {rect.x, rect.y, rect.x + rect.width, rect.y + rect.height};
		window_info.SetAsChild(parent_handle, wnd_rect);

		CefBrowserHost::CreateBrowser(window_info, m_ClientHandler,
			url.empty() ? MainContext::Get()->GetMainURL() : url, settings, request_context);
	}

	void BrowserWindow::GetPopupConfig(CefWindowHandle temp_handle,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings)
	{
		// Note: This method may be called on any thread.
		// The window will be properly sized after the browser is created.
		windowInfo.SetAsChild(temp_handle, RECT());
		client = m_ClientHandler;
	}

	void BrowserWindow::ShowPopup(CefWindowHandle hParentWnd, int x, int y, size_t width, size_t height)
	{
		REQUIRE_MAIN_THREAD();

		//HWND hWnd = browser->GetHost()->GetWindowHandle();
		//HWND hWnd = GetWindowHandle();
		//if (hWnd) {
		//	SetParent(hWnd, hParentWnd);
		//	SetWindowPos(hWnd, NULL, x, y,static_cast<int>(width), static_cast<int>(height),SWP_NOZORDER);
		//	::ShowWindow(hWnd, SW_SHOW);
		//}
	}

	void BrowserWindow::SetBounds(int nBrowserId, int x, int y, size_t width, size_t height)
	{
		CefRefPtr<CefBrowser> pBrowser = GetBrowser(nBrowserId);
		if (pBrowser){
			HWND hWnd = pBrowser->GetHost()->GetWindowHandle();
			if (hWnd){
				SetWindowPos(hWnd, NULL, x, y,static_cast<int>(width), static_cast<int>(height),SWP_NOZORDER);
				::ShowWindow(hWnd, SW_SHOW);
				pBrowser->GetHost()->SetFocus(true);
			}
		}
	}

	void BrowserWindow::ShowBrowser(int nBrowserId, int x, int y, size_t width, size_t height)
	{
		REQUIRE_MAIN_THREAD();

		std::vector<CefRefPtr<CefBrowser>>::iterator item = m_ClientHandler->m_BrowserList.begin();
		for (; item != m_ClientHandler->m_BrowserList.end(); item++)
		{
			if (*item){
				HWND hWnd = (*item)->GetHost()->GetWindowHandle();
				if ((*item)->GetIdentifier() == nBrowserId){
					if (hWnd){
						SetWindowPos(hWnd, NULL, x, y,static_cast<int>(width), static_cast<int>(height),SWP_NOZORDER);
						::ShowWindow(hWnd, SW_SHOW);
						(*item)->GetHost()->SetFocus(true);
					}
				}else{
					if (hWnd){
						SetWindowPos(hWnd, NULL,0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
					}
				}
			}
		}
	}

	CefWindowHandle BrowserWindow::GetWindowHandle(int nBrowserId) const
	{
		REQUIRE_MAIN_THREAD();

		CefRefPtr<CefBrowser> pBrowser = GetBrowser(nBrowserId);
		if (pBrowser)
			return pBrowser->GetHost()->GetWindowHandle();
		return NULL;
	}

	CefRefPtr<CefBrowser> BrowserWindow::GetBrowser(int nBrowserId) const
	{
		REQUIRE_MAIN_THREAD();
		std::vector<CefRefPtr<CefBrowser>>::iterator item = m_ClientHandler->m_BrowserList.begin();
		for (; item != m_ClientHandler->m_BrowserList.end(); item++)
		{
			if ((*item)->GetIdentifier() == nBrowserId){
				return (*item);
			}
		}
		return NULL;
	}

	void BrowserWindow::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();

		m_Delegate->OnBrowserCreated(browser);
	}

	void BrowserWindow::OnBrowserClosing(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();
	}

	void BrowserWindow::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();
	}

	void BrowserWindow::OnBrowserAllClosed()
	{
		REQUIRE_MAIN_THREAD();

		m_ClientHandler->DetachDelegate();
		m_ClientHandler = NULL;

		// |this| may be deleted.
		m_Delegate->OnBrowserAllClosed();
	}

	void BrowserWindow::OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url)
	{
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetAddress(browser, url);
	}

	void BrowserWindow::OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetTitle(browser, title);
	}

	void BrowserWindow::OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
	{
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetFullscreen(browser, fullscreen);
	}

	void BrowserWindow::OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward)
	{
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetLoadingState(browser, isLoading, canGoBack, canGoForward);
	}

	void BrowserWindow::OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions)
	{
		REQUIRE_MAIN_THREAD();
		m_Delegate->OnSetDraggableRegions(browser, regions);
	}

	void BrowserWindow::OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url)
	{
		m_Delegate->OnNewTab(browser, url);
	}
}
