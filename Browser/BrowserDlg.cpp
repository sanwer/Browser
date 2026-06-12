#include "stdafx.h"
#include "BrowserDlg.h"
#include "MainContext.h"
#include "BrowserDlgManager.h"
#include "TempWindow.h"
#include "BrowserUI.h"

namespace Browser
{
	const int FrameLeft = 2;
	const int FrameTop = 1;
	const int FrameRight = 2;
	const int FrameBottom = 2;
	const int TitleHeight = 32;
	const int ToolbarHeight = 28;

	const int TabHeight = 30;
	const int TabMinWidth = 100;
	const int TabMaxWidth = 256;
	const int TabCloseWidth = 26;
	const int TabCloseHeight = 28;
	const int TabTextPaddingLeft = 5;
	const int TabTextPaddingTop = 1;
	const int TabTextPaddingRight = 20;
	const int TabTextPaddingBottom = 2;
	const int TabCloseDestLeft = 8;
	const int TabCloseDestTop = 8;
	const int TabCloseDestRight = 19;
	const int TabCloseDestBottom = 19;
	const int TabBorderRound = 2;

	const LPCTSTR TabBkColor = _T("FF1587D8");
	const LPCTSTR TabSelectedBkColor = _T("FF3498DB");
	const LPCTSTR TabTextColor = _T("FFFFFFFF");

	const LPCTSTR CtrlNameUiTabs = _T("uiTabs");
	const LPCTSTR CtrlNameTabNew = _T("tabNew");
	const LPCTSTR CtrlNameUiToolbar = _T("uiToolbar");
	const LPCTSTR CtrlNameBtnBackward = _T("btnBackward");
	const LPCTSTR CtrlNameBtnForward = _T("btnForward");
	const LPCTSTR CtrlNameBtnGoto = _T("btnGoto");
	const LPCTSTR CtrlNameBtnSearch = _T("btnSearch");
	const LPCTSTR CtrlNameBtnHome = _T("btnHome");
	const LPCTSTR CtrlNameBtnSettings = _T("btnSettings");
	const LPCTSTR CtrlNameEditUrl = _T("editUrl");
	const LPCTSTR CtrlNameEditKeyword = _T("editKeyword");
	const LPCTSTR CtrlClassBrowserUI = _T("BrowserUI");
	const LPCTSTR CtrlClassTitle = _T("Title");
	const LPCTSTR CtrlClassButtonUI = _T("ButtonUI");
	const LPCTSTR CtrlGroupNameTitles = _T("Titles");

	const LPCTSTR UrlBlank = _T("about:blank");
	const LPCTSTR UrlSettings = _T("about:settings");
	const LPCTSTR UrlSearchFormat = _T("https://www.baidu.com/s?wd=%s");

