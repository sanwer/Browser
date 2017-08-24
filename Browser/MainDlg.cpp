#include "stdafx.h"
#include <windows.h>
#include "MainDlg.h"
#include "BrowserUI.h"
#include "BrowserApp.h"

#include <objbase.h>

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
	CefRefPtr<Browser::CBrowserApp> app(new Browser::CBrowserApp);
	CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
	command_line->InitFromString(::GetCommandLineW());
	const std::string& process_type = command_line->GetSwitchValue("type");
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

	Browser::CBrowserDlg* pFrame = new Browser::CBrowserDlg();
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

namespace Browser
{
	int CBrowserDlg::m_sCount=0;

	CBrowserDlg::CBrowserDlg(BOOL bPopup,LPCTSTR sUrl)
	{
		labTitle = NULL;
		uiToolbar = NULL;
		btnBackward = NULL;
		btnForward = NULL;
		editUrl = NULL;
		editKeyword = NULL;
		m_bPopup = bPopup;
		sHomepage = _T("https://github.com/sanwer/Browser");
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
		labTitle = static_cast<CLabelUI*>(m_Manager.FindControl(_T("labTitle")));
		uiToolbar = static_cast<CControlUI*>(m_Manager.FindControl(_T("uiToolbar")));
		btnBackward = static_cast<CButtonUI*>(m_Manager.FindControl(_T("btnBackward")));
		btnForward = static_cast<CButtonUI*>(m_Manager.FindControl(_T("btnForward")));
		editUrl = static_cast<CEditUI*>(m_Manager.FindControl(_T("editUrl")));
		editKeyword = static_cast<CEditUI*>(m_Manager.FindControl(_T("editKeyword")));
		mBrowser = static_cast<CBrowserUI*>(m_Manager.FindControl(_T("mBrowser")));
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

	LRESULT CBrowserDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//if(mBrowser)
		//	mBrowser->CloseAllBrowsers();
		bHandled = FALSE;
		return 0;
	}

	LRESULT CBrowserDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	void CBrowserDlg::OnSetAddress(const std::wstring& url)
	{
		if(m_sUrl.CompareNoCase(url.c_str())!=0)
		{
			m_sUrl = url.c_str();
			editUrl->SetText(m_sUrl);
		}
	}

	void CBrowserDlg::OnSetTitle(const std::wstring& title)
	{
		labTitle->SetText(title.c_str());
		SetWindowText(m_hWnd, title.c_str());
	}

	void CBrowserDlg::OnSetFullscreen(bool fullscreen)
	{
	}

	void CBrowserDlg::OnSetLoadingState(bool isLoading,bool canGoBack,bool canGoForward)
	{
		if(btnBackward)
			btnBackward->SetEnabled(canGoBack);
		if(btnForward)
			btnForward->SetEnabled(canGoForward);
	}
}
