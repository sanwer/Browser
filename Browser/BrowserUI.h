#ifndef __BROWSERUI_H__
#define __BROWSERUI_H__
#pragma once
#include "ClientHandler.h"
#include <UIlib.h>

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
			virtual void OnBrowserWindowDestroyed() = 0;
			virtual void OnSetAddress(const std::wstring& url) = 0;
			virtual void OnSetTitle(const std::wstring& title) = 0;
			virtual void OnSetFullscreen(bool fullscreen) = 0;
			virtual void OnSetLoadingState(bool isLoading,bool canGoBack,bool canGoForward) = 0;
			virtual void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) = 0;

		protected:
			virtual ~Delegate() {}
		};

		// Constructor may be called on any thread.
		// |delegate| must outlive this object.
		explicit BrowserCtrl(Delegate* delegate, const std::wstring& startup_url);


		// Create a new browser and native window.
		virtual void CreateBrowser(
			CefWindowHandle parent_handle,
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
		virtual void ShowPopup(CefWindowHandle parent_handle, int x, int y, size_t width, size_t height);

		// Show the window.
		virtual void Show();

		// Hide the window.
		virtual void Hide();

		// Set the window bounds in parent coordinates.
		virtual void SetBounds(int x, int y, size_t width, size_t height);

		// Set focus to the window.
		virtual void SetFocus(bool focus);

		// Set the device scale factor. Only used in combination with off-screen
		// rendering.
		virtual void SetDeviceScaleFactor(float device_scale_factor);

		// Returns the device scale factor. Only used in combination with off-screen
		// rendering.
		virtual float GetDeviceScaleFactor() const;

		// Returns the window handle.
		virtual CefWindowHandle GetWindowHandle() const;

		// Returns the browser owned by the window.
		CefRefPtr<CefBrowser> GetBrowser() const;

		// Returns true if the browser is closing.
		bool IsClosing() const;

	protected:
		// Allow deletion via scoped_ptr only.
		friend struct base::DefaultDeleter<BrowserCtrl>;

		// ClientHandler::Delegate methods.
		void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosing(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBrowserClosed(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnSetAddress(const std::wstring& url) OVERRIDE;
		void OnSetTitle(const std::wstring& title) OVERRIDE;
		void OnSetFullscreen(bool fullscreen) OVERRIDE;
		void OnSetLoadingState(bool isLoading,bool canGoBack,bool canGoForward) OVERRIDE;
		void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) OVERRIDE;

		Delegate* m_Delegate;
		CefRefPtr<CefBrowser> m_Browser;
		CefRefPtr<ClientHandler> m_ClientHandler;
		bool m_IsClosing;

	private:
		DISALLOW_COPY_AND_ASSIGN(BrowserCtrl);
	};

	class BrowserUI : public DuiLib::CControlUI
	{
		DECLARE_DUICONTROL(BrowserUI)
	public:
		BrowserUI(BrowserDlg* pParent);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetPos(RECT rc, bool bNeedInvalidate = true);

	protected:
		BrowserDlg* m_pParent;

		DISALLOW_COPY_AND_ASSIGN(BrowserUI);
	};
}

#endif
