#include "stdafx.h"
#include <stdio.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include "include/base/cef_bind.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_parser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_closure_task.h"
#include "ClientHandler.h"
#include "MessageLoop.h"
#include "BrowserManager.h"

namespace Browser
{
	// Custom menu command Ids.
	#define CLIENT_ID_REFRESH		(MENU_ID_USER_FIRST)
	#define CLIENT_ID_SHOW_DEVTOOLS	(MENU_ID_USER_FIRST+1)
	#define CLIENT_ID_SAMPLE		(MENU_ID_USER_FIRST+2)

	ClientHandler::ClientHandler(Delegate* delegate)
		: m_Delegate(delegate),
		m_MessageRouter(NULL)
	{
	}

	void ClientHandler::DetachDelegate()
	{
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::DetachDelegate, this));
			return;
		}

		DCHECK(m_Delegate);
		m_Delegate = NULL;
	}

	bool ClientHandler::OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)
	{
		CEF_REQUIRE_UI_THREAD();
		if (m_MessageRouter->OnProcessMessageReceived(browser, source_process,message)) {
			return true;
		}

		return false;
	}

	void ClientHandler::OnBeforeContextMenu(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model)
	{
		CEF_REQUIRE_UI_THREAD();
		//model->Clear();//清空上下文菜单项

		//if(model->IsVisible(MENU_ID_PRINT))
		//	model->Remove(MENU_ID_PRINT);//删除打印菜单

		//if(model->IsVisible(MENU_ID_VIEW_SOURCE))
		//	model->Remove(MENU_ID_VIEW_SOURCE);//删除查看源码菜单

		//model->AddSeparator();//增加分隔符

		model->AddItem(CLIENT_ID_REFRESH,	CefString(L"刷新(&R)"));
		model->AddItem(CLIENT_ID_SHOW_DEVTOOLS,	CefString(L"开发工具"));
		//model->AddItem(CLIENT_ID_SAMPLE,	CefString(L"计算机名"));
	}

	bool ClientHandler::OnContextMenuCommand(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags)
	{
		CEF_REQUIRE_UI_THREAD();
		switch (command_id)
		{
		case CLIENT_ID_REFRESH:
			browser->Reload();
			return true;
		case CLIENT_ID_SHOW_DEVTOOLS:
			{
				CefWindowInfo windowInfo;
				CefBrowserSettings settings;
				HWND hWnd = browser->GetHost()->GetWindowHandle();
				RECT rcWnd={0};
				::GetWindowRect(hWnd, &rcWnd);
				windowInfo.SetAsPopup(hWnd,L"开发工具");
				CefPoint point;
				point.Set(rcWnd.left,rcWnd.top);
				browser->GetHost()->ShowDevTools(windowInfo,this,settings,point);
				return true;
			}
		case CLIENT_ID_SAMPLE:
			{
				std::string html =
					"<html><body>"
					"<script language=\"JavaScript\">"
					"document.writeln('ComputerName:');"
					"document.writeln(Client.GetComputerName());"
					"</script>"
					"</body></html>";
				frame->LoadString(html, "about:blank");
			}
			return true;
		default:
			return false;
		}
	}

	void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
	{
		CEF_REQUIRE_UI_THREAD();
		if (frame->IsMain())
			NotifyAddress(browser, url);
	}

	void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		CEF_REQUIRE_UI_THREAD();
		NotifyTitle(browser, title);
	}

	void ClientHandler::OnFaviconURLChange(
		CefRefPtr<CefBrowser> browser,
		const std::vector<CefString>& icon_urls)
	{
		CEF_REQUIRE_UI_THREAD();
	}

	void ClientHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
	{
		CEF_REQUIRE_UI_THREAD();
		NotifyFullscreen(browser, fullscreen);
	}

	bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
	{
		CEF_REQUIRE_UI_THREAD();

		//FILE* file = fopen(m_strConsoleLog.c_str(), "a");
		//if (file) {
		//	std::stringstream ss;
		//	ss << "Message: " << message.ToString() << NEWLINE <<
		//		"Source: " << source.ToString() << NEWLINE <<
		//		"Line: " << line << NEWLINE <<
		//		"-----------------------" << NEWLINE;
		//	fputs(ss.str().c_str(), file);
		//	fclose(file);

		//	if (m_bFirstConsoleMessage) {
		//		test_runner::Alert(
		//			browser, "Console messages written to \"" + m_strConsoleLog + "\"");
		//		m_bFirstConsoleMessage = false;
		//	}
		//}

		return false;
	}

	bool ClientHandler::OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text)
	{
		CEF_REQUIRE_UI_THREAD();

		//if (m_pEvent != NULL)
		//	m_pEvent->OnTooltip(text);

		return false;
	}
	void ClientHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser,
		const CefString& value)
	{
		CEF_REQUIRE_UI_THREAD();
	}

	void ClientHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		// Continue the download and show the "Save As" dialog.
		callback->Continue(BrowserManager::Get()->GetDownloadPath(suggested_name), true);
	}

	void ClientHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		if (download_item->IsComplete())
		{
			//SetLastDownloadFile(download_item->GetFullPath());
			//SendNotification(NOTIFY_DOWNLOAD_COMPLETE);
		}
	}

	bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> dragData,
		CefDragHandler::DragOperationsMask mask)
	{
		CEF_REQUIRE_UI_THREAD();

		// Forbid dragging of link URLs.
		if (mask & DRAG_OPERATION_LINK)
			return true;

		return false;
	}

	void ClientHandler::OnDraggableRegionsChanged(
		CefRefPtr<CefBrowser> browser,
		const std::vector<CefDraggableRegion>& regions)
	{
		CEF_REQUIRE_UI_THREAD();

		//NotifyDraggableRegions(regions);
	}

	bool ClientHandler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser,
		const CefString& requesting_url,
		int request_id,
		CefRefPtr<CefGeolocationCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		// 允许来自所有网站的地理位置访问.
		callback->Continue(true);
		return true;
	}

	bool ClientHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut)
	{
		if (!event.focus_on_editable_field && event.windows_key_code == 0x20){
			if (event.type == KEYEVENT_RAWKEYDOWN){
				//browser->GetMainFrame()->ExecuteJavaScript("alert('You pressed the space bar!');", "", 0);
			}
			return true;
		}

		return false;
	}

	bool ClientHandler::OnBeforePopup(
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
		bool* no_javascript_access)
	{
		CEF_REQUIRE_IO_THREAD();

		// Return true to cancel the popup window.
		bool bRet = CreatePopupWindow(browser, frame, target_url, target_frame_name, target_disposition, user_gesture, popupFeatures, windowInfo, client,settings,no_javascript_access);
		return !bRet;
	}

	void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		if (!m_MessageRouter) {
			// Create the browser-side router for query handling.
			CefMessageRouterConfig config;
			m_MessageRouter = CefMessageRouterBrowserSide::Create(config);

			// Register handlers with the router.
			//test_runner::CreateMessageHandlers(m_MessageHandlerSet);
			MessageHandlerSet::const_iterator it = m_MessageHandlerSet.begin();
			for (; it != m_MessageHandlerSet.end(); ++it)
				m_MessageRouter->AddHandler(*(it), false);
		}

		m_BrowserList.push_back(browser);

		NotifyBrowserCreated(browser);
	}

	bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		NotifyBrowserClosing(browser);

		HWND hWnd = browser->GetHost()->GetWindowHandle();
		browser = NULL;
		if(hWnd){
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}

		// Allow the close. For windowed browsers this will result in the OS close event being sent.
		return false;
	}

	void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		//AutoLock lock_scope(this);

		m_MessageRouter->OnBeforeClose(browser);

		std::vector<CefRefPtr<CefBrowser>>::iterator item = m_BrowserList.begin();
		for (; item != m_BrowserList.end(); item++)
		{
			if ((*item)->IsSame(browser)){
				m_BrowserList.erase(item);
				browser = NULL;
				break;
			}
		}

		NotifyBrowserClosed(browser);

		if (m_BrowserList.empty()) {
			// Remove and delete message router handlers.
			MessageHandlerSet::const_iterator it = m_MessageHandlerSet.begin();
			for (; it != m_MessageHandlerSet.end(); ++it) {
				m_MessageRouter->RemoveHandler(*(it));
				delete *(it);
			}
			m_MessageHandlerSet.clear();
			m_MessageRouter = NULL;
		}
	}

	void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
	{
		CEF_REQUIRE_UI_THREAD();

		NotifyLoadingState(browser, isLoading, canGoBack, canGoForward);
	}

	void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame/*,TransitionType transition_type*/)
	{
		CEF_REQUIRE_UI_THREAD();

		//if (m_BrowserId == browser->GetIdentifier() && frame->IsMain())
		//{
		//	// We've just started loading a page
		//	SetLoading(true);
		//	Invoke_LoadStart(browser, frame);
		//}
	}

	void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
	{
		CEF_REQUIRE_UI_THREAD();

		//if (m_BrowserId == browser->GetIdentifier() && frame->IsMain())
		//{
		//	frame->ExecuteJavaScript("alert('ExecuteJavaScript works!');", frame->GetURL(), 0);
		//	CefRefPtr<CefV8Context> v8 = frame->GetV8Context();
		//}
	}

	void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl)
	{
		CEF_REQUIRE_UI_THREAD();

		// Don't display an error for downloaded files.
		if (errorCode == ERR_ABORTED)
			return;

		// Don't display an error for external protocols that we allow the OS to
		// handle. See OnProtocolExecution().
		if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
			std::string urlStr = frame->GetURL();
			if (urlStr.find("spotify:") == 0)
				return;
		}

		// Load the error page.
		//LoadErrorPage(frame, failedUrl, errorCode, errorText);
	}


	bool ClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect)
	{
		CEF_REQUIRE_UI_THREAD();

		m_MessageRouter->OnBeforeBrowse(browser, frame);
		return false;
	}

	cef_return_value_t ClientHandler::OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefRequestCallback> callback)
	{
		//OutputDebugStringW(request->GetURL().c_str());
		//https://easylist-downloads.adblockplus.org/easylistchina.txt
		//正则过滤
		return RV_CONTINUE;
	}

	bool ClientHandler::OnOpenURLFromTab(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		CefRequestHandler::WindowOpenDisposition target_disposition,
		bool user_gesture) {
			if (target_disposition == WOD_NEW_BACKGROUND_TAB ||
				target_disposition == WOD_NEW_FOREGROUND_TAB) {
					// Handle middle-click and ctrl + left-click by opening the URL in a new
					// browser window.
					//MainContext::Get()->GetRootWindowManager()->CreateBrowser(true, is_osr(), CefRect(), target_url);
					return true;
			}

			// Open the URL in the current browser window.
			return false;
	}

	bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
		const CefString& origin_url,
		int64 new_size,
		CefRefPtr<CefRequestCallback> callback) {
			CEF_REQUIRE_IO_THREAD();

			static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

			// Grant the quota request if the size is reasonable.
			callback->Continue(new_size <= max_size);
			return true;
	}

	void ClientHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
		const CefString& url,
		bool& allow_os_execution) {
			CEF_REQUIRE_UI_THREAD();

			std::wstring urlStr = url;

			// Allow OS execution of Spotify URIs.
			if (urlStr.find(L"spotify:") == 0)
				allow_os_execution = true;
	}

	void ClientHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
	{
		CEF_REQUIRE_UI_THREAD();
		m_MessageRouter->OnRenderProcessTerminated(browser);
	}

	bool ClientHandler::CreatePopupWindow(
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
		bool* no_javascript_access)
	{
		// Redirect all popup page into the source frame forcefully
		//frame->LoadURL(target_url);

		if(target_disposition == WOD_NEW_POPUP){
			BrowserManager::Get()->CreateRootWindowAsPopup(false, target_url, popupFeatures, windowInfo, client, settings);
			return true;
		}else{
			NotifyNewTab(browser,target_url);
		}
		return false;
	}

	void ClientHandler::NotifyBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyBrowserCreated, this, browser));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnBrowserCreated(browser);
	}

	void ClientHandler::NotifyBrowserClosing(CefRefPtr<CefBrowser> browser)
	{
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyBrowserClosing, this, browser));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnBrowserClosing(browser);
	}

	void ClientHandler::NotifyBrowserClosed(CefRefPtr<CefBrowser> browser) {
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyBrowserClosed, this, browser));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnBrowserClosed(browser);
	}

	void ClientHandler::NotifyAddress(CefRefPtr<CefBrowser> browser, const CefString& url) {
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyAddress, this, browser, url));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnSetAddress(browser, url);
	}

	void ClientHandler::NotifyTitle(CefRefPtr<CefBrowser> browser, const CefString& title) {
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyTitle, this, browser, title));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnSetTitle(browser, title);
	}

	void ClientHandler::NotifyFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) {
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyFullscreen, this, browser, fullscreen));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnSetFullscreen(browser, fullscreen);
	}

	void ClientHandler::NotifyLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward) {
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyLoadingState, this, browser, isLoading, canGoBack, canGoForward));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnSetLoadingState(browser, isLoading, canGoBack, canGoForward);
	}

	void ClientHandler::NotifyDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions) {
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyDraggableRegions, this, browser, regions));
			return;
		}

		if (m_Delegate)
			m_Delegate->OnSetDraggableRegions(browser, regions);
	}

	void ClientHandler::NotifyNewTab(CefRefPtr<CefBrowser> browser, const CefString& url)
	{
		if (!CURRENTLY_ON_MAIN_THREAD()) {
			// Execute this method on the main thread.
			MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyNewTab, this, browser, url));
			return;
		}

		if (m_Delegate){
			if(wcsnicmp(url.c_str(),L"chrome-devtools:",16)!=0){
				m_Delegate->OnNewPage(url);
			}
		}
	}
}
