#include "StdAfx.h"
#include <sstream>
#include <string>
#include "BrowserUI.h"

namespace Browser
{
	CBrowserUI::CBrowserUI()
		: m_Browser(NULL)
	{
		SetBkColor(0xFFFFFFFF);
	}

	CBrowserUI::~CBrowserUI()
	{
	}

	LPCTSTR CBrowserUI::GetClass() const
	{
		return _T("BrowserUI");
	}

	LPVOID CBrowserUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("BrowserUI")) == 0 ) return static_cast<CBrowserUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	bool CBrowserUI::CreateBrowser(const CefString& sHomepage, CBrowserHandler::Delegate* pDelegate)
	{
		CefWindowInfo info;
		if(m_Browser == NULL)
			m_Browser = new CBrowserHandler(pDelegate,false,sHomepage);
		if(m_Browser != NULL){
			//m_Browser->SetParentHandle(GetManager()->GetPaintWindow());
			//m_Browser->SetBrowserEvent(pEvent);
			info.SetAsChild(GetManager()->GetPaintWindow(), GetPos());
			CefBrowserSettings browser_settings;
			return CefBrowserHost::CreateBrowser(info, m_Browser.get(), sHomepage, browser_settings, NULL);
		}
		return false;
	}

	void CBrowserUI::LoadURL(const CefString& url)
	{
		if (m_Browser.get())
		{
			CefRefPtr<CefFrame> mainfram = m_Browser->GetMainFrame();
			if (mainfram)
				mainfram->LoadURL(url);
		}
	}

	void CBrowserUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		if (m_Browser.get())
		{
			CefRefPtr<CefBrowser> browser = m_Browser->GetBrowser();
			DuiLib::CDuiRect rc = GetPos();
			if (browser && !rc.IsNull())
			{
				::SetWindowPos(browser->GetHost()->GetWindowHandle(),
					NULL, rc.left, rc.top, rc.right - rc.left,
					rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
	}
}
