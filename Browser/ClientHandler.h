#ifndef __BROWSER_HANDLER_H__
#define __BROWSER_HANDLER_H__
#pragma once
#include <set>
#include <string>
#include "include/cef_version.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"

namespace Browser
{
	class ClientHandler
		: public CefClient				// 浏览器客户区
		, public CefContextMenuHandler	// 浏览器上下文菜单处理
		, public CefDisplayHandler		// 浏览器显示处理
		, public CefDownloadHandler		// 浏览器下载处理
		, public CefDragHandler
		, public CefJSDialogHandler		// 浏览器JS对话框处理
		, public CefKeyboardHandler		// 浏览器键盘处理
		, public CefLifeSpanHandler		// 浏览器生命周期处理
		, public CefLoadHandler			// 浏览器加载处理
		, public CefRequestHandler		// 浏览器请求处理
	{
	public:
		friend class BrowserWindow;
		class Delegate {
		public:
			virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) = 0;
			virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) = 0;
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
		typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

	public:
		ClientHandler(Delegate* delegate, const CefString& startup_url);
		void DetachDelegate();

		//////////////////////////////////////////////////////////////////////////
		// CefClient methods
		CefRefPtr<CefContextMenuHandler>	GetContextMenuHandler()	OVERRIDE {return this;}
		CefRefPtr<CefDisplayHandler>		GetDisplayHandler()		OVERRIDE {return this;}
		CefRefPtr<CefDownloadHandler>		GetDownloadHandler()	OVERRIDE {return this;}
		CefRefPtr<CefDragHandler>			GetDragHandler()		OVERRIDE {return this;}
		CefRefPtr<CefJSDialogHandler>		GetJSDialogHandler()	OVERRIDE {return this;}
		CefRefPtr<CefKeyboardHandler>		GetKeyboardHandler()	OVERRIDE {return this;}
		CefRefPtr<CefLifeSpanHandler>		GetLifeSpanHandler()	OVERRIDE {return this;}
		CefRefPtr<CefLoadHandler>			GetLoadHandler()		OVERRIDE {return this;}
		CefRefPtr<CefRequestHandler>		GetRequestHandler()		OVERRIDE {return this;}

		bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefContextMenuHandler methods
		void OnBeforeContextMenu(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefContextMenuParams> params,
			CefRefPtr<CefMenuModel> model) OVERRIDE;

