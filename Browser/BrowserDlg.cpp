#include "stdafx.h"
#include "BrowserDlg.h"
#include "MainContext.h"
#include "BrowserDlgManager.h"

namespace Browser
{
	namespace {
		const int FrameLeft = 2;
		const int FrameTop = 1;
		const int FrameRight = 2;
		const int FrameBottom = 2;
		const int TitleHeight = 32;
		const int ToolbarHeight = 28;
	}
	// static
	scoped_refptr<BrowserDlg> BrowserDlg::GetForBrowser(int browser_id)
	{
		return MainContext::Get()->GetBrowserDlgManager()->GetWindowForBrowser(browser_id);
	}

	BrowserDlg::BrowserDlg()
		: m_Delegate(NULL),
		m_hParent(NULL),
		m_pBrowserUI(NULL),
		m_bWithControls(false),
		m_bIsPopup(false),
		m_rcStart(),
		m_bInitialized(false),
		m_bWindowDestroyed(false),
		m_bBrowserDestroyed(false),
		m_nCurBrowserId(-1)
	{
		uiTabs = NULL;
		tabNew = NULL;
		uiToolbar = NULL;
		btnBackward = NULL;
		btnForward = NULL;
		editUrl = NULL;
		editKeyword = NULL;
	}

	BrowserDlg::~BrowserDlg()
	{
		DCHECK(m_bWindowDestroyed);
		DCHECK(m_bBrowserDestroyed);
		//PostQuitMessage(0);
	}

	LPCTSTR BrowserDlg::GetWindowClassName() const
	{
		return _T("BrowserDlg");
	}

	void BrowserDlg::InitWindow()
	{
		SetIcon(IDR_MAINFRAME);
		uiTabs = static_cast<DuiLib::CHorizontalLayoutUI*>(m_Manager.FindControl(_T("uiTabs")));
		tabNew = static_cast<DuiLib::CButtonUI*>(m_Manager.FindControl(_T("tabNew")));
		uiToolbar = static_cast<DuiLib::CControlUI*>(m_Manager.FindControl(_T("uiToolbar")));
		btnBackward = static_cast<DuiLib::CButtonUI*>(m_Manager.FindControl(_T("btnBackward")));
		btnForward = static_cast<DuiLib::CButtonUI*>(m_Manager.FindControl(_T("btnForward")));
		editUrl = static_cast<DuiLib::CEditUI*>(m_Manager.FindControl(_T("editUrl")));
		editKeyword = static_cast<DuiLib::CEditUI*>(m_Manager.FindControl(_T("editKeyword")));
		if (uiTabs == NULL || tabNew == NULL || uiToolbar == NULL || editUrl == NULL || editKeyword == NULL)
		{
			MessageBox(NULL,_T("加载资源文件失败"),_T("Browser"),MB_OK|MB_ICONERROR);
			return;
		}
		
		if (m_bWithControls) {
			tabNew->SetVisible(true);
			uiToolbar->SetVisible(true);
		}else{
			uiToolbar->SetVisible(false);
		}

		if(btnBackward)
			btnBackward->SetEnabled(false);
		if(btnForward)
			btnForward->SetEnabled(false);
	}

	void BrowserDlg::OnFinalMessage(HWND hWnd)
	{
		WindowImplBase::OnFinalMessage(hWnd);
		//delete this;
		m_bWindowDestroyed = true;
		NotifyDestroyedIfDone();
	}

	DuiLib::CDuiString BrowserDlg::GetSkinFile()
	{
		return _T("BrowserDlg.xml");
	}

	LRESULT BrowserDlg::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if (wParam == VK_RETURN)
		{
			return FALSE;
		}
		else if (wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		return FALSE;
	}

