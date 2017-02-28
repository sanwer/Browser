#include "stdafx.h"
#include "Browser.h"
#include "BrowserUI.h"
#include "MiniDumper.h"

CMiniDumper g_miniDumper(true);
int CBrowserDlg::m_sCount=0;

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if(FAILED(Hr)) return 0;

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));

	CefMainArgs main_args(hInstance);
	CefRefPtr<CefApp> app;
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());
	const std::string& process_type = command_line->GetSwitchValue("type");
	if (process_type == "renderer"){
		app = new CRendererApp();
	}else{
		app = new CBrowserApp();
	}
	int exit_code = CefExecuteProcess(main_args, app.get(), NULL);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	CefSettings settings;// Specify CEF global settings here.
	WCHAR szBuffer[MAX_PATH];
	::ZeroMemory(&szBuffer, sizeof(szBuffer));
	GetTempPathW(MAX_PATH,szBuffer);
	std::wstring sBuffer = szBuffer;
	sBuffer += L"\\Browser";
	CefString(&settings.cache_path).FromWString(sBuffer);
	settings.multi_threaded_message_loop=true;	//使用主程序消息循环
	settings.ignore_certificate_errors = true;	//忽略掉ssl证书验证错误
    //settings.command_line_args_disabled = true;
	settings.no_sandbox = true;
	CefString(&settings.locale).FromASCII("zh-CN");

	// Initialize CEF.
	CefInitialize(main_args, settings, app.get(), NULL);

	CBrowserDlg* pFrame = new CBrowserDlg();
	if(pFrame == NULL) return 0;
	pFrame->Create(NULL,_T("Browser"),UI_WNDSTYLE_FRAME,WS_EX_APPWINDOW,0,0,800,600);
	pFrame->CenterWindow();

	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();

	// Shut down CEF.
	CefShutdown();

	::CoUninitialize();
	return 0;
}

CBrowserDlg::CBrowserDlg(BOOL bPopup,LPCTSTR sUrl)
{
	labTitle = NULL;
	uiToolbar = NULL;
	btnBackward = NULL;
	btnForward = NULL;
	editUrl = NULL;
	editKeyword = NULL;
	m_bPopup = bPopup;
	sHomepage = _T("https://www.baidu.com/");
	if(sUrl==NULL)
		m_sUrl = sHomepage;
	else
		m_sUrl = sUrl;
	m_sCount++;
}

CBrowserDlg::~CBrowserDlg()
{
	if(m_sCount--==1){
		PostQuitMessage(0);
	}
}

LPCTSTR CBrowserDlg::GetWindowClassName() const
{
	return _T("CBrowserDlg");
}

void CBrowserDlg::InitWindow()
{
	SetIcon(IDR_MAINFRAME);
	labTitle = static_cast<CLabelUI*>(m_pm.FindControl(_T("labTitle")));
	uiToolbar = static_cast<CControlUI*>(m_pm.FindControl(_T("uiToolbar")));
	btnBackward = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnBackward")));
	btnForward = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnForward")));
	editUrl = static_cast<CEditUI*>(m_pm.FindControl(_T("editUrl")));
	editKeyword = static_cast<CEditUI*>(m_pm.FindControl(_T("editKeyword")));
	mBrowser = static_cast<CBrowserUI*>(m_pm.FindControl(_T("mBrowser")));
	if (labTitle == NULL || uiToolbar == NULL || editUrl == NULL || editKeyword == NULL || mBrowser == NULL)
	{
		MessageBox(NULL,_T("加载资源文件失败"),_T("Browser"),MB_OK|MB_ICONERROR);
		return;
	}
	//if(m_bPopup)
	//	uiToolbar->SetVisible(false);
	if(btnBackward)
		btnBackward->SetEnabled(false);
	if(btnForward)
		btnForward->SetEnabled(false);
}

void CBrowserDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

CDuiString CBrowserDlg::GetSkinFile()
{
	return _T("BrowserDlg.xml");
}

LRESULT CBrowserDlg::ResponseDefaultKeyEvent(WPARAM wParam)
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

CControlUI* CBrowserDlg::CreateControl(LPCTSTR pstrClass)
{
	CControlUI* pUI = NULL;
	if (_tcsicmp(pstrClass, _T("BrowserUI")) == 0)
	{
		pUI = mBrowser = new CBrowserUI();
	}
	return pUI;
}

