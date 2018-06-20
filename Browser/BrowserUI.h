#ifndef __BROWSERUI_H__
#define __BROWSERUI_H__
#pragma once
#include "ClientHandler.h"
#include <UIlib.h>
using namespace DuiLib;

namespace Browser
{
	class BrowserDlg;
	class BrowserWindow;

	class BrowserUI : public DuiLib::CControlUI
	{
		DECLARE_DUICONTROL(BrowserUI)
	public:
		BrowserUI(BrowserDlg* pParent, HWND hParentWnd);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetPos(RECT rc, bool bNeedInvalidate = true);
		void SetCtrl(BrowserWindow* pCtrl);
		void CreateBrowser(const CefString& url, CefRefPtr<CefRequestContext> request_context);
		void ShowBrowser(int nBrowserId=0);
		void CloseBrowser(int nBrowserId=0);

	protected:
		HWND m_hParentWnd;
		BrowserDlg* m_pParent;
		BrowserWindow* m_pCtrl;
		int m_nCurBrowserId;

		DISALLOW_COPY_AND_ASSIGN(BrowserUI);
	};

	class TitleUI : public CHorizontalLayoutUI
	{
		DECLARE_DUICONTROL(TitleUI)
	public:
		TitleUI();
		~TitleUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);

		bool IsSelected() const;
		virtual void Selected(bool bSelected = true);

		bool Activate();
		void DoEvent(TEventUI& event);
		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();
		void PaintBkColor(HDC hDC);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		void PaintText(HDC hDC);

	protected:
		bool		m_bSelected;
		UINT		m_uButtonState;
		DWORD		m_dwSelectedBkColor;
		CDuiString	m_sGroupName;

		int			m_iFont;
		UINT		m_uTextStyle;
		RECT		m_rcTextPadding;
		DWORD		m_dwTextColor;
		DWORD		m_dwDisabledTextColor;
	};
}

#endif