	DuiLib::CControlUI* BrowserDlg::CreateControl(LPCTSTR pstrClass)
	{
		DuiLib::CControlUI* pUI = NULL;
		if (_tcsicmp(pstrClass, _T("BrowserUI")) == 0)
		{
			if(m_pBrowserUI == NULL){
				m_pBrowserUI = new Browser::BrowserUI(this, m_hWnd);
				if (m_BrowserCtrl.get()){
					m_pBrowserUI->SetCtrl(m_BrowserCtrl.get());
				}
			}
			pUI = m_pBrowserUI;
		}
		else if (_tcsicmp(pstrClass, _T("Title")) == 0)
		{
			pUI = new Browser::TitleUI();
		}

		return pUI;
	}

	LRESULT BrowserDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
		case WM_CLOSE:
			if(wParam == 0L)
				return 0L;
			break;
		default:
			break;
		}
		return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
	}

	void BrowserDlg::Notify(DuiLib::TNotifyUI& msg)
	{
		DuiLib::CDuiString sCtrlName = msg.pSender->GetName();

		if (_tcsicmp(msg.sType,_T("click")) == 0)
		{
			if (_tcsicmp(sCtrlName,_T("btnGoto")) == 0){//跳转
				DuiLib::CDuiString sUrl = editUrl->GetText();
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}else if (_tcsicmp(sCtrlName,_T("btnSearch")) == 0){//搜索
				DuiLib::CDuiString sUrl,sKeyword = editKeyword->GetText();
				sUrl.Format(_T("https://www.baidu.com/s?wd=%s"),sKeyword.GetData());
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}else if (_tcsicmp(sCtrlName,_T("btnHome")) == 0){//主页
				editUrl->SetText(MainContext::Get()->GetMainURL().c_str());
				LoadURL(MainContext::Get()->GetMainURL().c_str());
			}else if (_tcsicmp(sCtrlName,_T("btnSettings")) == 0){
				DuiLib::CDuiString sUrl = _T("about:settings");
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}else if (_tcsicmp(sCtrlName,_T("btnBackward")) == 0){
				CefRefPtr<CefBrowser> browser = GetBrowser();
				if (browser) {
					browser->GoBack();
				}
			}else if (_tcsicmp(sCtrlName,_T("btnForward")) == 0){
				CefRefPtr<CefBrowser> browser = GetBrowser();
				if (browser) {
					browser->GoForward();
				}
			}else if (_tcsicmp(sCtrlName,_T("tabNew")) == 0){
				NewTab(MainContext::Get()->GetMainURL());//about:blank
			}else if (_tcsnicmp(sCtrlName,_T("tabClose"),8) == 0){
				CDuiString sBuffer = sCtrlName;
				sBuffer.Replace(_T("tabClose"),_T(""));
				int nBrowserId = _ttoi(sBuffer.GetData());
				sBuffer.Format(_T("tabTitle%d"),nBrowserId);
				int nTabsCount = uiTabs->GetCount();
				for (int idx = 0; idx < nTabsCount; idx++)
				{
					TitleUI* pItem = (TitleUI*)uiTabs->GetItemAt(idx);
					if (pItem != NULL){
						if(nTabsCount <= 2){
							LoadURL(L"about:blank");
						}else{
							if(pItem->GetTag() == nBrowserId){
								TitleUI* pTitle;
								if(idx > 0){
									pTitle = (TitleUI*)uiTabs->GetItemAt(idx-1);
								}else{
									pTitle = (TitleUI*)uiTabs->GetItemAt(idx+1);
								}
								uiTabs->Remove(pItem);
								pTitle->Selected(true);
								if (m_pBrowserUI){
									m_pBrowserUI->CloseBrowser(nBrowserId);
								}
							}
						}
					}
				}
			}else if (_tcsnicmp(sCtrlName,_T("closebtn"),8) == 0){
				MainContext::Get()->GetBrowserDlgManager()->CloseAllWindows(true);
			}
		}
		else if (_tcsicmp(msg.sType,DUI_MSGTYPE_RETURN) == 0)
		{
			if (_tcsicmp(sCtrlName,_T("editUrl")) == 0){
				DuiLib::CDuiString sUrl = editUrl->GetText();
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}else if (_tcsicmp(sCtrlName,_T("editKeyword")) == 0){
				DuiLib::CDuiString sUrl,sKeyword = editKeyword->GetText();
				sUrl.Format(_T("https://www.baidu.com/s?wd=%s"),sKeyword.GetData());
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}
		}
		else if (_tcsicmp(msg.sType,DUI_MSGTYPE_SELECTCHANGED) == 0)
		{
			if (_tcsnicmp(sCtrlName,_T("tabTitle"),8) == 0){
				CDuiString sBuffer = msg.pSender->GetText();
				SetWindowText(m_hWnd, sBuffer.GetData());
				sBuffer = msg.pSender->GetUserData();
				editUrl->SetText(sBuffer.GetData());
				if (m_pBrowserUI){
					m_nCurBrowserId = msg.pSender->GetTag();
					m_pBrowserUI->ShowBrowser(m_nCurBrowserId);
					m_pBrowserUI->Invalidate();
				}
			}
		}
		return WindowImplBase::Notify(msg);
	}

	void BrowserDlg::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();

		if (m_bIsPopup) {
			// For popup browsers create the root window once the browser has been created.
			CreateBrowserDlg(CefBrowserSettings());
		} else {
			if (m_pBrowserUI){
				RECT rcPos = m_pBrowserUI->GetPos();
				if(m_BrowserCtrl){
					m_BrowserCtrl->SetBounds(browser->GetIdentifier(), rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
				}
			}
		}
	}

	void BrowserDlg::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();
	}

	void BrowserDlg::OnBrowserAllClosed()
	{
		REQUIRE_MAIN_THREAD();

		m_BrowserCtrl.reset();

		if (!m_bWindowDestroyed) {
			// The browser was destroyed first. This could be due to the use of
			// off-screen rendering or execution of JavaScript window.close().
			// Close the RootWindow.
			Close(true);
		}

		m_bBrowserDestroyed = true;
		NotifyDestroyedIfDone();
	}

	void BrowserDlg::OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url)
	{
		CDuiString sBuffer;
		bool bAddTab = true;
		int nTabsCount = uiTabs->GetCount();
		int nBrowserId = browser->GetIdentifier();
		for (int idx = 0; idx < nTabsCount; idx++)
		{
			DuiLib::CControlUI* pTitle = (DuiLib::CControlUI*)uiTabs->GetItemAt(idx);
			if (pTitle == NULL || _tcsicmp(pTitle->GetClass(), _T("ButtonUI")) == 0)
				continue;
			if (pTitle->GetTag() == nBrowserId){
				bAddTab = false;
				pTitle->SetUserData(url.c_str());
				CDuiString sBuffer = pTitle->GetText();
				if(sBuffer.GetLength() == 0)
					pTitle->SetText(url.c_str());
				break;
			}
		}
		if (nBrowserId == m_nCurBrowserId) {
			editUrl->SetText(url.c_str());
		}
		if(bAddTab){
			if(m_bIsPopup){
				CLabelUI* pTitle = new CLabelUI;
				pTitle->SetTag(nBrowserId);
				uiTabs->AddAt(pTitle, nTabsCount - 1);
				sBuffer.Format(_T("name=\"labTitle\" height=\"30\" floatalign=\"right\" textpadding=\"5,1,20,2\" textcolor=\"FFFFFFFF\""));
				pTitle->ApplyAttributeList(sBuffer);
			}else{
				if(wcsnicmp(url.c_str(),L"chrome-devtools:",16)!=0){
				TitleUI* pTitle = new TitleUI;
				pTitle->SetTag(nBrowserId);
				pTitle->SetUserData(url.c_str());
				uiTabs->AddAt(pTitle, nTabsCount - 1);
					sBuffer.Format(_T("name=\"tabTitle%d\" height=\"30\" minwidth=\"100\" maxwidth=\"256\" floatalign=\"right\" borderround=\"2,2\" textpadding=\"5,1,20,2\" bkcolor=\"FF1587D8\" selectedbkcolor=\"FF3498DB\" textcolor=\"FFFFFFFF\" selectedtextcolor=\"FFFFFFFF\" group=\"Titles\""), nBrowserId);
					pTitle->ApplyAttributeList(sBuffer);
					CControlUI* pControl = new CControlUI;
					CButtonUI* pClose = new CButtonUI;
					pTitle->Add(pControl);
					pTitle->Add(pClose);
					sBuffer.Format(_T("name=\"tabClose%d\" width=\"26\" height=\"28\" floatalign=\"right\" normalimage=\"file='btnTabClose.png' source='0,0,11,11' dest='8,8,19,19'\" hotimage=\"file='btnTabClose.png' source='11,0,22,11' dest='8,8,19,19'\" pushedimage=\"file='btnTabClose.png' source='22,0,33,11' dest='8,8,19,19'\""), nBrowserId);
					pClose->ApplyAttributeList(sBuffer);
					pTitle->Selected(true);
					editUrl->SetText(url.c_str());
				}
			}
		}
	}

	void BrowserDlg::OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		int nTabsCount = uiTabs->GetCount();
		int nBrowserId = browser->GetIdentifier();
		for (int idx = 0; idx < nTabsCount; idx++)
		{
			DuiLib::CControlUI* pTitle = (DuiLib::CControlUI*)uiTabs->GetItemAt(idx);
			if (pTitle == NULL || _tcsicmp(pTitle->GetClass(), _T("ButtonUI")) == 0)
				continue;
			if (pTitle->GetTag() == nBrowserId){
				pTitle->SetText(title.c_str());
			}
		}
		if (m_bIsPopup || nBrowserId == m_nCurBrowserId) {
			SetWindowText(m_hWnd, title.c_str());
		}
	}

	void BrowserDlg::OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
	{
	}

	void BrowserDlg::OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading,bool canGoBack,bool canGoForward)
	{
		if(btnBackward)
			btnBackward->SetEnabled(canGoBack);
		if(btnForward)
			btnForward->SetEnabled(canGoForward);
	}

	void BrowserDlg::OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions)
	{
		REQUIRE_MAIN_THREAD();
	}

	void BrowserDlg::OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) {
		REQUIRE_MAIN_THREAD();
		NewTab(url);
	}

	void BrowserDlg::NotifyDestroyedIfDone() {
		// Notify once both the window and the browser have been destroyed.
		if (m_bWindowDestroyed && m_bBrowserDestroyed)
			m_Delegate->OnRootWindowDestroyed(this);
	}

	void BrowserDlg::Init(
		BrowserDlg::Delegate* delegate,
		HWND hParent,
		bool with_controls,
		const CefRect& bounds,
		const CefBrowserSettings& settings,
		const CefString& url)
	{
		DCHECK(delegate);
		DCHECK(!m_bInitialized);

		m_Delegate = delegate;
		m_bWithControls = true;

		m_rcStart.left = bounds.x;
		m_rcStart.top = bounds.y;
		m_rcStart.right = bounds.x + bounds.width;
		m_rcStart.bottom = bounds.y + bounds.height;

		CreateBrowserWindow(url);

		m_bInitialized = true;

		// Create the native root window on the main thread.
		if (CURRENTLY_ON_MAIN_THREAD()) {
			CreateBrowserDlg(settings);
		} else {
			MAIN_POST_CLOSURE(base::Bind(&BrowserDlg::CreateBrowserDlg, this, settings));
		}
	}

	void BrowserDlg::InitAsPopup(
		BrowserDlg::Delegate* delegate,
		bool with_controls,
		const CefString& target_url,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings)
	{
		DCHECK(delegate);
		DCHECK(!m_bInitialized);

		m_Delegate = delegate;
		m_bWithControls = with_controls;
		m_bIsPopup = true;

		if (popupFeatures.xSet)      m_rcStart.left = popupFeatures.x;
		if (popupFeatures.ySet)      m_rcStart.top = popupFeatures.y;
		if (popupFeatures.widthSet)  m_rcStart.right = m_rcStart.left + popupFeatures.width;
		if (popupFeatures.heightSet) m_rcStart.bottom = m_rcStart.top + popupFeatures.height;
		
		CreateBrowserWindow(std::string());

		m_bInitialized = true;

		// The new popup is initially parented to a temporary window. The native root
		// window will be created after the browser is created and the popup window
		// will be re-parented to it at that time.
		m_BrowserCtrl->GetPopupConfig(TempWindow::GetWindowHandle(),windowInfo, client, settings);
	}

	void BrowserDlg::NewTab(const std::wstring& url)
	{
		if(m_BrowserCtrl && m_pBrowserUI){
			RECT rcPos = m_pBrowserUI->GetPos();
			CefRect cef_rect(rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
			CefBrowserSettings settings;
			MainContext::Get()->PopulateBrowserSettings(&settings);
			m_BrowserCtrl->CreateBrowser(m_hWnd, url, cef_rect, settings, m_Delegate->GetRequestContext());
		}
	}

	CefRefPtr<CefBrowser> BrowserDlg::GetBrowser()
	{
		REQUIRE_MAIN_THREAD();

		if (m_BrowserCtrl)
			return m_BrowserCtrl->GetBrowser(m_nCurBrowserId);
		return NULL;
	}

	CefWindowHandle BrowserDlg::GetWindowHandle()
	{
		return m_hWnd;
	}

	void BrowserDlg::LoadURL(const CefString& url)
	{
		if (m_pBrowserUI){
			CefRefPtr<CefBrowser> pBrowser = m_pBrowserUI->GetBrowser();
			if(pBrowser){
				CefRefPtr<CefFrame> pFrame = pBrowser->GetMainFrame();
				if (pFrame){
					pFrame->LoadURL(url);
				}
			}
		}
	}

	void BrowserDlg::CreateBrowserWindow(const std::string& startup_url)
	{
		m_BrowserCtrl.reset(new BrowserWindow(this, startup_url));
	}

	void BrowserDlg::CreateBrowserDlg(const CefBrowserSettings& settings)
	{
		REQUIRE_MAIN_THREAD();

		HINSTANCE hInstance = GetModuleHandle(NULL);

		int x, y, width, height;
		if (::IsRectEmpty(&m_rcStart)) {
			// Use the default window position/size.
			x = y = width = height = CW_USEDEFAULT;
		} else {
			RECT rcWindow = m_rcStart;
			rcWindow.bottom += FrameTop + TitleHeight + FrameBottom;
			rcWindow.right += FrameLeft + FrameRight;
			if (m_bWithControls){
				rcWindow.bottom += ToolbarHeight;
			}
			x = rcWindow.left;
			y = rcWindow.top;
			width = rcWindow.right - rcWindow.left;
			height = rcWindow.bottom - rcWindow.top;
		}

		// Create the main window initially hidden.
		Create(m_hParent,_T("Browser"),UI_WNDSTYLE_FRAME,WS_EX_APPWINDOW,x, y, width, height,NULL);

		RECT rect;
		GetClientRect(m_hWnd, &rect);

		if (m_bWithControls) {
			// Create the child controls.
		} else {
			// No controls so also remove the default menu.
			::SetMenu(m_hWnd, NULL);
		}

		if(m_bIsPopup){
			// With popups we already have a browser window. Parent the browser window
			// to the root window and show it in the correct location.
			m_BrowserCtrl->ShowPopup(m_hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		}else{
			// Create the browser window.
			CefRect cef_rect(rect.left, rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top);
			m_BrowserCtrl->CreateBrowser(m_hWnd, std::wstring(), cef_rect, settings, m_Delegate->GetRequestContext());
		}
		//if (m_pBrowserUI){
		//	m_pBrowserUI->ShowPage(browser->GetIdentifier());
		//}
	}
}
