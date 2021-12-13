#pragma once

#include <string>
#include "assert.h"

//----------------------------------------------------------------------------------
void	SetWin_Num_int( CDialog *pDlg, int nID, int nValue );
int		GetWin_Num_int( CDialog *pDlg, int nID );

//----------------------------------------------------------------------------------
void	SetWin_Num_float( CDialog *pDlg, int nID, float fValue );
float	GetWin_Num_float( CDialog *pDlg, int nID );

//----------------------------------------------------------------------------------
void	SetWin_Text( CDialog *pDlg, int nID, const char *szText );
CString	GetWin_Text( CDialog *pDlg, int nID );

//----------------------------------------------------------------------------------
void	SetWin_Combo_Init( CDialog *pDlg, int nID, std::string *strTexts, int nNum );
void	SetWin_Combo_Del( CDialog *pDlg, int nID, const char *szTexts );
bool	SetWin_Combo_SelMfc( CDialog *pDlg, int nID, CString strText );
bool	SetWin_Combo_Sel( CDialog *pDlg, int nID, std::string strText );
void	SetWin_Combo_Sel( CDialog *pDlg, int nID, int nSelect );
DWORD	GetWin_Combo_Sel( CDialog *pDlg, int nID );
CString	GetWin_Combo_Sel_Text( CDialog *pDlg, int nID );
DWORD   GetWin_Combo_Sel( CDialog *pDlg, int nID, const char *szTexts );

//----------------------------------------------------------------------------------
void	SetWin_ListBox_Sel( CDialog *pDlg, int nID, CString strText );
int		GetWin_ListBox_Sel( CDialog *pDlg, int nID );
bool	GetWin_ListBox_Text( CDialog *pDlg, int nID, CString &strText );
void	SetWin_ListBox_Text( CDialog *pDlg, int nID, const CString strText );


//----------------------------------------------------------------------------------
void	SetWin_Check( CDialog *pDlg, int nID, BOOL bCheck );
BOOL	GetWin_Check( CDialog *pDlg, int nID );
void	GetWin_Check( CDialog *pDlg, int nID, DWORD &dwFlags, DWORD dwOnOffFlag );

//----------------------------------------------------------------------------------
void	SetCheck_Flags( BOOL bCheck, DWORD &dwFlags, DWORD dwOnOffFlag );
bool 	GetCheck_Flags( DWORD &dwFlags, DWORD dwOnOffFlag );

//----------------------------------------------------------------------------------
void	SetWin_Enable( CDialog *pDlg, int nID, BOOL bEnable );

void	SetWin_ShowWindow( CDialog* pDlg, int nID, int nCmdShow );