void CBrowserDlg::Notify(TNotifyUI& msg)
{
	CDuiString sCtrlName = msg.pSender->GetName();

	if (_tcsicmp(msg.sType,DUI_MSGTYPE_WINDOWINIT) == 0)
	{
		if(mBrowser){
			editUrl->SetText(m_sUrl);
			mBrowser->CreateBrowser(m_sUrl.GetData(),this);
		}
	}
	else if (_tcsicmp(msg.sType,_T("click")) == 0)
	{
		if (_tcsicmp(sCtrlName,_T("btnGoto")) == 0){//跳转
			if(mBrowser){
				CDuiString sUrl = editUrl->GetText();
				editUrl->SetText(sUrl);
				mBrowser->LoadURL(sUrl.GetData());
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnSearch")) == 0){//搜索
			if(mBrowser){
				CDuiString sAddr,sKeyword = editKeyword->GetText();
				sAddr.Format(_T("https://www.baidu.com/s?wd=%s"),sKeyword.GetData());
				editUrl->SetText(sAddr);
				//editKeyword->SetText(_T(""));
				mBrowser->LoadURL(sAddr.GetData());
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnHome")) == 0){//主页
			if(mBrowser){
				editUrl->SetText(sHomepage);
				mBrowser->LoadURL(sHomepage.GetData());
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnSettings")) == 0){
			CDuiString sUrl = _T("about:settings");
			if(mBrowser){
				editUrl->SetText(sUrl);
				mBrowser->LoadURL(sUrl.GetData());
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnMusic")) == 0){//音乐
			CDuiString sUrl = _T("http://fm.baidu.com/");
			if(m_sUrl.CompareNoCase(sHomepage)==0)
			{
				if(mBrowser){
					editUrl->SetText(sUrl);
					mBrowser->LoadURL(sUrl.GetData());
				}
			}
			else if(m_sUrl.CompareNoCase(sUrl)==0)
			{
			}
			else
			{
				Popup(_T("http://fm.baidu.com"));
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnInfo")) == 0){
			CDuiString sUrl = _T("http://www.sanwer.com");
			if(mBrowser){
				editUrl->SetText(sUrl);
				mBrowser->LoadURL(sUrl.GetData());
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnBackward")) == 0){
			if(mBrowser){
				mBrowser->GetBrowser()->GoBack();
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("btnForward")) == 0){
			if(mBrowser){
				mBrowser->GetBrowser()->GoForward();
			}
			return;
		}
	}
	else if (_tcsicmp(msg.sType,DUI_MSGTYPE_RETURN) == 0)
	{
		if (_tcsicmp(sCtrlName,_T("editUrl")) == 0){
			if(mBrowser){
				CDuiString sUrl = editUrl->GetText();
				editUrl->SetText(sUrl);
				mBrowser->LoadURL(sUrl.GetData());
			}
			return;
		}else if (_tcsicmp(sCtrlName,_T("editKeyword")) == 0){
			if(mBrowser){
				CDuiString sAddr,sKeyword = editKeyword->GetText();
				sAddr.Format(_T("https://www.baidu.com/s?wd=%s"),sKeyword.GetData());
				editUrl->SetText(sAddr);
				//editKeyword->SetText(_T(""));
				mBrowser->LoadURL(sAddr.GetData());
			}
			return;
		}
	}
	return WindowImplBase::Notify(msg);
}

void CBrowserDlg::SetAddress(LPCTSTR pstrAddress)
{
	if(m_sUrl.CompareNoCase(pstrAddress)!=0)
	{
		m_sUrl = pstrAddress;
		editUrl->SetText(pstrAddress);
	}
}

void CBrowserDlg::SetTitle(LPCTSTR pstrTitle)
{
	labTitle->SetText(pstrTitle);
	SetWindowText(m_hWnd, pstrTitle);
}

void CBrowserDlg::SetLoadingState(bool isLoading,bool canGoBack,bool canGoForward)
{
	if(btnBackward)
		btnBackward->SetEnabled(canGoBack);
	if(btnForward)
		btnForward->SetEnabled(canGoForward);
}

void CBrowserDlg::Popup(LPCTSTR pstrUrl)
{
	m_sPopup = pstrUrl;
	SendMessage(WM_POPUP_WINDOW);
}

void CBrowserDlg::OnPopup()
{
	CBrowserDlg* pPopup = new CBrowserDlg(TRUE,m_sPopup.GetData());
	if(pPopup == NULL) return;
	pPopup->Create(NULL,_T("Browser"),UI_WNDSTYLE_FRAME,WS_EX_APPWINDOW,0,0,800,600);
	pPopup->CenterWindow();
}

LRESULT CBrowserDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
	case WM_POPUP_WINDOW:
		bHandled = TRUE;
		OnPopup();
		break;
	default:
		bHandled = FALSE;
		break;
	}
	return 0;
}
LRESULT CBrowserDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(mBrowser)
		mBrowser->CloseAllBrowsers();
	bHandled = FALSE;
	return 0;
}

LRESULT CBrowserDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}