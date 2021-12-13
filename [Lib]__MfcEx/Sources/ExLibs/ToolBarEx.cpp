#include "pch.h"
#include "ToolBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolBarWithCombo

BEGIN_MESSAGE_MAP(CToolBarWithCombo, CToolBar)
	//{{AFX_MSG_MAP(CToolBarWithCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CToolBarWithCombo::CreateComboBox(CComboBox& comboBox, UINT nIndex, UINT nID, 
								int nWidth, int nDropHeight)
{
	// Create the combo box
	SetButtonInfo(nIndex, nID, TBBS_SEPARATOR, nWidth);

	CRect rect;
	GetItemRect(nIndex, &rect);
	rect.top = 1;
	rect.bottom = rect.top + nDropHeight;
	if (!comboBox.Create(
			CBS_DROPDOWNLIST|WS_VISIBLE|WS_TABSTOP|WS_VSCROLL,
			rect, this, nID))
	{
		TRACE("Failed to create combo-box\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CToolBarWithCombo::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if( wParam == IDOK && lParam == 0 )
	{
		CString strText;
		CString strCommand;
		CComboBox* rgComboBox[] = {&m_comboMatrix, &m_comboBox2};
		for( int index = 0; index < sizeof(rgComboBox) / sizeof(rgComboBox[0]); ++index )
		{
			if( rgComboBox[index]->GetEditSel() != 0 )
			{
				rgComboBox[index]->GetWindowText(strText);
				strCommand.Format(_T("Command from ComboBox[%d]: %s"), index+1, (LPCTSTR)strText);
				AfxMessageBox(strCommand);
				rgComboBox[index]->AddString(strText);
				rgComboBox[index]->SetWindowText(_T(""));
			}
		}
	}
	
	return CToolBar::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CSmartComboBox

CSmartComboBox::CSmartComboBox()
{
	m_lfHeight = -10;
	m_lfWeight = FW_NORMAL;
	m_strFaceName = _T("MS Sans Serif");
	m_nMaxStrings = 10;
}

BEGIN_MESSAGE_MAP(CSmartComboBox, CComboBox)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CSmartComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( !CreateFont(m_lfHeight, m_lfWeight, m_strFaceName) )
	{
		TRACE0("Failed to create font for combo box\n");
		return -1;      // fail to create
	}

	return 0;
}

BOOL CSmartComboBox::CreateFont(LONG lfHeight, LONG lfWeight, LPCTSTR lpszFaceName)
{
	//  Create a font for the combobox
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));

	if (!::GetSystemMetrics(SM_DBCSENABLED))
	{
		// Since design guide says toolbars are fixed height so is the font.
		logFont.lfHeight = lfHeight;
		logFont.lfWeight = lfWeight;
		CString strDefaultFont = lpszFaceName;
		lstrcpy(logFont.lfFaceName, strDefaultFont);
		if (!m_font.CreateFontIndirect(&logFont))
		{
			TRACE("Could Not create font for combo\n");
			return FALSE;
		}
		SetFont(&m_font);
	}
	else
	{
		m_font.Attach(::GetStockObject(SYSTEM_FONT));
		SetFont(&m_font);
	}
	return TRUE;
}

int CSmartComboBox::AddString(LPCTSTR str)
{
	if( _tcslen(str) == 0 )
		return -1;

	int oldIndex = FindStringExact(-1, str);
	if( oldIndex >= 0 )
		DeleteString(oldIndex);

	if( GetCount() == m_nMaxStrings )
		DeleteString(m_nMaxStrings-1);

	return CComboBox::InsertString(0, str);
}

int CSmartComboBox::InsertString(int index, LPCTSTR str)
{
	if( _tcslen(str) == 0 )
		return -1;

	int oldIndex = FindStringExact(-1, str);
	if( oldIndex >= 0 )
	{
		DeleteString(oldIndex);
		if( index >= oldIndex )
			--index;
	}

	if( GetCount() == m_nMaxStrings )
		DeleteString(m_nMaxStrings-1);

	return CComboBox::InsertString(index, str);
}

int	CSmartComboBox::SetCurSel(int nSelect)
{
	return CComboBox::SetCurSel(nSelect);
}

int	CSmartComboBox::GetCurSel() const
{
	return CComboBox::GetCurSel();
}

/////////////////////////////////////////////////////////////////////////////