		bool OnContextMenuCommand(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefContextMenuParams> params,
			int command_id,
			EventFlags event_flags) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefDisplayHandler methods
		void OnAddressChange(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& url) OVERRIDE;
		void OnTitleChange(
			CefRefPtr<CefBrowser> browser,
			const CefString& title) OVERRIDE;
		void OnFaviconURLChange(
			CefRefPtr<CefBrowser> browser,
			const std::vector<CefString>& icon_urls) OVERRIDE;
		void OnFullscreenModeChange(
			CefRefPtr<CefBrowser> browser,
			bool fullscreen) OVERRIDE;
		bool OnConsoleMessage(
			CefRefPtr<CefBrowser> browser,
#if CHROME_VERSION_BUILD >= 3239
			cef_log_severity_t level,
#endif
			const CefString& message,
			const CefString& source,
			int line) OVERRIDE;
		bool OnTooltip(
			CefRefPtr<CefBrowser> browser,
			CefString& text) OVERRIDE;
		void OnStatusMessage(
			CefRefPtr<CefBrowser> browser,
			const CefString& value) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefDownloadHandler methods
		void OnBeforeDownload(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDownloadItem> download_item,
			const CefString& suggested_name,
			CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;
		void OnDownloadUpdated(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDownloadItem> download_item,
			CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefDragHandler methods
		bool OnDragEnter(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDragData> dragData,
			CefDragHandler::DragOperationsMask mask) OVERRIDE;
		void OnDraggableRegionsChanged(
			CefRefPtr<CefBrowser> browser,
			const std::vector<CefDraggableRegion>& regions) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefKeyboardHandler methods
		bool OnPreKeyEvent(
			CefRefPtr<CefBrowser> browser,
			const CefKeyEvent& event,
			CefEventHandle os_event,
			bool* is_keyboard_shortcut) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefLifeSpanHandler methods
		virtual bool OnBeforePopup(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& target_url,
			const CefString& target_frame_name,
			cef_window_open_disposition_t target_disposition,
			bool user_gesture,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings,
			bool* no_javascript_access) OVERRIDE;

		void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
		bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
		void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefLoadHandler methods
		void OnLoadingStateChange(
			CefRefPtr<CefBrowser> browser,
			bool isLoading,
			bool canGoBack,
			bool canGoForward) OVERRIDE;
		void OnLoadStart(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame
			#if CHROME_VERSION_BUILD >= 2743
			,TransitionType transition_type
#endif
			) OVERRIDE;
		void OnLoadEnd(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			int httpStatusCode) OVERRIDE;
		void OnLoadError(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl) OVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		// CefRequestHandler methods
		bool OnBeforeBrowse(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
#if CHROME_VERSION_BUILD >= 3359
			bool user_gesture,
#endif
			bool is_redirect) OVERRIDE;
		bool OnOpenURLFromTab(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			const CefString& target_url,
			cef_window_open_disposition_t target_disposition,
			bool user_gesture) OVERRIDE;
		cef_return_value_t OnBeforeResourceLoad(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefRequestCallback> callback) OVERRIDE;
		CefRefPtr<CefResourceHandler> GetResourceHandler(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request) OVERRIDE;
		CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request,
			CefRefPtr<CefResponse> response) OVERRIDE;
		bool OnQuotaRequest(
			CefRefPtr<CefBrowser> browser,
			const CefString& origin_url,
			int64 new_size,
			CefRefPtr<CefRequestCallback> callback) OVERRIDE;
		void OnProtocolExecution(
			CefRefPtr<CefBrowser> browser,
			const CefString& url,
			bool& allow_os_execution) OVERRIDE;
		bool OnCertificateError(
			CefRefPtr<CefBrowser> browser,
			ErrorCode cert_error,
			const CefString& request_url,
			CefRefPtr<CefSSLInfo> ssl_info,
			CefRefPtr<CefRequestCallback> callback) OVERRIDE;
		void OnRenderProcessTerminated(
			CefRefPtr<CefBrowser> browser,
			TerminationStatus status) OVERRIDE;

		// Returns the Delegate.
		Delegate* delegate() const { return m_Delegate; }

		// Returns the Browser.
		CefRefPtr<CefBrowser> GetBrowser(int nBrowserId=-1);

		// Show a new DevTools popup window.
		void ShowDevTools(CefRefPtr<CefBrowser> browser,
			const CefPoint& inspect_element_at);

		// Close the existing DevTools popup window, if any.
		void CloseDevTools(CefRefPtr<CefBrowser> browser);

		// Returns the startup URL.
		std::string StartupUrl() const { return m_sStartupUrl; }

	private:
		bool CreatePopupWindow(
			CefRefPtr<CefBrowser> browser,
			bool is_devtools,
			const CefPopupFeatures& popupFeatures,
			CefWindowInfo& windowInfo,
			CefRefPtr<CefClient>& client,
			CefBrowserSettings& settings);

		// Execute Delegate notifications on the main thread.
		void NotifyBrowserCreated(CefRefPtr<CefBrowser> browser);
		void NotifyBrowserClosing(CefRefPtr<CefBrowser> browser);
		void NotifyBrowserClosed(CefRefPtr<CefBrowser> browser);
		void NotifyBrowserAllClosed();
		void NotifyAddress(CefRefPtr<CefBrowser> browser, const CefString& url);
		void NotifyTitle(CefRefPtr<CefBrowser> browser, const CefString& title);
		void NotifyFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen);
		void NotifyLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward);
		void NotifyDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions);
		void NotifyNewTab(CefRefPtr<CefBrowser> browser, const CefString& url);

	private:
		Delegate* m_Delegate;
		std::vector<CefRefPtr<CefBrowser>> m_BrowserList;
		int m_nDefaultBrowserId;

		// Handles the browser side of query routing. The renderer side is handled
		// in client_renderer.cc.
		CefRefPtr<CefMessageRouterBrowserSide> m_MessageRouter;

		// Manages the registration and delivery of resources.
		CefRefPtr<CefResourceManager> m_ResourceManager;

		// Set of Handlers registered with the message router.
		MessageHandlerSet m_MessageHandlerSet;

		// True if this handler uses off-screen rendering.
		//const bool is_osr_;

		// The startup URL.
		const std::string m_sStartupUrl;

		// Console logging state.
		const std::string m_sConsoleLogFile;
		bool m_bFirstConsoleMessage;

		// True if an editable field currently has focus.
		bool m_bFocusOnEditableField;

		// True if mouse cursor change is disabled.
		bool m_bMouseCursorChangeDisabled;

		IMPLEMENT_REFCOUNTING(ClientHandler);
		//IMPLEMENT_LOCKING(ClientHandler);
		DISALLOW_COPY_AND_ASSIGN(ClientHandler);
	};
}

#endif
