// ItemEditDlg.cpp : ���� ����
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


// CItemEditDlg ��ȭ ����



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


// CItemEditDlg �޽��� ó����

BOOL CItemEditDlg::OnInitDialog()
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

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CItemEditDlg::OnPaint() 
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
HCURSOR CItemEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CItemEditDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Item Setting File (*.isf)|*.isf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".isf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemEditDlg*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLItemMan::GetInstance().LoadFile ( dlg.GetFileName().GetString(), TRUE, TRUE );

		// ������ ��Ʈ�����̺��� �ε��Ѵ�. by ���
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
	
	//	Note : ���� ���� ���̾˷α׸� ����.
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	SendMessage ( WM_CLOSE );
}

void CItemEditDlg::OnMenuitemNew()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.	
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

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=ITEMSUITTREEPAGE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}

void CItemEditDlg::OnBnClickedButtonStrtbSync()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	GLItemMan::GetInstance().SyncStringTable();
	AfxMessageBox("Sync Completion!");
}

void CItemEditDlg::OnBnClickedButtonReloadStrtbl()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	// ������ ��Ʈ�����̺��� �ٽ� �ε��Ѵ�. by ���
	GLStringTable::GetInstance().CLEAR();
	GLStringTable::GetInstance().LOADFILE( GLItemMan::GetInstance()._STRINGTABLE, GLStringTable::ITEM );
}

void CItemEditDlg::OnBnClickedBtnCsvSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( GLItemMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CItemEditDlg::OnBnClickedBtnCsvLoad()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetDlgItem(IDC_ITEM_SEARCH_EDIT)->GetWindowText( szTempChar, 256 );

	if( strlen(szTempChar) == 0 ) 
	{
		MessageBox( "ã�� ���� ��������!!!!!", "�˸�" );
		return;
	}

	GetDlgItem(IDC_ITEM_SEARCH_BUTTON)->SetWindowText( "ã����" );

	int nSearchType = (int)GetDlgItem(IDC_ITEM_SEARCH_COMBO)->SendMessage( CB_GETCURSEL, 0, 0 );	
	m_pSheetTab->SearchItemSelect( nSearchType, szTempChar );
	m_wndSearchDlg.InsertReportItem( m_pSheetTab->GetSearchItemList() );

	GetDlgItem(IDC_ITEM_SEARCH_BUTTON)->SetWindowText( "ã��" );
}

void CItemEditDlg::OnBnClickedSearchReportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( m_wndSearchDlg.DoModal() == IDOK )
	{
		m_pSheetTab->SetSelectIndex( m_wndSearchDlg.GetSelectItem() );
	}
}

void CItemEditDlg::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	SetWin_ShowWindow( this, IDC_BTN_CSV_LOAD, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_UPDATE_SYNC, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_RELOAD_STRTBL, FALSE );
	SetWin_ShowWindow( this, IDC_BUTTON_STRTB_SYNC, FALSE );

	EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_NEW, FALSE );

#endif
}
