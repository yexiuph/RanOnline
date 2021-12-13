// MobEditDlg.cpp : implementation file
//

#include "pch.h"
#include "MobEdit.h"
#include "MobEditDlg.h"
#include "GLOGIC.h"
#include "DxResponseMan.h"
#include "RANPARAM.h"

#include "SheetWithTab.h"
#include "GLCrowData.h"
#include "DxServerInstance.h"

#include "SUBPATH.h"
#include "GLOGIC.h"
#include "GLCommentFile.h"

#include "GLStringTable.h"

#include ".\mobeditdlg.h"
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


// CMobEditDlg dialog



CMobEditDlg::CMobEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMobEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMobEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMobEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_SAVEOLD, OnBnClickedButtonSaveold)
	ON_BN_CLICKED(IDC_BUTTON_LOADCSV, OnBnClickedButtonLoadcsv)
	ON_BN_CLICKED(IDC_BUTTON_SAVECSV, OnBnClickedButtonSavecsv)
END_MESSAGE_MAP()


// CMobEditDlg message handlers

BOOL CMobEditDlg::OnInitDialog()
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
	GLCrowDataMan::GetInstance().OneTimeSceneInit();

	GMTOOL::Create(m_strAppPath.GetString());

	CRect SheetRect;
	CWnd *pWnd;

	////	
	////
	pWnd = GetDlgItem ( IDC_STATIC_BASE );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );


	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );


	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( CROWTREEPAGE );

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMobEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMobEditDlg::OnPaint() 
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
HCURSOR CMobEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMobEditDlg::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	CString szFilterInput = "MNSF|*.mnsf|";
	CFileDialog dlgInput(true,"*.*",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterInput,this);
	dlgInput.m_ofn.lpstrInitialDir = GLOGIC::GetPath();
	if ( dlgInput.DoModal() != IDOK ) return;

	GLStringTable::GetInstance().CLEAR();
	GLStringTable::GetInstance().LOADFILE( GLCrowDataMan::GetInstance()._STRINGTABLE, GLStringTable::CROW  );
	GLCrowDataMan::GetInstance().FinalCleanup (  );
	HRESULT hr = GLCrowDataMan::GetInstance().LoadFile ( dlgInput.GetFileName(),FALSE , TRUE );

	if ( SUCCEEDED(hr) )
	{
		SetWindowText( dlgInput.GetFileName().GetString() );
		MessageBox ( "Success to Load File." );
		m_pSheetTab->m_CrowTreePage.UpdateTree ();
	}
}

void CMobEditDlg::OnBnClickedButtonSave()
{
	DoSave();	
}

void CMobEditDlg::OnBnClickedButtonSaveold()
{
	/*
	if ( GLCrowDataMan::GetInstance().SaveCsvFile_old((CMobEditDlg*)this) == S_OK )
	{
		MessageBox ( "Save CSV Successful." );
	}
	*/
}

void CMobEditDlg::OnBnClickedButtonLoadcsv()
{
	if(  GLCrowDataMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
		m_pSheetTab->m_CrowTreePage.UpdateTree ();
	}
}

void CMobEditDlg::OnBnClickedButtonSavecsv()
{
	if ( GLCrowDataMan::GetInstance().SaveCsvFile((CMobEditDlg*)this) == S_OK )
	{
		MessageBox ( "Save CSV Successful." );
	}
}
bool CMobEditDlg::DoSave ()
{
	CString szFilter = "Mob Npc Setting File (*.mnsf)|*.mnsf|";
	
	CFileDialog dlg(FALSE,".mnsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CMobEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLCrowDataMan::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save Setting File" );
			return false;
		}

		if ( !GLStringTable::GetInstance().SAVEFILE( GLStringTable::CROW ) )
		{
			MessageBox ( "Failure to Save String File" );
			return false;
		}

		return true;
	}

	return false;
}

void CMobEditDlg::PostNcDestroy()
{
	//SAFE_DELETE ( m_pSheetTab );

	GLCrowDataMan::GetInstance().FinalCleanup();
	DxResponseMan::GetInstance().FinalCleanup ();
	GMTOOL::CleanUp ();

	CDialog::PostNcDestroy();
}