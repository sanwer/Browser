#ifndef  _CLIENT_H_
#define  _CLIENT_H_
#pragma once

#define WM_ADDRESS_CHANGE	(WM_USER+201)
#define WM_TITLE_CHANGE		(WM_USER+202)
#define WM_POPUP_WINDOW		(WM_USER+203)

class CBrowserUI;
class CBrowserDlg : public WindowImplBase
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
	void SetAddress(LPCTSTR pstrAddress);
	void SetTitle(LPCTSTR pstrTitle);
	void SetLoadingState(bool isLoading,bool canGoBack,bool canGoForward);
	void Popup(LPCTSTR pstrUrl);
	void OnPopup();
	
protected:
	void Notify(TNotifyUI& msg);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

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

#endif