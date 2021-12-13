// QuestEditDlg.cpp : ���� ����
//

#include "pch.h"
#include <fstream>
#include "QuestEdit.h"
#include "QuestEditDlg.h"
#include "GLQuestMan.h"
#include "SheetWithTab.h"

#include "GLOGIC.h"
#include "DxResponseMan.h"
#include "RANPARAM.h"
#include ".\questeditdlg.h"
#include "DxServerInstance.h"
#include "StringFile.h"
#include "StringUtils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


GLQuestMan::SNODE CQuestEditDlg::m_sQuestNode;

GLMapList CQuestEditDlg::m_sMapList;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
public:
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


// CQuestEditDlg ��ȭ ����



CQuestEditDlg::CQuestEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuestEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQuestEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQuestEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_CSV_LO, OnMenuitemCsvLO)
	ON_COMMAND(ID_MENUITEM_CSV_SA, OnMenuitemCsvSA )
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENUITEM_TEXT_EXPORT, OnMenuitemTextExport)
	ON_COMMAND(ID_MENUITEM_TEXT_IMPORT, OnMenuitemTextImport)
END_MESSAGE_MAP()


// CQuestEditDlg �޽��� ó����

BOOL CQuestEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

		
	MakeToolBar ();

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

	m_sQuestNode.pQUEST = new GLQUEST;

	DxResponseMan::GetInstance().OneTimeSceneInit ( m_szAppPath, this,
		RANPARAM::strFontType, RANPARAM::dwLangSet, RANPARAM::strGDIFont );

  	//	Note : GLOGIC init
	GMTOOL::Create(m_strAppPath.GetString());
	
	m_sMapList.LoadMapList ( "mapslist.ini" );

	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 �� �����.
	//
	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );


	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );

	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( QUESTTREE_PAGE );

	SetReadMode();

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CQuestEditDlg::PostNcDestroy()
{
	SAFE_DELETE ( m_pSheetTab );
	SAFE_DELETE ( m_sQuestNode.pQUEST );

	GLQuestMan::GetInstance().Clean();
	DxResponseMan::GetInstance().FinalCleanup ();
	GMTOOL::CleanUp ();

	CDialog::PostNcDestroy();
}

void CQuestEditDlg::MakeToolBar ()
{
	if( !m_wndToolBar.Create(this) || 
        !m_wndToolBar.LoadToolBar(IDR_TOOLBAR1) )
    {
        TRACE("Failed to Create Toolbar\n");
        EndDialog(IDCANCEL);
    }

    CRect rcClientStart;
    CRect rcClientNow;
    GetClientRect(&rcClientStart);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 
                   0, reposQuery, rcClientNow);

    CPoint ptOffset(rcClientNow.left - rcClientStart.left,
                    rcClientNow.top  - rcClientStart.top);

    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);
    while( pwndChild )
    {
        pwndChild->GetWindowRect(rcChild);
        ScreenToClient(rcChild);
        rcChild.OffsetRect(ptOffset);
        pwndChild->MoveWindow(rcChild, FALSE);
        pwndChild = pwndChild->GetNextWindow();
    }

    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.right += rcClientStart.Width() - rcClientNow.Width();
    rcWindow.bottom += rcClientStart.Height() - rcClientNow.Height();

    MoveWindow(rcWindow, FALSE);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, 
                   AFX_IDW_CONTROLBAR_LAST, 0);
}

void CQuestEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CQuestEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CQuestEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQuestEditDlg::OnMenuitemNew()
{
	int nret = MessageBox ( "Press the 'YES' Button to Save.", "Caution", MB_YESNOCANCEL );
	switch ( nret )
	{
	case IDYES:
		if ( !DoSave () )	return;	//	���нÿ��� �������� ����.
		break;

	case IDNO:
		//	�������� �ʰ� ����.
		break;

	case IDCANCEL:
		//	��ҽÿ��� �������� ����.
		return;
	};

	SetWindowText( "QuestEdit" );

	//	Note : ���� ����.
	//
	GLQuestMan::GetInstance().Clean();

	m_pSheetTab->m_cQuestTreePage.CleanAllQuest ();
}

void CQuestEditDlg::OnMenuitemLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Quest List File (*.lst)|*.lst|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".qst",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,this);

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		GLQuestMan::GetInstance().Clean();
		hr = GLQuestMan::GetInstance().LoadFile ( dlg.GetFileName().GetString(), TRUE );


		if ( FAILED ( hr ) ) 
		{
			MessageBox ( "Failure to Load File." );
		}

		//	Note : ���� ����.
		//
		m_pSheetTab->m_cQuestTreePage.UpdateTree();
	}
}

bool CQuestEditDlg::DoSave ()
{
	CString szFilter = "Quest List File (*.lst)|*.lst|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".qst",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this);

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLQuestMan::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		return true;
	}

	return false;
}

void CQuestEditDlg::OnMenuitemSave()
{
	DoSave ();
}

void CQuestEditDlg::OnMenuitemExit()
{
	SendMessage ( WM_CLOSE );
}


