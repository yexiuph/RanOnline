// SkillEditorDlg.cpp : implementation file
//

#include "pch.h"
#include <vector>
#include "SkillEditor.h"
#include "GLOGIC.h"
#include "DxResponseMan.h"
#include "RANPARAM.h"

#include "SheetWithTab.h"
#include "GLSkill.h"
#include "DxServerInstance.h"

#include "SUBPATH.h"
#include "GLCommentFile.h"




#include "GLStringTable.h"

#include ".\skilleditordlg.h"
#include "EtcFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSkillEditorDlg dialog



CSkillEditorDlg::CSkillEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkillEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSkillEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSkillEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOADSKILL, OnBnClickedButtonLoadskill)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOADCSV, OnBnClickedButtonLoadcsv)
	ON_BN_CLICKED(IDC_BUTTON_SAVECSV, OnBnClickedButtonSavecsv)
	ON_BN_CLICKED(IDC_BUTTON_SAVE2, OnBnClickedButtonSave2)
	ON_BN_CLICKED(IDC_BUTTON_SAVE3, OnBnClickedButtonSave3)
	ON_BN_CLICKED(IDC_BUTTON_LOADCSV2, OnBnClickedButtonLoadcsv2)
END_MESSAGE_MAP()


// CSkillEditorDlg message handlers

BOOL CSkillEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );
			
			if ( !m_strAppPath.IsEmpty() )
			if ( m_strAppPath.GetAt(0) == '"' )
				m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

			strcpy_s ( m_szAppPath, m_strAppPath.GetString() );
		}
	}

	DxResponseMan::GetInstance().OneTimeSceneInit ( m_szAppPath, this, RANPARAM::strFontType, RANPARAM::dwLangSet, RANPARAM::strGDIFont );
	//GLSkillMan::GetInstance().FinalCleanup();
	GLSkillMan::GetInstance().OneTimeSceneInit();

	GMTOOL::Create ( m_szAppPath ); //this cause long loading


	CRect SheetRect;
	CWnd *pWnd;

	////	
	////
	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );


	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );


	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( SKILLTREEPAGE );



	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSkillEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSkillEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSkillEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSkillEditorDlg::OnBnClickedButtonLoadskill()
{
	CString szFilter = "Skill Setting File (*.ssf)|*.ssf|";
	
	CFileDialog dlg(TRUE,".ssf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillEditorDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		GLSkillMan::GetInstance().FinalCleanup();

		HRESULT hr;
		hr = GLSkillMan::GetInstance().LoadFile ( dlg.GetFileName().GetString(), TRUE );

		GLStringTable::GetInstance().CLEAR();
		if( !GLStringTable::GetInstance().LOADFILE( GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL ) )
		{
			MessageBox ( "Failure to load String file." );
		}

		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to load Skill Setting file." );
		}
        
		m_pSheetTab->m_SkillEditorTreePage.UpdateTree ();
	}
}

void CSkillEditorDlg::OnBnClickedButtonSave()
{
	DoSave();
}

void CSkillEditorDlg::OnBnClickedButtonLoadcsv()
{
	if( GLSkillMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		if( m_pSheetTab )
			m_pSheetTab->UpdateSkillTreePage();
		OnBnClickedButtonReloadStrtbl();
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CSkillEditorDlg::OnBnClickedButtonSavecsv()
{
	if( GLSkillMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CSkillEditorDlg::OnBnClickedButtonReloadStrtbl()
{
	GLStringTable::GetInstance().CLEAR();
	GLStringTable::GetInstance().LOADFILE( GLSkillMan::GetInstance()._STRINGTABLE, GLStringTable::SKILL );
}
bool CSkillEditorDlg::DoSave ()
{
	CString szFilter = "Skill Setting File (*.ssf)|*.ssf|";
	
	CFileDialog dlg(FALSE,".ssf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CSkillEditorDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLSkillMan::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save Skill Setting File" );
			return false;
		}

		if ( !GLStringTable::GetInstance().SAVEFILE( GLStringTable::SKILL ) )
		{
			MessageBox ( "Failure to Save String File" );
			return false;
		}

		return true;
	}

	return false;
}
void CSkillEditorDlg::PostNcDestroy() //this crash every exit dont know why
{
	// cleanup
	//bool bValid = GLSkillMan::GetInstance().ValidData ();
	//if ( bValid )	GLSkillMan::GetInstance().SaveFile ( "lastedit.ssf.bak" );

	m_pSheetTab->ClearData();

	//SAFE_DELETE
	//SAFE_DELETE ( m_pSheetTab );

	GLSkillMan::GetInstance().FinalCleanup ();
	DxResponseMan::GetInstance().FinalCleanup ();
	GMTOOL::CleanUp ();
	CDialog::PostNcDestroy();
}


void CSkillEditorDlg::OnBnClickedButtonSave2()
{
	/*
	if( GLSkillMan::GetInstance().SaveCsvFile_old( (CWnd*)this ) == S_OK )
		MessageBox( _T("Saved As Old format"), _T("Success"), MB_OK );
		*/
}


void CSkillEditorDlg::OnBnClickedButtonSave3()
{
	/*
	if( GLSkillMan::GetInstance().SaveCsvFile_oldj( (CWnd*)this ) == S_OK )
		MessageBox( _T("Saved As Old format (J)"), _T("Success"), MB_OK );
		*/
}

void CSkillEditorDlg::OnBnClickedButtonLoadcsv2()
{
	/*
	if( GLSkillMan::GetInstance().LoadCsvFile_oldj( (CWnd*)this ) == S_OK )
	{
		if( m_pSheetTab )
			m_pSheetTab->UpdateSkillTreePage();
		OnBnClickedButtonReloadStrtbl();
		MessageBox( _T("CSV Old Format Load Success (J)"), _T("Success"), MB_OK );
	}
	*/
}
