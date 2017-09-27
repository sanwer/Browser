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
		m_bBrowserDestroyed(false)
	{
		labTitle = NULL;
		uiToolbar = NULL;
		btnBackward = NULL;
		btnForward = NULL;
		editUrl = NULL;
		editKeyword = NULL;
		sHomepage = _T("https://www.hao123.com/");
		m_sUrl = sHomepage;
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
		labTitle = static_cast<DuiLib::CLabelUI*>(m_Manager.FindControl(_T("labTitle")));
		uiToolbar = static_cast<DuiLib::CControlUI*>(m_Manager.FindControl(_T("uiToolbar")));
		btnBackward = static_cast<DuiLib::CButtonUI*>(m_Manager.FindControl(_T("btnBackward")));
		btnForward = static_cast<DuiLib::CButtonUI*>(m_Manager.FindControl(_T("btnForward")));
		editUrl = static_cast<DuiLib::CEditUI*>(m_Manager.FindControl(_T("editUrl")));
		editKeyword = static_cast<DuiLib::CEditUI*>(m_Manager.FindControl(_T("editKeyword")));
		if (labTitle == NULL || uiToolbar == NULL || editUrl == NULL || editKeyword == NULL)
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
			pUI = m_pBrowser = new Browser::BrowserUI();
		}
		return pUI;
	}

	void BrowserDlg::Notify(DuiLib::TNotifyUI& msg)
	{
		DuiLib::CDuiString sCtrlName = msg.pSender->GetName();

		if (_tcsicmp(msg.sType,DUI_MSGTYPE_WINDOWINIT) == 0)
		{
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
				editUrl->SetText(sHomepage);
				LoadURL(sHomepage.GetData());
			}else if (_tcsicmp(sCtrlName,_T("btnSettings")) == 0){
				DuiLib::CDuiString sUrl = _T("http://fm.baidu.com");//about:settings
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}else if (_tcsicmp(sCtrlName,_T("btnBackward")) == 0){
				if(m_BrowserCtrl){
					m_BrowserCtrl->GetBrowser()->GoBack();
				}
			}else if (_tcsicmp(sCtrlName,_T("btnForward")) == 0){
				if(m_BrowserCtrl){
					m_BrowserCtrl->GetBrowser()->GoForward();
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
		return WindowImplBase::Notify(msg);
	}

	void BrowserDlg::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		REQUIRE_MAIN_THREAD();

		m_Browser = browser;

		if (m_bIsPopup) {
			CreateBrowserWindow(CefBrowserSettings());
		} else {
			//OnSize(false);
		}
	}

	void BrowserDlg::OnBrowserWindowDestroyed()
	{
		REQUIRE_MAIN_THREAD();

		if (m_Browser) {
			m_Browser = NULL;
		}

		m_BrowserCtrl.reset();

		if (!m_bWindowDestroyed) {
			Close();
		}

		m_bBrowserDestroyed = true;
		NotifyDestroyedIfDone();
	}

	void BrowserDlg::OnSetAddress(const std::wstring& url)
	{
		m_sUrl = url.c_str();
		editUrl->SetText(m_sUrl);
	}

	void BrowserDlg::OnSetTitle(const std::wstring& title)
	{
		labTitle->SetText(title.c_str());
		SetWindowText(m_hWnd, title.c_str());
	}

	void BrowserDlg::OnSetFullscreen(bool fullscreen)
	{
	}

	void BrowserDlg::OnSetLoadingState(bool isLoading,bool canGoBack,bool canGoForward)
	{
		if(btnBackward)
			btnBackward->SetEnabled(canGoBack);
		if(btnForward)
			btnForward->SetEnabled(canGoForward);
	}

	void BrowserDlg::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions)
	{
		REQUIRE_MAIN_THREAD();

		/*
		// Reset draggable region.
		::SetRectRgn(draggable_region_, 0, 0, 0, 0);

		// Determine new draggable region.
		std::vector<CefDraggableRegion>::const_iterator it = regions.begin();
		for (;it != regions.end(); ++it) {
		HRGN region = ::CreateRectRgn(
		it->bounds.x, it->bounds.y,
		it->bounds.x + it->bounds.width,
		it->bounds.y + it->bounds.height);
		::CombineRgn(
		draggable_region_, draggable_region_, region,
		it->draggable ? RGN_OR : RGN_DIFF);
		::DeleteObject(region);
		}

		// Subclass child window procedures in order to do hit-testing.
		// This will be a no-op, if it is already subclassed.
		if (m_hWnd) {
		WNDENUMPROC proc = !regions.empty() ? SubclassWindowsProc : UnSubclassWindowsProc;
		::EnumChildWindows(
		m_hWnd, proc, reinterpret_cast<LPARAM>(draggable_region_));
		}
		*/
	}

	void BrowserDlg::NotifyDestroyedIfDone() {
		// Notify once both the window and the browser have been destroyed.
		if (m_bWindowDestroyed && m_bBrowserDestroyed)
			m_Delegate->OnRootWindowDestroyed(this);
	}

	void BrowserDlg::Init(
		BrowserDlg::Delegate* delegate,
		bool with_controls,
		bool with_osr,
		const CefRect& bounds,
		const CefBrowserSettings& settings,
		const std::wstring& url)
	{
		DCHECK(delegate);
		DCHECK(!m_bInitialized);

		m_Delegate = delegate;
		m_bWithControls = with_controls;
		m_bWithOsr = with_osr;

		m_rcBrowser.left = bounds.x;
		m_rcBrowser.top = bounds.y;
		m_rcBrowser.right = bounds.x + bounds.width;
		m_rcBrowser.bottom = bounds.y + bounds.height;

		CreateBrowserCtrl(url);

		m_bInitialized = true;

		// Create the native root window on the main thread.
		if (CURRENTLY_ON_MAIN_THREAD()) {
			CreateBrowserWindow(settings);
		} else {
			MAIN_POST_CLOSURE(base::Bind(&BrowserDlg::CreateBrowserWindow, this, settings));
		}
	}

	void BrowserDlg::InitAsPopup(
		BrowserDlg::Delegate* delegate,
		bool with_controls,
		bool with_osr,
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

		CreateBrowserCtrl(std::wstring());

		m_bInitialized = true;

		// The new popup is initially parented to a temporary window. The native root
		// window will be created after the browser is created and the popup window
		// will be re-parented to it at that time.
		//m_BrowserCtrl->GetPopupConfig(TempWindow::GetWindowHandle(),windowInfo, client, settings);
	}

	CefRefPtr<CefBrowser> BrowserDlg::GetBrowser()
	{
		REQUIRE_MAIN_THREAD();

		if (m_BrowserCtrl)
			return m_BrowserCtrl->GetBrowser();
		return NULL;
	}

	void BrowserDlg::LoadURL(const CefString& url)
	{
		if(m_Browser){
			CefRefPtr<CefFrame> mainfram = m_Browser->GetMainFrame();
			if (mainfram){
				mainfram->LoadURL(url);
			}
		}
	}

	void BrowserDlg::CreateBrowserCtrl(const std::wstring& startup_url)
	{
		if (m_bWithOsr) {
			//OsrRenderer::Settings settings;
			//MainContext::Get()->PopulateOsrSettings(&settings);
			//m_BrowserCtrl.reset(new BrowserWindowOsrWin(this, startup_url, settings));
		} else {
			if(m_pBrowser){
				m_BrowserCtrl.reset(m_pBrowser->CreateBrowserCtrl(this, startup_url));
			}
		}
	}

	void BrowserDlg::CreateBrowserWindow(const CefBrowserSettings& settings) {
		REQUIRE_MAIN_THREAD();

		int x, y, width, height;

		RECT rcWindow = m_rcBrowser;
		if (::IsRectEmpty(&rcWindow)) {
			CenterWindow();
		} else {
			RECT rcWindow = m_rcBrowser;
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

		if (m_bIsPopup) {
			m_BrowserCtrl->ShowPopup(m_hWnd, rcBrowser.left, rcBrowser.top, rcBrowser.right - rcBrowser.left, rcBrowser.bottom - rcBrowser.top);
		} else {
			// Create the browser window.
			CefRect cef_rect(rcBrowser.left, rcBrowser.top, rcBrowser.right - rcBrowser.left, rcBrowser.bottom - rcBrowser.top);
			m_BrowserCtrl->CreateBrowser(m_hWnd, cef_rect, settings, m_Delegate->GetRequestContext(this));
		}

		//Show(ShowNormal);
	}
}
