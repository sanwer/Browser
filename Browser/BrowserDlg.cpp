#include "stdafx.h"
#include "BrowserDlg.h"
#include "BrowserManager.h"

namespace Browser
{
	BrowserDlg::BrowserDlg()
		: m_pBrowser(NULL),
		m_Delegate(NULL),
		m_bWithControls(false),
		m_rcBrowser(),
		m_bWithOsr(false),
		m_bIsPopup(false),
		m_bInitialized(false),
		m_bWindowDestroyed(false),
		m_bBrowserDestroyed(false),
		m_nBrowserSelectedId(-1)
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
			if(m_pBrowser == NULL){
				m_pBrowser = new Browser::BrowserUI(this, m_hWnd);
				if (m_BrowserCtrl.get()){
					m_pBrowser->SetCtrl(m_BrowserCtrl.get());
				}
			}
			pUI = m_pBrowser;
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

		if (_tcsicmp(msg.sType,DUI_MSGTYPE_WINDOWINIT) == 0)
		{
			if(m_pBrowser && !m_bIsPopup){
				m_pBrowser->NewPage(BrowserManager::Get()->GetHomepage());
			}
		}
		else if (_tcsicmp(msg.sType,_T("click")) == 0)
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
				editUrl->SetText(BrowserManager::Get()->GetHomepage().c_str());
				LoadURL(BrowserManager::Get()->GetHomepage().c_str());
			}else if (_tcsicmp(sCtrlName,_T("btnSettings")) == 0){
				DuiLib::CDuiString sUrl = _T("about:settings");
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}else if (_tcsicmp(sCtrlName,_T("btnBackward")) == 0){
				if(m_pBrowser){
					m_pBrowser->GetBrowser()->GoBack();
				}
			}else if (_tcsicmp(sCtrlName,_T("btnForward")) == 0){
				if(m_pBrowser){
					m_pBrowser->GetBrowser()->GoForward();
				}
			}else if (_tcsicmp(sCtrlName,_T("tabNew")) == 0){
				if(m_pBrowser){
					m_pBrowser->NewPage(L"about:blank");
				}
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
								m_pBrowser->DelPage(nBrowserId);
							}
						}
					}
				}
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
				if (m_pBrowser){
					m_nBrowserSelectedId = msg.pSender->GetTag();
					m_pBrowser->ShowPage(m_nBrowserSelectedId);
				}
			}
		}
		return WindowImplBase::Notify(msg);
	}

	void BrowserDlg::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();

		CDuiString sBuffer;
		if (m_bIsPopup) {
			CreateBrowserWindow(browser,CefBrowserSettings());
		} 

		if(m_bIsPopup){
			CLabelUI* pTitle = new CLabelUI;
			int nBrowserId = browser->GetIdentifier();
			int iCount = uiTabs->GetCount();
			uiTabs->AddAt(pTitle, iCount - 1);
			sBuffer.Format(_T("name=\"labTitle\" height=\"30\" floatalign=\"right\" textpadding=\"5,1,20,2\" textcolor=\"FFFFFFFF\""));
			pTitle->ApplyAttributeList(sBuffer);
			pTitle->SetTag(nBrowserId);
		}else{
			TitleUI* pTitle = new TitleUI;
			int nBrowserId = browser->GetIdentifier();
			int iCount = uiTabs->GetCount();
			uiTabs->AddAt(pTitle, iCount - 1);
			sBuffer.Format(_T("name=\"tabTitle%d\" height=\"30\" minwidth=\"100\" maxwidth=\"256\" floatalign=\"right\" borderround=\"2,2\" textpadding=\"5,1,20,2\" bkcolor=\"FF1587D8\" selectedbkcolor=\"FF3498DB\" textcolor=\"FFFFFFFF\" selectedtextcolor=\"FFFFFFFF\" group=\"Titles\""), nBrowserId);
			pTitle->ApplyAttributeList(sBuffer);
			CControlUI* pControl = new CControlUI;
			CButtonUI* pClose = new CButtonUI;
			pTitle->Add(pControl);
			pTitle->Add(pClose);
			sBuffer.Format(_T("name=\"tabClose%d\" width=\"26\" height=\"28\" floatalign=\"right\" normalimage=\"file='btnTabClose.png' source='0,0,11,11' dest='8,8,19,19'\" hotimage=\"file='btnTabClose.png' source='11,0,22,11' dest='8,8,19,19'\" pushedimage=\"file='btnTabClose.png' source='22,0,33,11' dest='8,8,19,19'\""), nBrowserId);
			pClose->ApplyAttributeList(sBuffer);
			pTitle->SetTag(nBrowserId);
			pTitle->Selected(true);
		}
	}

	void BrowserDlg::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();
	}

	void BrowserDlg::OnBrowserExit(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();

		m_BrowserCtrl.reset();

		if (!m_bWindowDestroyed) {
			Close();
		}

		m_bBrowserDestroyed = true;
		NotifyDestroyedIfDone();
	}

	void BrowserDlg::OnSetAddress(CefRefPtr<CefBrowser> browser, const std::wstring& url)
	{
		editUrl->SetText(url.c_str());
	}

	void BrowserDlg::OnSetTitle(CefRefPtr<CefBrowser> browser, const std::wstring& title)
	{
		int nTabsCount = uiTabs->GetCount();
		UINT_PTR nTag = browser->GetIdentifier();
		for (int idx = 0; idx < nTabsCount; idx++)
		{
			DuiLib::CControlUI* pTitle = (DuiLib::CControlUI*)uiTabs->GetItemAt(idx);
			if (pTitle != NULL && pTitle->GetTag() == nTag){
				pTitle->SetText(title.c_str());
			}
		}
		if(nTag == m_nBrowserSelectedId){
			SetWindowText(m_hWnd, title.c_str());
		}
		if (m_bIsPopup) {
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

	void BrowserDlg::OnNewPage(const std::wstring& url) {
		REQUIRE_MAIN_THREAD();
		if(m_pBrowser){
			m_pBrowser->NewPage(url);
		}
	}

	void BrowserDlg::NotifyDestroyedIfDone() {
		// Notify once both the window and the browser have been destroyed.
		if (m_bWindowDestroyed && m_bBrowserDestroyed)
			m_Delegate->OnRootWindowDestroyed(this);
	}

	void BrowserDlg::Init(BrowserDlg::Delegate* delegate,const std::wstring& url)
	{
		DCHECK(delegate);
		DCHECK(!m_bInitialized);

		m_Delegate = delegate;
		m_bWithControls = true;
		m_bWithOsr = false;

		m_BrowserCtrl.reset(new BrowserCtrl(this));

		m_bInitialized = true;

		Create(NULL,_T("Browser"),UI_WNDSTYLE_FRAME,WS_EX_APPWINDOW,0,0,0,0,NULL);
		CenterWindow();

		tabNew->SetVisible(true);
		uiToolbar->SetVisible(true);
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

		if (popupFeatures.xSet)      m_rcBrowser.left = popupFeatures.x;
		if (popupFeatures.ySet)      m_rcBrowser.top = popupFeatures.y;
		if (popupFeatures.widthSet)  m_rcBrowser.right = m_rcBrowser.left + popupFeatures.width;
		if (popupFeatures.heightSet) m_rcBrowser.bottom = m_rcBrowser.top + popupFeatures.height;

		m_BrowserCtrl.reset(new BrowserCtrl(this));

		m_bInitialized = true;

		// The new popup is initially parented to a temporary window. The native root
		// window will be created after the browser is created and the popup window
		// will be re-parented to it at that time.
		m_BrowserCtrl->GetPopupConfig(TempWindow::GetWindowHandle(),windowInfo, client, settings);
	}

	CefRefPtr<CefBrowser> BrowserDlg::GetBrowser()
	{
		REQUIRE_MAIN_THREAD();

		if (m_pBrowser)
			return m_pBrowser->GetBrowser();
		return NULL;
	}

	void BrowserDlg::LoadURL(const CefString& url)
	{
		if (m_pBrowser){
			CefRefPtr<CefBrowser> pBrowser = m_pBrowser->GetBrowser();
			if(pBrowser){
				CefRefPtr<CefFrame> pFrame = pBrowser->GetMainFrame();
				if (pFrame){
					pFrame->LoadURL(url);
				}
			}
		}
	}

	void BrowserDlg::CreateBrowserWindow(CefRefPtr<CefBrowser> browser, const CefBrowserSettings& settings)
	{
		REQUIRE_MAIN_THREAD();

		int x, y, width, height;
		
		RECT rcWindow = m_rcBrowser;
		//if(m_bIsPopup)
			Create(NULL,_T("Browser"),UI_WNDSTYLE_FRAME,WS_EX_APPWINDOW,0,0,0,0,NULL);
		if (::IsRectEmpty(&rcWindow)) {
			CenterWindow();
		} else {
			rcWindow.bottom += 30 + 1;//BrowserHeight + bgInsetHeight
			rcWindow.right += 2;//BrowserWidth + bgInsetWidth
			if (m_bWithControls){
				rcWindow.bottom += 36;//BrowserHeight + TitleHeight + ToolbarHeight + bgInsetHeight
			}
			x = rcWindow.left;
			y = rcWindow.top;
			width = rcWindow.right - rcWindow.left;
			height = rcWindow.bottom - rcWindow.top;
			SetWindowPos(m_hWnd, NULL, x, y,width, height,SWP_NOZORDER);
		}

		RECT rcBrowser;
		GetClientRect(m_hWnd, &rcBrowser);

		if (m_bWithControls) {
			// Create the child controls.
			uiToolbar->SetVisible(true);
			rcBrowser.left += 1;//Top + bgInsetWidth
			rcBrowser.top += 30 + 36;//Top + TitleHeight + ToolbarHeight
			rcBrowser.right -= 1;//Top + bgInsetWidth
			rcBrowser.bottom -= 1;//Bottom - bgInsetHeight
		} else {
			// No controls so also remove the default menu.
			::SetMenu(m_hWnd, NULL);
			uiToolbar->SetVisible(false);
			rcBrowser.left += 1;//Top + bgInsetWidth
			rcBrowser.top += 30;//Top + TitleHeight
			rcBrowser.right -= 1;//Top + bgInsetWidth
			rcBrowser.bottom -= 1;//Bottom - bgInsetHeight
		}

		CefRefPtr<CefFrame> pFrame = browser->GetMainFrame();

		tabNew->SetVisible(false);
		uiToolbar->SetVisible(false);
		m_BrowserCtrl->ShowPopup(browser, m_hWnd, rcBrowser.left, rcBrowser.top, rcBrowser.right - rcBrowser.left, rcBrowser.bottom - rcBrowser.top);
		if (m_pBrowser){
			m_pBrowser->ShowPage(browser->GetIdentifier());
		}
	}
}
