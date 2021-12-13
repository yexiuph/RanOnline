// QuestEditDlg.cpp : 구현 파일
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

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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


// CQuestEditDlg 대화 상자



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


// CQuestEditDlg 메시지 처리기

BOOL CQuestEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

		
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

	//	Note : Sheet 1 를 만든다.
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

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CQuestEditDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
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
		if ( !DoSave () )	return;	//	실패시에는 종료하지 않음.
		break;

	case IDNO:
		//	저장하지 않고 종료.
		break;

	case IDCANCEL:
		//	취소시에는 종료하지 않음.
		return;
	};

	SetWindowText( "QuestEdit" );

	//	Note : 정보 갱신.
	//
	GLQuestMan::GetInstance().Clean();

	m_pSheetTab->m_cQuestTreePage.CleanAllQuest ();
}

void CQuestEditDlg::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Quest List File (*.lst)|*.lst|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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

		//	Note : 정보 갱신.
		//
		m_pSheetTab->m_cQuestTreePage.UpdateTree();
	}
}

bool CQuestEditDlg::DoSave ()
{
	CString szFilter = "Quest List File (*.lst)|*.lst|";

	//	Note : 파일 오픈 다이알로그를 만듬.
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
			if ( !DoSave () )	return;	//	실패시에는 종료하지 않음.
			break;

		case IDNO:
			//	저장하지 않고 종료.
			break;

		case IDCANCEL:
			//	취소시에는 종료하지 않음.
			return;
		};
	}

	DxResponseMan::GetInstance().FinalCleanup();

	CDialog::OnClose();
}

void CQuestEditDlg::OnMenuitemTextExport()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
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
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

    EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableMenu( ID_MENUITEM_CSV_LO, FALSE );
	
	EnableToobar( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_CSV_SA, FALSE );
	EnableToobar( ID_MENUITEM_CSV_LO, FALSE );

#endif

}