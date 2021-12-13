#include "pch.h"
#include "EtcFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//----------------------------------------------------------------------------------
void SetWin_Num_int ( CDialog *pDlg, int nID, int nValue )
{
	CString	str;

	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	str.Format ( "%d", nValue );
	pWnd->SetWindowText ( str );
}

int GetWin_Num_int ( CDialog *pDlg, int nID )
{
	CString	str;

	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	pWnd->GetWindowText ( str );

	return atoi ( str.GetString() );
}

//----------------------------------------------------------------------------------
void SetWin_Num_float ( CDialog *pDlg, int nID, float fValue )
{
	CString	str;

	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	str.Format ( "%g", fValue );
	pWnd->SetWindowText ( str );
}

float GetWin_Num_float ( CDialog *pDlg, int nID )
{
	CString	str;

	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	pWnd->GetWindowText ( str );

	return static_cast<float> ( atof ( str.GetString() ) );
}

//----------------------------------------------------------------------------------
void SetWin_Text ( CDialog *pDlg, int nID, const char *szText )
{
	CString	str;

	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	str.Format ( "%s", szText );
	pWnd->SetWindowText ( str );
}

CString GetWin_Text ( CDialog *pDlg, int nID )
{
	CString	str;

	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	pWnd->GetWindowText ( str );

	return str;
}

//----------------------------------------------------------------------------------
void SetWin_Combo_Init ( CDialog *pDlg, int nID, std::string *strTexts, int nNum )
{
	CString	str;

	CComboBox *pCombo = (CComboBox*) ( pDlg->GetDlgItem(nID) );
	assert(pCombo&&"콘트롤이 존재하지 않습니다.");

	pCombo->ResetContent();
	int nIndex = 0;
	for ( int i=0; i<nNum; ++i )
	{
		nIndex = pCombo->AddString ( strTexts[i].c_str() );
		pCombo->SetItemData ( nIndex, i );
	}

	pCombo->SetCurSel ( 0 );
}

void SetWin_Combo_Del ( CDialog *pDlg, int nID, const char *szTexts )
{
	CString	str;

	CComboBox *pCombo = (CComboBox*) pDlg->GetDlgItem(nID);
	assert(pCombo&&"콘트롤이 존제하지 않습니다.");

	int nSize = pCombo->GetCount ();
	for ( int i=0; i<nSize; ++i )
	{
		pCombo->GetLBText ( i, str );

		if ( str==szTexts )
		{
			pCombo->DeleteString ( i );
			return;
		}
	}

	pCombo->SetCurSel ( LB_ERR );
}

bool SetWin_Combo_SelMfc ( CDialog *pDlg, int nID, CString strText )
{
	CString	str;

	CComboBox *pCombo = (CComboBox*) pDlg->GetDlgItem(nID);
	assert(pCombo&&"콘트롤이 존제하지 않습니다.");

	int nSize = pCombo->GetCount ();
	for ( int i=0; i<nSize; ++i )
	{
		pCombo->GetLBText ( i, str );

		if ( str==strText )
		{
			pCombo->SetCurSel ( i );
			return true;
		}
	}

	pCombo->SetCurSel ( LB_ERR );

	return false;
}

bool SetWin_Combo_Sel( CDialog *pDlg, int nID, std::string strText )
{	
	CString str;
	CString strTemp(strText.c_str());

	str.GetLength();

	CComboBox *pCombo = (CComboBox*) pDlg->GetDlgItem(nID);
	assert(pCombo&&"콘트롤이 존재하지 않습니다.");

	int nSize = pCombo->GetCount ();
	for ( int i=0; i<nSize; ++i )
	{
		pCombo->GetLBText ( i, str );

		CString strTemp2( str );

		if ( !strTemp2.CompareNoCase( strTemp.GetString() ) )
		{
			pCombo->SetCurSel( i );
			return true;
		}
	}

	pCombo->SetCurSel( LB_ERR );

	return false;
}

void SetWin_Combo_Sel ( CDialog *pDlg, int nID, int nSelect )
{
	CString	str;

	CComboBox *pCombo = (CComboBox*) pDlg->GetDlgItem(nID);
	assert(pCombo&&"콘트롤이 존제하지 않습니다.");

	int nSize = pCombo->GetCount ();
	if ( nSize < nSelect )		return;

	pCombo->SetCurSel ( nSelect );
}

DWORD GetWin_Combo_Sel ( CDialog *pDlg, int nID )
{
	CString	str;

	CComboBox *pCombo = (CComboBox*) ( pDlg->GetDlgItem(nID) );
	assert(pCombo&&"콘트롤이 존제하지 않습니다.");

	int nIndex = pCombo->GetCurSel ();
	assert(nIndex != LB_ERR);

	return (DWORD) pCombo->GetItemData ( nIndex );
}