void CQuestEditDlg::OnMenuitemCsvLO()
{
	CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
		return;

	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return;

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	CStringArray StrArrayProgress;

	CString strError;
	int MID = 0;

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	while( StrFile.GetNextLine( strLine ) ) // head
	{
		StrFile.GetNextLine( strLine ); // data

		STRUTIL::StringSeparate( strLine, StrArray );

		MID = atoi( StrArray[1] );

		
		iter_pos = m_mapQuestMap.find( MID );

		if ( iter_pos == m_mapQuestMap.end() ) 
		{
			strError.Format("%d ID not Found",MID);
			MessageBox(strError,_T("Error"), MB_OK );

			StrFile.GetNextLine( strLine ); // progress Head
			StrFile.GetNextLine( strLine ); // progress Data
			continue;
		}
		else
		{
			StrFile.GetNextLine( strLine ); // progress Head
			StrFile.GetNextLine( strLine ); // progress Data
			STRUTIL::StringSeparate( strLine, StrArrayProgress );

			sNode = (GLQuestMan::SNODE) iter_pos->second;
			sNode.pQUEST->LoadCsv( StrArray, StrArrayProgress );
			sNode.pQUEST->SAVE( sNode.strFILE.c_str() );
		}
	}

	MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );

	m_pSheetTab->UpdateQuestTreePage();
}


void CQuestEditDlg::OnMenuitemCsvSA()
{

	CFileDialog dlg( FALSE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"csv file (*.csv)|*.csv|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
	{
		return;
	}

	std::fstream streamFILE;
	streamFILE.open( dlg.GetPathName().GetString(), std::ios_base::out );


	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (GLQuestMan::SNODE) iter_pos->second;
		sNode.pQUEST->SaveCsv( streamFILE, sNode.strFILE.c_str() );
		streamFILE << std::endl;
	}

	MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CQuestEditDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
{
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(0);
	
	if ( bEnable )
	{
		submenu->EnableMenuItem ( uIDEnableItem, MF_BYCOMMAND | MF_ENABLED );
	}
	else
	{
		submenu->EnableMenuItem ( uIDEnableItem, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
}

void CQuestEditDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CQuestEditDlg::OnClose()
{
	//if ( GLQuestMan::GetInstance().IsModify() )
	{
		int nret = MessageBox ( "Press the 'YES' Button to save.", "Caution", MB_YESNOCANCEL );
		switch ( nret )
		{
		case IDYES:
			if ( !DoSave () )	return;	//	���нÿ��� �������� ����.
			break;

		case IDNO:
			//	�������� �ʰ� ����.
			break;

		case IDCANCEL:
			//	��ҽÿ��� �������� ����.
			return;
		};
	}

	DxResponseMan::GetInstance().FinalCleanup();

	CDialog::OnClose();
}

void CQuestEditDlg::OnMenuitemTextExport()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	
	CFileDialog dlg( FALSE, ".txt", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"text file (*.txt)|*.txt|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
	{
		return;
	}

	std::fstream streamFILE;
	streamFILE.open( dlg.GetPathName().GetString(), std::ios_base::out );


	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	for ( iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); iter_pos++ )
	{
		sNode = (GLQuestMan::SNODE) iter_pos->second;
		sNode.pQUEST->ExportText( streamFILE );
		streamFILE << std::endl;
	}

	MessageBox( _T("Text Export Success"), _T("Success"), MB_OK );

}

void CQuestEditDlg::OnMenuitemTextImport()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog dlg( TRUE, ".txt", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					"txt file (*.txt)|*.txt|", this );

	dlg.m_ofn.lpstrInitialDir = GLQuestMan::GetInstance().GetPath();
	if ( dlg.DoModal() != IDOK )
		return;

	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
		return;

	CString strLine;
	CString strFlag;
	CString strError;
	int MID = 0;

	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();
	GLQuestMan::MAPQUEST_ITER iter_pos;
	GLQuestMan::SNODE sNode;

	while( StrFile.GetNextLine( strLine ) ) // <begin>
	{
		if ( strLine != "<BEGIN>") continue;

		StrFile.GetNextLine( strLine ); // <QUEST_ID>

		MID = GetID( strLine );

		iter_pos = m_mapQuestMap.find( MID );

		if ( iter_pos == m_mapQuestMap.end() ) 
		{
			strError.Format("%d ID not Found",MID);
			MessageBox(strError,_T("Error"), MB_OK );

			continue;
		}
		else
		{
			sNode = (GLQuestMan::SNODE) iter_pos->second;
			sNode.pQUEST->ImportText( StrFile );
			sNode.pQUEST->SAVE( sNode.strFILE.c_str() );
		}
	}

	MessageBox( _T("Text Import Success"), _T("Success"), MB_OK );
	
	m_pSheetTab->UpdateQuestTreePage ();
}

int CQuestEditDlg::GetID( CString& strID )
{
	int begin=0;

	CString strTemp = strID.Tokenize("<>",begin);

	if ( strTemp != "QUEST_ID" ) return -1;

	strTemp = strID.Tokenize("<>",begin);

	return atoi( strTemp );
}

void CQuestEditDlg::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

    EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableMenu( ID_MENUITEM_CSV_LO, FALSE );
	
	EnableToobar( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_CSV_SA, FALSE );
	EnableToobar( ID_MENUITEM_CSV_LO, FALSE );

#endif

}