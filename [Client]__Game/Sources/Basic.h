// Basic.h : main header file for the BASIC application
//

#if !defined(AFX_BASIC_H__9FF379E7_FAE2_11D1_BFC5_D41F722B624A__INCLUDED_)
#define AFX_BASIC_H__9FF379E7_FAE2_11D1_BFC5_D41F722B624A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#if !defined(MY_PARAM) && !defined(MYE_PARAM) && !defined(PH_PARAM) //&& !defined( VN_PARAM ) //&& !defined(TH_PARAM) && !defined ( HK_PARAM ) && !defined(TW_PARAM) // Nprotect Á¤ÀÇ
#define NO_GAMEGUARD
#endif

#ifdef _RELEASED
#define NO_GAMEGUARD
#endif

#if !defined( TH_PARAM ) 
#include "../Dependency/common/NPGameLib.h"
#endif

extern CNPGameLib npgl;

class CBasicWnd;

/////////////////////////////////////////////////////////////////////////////
// CBasicApp:
// See Basic.cpp for the implementation of this class
//
class CBasicApp : public CWinApp
{
	void ErrorMsgByNP( DWORD dwResult );

public:
	CBasicApp();
	~CBasicApp();

protected:
	CBasicWnd*	m_pWnd;
	BOOL		m_bIsActive;    
	BOOL		m_bRedrawScreen;
	BOOL		m_bShutDown;

public:
	void SetActive(BOOL bActive);
	void ShutDown() { m_bShutDown = TRUE; };

	void SetAppPath();

public:
	CString m_strAppPath;
	TCHAR	m_szAppPath[MAX_PATH];

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CBasicApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASIC_H__9FF379E7_FAE2_11D1_BFC5_D41F722B624A__INCLUDED_)