DWORD GetWin_Combo_Sel ( CDialog *pDlg, int nID, const char *szTexts )
{
	CString	str;

	CComboBox *pCombo = (CComboBox*) pDlg->GetDlgItem(nID);
	assert(pCombo&&"콘트롤이 존제하지 않습니다.");

	int nSize = pCombo->GetCount ();
	for ( int i=0; i<nSize; ++i )
	{
		pCombo->GetLBText ( i, str );

		if ( str==szTexts )
		{			
			return pCombo->GetItemData ( i );
		}
	}

    return -1;
}

CString GetWin_Combo_Sel_Text ( CDialog *pDlg, int nID )
{
	CString str;

	CComboBox *pCombo = (CComboBox*) pDlg->GetDlgItem(nID);
	assert(pCombo&&"콘트롤이 존제하지 않습니다.");

	int nIndex = pCombo->GetCurSel ();

	pCombo->GetLBText ( nIndex, str );
	return str;
}

//----------------------------------------------------------------------------------
void SetWin_ListBox_Sel ( CDialog *pDlg, int nID, CString strText )
{
	CString	str;

	CListBox *pListBox = (CListBox*) pDlg->GetDlgItem(nID);
	assert(pListBox&&"콘트롤이 존제하지 않습니다.");

	int nSize = pListBox->GetCount ();
	for ( int i=0; i<nSize; ++i )
	{
		pListBox->GetText ( i, str );

		if ( str==strText )
		{
			pListBox->SetCurSel ( i );
			return;
		}
	}

	pListBox->SetCurSel ( LB_ERR );
}

int GetWin_ListBox_Sel( CDialog *pDlg, int nID )
{
	CListBox* pListBox = (CListBox*)( pDlg->GetDlgItem( nID ) );
	assert( pListBox&&"콘트롤이 존제하지 않습니다." );

	return pListBox->GetCurSel();
}

bool GetWin_ListBox_Text( CDialog *pDlg, int nID, CString &strText )
{
	CListBox* pListBox = (CListBox*)( pDlg->GetDlgItem( nID ) );
	assert( pListBox&&"콘트롤이 존제하지 않습니다." );

	int iCurSel = GetWin_ListBox_Sel( pDlg, nID );
	if( iCurSel == LB_ERR )
		return false;

	pListBox->GetText( iCurSel, strText );

	return true;
}

//----------------------------------------------------------------------------------
void SetWin_ListBox_Text( CDialog *pDlg, int nID, const CString strText )
{
	CString	str;

	CListBox *pListBox = (CListBox*) pDlg->GetDlgItem(nID);
	assert(pListBox&&"콘트롤이 존제하지 않습니다.");

	pListBox->AddString( strText.GetString() );
	int n = pListBox->GetCount();
	pListBox->SetCurSel( n );
	
}

//----------------------------------------------------------------------------------
void SetWin_Check ( CDialog *pDlg, int nID, BOOL bCheck )
{
	CButton *pButton = (CButton*) ( pDlg->GetDlgItem(nID) );
	assert(pButton&&"콘트롤이 존제하지 않습니다.");

	pButton->SetCheck ( bCheck );
}

BOOL GetWin_Check ( CDialog *pDlg, int nID )
{
	CButton *pButton = (CButton*) ( pDlg->GetDlgItem(nID) );
	assert(pButton&&"콘트롤이 존제하지 않습니다.");

	return pButton->GetCheck ();
}

void GetWin_Check ( CDialog *pDlg, int nID, DWORD &dwFlags, DWORD dwOnOffFlag )
{
	SetCheck_Flags ( GetWin_Check ( pDlg, nID), dwFlags, dwOnOffFlag );
}

//----------------------------------------------------------------------------------
void SetCheck_Flags ( BOOL bCheck, DWORD &dwFlags, DWORD dwOnOffFlag )
{
	if ( bCheck )	dwFlags |= dwOnOffFlag;
	else			dwFlags &= ~dwOnOffFlag;
}

bool GetCheck_Flags( DWORD &dwFlags, DWORD dwOnOffFlag )
{
	return ( dwFlags & dwOnOffFlag );
}

//----------------------------------------------------------------------------------
void SetWin_Enable ( CDialog *pDlg, int nID, BOOL bEnable )
{
	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	pWnd->EnableWindow(bEnable);
}

void SetWin_ShowWindow( CDialog* pDlg, int nID, int nCmdShow )
{
	CWnd *pWnd = pDlg->GetDlgItem(nID);
	assert(pWnd&&"콘트롤이 존제하지 않습니다.");

	pWnd->ShowWindow(nCmdShow);
}