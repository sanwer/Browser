#ifndef __BROWSERUI_H__
#define __BROWSERUI_H__
#pragma once
//#include <algorithm>
//#include <sstream>
//#include <string>
#include <list>
#include <set>
#undef GetFirstChild
#undef GetNextSibling
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/cef_v8.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_message_router.h"

class CBrowserHandler
	: public CefClient				// 浏览器客户区
	, public CefContextMenuHandler	// 浏览器上下文菜单处理
	, public CefDisplayHandler		// 浏览器显示处理
	, public CefDownloadHandler		// 浏览器下载处理
	, public CefGeolocationHandler	// 浏览器地理定位处理
	, public CefJSDialogHandler		// 浏览器JS对话框处理
	, public CefKeyboardHandler		// 浏览器键盘处理
	, public CefLifeSpanHandler		// 浏览器生命周期处理
	, public CefLoadHandler			// 浏览器加载处理
	, public CefRequestHandler		// 浏览器请求处理
{

public:
	// Interface for process message delegates. Do not perform work in the
	// RenderDelegate constructor.
	class ProcessMessageDelegate : public virtual CefBase
	{
	public:
		// Called when a process message is received. Return true if the message was
		// handled and should not be passed on to other handlers.
		// ProcessMessageDelegates should check for unique message names to avoid
		// interfering with each other.
		virtual bool OnProcessMessageReceived(
			CefRefPtr<CBrowserHandler> handler,
			CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message)
		{
			return false;
		}
	};
	typedef std::set<CefRefPtr<ProcessMessageDelegate> > ProcessMessageDelegateSet;

	// Interface for request handler delegates. Do not perform work in the
	// RequestDelegate constructor.
	class RequestDelegate : public virtual CefBase
	{
	public:
		// Called to retrieve a resource handler.
		virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
			CefRefPtr<CBrowserHandler> handler,
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefRequest> request)
		{
			return NULL;
		}
	};
	typedef std::set<CefRefPtr<RequestDelegate> > RequestDelegateSet;

	typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

