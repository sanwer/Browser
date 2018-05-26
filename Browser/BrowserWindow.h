#ifndef __BROWSER_WINDOW_H__
#define __BROWSER_WINDOW_H__
#pragma once
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_browser.h"
#include "ClientHandler.h"
#include "MessageLoop.h"

namespace Browser
{
	class BrowserWindow : public ClientHandler::Delegate
	{
	public:
		class Delegate {
		public:
			virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) = 0;
			virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) = 0;
			virtual void OnBrowserAllClosed() = 0;
			virtual void OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url) = 0;
			virtual void OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title) = 0;
			virtual void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) = 0;
			virtual void OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) = 0;
			virtual void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) = 0;
			virtual void OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) = 0;

		protected:
			virtual ~Delegate() {}
		};

		// Constructor may be called on any thread.
		// |delegate| must outlive this object.
		explicit BrowserWindow(Delegate* delegate, const CefString& startup_url);


		// Create a new browser and native window.
		virtual void CreateBrowser(
			CefWindowHandle parent_handle,
			const CefString& url,
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
		virtual void ShowPopup(int nBrowserId, CefWindowHandle hParentWnd, int x, int y, size_t width, size_t height);

		// Show the window.
		virtual void ShowBrowser(int nBrowserId, int x, int y, size_t width, size_t height);

		// Returns the window handle.
		virtual CefWindowHandle GetWindowHandle(int nBrowserId) const;

		// Returns the browser owned by the window.
		CefRefPtr<CefBrowser> GetBrowser(int nBrowserId) const;

	protected:
		// Allow deletion via scoped_ptr only.
		friend struct base::DefaultDeleter<BrowserWindow>;

		// ClientHandler::Delegate methods.
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosing(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserAllClosed() OVERRIDE;
		void OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url) OVERRIDE;
		void OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
		void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) OVERRIDE;
		void OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) OVERRIDE;
		void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) OVERRIDE;
		void OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) OVERRIDE;

		Delegate* m_Delegate;
		CefRefPtr<ClientHandler> m_ClientHandler;

	private:
		DISALLOW_COPY_AND_ASSIGN(BrowserWindow);
	};
}

#endif
