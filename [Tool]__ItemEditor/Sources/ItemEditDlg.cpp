// ItemEditDlg.cpp : 구현 파일
//

#include "pch.h"
#include <vector>
#include "ItemEdit.h"
#include "GLOGIC.h"
#include "DxResponseMan.h"
#include "RANPARAM.h"

#include "SheetWithTab.h"
#include "GLItemMan.h"
#include "DxServerInstance.h"

#include "LogDialog.h"
#include "GLItem.h"

#include "GLStringTable.h"
#include "SearchDialog.h"
#include ".\itemeditdlg.h"
#include "EtcFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CItemEditDlg 대화 상자



CItemEditDlg::CItemEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CItemEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
}

BEGIN_MESSAGE_MAP(CItemEditDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_EXIT, OnMenuitemExit)
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SYNC, OnBnClickedButtonUpdateSync)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_STRTB_SYNC, OnBnClickedButtonStrtbSync)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_STRTBL, OnBnClickedButtonReloadStrtbl)
	ON_BN_CLICKED(IDC_BTN_CSV_SAVE, OnBnClickedBtnCsvSave)
	ON_BN_CLICKED(IDC_BTN_CSV_LOAD, OnBnClickedBtnCsvLoad)
	ON_BN_CLICKED(IDC_ITEM_SEARCH_BUTTON, OnBnClickedItemSearchButton)
	ON_BN_CLICKED(IDC_SEARCH_REPORT_BUTTON, OnBnClickedSearchReportButton)
END_MESSAGE_MAP()


// CItemEditDlg 메시지 처리기

BOOL CItemEditDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

	DxResponseMan::GetInstance().OneTimeSceneInit ( m_szAppPath, this, RANPARAM::strFontType, RANPARAM::dwLangSet, RANPARAM::strGDIFont );
	GLItemMan::GetInstance().OneTimeSceneInit();

	GMTOOL::Create ( m_szAppPath );

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
	
	m_pSheetTab->SetActivePage ( ITEMSUITTREEPAGE );

	CWnd *pComobWnd = GetDlgItem(IDC_ITEM_SEARCH_COMBO);
	char szTempChar[MAX_PATH] = {0,};

	sprintf_s( szTempChar, "MID/SID" );
	pComobWnd->SendMessage( CB_ADDSTRING, 0, (long)szTempChar );
	sprintf_s( szTempChar, "Item Name" );
	pComobWnd->SendMessage( CB_ADDSTRING, 0, (long)szTempChar );
	sprintf_s( szTempChar, "USE DDS" );
	pComobWnd->SendMessage( CB_ADDSTRING, 0, (long)szTempChar );
	sprintf_s( szTempChar, "USE CPS" );
	pComobWnd->SendMessage( CB_ADDSTRING, 0, (long)szTempChar );
	sprintf_s( szTempChar, "ITEM TYPE" );
	pComobWnd->SendMessage( CB_ADDSTRING, 0, (long)szTempChar );

	pComobWnd->SendMessage( CB_SETCURSEL, 0, 0 );	

	SetReadMode();

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CItemEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CItemEditDlg::OnPaint() 
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
HCURSOR CItemEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CItemEditDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	bool bValid = GLItemMan::GetInstance().ValidData ();
	if ( bValid )	GLItemMan::GetInstance().SaveFile ( "lastedit.isf.bak" );

	SAFE_DELETE ( m_pSheetTab );

	GLItemMan::GetInstance().FinalCleanup ();
	DxResponseMan::GetInstance().FinalCleanup ();
	GMTOOL::CleanUp ();


	CDialog::PostNcDestroy();
}

void CItemEditDlg::OnMenuitemLoad()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLItemMan::GetInstance().LoadFile ( dlg.GetFileName().GetString(), TRUE, TRUE );

		// 아이템 스트링테이블을 로드한다. by 경대
		GLStringTable::GetInstance().CLEAR();
		if( !GLStringTable::GetInstance().LOADFILE( GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM ) )
		{
			MessageBox ( "Failure to load file." );
		}


		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to load file." );
		}
        
		m_pSheetTab->m_ItemSuitTreePage.UpdateTree ();
	}
}

bool CItemEditDlg::DoSave ()
{
	CString szFilter = "Item Setting File (*.isf)|*.isf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLItemMan::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		if ( !GLStringTable::GetInstance().SAVEFILE( GLStringTable::ITEM ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		return true;
	}

	return false;
}

void CItemEditDlg::OnMenuitemSave()
{
	DoSave ();
}

void CItemEditDlg::OnMenuitemExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SendMessage ( WM_CLOSE );
}

void CItemEditDlg::OnMenuitemNew()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.	
	GLItemMan::GetInstance().FinalCleanup ();
	GLItemMan::GetInstance().OneTimeSceneInit();

	m_pSheetTab->m_ItemSuitTreePage.CleanAllItem ();
}

void CItemEditDlg::MakeToolBar ()
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

void CItemEditDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
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

void CItemEditDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CItemEditDlg::OnBnClickedButtonUpdateSync()
{
	GLItemMan::GetInstance().SyncUpdateData ();

	CLogDialog LogDlg;
	LogDlg.SetLogFile ( GLItemMan::_LOGFILE );
	LogDlg.DoModal ();
}

void CItemEditDlg::OnClose()
{
	if ( GLItemMan::GetInstance().IsModify() )
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
	}

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=ITEMSUITTREEPAGE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}

void CItemEditDlg::OnBnClickedButtonStrtbSync()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GLItemMan::GetInstance().SyncStringTable();
	AfxMessageBox("Sync Completion!");
}

void CItemEditDlg::OnBnClickedButtonReloadStrtbl()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	// 아이템 스트링테이블을 다시 로드한다. by 경대
	GLStringTable::GetInstance().CLEAR();
	GLStringTable::GetInstance().LOADFILE( GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM );
}

void CItemEditDlg::OnBnClickedBtnCsvSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GLItemMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CItemEditDlg::OnBnClickedBtnCsvLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GLItemMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		if( m_pSheetTab )
			m_pSheetTab->UpdateSuitTreePage();
		OnBnClickedButtonReloadStrtbl();
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CItemEditDlg::OnBnClickedItemSearchButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetDlgItem(IDC_ITEM_SEARCH_EDIT)->GetWindowText( szTempChar, 256 );

	if( strlen(szTempChar) == 0 ) 
	{
		MessageBox( "찾을 값을 넣으세요!!!!!", "알림" );
		return;
	}

	GetDlgItem(IDC_ITEM_SEARCH_BUTTON)->SetWindowText( "찾는중" );

	int nSearchType = (int)GetDlgItem(IDC_ITEM_SEARCH_COMBO)->SendMessage( CB_GETCURSEL, 0, 0 );	
	m_pSheetTab->SearchItemSelect( nSearchType, szTempChar );
	m_wndSearchDlg.InsertReportItem( m_pSheetTab->GetSearchItemList() );

	GetDlgItem(IDC_ITEM_SEARCH_BUTTON)->SetWindowText( "찾기" );
}

void CItemEditDlg::OnBnClickedSearchReportButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_wndSearchDlg.DoModal() == IDOK )
	{
		m_pSheetTab->SetSelectIndex( m_wndSearchDlg.GetSelectItem() );
	}
}

void CItemEditDlg::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	SetWin_ShowWindow( this, IDC_BTN_CSV_LOAD, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_UPDATE_SYNC, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_RELOAD_STRTBL, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_STRTB_SYNC, FALSE );

	EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_NEW, FALSE );

#endif
}
