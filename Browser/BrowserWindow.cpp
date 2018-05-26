#include "stdafx.h"
#include "MainContext.h"
#include "BrowserWindow.h"
#include "include/base/cef_bind.h"

namespace Browser
{
	BrowserWindow::BrowserWindow(Delegate* delegate, const CefString& startup_url)
		: m_Delegate(delegate)
	{
		DCHECK(m_Delegate);
		m_ClientHandler = new ClientHandler(this, startup_url);
	}

	void BrowserWindow::CreateBrowser(
		CefWindowHandle parent_handle,
		const CefString& url,
		const CefRect& rect,
		const CefBrowserSettings& settings,
		CefRefPtr<CefRequestContext> request_context)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		CefWindowInfo window_info;
		RECT wnd_rect = {rect.x, rect.y, rect.x + rect.width, rect.y + rect.height};
		window_info.SetAsChild(parent_handle, wnd_rect);

		CefBrowserHost::CreateBrowser(window_info, m_ClientHandler,
			url.empty() ? m_ClientHandler->StartupUrl() : url, settings, request_context);
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

	void BrowserWindow::ShowPopup(int nBrowserId, CefWindowHandle hParentWnd, int x, int y, size_t width, size_t height)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		CefRefPtr<CefBrowser> pBrowser = m_ClientHandler->GetBrowser(nBrowserId);
		if (pBrowser){
			HWND hWnd = pBrowser->GetHost()->GetWindowHandle();
			if (hWnd){
				SetParent(hWnd, hParentWnd);
				SetWindowPos(hWnd, NULL, x, y, static_cast<int>(width), static_cast<int>(height), SWP_NOZORDER);
				::ShowWindow(hWnd, SW_SHOW);
				pBrowser->GetHost()->SetFocus(true);
			}
		}
	}

	void BrowserWindow::ShowBrowser(int nBrowserId, int x, int y, size_t width, size_t height)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		CefRefPtr<CefBrowser> pBrowser = m_ClientHandler->GetBrowser(nBrowserId);
		std::vector<CefRefPtr<CefBrowser>>::iterator item = m_ClientHandler->m_BrowserList.begin();
		for (; item != m_ClientHandler->m_BrowserList.end(); item++)
		{
			if (*item){
				HWND hWnd = (*item)->GetHost()->GetWindowHandle();
				if ((*item)->IsSame(pBrowser)){
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
		DCHECK(CefCurrentlyOn(TID_UI));

		CefRefPtr<CefBrowser> pBrowser = m_ClientHandler->GetBrowser(nBrowserId);
		if (pBrowser)
			return pBrowser->GetHost()->GetWindowHandle();
		return NULL;
	}

	CefRefPtr<CefBrowser> BrowserWindow::GetBrowser(int nBrowserId) const
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		return m_ClientHandler->GetBrowser(nBrowserId);;
	}

	void BrowserWindow::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		m_Delegate->OnBrowserCreated(browser);
	}

	void BrowserWindow::OnBrowserClosing(CefRefPtr<CefBrowser> browser)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
	}

	void BrowserWindow::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
	}

	void BrowserWindow::OnBrowserAllClosed()
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		m_ClientHandler->DetachDelegate();
		m_ClientHandler = NULL;

		// |this| may be deleted.
		m_Delegate->OnBrowserAllClosed();
	}

	void BrowserWindow::OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		m_Delegate->OnSetAddress(browser, url);
	}

	void BrowserWindow::OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		m_Delegate->OnSetTitle(browser, title);
	}

	void BrowserWindow::OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		m_Delegate->OnSetFullscreen(browser, fullscreen);
	}

	void BrowserWindow::OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		m_Delegate->OnSetLoadingState(browser, isLoading, canGoBack, canGoForward);
	}

	void BrowserWindow::OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		m_Delegate->OnSetDraggableRegions(browser, regions);
	}

	void BrowserWindow::OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url)
	{
		m_Delegate->OnNewTab(browser, url);
	}
}
