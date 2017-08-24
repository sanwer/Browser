#include "BrowserHandler.h"
#include <stdio.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

//#include "include/wrapper/cef_closure_task.h"
//#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_bind.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_closure_task.h"
//#include "cefclient/browser/main_context.h"
//#include "cefclient/browser/resource_util.h"
//#include "cefclient/browser/root_window_manager.h"
//#include "cefclient/browser/test_runner.h"
//#include "cefclient/common/client_switches.h"
//#include "MainMessageLoop.h"

namespace Browser
{
	// Custom menu command Ids.
	enum client_menu_ids {
		CLIENT_ID_REFRESH   = MENU_ID_USER_FIRST,
		CLIENT_ID_SAMPLE,
	};

	CBrowserHandler::CBrowserHandler(Delegate* delegate,bool is_osr,const std::wstring& startup_url)
		: startup_url_(startup_url)
		, delegate_(delegate)
		, browser_count_(0)
		//, console_log_file_(MainContext::Get()->GetConsoleLogPath())
		, first_console_message_(true)
		, focus_on_editable_field_(false)
		//, m_Browser(NULL)
		//, m_pEvent(NULL)
		//, m_hParentHandle(NULL)
		//, m_BrowserId(0)
		//, m_Cookie(NULL)
	{
			//DCHECK(!console_log_file_.empty());

			// Read command line settings.
			//CefRefPtr<CefCommandLine> command_line =
			//	CefCommandLine::GetGlobalCommandLine();
			//mouse_cursor_change_disabled_ =
			//	command_line->HasSwitch(switches::kMouseCursorChangeDisabled);
	}

	void CBrowserHandler::DetachDelegate()
	{
		//if (!CURRENTLY_ON_MAIN_THREAD()) {
		//	MAIN_POST_CLOSURE(base::Bind(&CBrowserHandler::DetachDelegate, this));
		//	return;
		//}

		DCHECK(delegate_);
		delegate_ = NULL;
	}

