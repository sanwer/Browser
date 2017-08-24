#ifndef __BROWSER_H__
#define __BROWSER_H__
#pragma once
#include "BrowserUI.h"

namespace Browser
{
	class CBrowserDlg : public WindowImplBase, CBrowserHandler::Delegate
	{
	public:
		CBrowserDlg(BOOL bPopup=FALSE,LPCTSTR sUrl=NULL);
		~CBrowserDlg();

	public:
		LPCTSTR GetWindowClassName() const;
		virtual void InitWindow();
		virtual void OnFinalMessage(HWND hWnd);
		virtual CDuiString GetSkinFile();
		virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
		virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	protected:
		void Notify(TNotifyUI& msg);
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	private:
		void OnSetAddress(const std::wstring& url);
		void OnSetTitle(const std::wstring& title);
		void OnSetFullscreen(bool fullscreen);
		void OnSetLoadingState(bool isLoading,bool canGoBack,bool canGoForward);

	private:
		CLabelUI* labTitle;
		CControlUI* uiToolbar;
		CButtonUI* btnBackward;
		CButtonUI* btnForward;
		CEditUI* editUrl;
		CEditUI* editKeyword;
		CBrowserUI* mBrowser;
		CDuiString sHomepage;
		CDuiString m_sUrl;
		CDuiString m_sPopup;
		BOOL m_bPopup;
		static int m_sCount;
	};
}

#endif