#ifndef __BROWSERUI_H__
#define __BROWSERUI_H__
#pragma once
#include "BrowserHandler.h"
#include <UIlib.h>
using namespace DuiLib;

namespace Browser
{
	class CBrowserUI : public CControlUI
	{
	public:
		CBrowserUI();
		~CBrowserUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		bool CreateBrowser(const CefString& sHomepage, CBrowserHandler::Delegate* pDelegate);
		void LoadURL(const CefString& url);

		CefRefPtr<CefBrowser> GetBrowser() { return m_Browser->GetBrowser(); }
		CefRefPtr<CefFrame> GetMainFrame() { return m_Browser->GetMainFrame(); }

	protected:
		CefRefPtr<CBrowserHandler> m_Browser;
	};
}

#endif