	bool CBrowserHandler::OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)
	{
		CEF_REQUIRE_UI_THREAD();

		if (message_router_->OnProcessMessageReceived(browser, source_process,message)) {
			return true;
		}

		// Check for messages from the client renderer.
		//std::string message_name = message->GetName();
		//if (message_name == kFocusedNodeChangedMessage) {
		//	// A message is sent from ClientRenderDelegate to tell us whether the
		//	// currently focused DOM node is editable. Use of |focus_on_editable_field_|
		//	// is redundant with CefKeyEvent.focus_on_editable_field in OnPreKeyEvent
		//	// but is useful for demonstration purposes.
		//	focus_on_editable_field_ = message->GetArgumentList()->GetBool(0);
		//	return true;
		//}

		return false;
	}

	void CBrowserHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model)
	{
		//model->Clear();//清空上下文菜单项

		//if(model->IsVisible(MENU_ID_PRINT))
		//	model->Remove(MENU_ID_PRINT);//删除打印菜单

		//if(model->IsVisible(MENU_ID_VIEW_SOURCE))
		//	model->Remove(MENU_ID_VIEW_SOURCE);//删除查看源码菜单
		
		//model->AddSeparator();//增加分隔符

		model->AddItem(CLIENT_ID_REFRESH,	CefString(L"刷新(&R)"));
		//model->AddItem(CLIENT_ID_SAMPLE,	CefString(L"计算机名"));
	}

	bool CBrowserHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags)
	{
		switch (command_id)
		{
		case CLIENT_ID_REFRESH:
			browser->Reload();
			return true;
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

	void CBrowserHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url)
	{
		CEF_REQUIRE_UI_THREAD();
		if (frame->IsMain())
			NotifyAddress(url);
	}

	void CBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		CEF_REQUIRE_UI_THREAD();
		
		NotifyTitle(title);
	}

	void CBrowserHandler::OnFaviconURLChange(
		CefRefPtr<CefBrowser> browser,
		const std::vector<CefString>& icon_urls)
	{
		CEF_REQUIRE_UI_THREAD();

		//if (!icon_urls.empty() && download_favicon_images_) {
		//	browser->GetHost()->DownloadImage(icon_urls[0], true, 16, false,
		//		new ClientDownloadImageCallback(this));
		//}
	}

	void CBrowserHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
		bool fullscreen) {
			CEF_REQUIRE_UI_THREAD();

			NotifyFullscreen(fullscreen);
	}

	bool CBrowserHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
		const CefString& message,
		const CefString& source,
		int line) {
			CEF_REQUIRE_UI_THREAD();

			//FILE* file = fopen(console_log_file_.c_str(), "a");
			//if (file) {
			//	std::stringstream ss;
			//	ss << "Message: " << message.ToString() << NEWLINE <<
			//		"Source: " << source.ToString() << NEWLINE <<
			//		"Line: " << line << NEWLINE <<
			//		"-----------------------" << NEWLINE;
			//	fputs(ss.str().c_str(), file);
			//	fclose(file);

			//	if (first_console_message_) {
			//		test_runner::Alert(
			//			browser, "Console messages written to \"" + console_log_file_ + "\"");
			//		first_console_message_ = false;
			//	}
			//}

			return false;
	}

	bool CBrowserHandler::OnTooltip(CefRefPtr<CefBrowser> browser,
		CefString& text)
	{
		//CEF_REQUIRE_UI_THREAD();

		//if (m_pEvent != NULL)
		//	m_pEvent->OnTooltip(text);

		return false;
	}
	void CBrowserHandler::OnStatusMessage(CefRefPtr<CefBrowser> browser,
		const CefString& value)
	{
		CEF_REQUIRE_UI_THREAD();
	}

	void CBrowserHandler::OnBeforeDownload(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		// Continue the download and show the "Save As" dialog.
		//callback->Continue(MainContext::Get()->GetDownloadPath(suggested_name), true);
	}

	void CBrowserHandler::OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
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

	bool CBrowserHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDragData> dragData,
		CefDragHandler::DragOperationsMask mask)
	{
			CEF_REQUIRE_UI_THREAD();

			// Forbid dragging of link URLs.
			if (mask & DRAG_OPERATION_LINK)
				return true;

			return false;
	}

	void CBrowserHandler::OnDraggableRegionsChanged(
		CefRefPtr<CefBrowser> browser,
		const std::vector<CefDraggableRegion>& regions)
	{
			CEF_REQUIRE_UI_THREAD();

			//NotifyDraggableRegions(regions);
	}

	bool CBrowserHandler::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser,
		const CefString& requesting_url,
		int request_id,
		CefRefPtr<CefGeolocationCallback> callback)
	{
		CEF_REQUIRE_UI_THREAD();
		// 允许来自所有网站的地理位置访问.
		callback->Continue(true);
		return true;
	}

	bool CBrowserHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut)
	{
		if (!event.focus_on_editable_field && event.windows_key_code == 0x20)
		{
			if (event.type == KEYEVENT_RAWKEYDOWN)
			{
		//		browser->GetMainFrame()->ExecuteJavaScript("alert('You pressed the space bar!');", "", 0);
			}
			return true;
		}

		return false;
	}

	bool CBrowserHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
		return !CreatePopupWindow(browser, popupFeatures, windowInfo, client, settings);
	}

	void CBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		browser_count_++;

		if (!message_router_) {
			// Create the browser-side router for query handling.
			CefMessageRouterConfig config;
			message_router_ = CefMessageRouterBrowserSide::Create(config);

			// Register handlers with the router.
			//test_runner::CreateMessageHandlers(message_handler_set_);
			MessageHandlerSet::const_iterator it = message_handler_set_.begin();
			for (; it != message_handler_set_.end(); ++it)
				message_router_->AddHandler(*(it), false);
		}

		// Disable mouse cursor change if requested via the command-line flag.
		if (mouse_cursor_change_disabled_)
			browser->GetHost()->SetMouseCursorChangeDisabled(true);

		//NotifyBrowserCreated(browser);
	}

	bool CBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		//NotifyBrowserClosing(browser);
		//::PostMessage(m_hParentHandle, WM_CLOSE, 0, 0);

		// Allow the close. For windowed browsers this will result in the OS close
		// event being sent.
		return false;
	}

	void CBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
	{
		CEF_REQUIRE_UI_THREAD();

		if (--browser_count_ == 0) {
			// Remove and delete message router handlers.
			MessageHandlerSet::const_iterator it =
				message_handler_set_.begin();
			for (; it != message_handler_set_.end(); ++it) {
				message_router_->RemoveHandler(*(it));
				delete *(it);
			}
			message_handler_set_.clear();
			message_router_ = NULL;
		}

		//NotifyBrowserClosed(browser);
	}

	void CBrowserHandler::OnLoadingStateChange(
		CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward)
	{
		CEF_REQUIRE_UI_THREAD();

		NotifyLoadingState(isLoading, canGoBack, canGoForward);
	}

	void CBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame/*,TransitionType transition_type*/)
	{
		//CEF_REQUIRE_UI_THREAD();

		if (m_BrowserId == browser->GetIdentifier() && frame->IsMain())
		{
			// We've just started loading a page
			//SetLoading(true);
			//Invoke_LoadStart(browser, frame);
		}
	}

	void CBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode)
	{
		//CEF_REQUIRE_UI_THREAD();

		if (m_BrowserId == browser->GetIdentifier() && frame->IsMain())
		{
			//frame->ExecuteJavaScript("alert('ExecuteJavaScript works!');", frame->GetURL(), 0);
			//CefRefPtr<CefV8Context> v8 = frame->GetV8Context();
		}
	}

	void CBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
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


	bool CBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect)
	{
		CEF_REQUIRE_UI_THREAD();

		message_router_->OnBeforeBrowse(browser, frame);
		return false;
	}

	bool CBrowserHandler::OnOpenURLFromTab(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		CefRequestHandler::WindowOpenDisposition target_disposition,
		bool user_gesture) {
			if (target_disposition == WOD_NEW_BACKGROUND_TAB ||
				target_disposition == WOD_NEW_FOREGROUND_TAB) {
					// Handle middle-click and ctrl + left-click by opening the URL in a new
					// browser window.
					//MainContext::Get()->GetRootWindowManager()->CreateRootWindow(true, is_osr(), CefRect(), target_url);
					return true;
			}

			// Open the URL in the current browser window.
			return false;
	}

	bool CBrowserHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
		const CefString& origin_url,
		int64 new_size,
		CefRefPtr<CefRequestCallback> callback) {
			CEF_REQUIRE_IO_THREAD();

			static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

			// Grant the quota request if the size is reasonable.
			callback->Continue(new_size <= max_size);
			return true;
	}

	void CBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
		const CefString& url,
		bool& allow_os_execution) {
			CEF_REQUIRE_UI_THREAD();

			std::wstring urlStr = url;

			// Allow OS execution of Spotify URIs.
			if (urlStr.find(L"spotify:") == 0)
				allow_os_execution = true;
	}

	void CBrowserHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
		TerminationStatus status)
	{
		CEF_REQUIRE_UI_THREAD();

		message_router_->OnRenderProcessTerminated(browser);

		// Don't reload if there's no start URL, or if the crash URL was specified.
		if (startup_url_.empty() || startup_url_ == L"chrome://crash")
			return;

		CefRefPtr<CefFrame> frame = browser->GetMainFrame();
		std::wstring url = frame->GetURL();

		// Don't reload if the termination occurred before any URL had successfully
		// loaded.
		if (url.empty())
			return;

		std::wstring start_url = startup_url_;

		// Convert URLs to lowercase for easier comparison.
		std::transform(url.begin(), url.end(), url.begin(), tolower);
		std::transform(start_url.begin(), start_url.end(), start_url.begin(),
			tolower);

		// Don't reload the URL that just resulted in termination.
		if (url.find(start_url) == 0)
			return;

		frame->LoadURL(startup_url_);
	}


	int CBrowserHandler::GetBrowserCount() const {
		CEF_REQUIRE_UI_THREAD();
		return browser_count_;
	}


	bool CBrowserHandler::CreatePopupWindow(
		CefRefPtr<CefBrowser> browser,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings)
	{
		//MainContext::Get()->GetRootWindowManager()->CreateRootWindowAsPopup(!is_devtools, popupFeatures, windowInfo, client, settings);
		return true;
	}

	void CBrowserHandler::NotifyAddress(const CefString& url) {
		//if (!CURRENTLY_ON_MAIN_THREAD()) {
		//	// Execute this method on the main thread.
		//	MAIN_POST_CLOSURE(
		//		base::Bind(&CBrowserHandler::NotifyAddress, this, url));
		//	return;
		//}

		if (delegate_)
			delegate_->OnSetAddress(url);
	}

	void CBrowserHandler::NotifyTitle(const CefString& title) {
		//if (!CURRENTLY_ON_MAIN_THREAD()) {
		//	// Execute this method on the main thread.
		//	MAIN_POST_CLOSURE(
		//		base::Bind(&CBrowserHandler::NotifyTitle, this, title));
		//	return;
		//}

		if (delegate_)
			delegate_->OnSetTitle(title);
	}

	void CBrowserHandler::NotifyFullscreen(bool fullscreen) {
		//if (!CURRENTLY_ON_MAIN_THREAD()) {
		//	// Execute this method on the main thread.
		//	MAIN_POST_CLOSURE(
		//		base::Bind(&CBrowserHandler::NotifyFullscreen, this, fullscreen));
		//	return;
		//}

		//if (delegate_)
		//	delegate_->OnSetFullscreen(fullscreen);
	}

	void CBrowserHandler::NotifyLoadingState(bool isLoading,bool canGoBack,bool canGoForward) {
			//if (!CURRENTLY_ON_MAIN_THREAD()) {
			//	// Execute this method on the main thread.
			//	MAIN_POST_CLOSURE(
			//		base::Bind(&CBrowserHandler::NotifyLoadingState, this,
			//		isLoading, canGoBack, canGoForward));
			//	return;
			//}

			//if (delegate_)
			//	delegate_->OnSetLoadingState(isLoading, canGoBack, canGoForward);
	}
}