public:
	CBrowserHandler(CBrowserDlg* pWindow=NULL);
	~CBrowserHandler();

	//////////////////////////////////////////////////////////////////////////
	// CefClient methods
	virtual CefRefPtr<CefContextMenuHandler>	GetContextMenuHandler()	OVERRIDE { return this; }
	virtual CefRefPtr<CefDisplayHandler>		GetDisplayHandler()		OVERRIDE { return this; }
	virtual CefRefPtr<CefDownloadHandler>		GetDownloadHandler()	OVERRIDE { return this; }
	//	virtual CefRefPtr<CefFocusHandler>			GetFocusHandler()		OVERRIDE { return this; }
	//	virtual CefRefPtr<CefGeolocationHandler>	GetGeolocationHandler()	OVERRIDE { return this; }
	virtual CefRefPtr<CefJSDialogHandler>		GetJSDialogHandler()	OVERRIDE { return this; }
	//	virtual CefRefPtr<CefKeyboardHandler>		GetKeyboardHandler()	OVERRIDE { return this; }
	virtual CefRefPtr<CefLifeSpanHandler>		GetLifeSpanHandler()	OVERRIDE { return this; }
	virtual CefRefPtr<CefLoadHandler>			GetLoadHandler()		OVERRIDE { return this; }
	virtual CefRefPtr<CefRequestHandler>		GetRequestHandler()		OVERRIDE { return this; }

	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message)
		OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefContextMenuHandler methods
	virtual void OnBeforeContextMenu(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE;

	virtual bool OnContextMenuCommand(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		int command_id,
		EventFlags event_flags) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefDisplayHandler methods
	virtual void OnLoadingStateChange(
		CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward) OVERRIDE;

	virtual void OnAddressChange(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url) OVERRIDE;

	virtual void OnTitleChange(
		CefRefPtr<CefBrowser> browser,
		const CefString& title) OVERRIDE;

	virtual bool OnTooltip(
		CefRefPtr<CefBrowser> browser,
		CefString& text) OVERRIDE;

	virtual void OnStatusMessage(
		CefRefPtr<CefBrowser> browser,
		const CefString& value) OVERRIDE;

	virtual bool OnConsoleMessage(
		CefRefPtr<CefBrowser> browser,
		const CefString& message,
		const CefString& source,
		int line) OVERRIDE;


	//////////////////////////////////////////////////////////////////////////
	// CefDownloadHandler methods
	virtual void OnBeforeDownload(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		const CefString& suggested_name,
		CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;

	virtual void OnDownloadUpdated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefDownloadItem> download_item,
		CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefGeolocationHandler methods
	virtual bool OnRequestGeolocationPermission(
		CefRefPtr<CefBrowser> browser,
		const CefString& requesting_url,
		int request_id,
		CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefKeyboardHandler methods
	virtual bool OnPreKeyEvent(
		CefRefPtr<CefBrowser> browser,
		const CefKeyEvent& event,
		CefEventHandle os_event,
		bool* is_keyboard_shortcut) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefLifeSpanHandler methods
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;

	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefLoadHandler methods
	virtual void OnLoadStart(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame) OVERRIDE;

	virtual void OnLoadEnd(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode) OVERRIDE;

	virtual void OnLoadError(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;

	virtual void OnRenderProcessTerminated(
		CefRefPtr<CefBrowser> browser,
		TerminationStatus status) OVERRIDE;

	//////////////////////////////////////////////////////////////////////////
	// CefRequestHandler methods
	virtual ReturnValue OnBeforeResourceLoad(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefRequestCallback> callback) OVERRIDE;

	virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) OVERRIDE;

	virtual CefRefPtr<CefCookieManager> GetCookieManager() { return NULL; }

	virtual void OnProtocolExecution(
		CefRefPtr<CefBrowser> browser,
		const CefString& url,
		bool& allow_os_execution) OVERRIDE;

public:
	CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }
	CefRefPtr<CefFrame> GetMainFrame() { return m_Browser.get()?m_Browser->GetMainFrame():NULL; }
	HWND GetBrowserHandle() { return m_Browser.get()?m_Browser->GetHost()->GetWindowHandle():NULL; }
	HWND GetParentHandle() const { return m_hParentHandle; }
	void SetParentHandle(CefWindowHandle hParent) { m_hParentHandle = hParent; }
	CefString GetHomepage() const { return m_sHomepage; }
	void SetHomepage(const CefString& sHomepage) { m_sHomepage = sHomepage; }
	int GetBrowserId() { return m_BrowserId; }

	// Request that all existing browser windows close.
	void CloseAllBrowsers(bool force_close);
	void CreateBrowser(HWND hParentWnd, const RECT& rect);

protected:
	// Create all of ProcessMessageDelegate objects.
	static void CreateProcessMessageDelegates(ProcessMessageDelegateSet& delegates);

	// Create all of RequestDelegateSet objects.
	static void CreateRequestDelegates(RequestDelegateSet& delegates);

	// The child browser window
	CefRefPtr<CefBrowser> m_Browser;
	CBrowserDlg* m_pWindow;

	// The child browser id
	int m_BrowserId;

	HWND m_hParentHandle;
	CefString m_sHomepage;

	// Registered delegates.
	ProcessMessageDelegateSet m_process_message_delegates;
	RequestDelegateSet m_request_delegates;

  // The current number of browsers using this handler.
  int browser_count_;

  // Set of Handlers registered with the message router.
  MessageHandlerSet message_handler_set_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CBrowserHandler);
	// Include the default locking implementation.
	IMPLEMENT_LOCKING(CBrowserHandler);
};

class CBrowserV8Handler : public CefV8Handler
{
public:
	CBrowserV8Handler() : sParam("An initial string value.") {}
	virtual ~CBrowserV8Handler() {}

	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);
private:
	CefString sParam;
	IMPLEMENT_REFCOUNTING(CBrowserV8Handler);
};

class CBrowserApp : public CefApp, public CefBrowserProcessHandler
{
public:
	CBrowserApp(){}
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
	virtual void OnContextInitialized() OVERRIDE{}

private:
	void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) OVERRIDE;
	IMPLEMENT_REFCOUNTING(CBrowserApp);
};

class CRendererApp : public CefApp, public CefRenderProcessHandler 
{
public:
	CRendererApp(){}
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {return this;}
	void OnWebKitInitialized() OVERRIDE;
	IMPLEMENT_REFCOUNTING(CRendererApp);
};

class CBrowserUI : public CControlUI, public IMessageFilterUI
{
public:
	CBrowserUI();
	~CBrowserUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void SetPos(RECT rc, bool bNeedInvalidate = true);
	bool CreateBrowser(const CefString& sHomepage, CBrowserDlg* pWindow);
	void LoadURL(const CefString& url);
	
	CefRefPtr<CefBrowser> GetBrowser() { return m_Browser->GetBrowser(); }
	CefRefPtr<CefFrame> GetMainFrame() { return m_Browser->GetMainFrame(); }

	void CloseAllBrowsers();
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
	CefRefPtr<CBrowserHandler> m_Browser;
};

#endif
