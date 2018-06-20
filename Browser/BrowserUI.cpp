#include "stdafx.h"
#include <sstream>
#include <string>
#include "BrowserUI.h"
#include "BrowserDlg.h"
#include "MessageLoop.h"
#include "BrowserDlgManager.h"
#include "MainContext.h"

namespace Browser
{
	BrowserUI::BrowserUI(BrowserDlg* pParent, HWND hParentWnd)
		: m_pParent(pParent),
		m_hParentWnd(hParentWnd),
		m_pCtrl(NULL),
		m_nCurBrowserId(0)
	{
		SetBkColor(0xFFFFFFFF);
	}

	LPCTSTR BrowserUI::GetClass() const
	{
		return _T("BrowserUI");
	}

	LPVOID BrowserUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("BrowserUI")) == 0 ) return static_cast<BrowserUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	void BrowserUI::SetPos(RECT rc, bool bNeedInvalidate)
	{
		DuiLib::CDuiRect rcPos = rc;
		if (m_pCtrl && !rcPos.IsNull()) {
			m_pCtrl->ShowBrowser(m_nCurBrowserId, rc.left, rc.top, rc.right - rc.left,rc.bottom - rc.top);
		}
		CControlUI::SetPos(rc, bNeedInvalidate);
	}

	void BrowserUI::SetCtrl(BrowserWindow* pCtrl)
	{
		m_pCtrl = pCtrl;
	}

	void BrowserUI::CreateBrowser(const CefString& url, CefRefPtr<CefRequestContext> request_context)
	{
		if(m_pCtrl){
			CefBrowserSettings settings;
			RECT rcPos = GetPos();
			CefRect cef_rect(rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
			m_pCtrl->CreateBrowser(m_hParentWnd, url, cef_rect, settings, request_context);
		}
	}

	void BrowserUI::ShowBrowser(int nBrowserId)
	{
		m_nCurBrowserId = nBrowserId;
		if(m_pCtrl){
			RECT rcPos = GetPos();
			m_pCtrl->ShowBrowser(nBrowserId, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
		}
	}

	void BrowserUI::CloseBrowser(int nBrowserId)
	{
		if(m_pCtrl){
			CefRefPtr<CefBrowser> pBrowser = m_pCtrl->GetBrowser(nBrowserId);
			if(pBrowser.get()){
				pBrowser->GetHost()->CloseBrowser(false);
			}
		}
	}

	IMPLEMENT_DUICONTROL(TitleUI)
		TitleUI::TitleUI() : m_bSelected(false), m_uButtonState(0), m_dwSelectedBkColor(0),
		m_uTextStyle(DT_VCENTER | DT_SINGLELINE), m_iFont(-1), m_dwTextColor(0), m_dwDisabledTextColor(0)
	{
		::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
	}

	TitleUI::~TitleUI()
	{
		if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	LPCTSTR TitleUI::GetClass() const
	{
		return _T("TitleUI");
	}

	UINT TitleUI::GetControlFlags() const
	{
		if( IsEnabled()) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID TitleUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("Title")) == 0 ) return static_cast<TitleUI*>(this);
		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	LPCTSTR TitleUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void TitleUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	bool TitleUI::IsSelected() const
	{
		return m_bSelected;
	}

	void TitleUI::Selected(bool bSelected)
	{
		if(!(m_bSelected^bSelected)) return;

		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( m_pManager != NULL ) {
			if( !m_sGroupName.IsEmpty() ) {
				if( m_bSelected ) {
					CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
					if(aOptionGroup){
						for( int i = 0; i < aOptionGroup->GetSize(); i++ ) {
							TitleUI* pControl = static_cast<TitleUI*>(aOptionGroup->GetAt(i));
							if( pControl != this ) {
								pControl->Selected(false);
							}
						}
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else {
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool TitleUI::Activate()
	{
		if( !CHorizontalLayoutUI::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void TitleUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CHorizontalLayoutUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS )
		{
			m_bFocused = true;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS )
		{
			m_bFocused = false;
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
		}

		CHorizontalLayoutUI::DoEvent(event);
	}

	void TitleUI::SetVisible(bool bVisible)
	{
		CHorizontalLayoutUI::SetVisible(bVisible);
		if( !IsVisible() && m_bSelected && !m_sGroupName.IsEmpty() )
		{
			Selected(false);
		}
	}

	void TitleUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
			else m_uButtonState = 0;
		}
	}

	void TitleUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("textcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("textpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
			SetTextPadding(rcTextPadding);
		}
		else if( _tcsicmp(pstrName, _T("group")) == 0 ){
			SetGroup(pstrValue);
		}
		else if( _tcsicmp(pstrName, _T("selected")) == 0 ){
			Selected(_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedBkColor(clrColor);
		}
		else CHorizontalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void TitleUI::SetSelectedBkColor( DWORD dwBkColor )
	{
		m_dwSelectedBkColor = dwBkColor;
	}

	DWORD TitleUI::GetSelectBkColor()
	{
		return m_dwSelectedBkColor;
	}

	void TitleUI::PaintBkColor(HDC hDC)
	{
		if(IsSelected()) {
			if(m_dwSelectedBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
			}
		}
		else {
			return CHorizontalLayoutUI::PaintBkColor(hDC);
		}
	}

	UINT TitleUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	void TitleUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		Invalidate();
	}

	DWORD TitleUI::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void TitleUI::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	DWORD TitleUI::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	void TitleUI::SetFont(int index)
	{
		m_iFont = index;
		Invalidate();
	}

	int TitleUI::GetFont() const
	{
		return m_iFont;
	}

	RECT TitleUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void TitleUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	void TitleUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		RECT rc = m_rcItem;
		GetManager()->GetDPIObj()->Scale(&m_rcTextPadding);
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		CDuiString sText = GetText();
		if( sText.IsEmpty() ) return;
		int nLinks = 0;
		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwTextColor, m_iFont, m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle);
		}
	}
}