	const LPCTSTR SkinFileBrowserDlg = _T("BrowserDlg.xml");
	const LPCTSTR ImageTabClose = _T("btnTabClose.png");

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
		m_nCurBrowserId(0)
	{
		uiTabs = NULL;
		pTitle = NULL;
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
		uiTabs = static_cast<DuiLib::CHorizontalLayoutUI*>(m_pm.FindControl(CtrlNameUiTabs));
		tabNew = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(CtrlNameTabNew));
		uiToolbar = static_cast<DuiLib::CControlUI*>(m_pm.FindControl(CtrlNameUiToolbar));
		btnBackward = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(CtrlNameBtnBackward));
		btnForward = static_cast<DuiLib::CButtonUI*>(m_pm.FindControl(CtrlNameBtnForward));
		editUrl = static_cast<DuiLib::CEditUI*>(m_pm.FindControl(CtrlNameEditUrl));
		editKeyword = static_cast<DuiLib::CEditUI*>(m_pm.FindControl(CtrlNameEditKeyword));
		
		if (uiTabs == NULL || tabNew == NULL || uiToolbar == NULL || editUrl == NULL || editKeyword == NULL)
		{
			MessageBox(NULL, _T("Failed to load resource file"), _T("Browser"), MB_OK | MB_ICONERROR);
			return;
		}
		
		if (m_bWithControls) {
			uiToolbar->SetVisible(true);
			if (btnBackward)
				btnBackward->SetEnabled(false);
			if (btnForward)
				btnForward->SetEnabled(false);
		} else {
			uiToolbar->SetVisible(false);
		}
		
		tabNew->SetVisible(!m_bIsPopup);
		if (m_bIsPopup) {
			uiToolbar->SetVisible(false);
		}
	}

	void BrowserDlg::OnFinalMessage(HWND hWnd)
	{
		WindowImplBase::OnFinalMessage(hWnd);
		m_bWindowDestroyed = true;
		NotifyDestroyedIfDone();
	}

	DuiLib::CDuiString BrowserDlg::GetSkinFile()
	{
		return SkinFileBrowserDlg;
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
		if (_tcsicmp(pstrClass, CtrlClassBrowserUI) == 0)
		{
			if (m_pBrowserUI == NULL) {
				m_pBrowserUI = new Browser::BrowserUI(this, m_hWnd);
			}
			if (m_pBrowserUI != NULL && m_BrowserCtrl.get() != NULL) {
				m_pBrowserUI->SetCtrl(m_BrowserCtrl.get());
			}
			pUI = m_pBrowserUI;
		}
		else if (_tcsicmp(pstrClass, CtrlClassTitle) == 0)
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
			if (wParam == 0L)
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

		if (_tcsicmp(msg.sType, _T("click")) == 0)
		{
			if (_tcsicmp(sCtrlName, CtrlNameBtnGoto) == 0) {
				DuiLib::CDuiString sUrl = editUrl->GetText();
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			} else if (_tcsicmp(sCtrlName, CtrlNameBtnSearch) == 0) {
				DuiLib::CDuiString sUrl, sKeyword = editKeyword->GetText();
				sUrl.Format(UrlSearchFormat, sKeyword.GetData());
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			} else if (_tcsicmp(sCtrlName, CtrlNameBtnHome) == 0) {
				editUrl->SetText(MainContext::Get()->GetMainURL().c_str());
				LoadURL(MainContext::Get()->GetMainURL().c_str());
			} else if (_tcsicmp(sCtrlName, CtrlNameBtnSettings) == 0) {
				DuiLib::CDuiString sUrl = UrlSettings;
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			} else if (_tcsicmp(sCtrlName, CtrlNameBtnBackward) == 0) {
				CefRefPtr<CefBrowser> browser = GetBrowser();
				if (browser) {
					browser->GoBack();
				}
			} else if (_tcsicmp(sCtrlName, CtrlNameBtnForward) == 0) {
				CefRefPtr<CefBrowser> browser = GetBrowser();
				if (browser) {
					browser->GoForward();
				}
			} else if (_tcsicmp(sCtrlName, CtrlNameTabNew) == 0) {
				NewTab(UrlBlank);
			} else if (_tcsnicmp(sCtrlName, _T("tabClose"), 8) == 0) {
				HandleTabClose(sCtrlName);
			}
		}
		else if (_tcsicmp(msg.sType, DUI_MSGTYPE_RETURN) == 0)
		{
			if (_tcsicmp(sCtrlName, CtrlNameEditUrl) == 0) {
				DuiLib::CDuiString sUrl = editUrl->GetText();
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			} else if (_tcsicmp(sCtrlName, CtrlNameEditKeyword) == 0) {
				DuiLib::CDuiString sUrl, sKeyword = editKeyword->GetText();
				sUrl.Format(UrlSearchFormat, sKeyword.GetData());
				editUrl->SetText(sUrl);
				LoadURL(sUrl.GetData());
			}
		}
		else if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
		{
			if (_tcsnicmp(sCtrlName, _T("tabTitle"), 8) == 0) {
				CDuiString sBuffer = msg.pSender->GetText();
				SetWindowText(m_hWnd, sBuffer.GetData());
				sBuffer = msg.pSender->GetUserData();
				editUrl->SetText(sBuffer.GetData());
				if (m_pBrowserUI) {
					m_nCurBrowserId = msg.pSender->GetTag();
					m_pBrowserUI->ShowBrowser(m_nCurBrowserId);
				}
			}
		}
		return WindowImplBase::Notify(msg);
	}

	void BrowserDlg::HandleTabClose(const DuiLib::CDuiString& sCtrlName)
	{
		DuiLib::CDuiString sBuffer = sCtrlName;
		sBuffer.Replace(_T("tabClose"), _T(""));
		int nBrowserId = _ttoi(sBuffer.GetData());
		
		int nTabsCount = uiTabs->GetCount();
		if (nTabsCount <= 2) {
			LoadURL(UrlBlank);
			return;
		}

		for (int idx = 0; idx < nTabsCount; idx++)
		{
			TitleUI* pItem = (TitleUI*)uiTabs->GetItemAt(idx);
			if (pItem != NULL && pItem->GetTag() == nBrowserId) {
				TitleUI* pTitle = (idx > 0) ? 
					(TitleUI*)uiTabs->GetItemAt(idx - 1) : 
					(TitleUI*)uiTabs->GetItemAt(idx + 1);
				
				uiTabs->Remove(pItem);
				pTitle->Selected(true);
				
				if (m_pBrowserUI) {
					m_pBrowserUI->CloseBrowser(nBrowserId);
				}
				break;
			}
		}
	}

	void BrowserDlg::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		
		int nBrowserId = browser->GetIdentifier();
		if (m_nCurBrowserId != nBrowserId) {
			m_nCurBrowserId = nBrowserId;
		}

		if (m_bIsPopup) {
			CreateBrowserDlg(CefBrowserSettings());
		} else if (m_pBrowserUI && m_BrowserCtrl) {
			RECT rcPos = m_pBrowserUI->GetPos();
			m_BrowserCtrl->ShowBrowser(m_nCurBrowserId, rcPos.left, rcPos.top, 
				rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
		}
	}

	void BrowserDlg::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
	}

	void BrowserDlg::OnBrowserAllClosed()
	{
		DCHECK(CefCurrentlyOn(TID_UI));

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
		DCHECK(CefCurrentlyOn(TID_UI));
		
		CDuiString sUrl;
		bool bAddTab = true;
		int nTabsCount = uiTabs->GetCount();
		int nBrowserId = browser->GetIdentifier();
		
		if (url.length() > 0) {
			sUrl = CefString(url).ToWString().c_str();
		}

		for (int idx = 0; idx < nTabsCount; idx++)
		{
			DuiLib::CControlUI* pTitle = (DuiLib::CControlUI*)uiTabs->GetItemAt(idx);
			if (pTitle == NULL || _tcsicmp(pTitle->GetClass(), CtrlClassButtonUI) == 0) {
				continue;
			}
			if (pTitle->GetTag() == nBrowserId) {
				bAddTab = false;
				pTitle->SetUserData(sUrl);
				CDuiString sBuffer = pTitle->GetText();
				if (sBuffer.GetLength() == 0) {
					pTitle->SetText(sUrl);
				}
				break;
			}
		}

		if (nBrowserId == m_nCurBrowserId) {
			editUrl->SetText(sUrl);
		}

		if (bAddTab) {
			AddNewTab(nBrowserId, sUrl);
		}
	}

	void BrowserDlg::AddNewTab(int nBrowserId, const DuiLib::CDuiString& sUrl)
	{
		if (m_bIsPopup) {
			if (pTitle == NULL) {
				pTitle = new DuiLib::CLabelUI;
			}
			pTitle->SetText(sUrl);
			uiTabs->AddAt(pTitle, uiTabs->GetCount() - 1);
			DuiLib::CDuiString sBuffer;
			sBuffer.Format(_T("name=\"labTitle\" height=\"%d\" floatalign=\"right\" textpadding=\"%d,%d,%d,%d\" textcolor=\"%s\""), 
				TabHeight, TabTextPaddingLeft, TabTextPaddingTop, TabTextPaddingRight, TabTextPaddingBottom, TabTextColor);
			pTitle->ApplyAttributeList(sBuffer);
		} else {
			TitleUI* pTitle = new TitleUI;
			pTitle->SetTag(nBrowserId);
			pTitle->SetUserData(sUrl);
			uiTabs->AddAt(pTitle, uiTabs->GetCount() - 1);
			m_nCurBrowserId = nBrowserId;
			
			DuiLib::CDuiString sBuffer;
			sBuffer.Format(_T("name=\"tabTitle%d\" height=\"%d\" minwidth=\"%d\" maxwidth=\"%d\" floatalign=\"right\" borderround=\"%d,%d\" textpadding=\"%d,%d,%d,%d\" bkcolor=\"%s\" selectedbkcolor=\"%s\" textcolor=\"%s\" selectedtextcolor=\"%s\" group=\"%s\""), 
				nBrowserId, TabHeight, TabMinWidth, TabMaxWidth, TabBorderRound, TabBorderRound,
				TabTextPaddingLeft, TabTextPaddingTop, TabTextPaddingRight, TabTextPaddingBottom,
				TabBkColor, TabSelectedBkColor, TabTextColor, TabTextColor, CtrlGroupNameTitles);
			pTitle->ApplyAttributeList(sBuffer);

			DuiLib::CControlUI* pControl = new DuiLib::CControlUI;
			DuiLib::CButtonUI* pClose = new DuiLib::CButtonUI;
			pTitle->Add(pControl);
			pTitle->Add(pClose);

			sBuffer.Format(_T("name=\"tabClose%d\" width=\"%d\" height=\"%d\" floatalign=\"right\" normalimage=\"file='%s' source='0,0,11,11' dest='%d,%d,%d,%d'\" hotimage=\"file='%s' source='11,0,22,11' dest='%d,%d,%d,%d'\" pushedimage=\"file='%s' source='22,0,33,11' dest='%d,%d,%d,%d'\""), 
				nBrowserId, TabCloseWidth, TabCloseHeight, ImageTabClose,
				TabCloseDestLeft, TabCloseDestTop, TabCloseDestRight, TabCloseDestBottom,
				ImageTabClose, TabCloseDestLeft, TabCloseDestTop, TabCloseDestRight, TabCloseDestBottom,
				ImageTabClose, TabCloseDestLeft, TabCloseDestTop, TabCloseDestRight, TabCloseDestBottom);
			pClose->ApplyAttributeList(sBuffer);

			pTitle->Selected(true);
			editUrl->SetText(sUrl);
		}
	}

	void BrowserDlg::OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		
		CDuiString sTitle;
		int nBrowserId = browser->GetIdentifier();
		
		if (title.length() > 0) {
			sTitle = CefString(title).ToWString().c_str();
		}

		if (m_bIsPopup) {
			if (pTitle == NULL) {
				pTitle = new CLabelUI;
			}
			pTitle->SetText(sTitle);
			SetWindowText(m_hWnd, sTitle);
		} else {
			int nTabsCount = uiTabs->GetCount();
			for (int idx = 0; idx < nTabsCount; idx++)
			{
				DuiLib::CControlUI* pTitle = (DuiLib::CControlUI*)uiTabs->GetItemAt(idx);
				if (pTitle == NULL || _tcsicmp(pTitle->GetClass(), CtrlClassButtonUI) == 0) {
					continue;
				}
				if (pTitle->GetTag() == nBrowserId) {
					pTitle->SetText(sTitle);
				}
			}
			if (nBrowserId == m_nCurBrowserId) {
				SetWindowText(m_hWnd, sTitle);
			}
		}
	}

	void BrowserDlg::OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
	}

	void BrowserDlg::OnSetLoadingState(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
		if (btnBackward) {
			btnBackward->SetEnabled(canGoBack);
		}
		if (btnForward) {
			btnForward->SetEnabled(canGoForward);
		}
	}

	void BrowserDlg::OnSetDraggableRegions(CefRefPtr<CefBrowser> browser, const std::vector<CefDraggableRegion>& regions)
	{
		DCHECK(CefCurrentlyOn(TID_UI));
	}

	void BrowserDlg::OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) {
		DCHECK(CefCurrentlyOn(TID_UI));
		NewTab(url);
	}

	void BrowserDlg::NotifyDestroyedIfDone() {
		// Notify once both the window and the browser have been destroyed.
		if (m_bWindowDestroyed && m_bBrowserDestroyed && m_Delegate) {
			m_Delegate->OnBrowserDlgDestroyed(this);
		}
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
		m_hParent = hParent;
		m_bWithControls = true;

		m_rcStart.left = bounds.x;
		m_rcStart.top = bounds.y;
		m_rcStart.right = bounds.x + bounds.width;
		m_rcStart.bottom = bounds.y + bounds.height;

		CreateBrowserWindow(url);

		m_bInitialized = true;

		// Create the native root window on the main thread.
		if (CefCurrentlyOn(TID_UI)) {
			CreateBrowserDlg(settings);
		} else {
			CefPostTask(TID_UI, base::BindRepeating(&BrowserDlg::CreateBrowserDlg, this, settings));
		}
	}

	void BrowserDlg::InitAsPopup(
		BrowserDlg::Delegate* delegate,
		bool with_controls,
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
		m_BrowserCtrl->GetPopupConfig(TempWindow::GetWindowHandle(), windowInfo, client, settings);
	}

	void BrowserDlg::NewTab(const CefString& url)
	{
		if (m_BrowserCtrl && m_pBrowserUI && !url.empty()) {
			RECT rcPos = m_pBrowserUI->GetPos();
			CefRect cef_rect(rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
			CefBrowserSettings settings;
			MainContext::Get()->PopulateBrowserSettings(&settings);
			m_BrowserCtrl->CreateBrowser(m_hWnd, url, cef_rect, settings, m_Delegate->GetRequestContext());
		}
	}

	CefRefPtr<CefBrowser> BrowserDlg::GetBrowser()
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		if (m_BrowserCtrl) {
			return m_BrowserCtrl->GetBrowser(m_nCurBrowserId);
		}
		return nullptr;
	}

	CefWindowHandle BrowserDlg::GetWindowHandle()
	{
		return m_hWnd;
	}

	void BrowserDlg::LoadURL(const CefString& url)
	{
		if (m_BrowserCtrl.get() != NULL) {
			CefRefPtr<CefBrowser> pBrowser = m_BrowserCtrl->GetBrowser(m_nCurBrowserId);
			if (pBrowser) {
				CefRefPtr<CefFrame> pFrame = pBrowser->GetMainFrame();
				if (pFrame) {
					pFrame->LoadURL(url);
				}
			}
		}
	}

	void BrowserDlg::CreateBrowserWindow(const CefString& startup_url)
	{
		m_BrowserCtrl.reset(new BrowserWindow(this, startup_url));
	}

	void BrowserDlg::CreateBrowserDlg(const CefBrowserSettings& settings)
	{
		DCHECK(CefCurrentlyOn(TID_UI));

		int x, y, width, height;
		if (::IsRectEmpty(&m_rcStart)) {
			// Use the default window position/size.
			x = y = width = height = CW_USEDEFAULT;
		} else {
			RECT rcWindow = m_rcStart;
			if (m_bWithControls) {
				rcWindow.bottom += FrameTop + TitleHeight + FrameBottom;
				rcWindow.right += FrameLeft + FrameRight;
				if (!m_bIsPopup) {
					rcWindow.bottom += ToolbarHeight;
				}
			}
			x = rcWindow.left;
			y = rcWindow.top;
			width = rcWindow.right - rcWindow.left;
			height = rcWindow.bottom - rcWindow.top;
		}

		// Create the main window.
		Create(m_hParent, _T("Browser"), UI_WNDSTYLE_FRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
			WS_EX_APPWINDOW, x, y, width, height, NULL);

		::SetMenu(m_hWnd, NULL);

		if (m_pBrowserUI) {
			RECT rect = m_pBrowserUI->GetPos();

			if (m_bIsPopup) {
				// With popups we already have a browser window. Parent the browser window
				// to the root window and show it in the correct location.
				m_BrowserCtrl->ShowPopup(m_nCurBrowserId, m_hWnd, rect.left, rect.top, 
					rect.right - rect.left, rect.bottom - rect.top);
			} else {
				// Create the browser window.
				CefRect cef_rect(rect.left, rect.top,
					rect.right - rect.left,
					rect.bottom - rect.top);
				m_BrowserCtrl->CreateBrowser(m_hWnd, std::wstring(), cef_rect, settings, m_Delegate->GetRequestContext());
			}
		}
	}
}