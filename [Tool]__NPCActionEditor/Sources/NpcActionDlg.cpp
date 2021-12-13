// NpcActionDlg.cpp : ���� ����
//

#include "pch.h"
#include "NpcAction.h"
#include "NpcActionDlg.h"
#include "DxServerInstance.h"

#include "SheetWithTab.h"
#include "npcactiondlg.h"
#include "NpcDialogueSet.h"

#include "SimpleEdit.h"

#include "StringUtils.h"
#include "StringFile.h"

#include ".\npcactiondlg.h"
#include "GLCrowData.h"
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


// CNpcActionDlg ��ȭ ����



CNpcActionDlg::CNpcActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcActionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNpcActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNpcActionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_NEW, OnMenuitemNew)
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_FILE_ALLTEXTEXPORT, OnMenuAllTextExport )
	ON_COMMAND(ID_FILE_ALLTEXTEXPORT2, OnMenuAllTextExport2 )
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CSV_SAVE_BUTTON, OnBnClickedCsvSaveButton)
	ON_BN_CLICKED(IDC_TEXTEXPORT_BUTTON, OnBnClickedTextexportButton)
	ON_BN_CLICKED(IDC_TEXTIMPORT_BUTTON, OnBnClickedTextimportButton)
	ON_BN_CLICKED(IDC_CSVEXPORT_BUTTON, OnBnClickedCsvexportButton)
	ON_BN_CLICKED(IDC_CSVIMPORT_BUTTON, OnBnClickedCsvimportButton)
	ON_BN_CLICKED(IDC_ALLSAVE_BUTTON, OnBnClickedAllSaveButton)
	ON_BN_CLICKED(IDC_CSVEXPORT_BUTTON2, OnBnClickedCsvexportButton2)
	ON_BN_CLICKED(IDC_CSVIMPORT_BUTTON2, OnBnClickedCsvimportButton2)
END_MESSAGE_MAP()


// CNpcActionDlg �޽��� ó����

BOOL CNpcActionDlg::OnInitDialog()
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

	// Note : ���������� ��θ� ã�Ƽ� �����Ѵ�. - �ݵ�� CBasicWnd(CD3dApp) ������ �����ؾ� �Ѵ�.
	//
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

  	//	Note : GLOGIC init
	GMTOOL::Create(m_strAppPath.GetString());

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	MakeToolBar ();

	CRect SheetRect;
	CWnd* pWnd = NULL;

	//	Note : Sheet 1 �� �����.
	//

	SetReadMode();

	pWnd = GetDlgItem ( IDC_STATIC_CONTROL );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	LOGFONT logfont = {0};

	CFont* pFont = GetFont();
	if ( pFont ) pFont->GetLogFont( &logfont );

	m_pSheetTab = new CsheetWithTab ( SheetRect, logfont, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	
	m_pSheetTab->SetActivePage ( NPCACTIONTABLE );
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CNpcActionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CNpcActionDlg::OnPaint() 
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
HCURSOR CNpcActionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNpcActionDlg::MakeToolBar ()
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

void CNpcActionDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	SAFE_DELETE ( m_pSheetTab );
	GMTOOL::CleanUp();

	CDialog::PostNcDestroy();
}

void CNpcActionDlg::OnMenuitemNew()
{
	m_pSheetTab->GetDialogueSet()->DoModified();

	SetWindowText( "NpcAction" );

	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->GetDialogueSet()->RemoveAllDialogue ();
	m_pSheetTab->m_NpcActionTable.UpdateItems ();
}

void CNpcActionDlg::OnMenuitemLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "NPC Conversation File (*.ntk)|*.ntk|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".ntk",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNpcActionDlg*)this);

	dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		BOOL bResult = m_pSheetTab->GetDialogueSet()->Load ( dlg.GetFileName().GetString() );
		if( bResult )
		{
			SetWindowText( dlg.GetFileName().GetString() );
		}
		else
		{
			MessageBox ( "Failure to Load File." );
		}

		m_pSheetTab->GetDialogueSet()->DoModified();
		m_pSheetTab->m_NpcActionTable.UpdateItems ();
	}
}

bool CNpcActionDlg::DoSave ()
{
	CString szFilter = "NPC Conversation File (*.ntk)|*.ntk|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".ntk",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNpcActionDlg*)this);

	dlg.m_ofn.lpstrInitialDir = CNpcDialogueSet::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		BOOL bResult = m_pSheetTab->GetDialogueSet()->Save ( dlg.GetFileName().GetString() );
		if( bResult )
		{
			SetWindowText( dlg.GetFileName().GetString() );
		}
		else
		{
			MessageBox ( "Failure to Save File." );
			return false;
		}

		return true;
	}

	return false;
}

void CNpcActionDlg::OnMenuitemSave()
{
	DoSave ();
	
//	// npctalk test
//	AllSave();
}

void CNpcActionDlg::EnableMenu ( UINT uIDEnableItem, BOOL bEnable )
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

void CNpcActionDlg::EnableToobar ( UINT uIDEnableItem, BOOL bEnable )
{
	m_wndToolBar.GetToolBarCtrl().EnableButton ( uIDEnableItem, bEnable );
}

void CNpcActionDlg::OnClose()
{
	if ( m_pSheetTab->GetDialogueSet()->IsModify() )
	{
		int nret = MessageBox ( "Press the 'Yes' button to save.", "Caution", MB_YESNOCANCEL );
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

	if ( m_pSheetTab && m_pSheetTab->GetActiveIndex()!=NPCACTIONTABLE ) 
	{
		MessageBox ( "Close the active window before quit.", "Caution", MB_OK );
		return;
	}

	CDialog::OnClose();
}
void CNpcActionDlg::OnBnClickedCsvSaveButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//BOOL bResult = m_pSheetTab->GetDialogueSet()->SimpleEdit( (CWnd*)this );
	CSimpleEdit dlg;
	dlg.SetNpcDialogueSet( m_pSheetTab->GetDialogueSet() );
	dlg.DoModal();

	m_pSheetTab->GetDialogueSet()->DoModified();
	m_pSheetTab->m_NpcActionTable.UpdateItems ();
}

void CNpcActionDlg::OnBnClickedTextexportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );
	GetWindowText( szTempChar, 1024 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, szTempChar, 1024, NULL, 0 );
	strcat_s( szTempChar, ".ntt" );	


	FILE *pFile;
	fopen_s(&pFile, szTempChar, "wt");
	if( pFile == NULL )
	{
		MessageBox( "���� ���� ����. Text������ ���������� �ݾ��ּ���" );
		return;
	}

	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();
	// ��ü NPC �ؽ�Ʈ
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = pDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_IT found;

	UINT i;
	// ��ü ����Ʈ
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;

		fprintf( pFile, "<START>\n" );
		sprintf_s( szTempChar, "<ID> %d\n\n", pNpcDialogue->GetDlgNID() );
		fprintf( pFile, szTempChar );

		// �ؽ�Ʈ ����Ʈ
		for( i = 0; i < pNpcDialogue->GetDlgCaseVec().size(); i++ )
		{
			sprintf_s( szTempChar, "<TEXT> %d\n", i + 1 );
			fprintf( pFile, szTempChar );
			sprintf_s( szTempChar, "%s\r\n", pNpcDialogue->GetDlgCase( i )->GetBasicTalk().c_str() );
			fprintf( pFile, szTempChar );
			fprintf( pFile, "</TEXT>\n\n" );

			// ��� ����Ʈ
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			fprintf( pFile, "<ANSWER>\n" );
			CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				SNpcTalk* pNpcTalk = (*iter).second;
				sprintf_s( szTempChar, "%s\n", pNpcTalk->GetTalk() );
				fprintf( pFile, szTempChar );
			}
			fprintf( pFile, "</ANSWER>\n\n" );
		}

		fprintf( pFile, "<END>\n\n\n" );
	}

	fclose( pFile );
}


void CNpcActionDlg::OnBnClickedTextimportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char szSectionChar[1024];
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );
	ZeroMemory( szSectionChar, 1024 );
	GetWindowText( szTempChar, 1024 );

	_splitpath_s( szTempChar, NULL, 0, NULL, 0, NULL, 0, szSectionChar, 1024 );
	if( strlen( szSectionChar ) == 0 )
	{
		MessageBox( "NTK ������ �ε��ϼ���." );
		return;
	}

	_splitpath_s( szTempChar, NULL, 0, NULL, 0, szTempChar, 1024, NULL, 0 );
	strcat_s( szTempChar, ".ntt" );

	FILE *pFile;
	fopen_s(&pFile, szTempChar, "rt");
	if( pFile == NULL )
	{
		MessageBox( "�ͽ���Ʈ�� �ؽ�Ʈ �����Ͱ� �����ϴ�." );
		return;
	}

	int iNpcNumber = -1;
	int iTextNumber = -1;
	int iAnswerNumber = -1;
	bool bAddText = FALSE;
	bool bAddAnswer = FALSE;



	std::vector<SNpcTextData> npcTextData;
	std::vector<SAnswerTextData> npcAnswerTextData;
	// �ε��� ������ ����
	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();

	
	char szTempAddChar[1024];
	ZeroMemory( szTempAddChar, 1024 );
	int iLineNumber = 0;

	bool bEndText = TRUE;
	while(!feof(pFile))
	{
		ZeroMemory( szSectionChar, 1024 );
		ZeroMemory( szTempChar, 1024 );

		fgets( szTempChar, 1024, pFile );
		sscanf( szTempChar, "%s", szSectionChar );

		iLineNumber++;

		if( _stricmp( szSectionChar, "//" ) == 0 ) continue;

		if( bAddAnswer == FALSE && _stricmp( szSectionChar, "</ANSWER>" ) == 0 )
		{
			sprintf_s( szTempChar, "<ANSWER>�� �����ϴ�.\n %d��° ��", iLineNumber );
			MessageBox( szTempChar, "����" );
			fclose( pFile );
			return;
		}

		if( bAddText == FALSE && _stricmp( szSectionChar, "</TEXT>" ) == 0 )
		{
			sprintf_s( szTempChar, "<TEXT>�� �����ϴ�.\n %d��° ��", iLineNumber );
			MessageBox( szTempChar, "����" );
			fclose( pFile );
			return;
		}

		if( bAddText )
		{
			if( _stricmp( szSectionChar, "<ANSWER>" ) == 0 )
			{
				sprintf_s( szTempChar, "</TEXT>�� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}


			if( _stricmp( szSectionChar, "</TEXT>" ) == 0 )
			{
				bAddText = FALSE;
				char szTemp[1024];
				ZeroMemory( szTemp, 1024 );
				strncpy_s( szTemp, szTempAddChar, strlen(szTempAddChar) );

				SNpcTextData inputData;
				inputData.dwNpcNumber  = iNpcNumber;
				inputData.TalkText     = szTemp;
				inputData.dwTextNumber = iTextNumber;
				npcTextData.push_back( inputData );
				continue;
			}
			
			if( strlen( szSectionChar ) == 0 )
			{
				strcat_s( szTempAddChar, "\r\n" );
			}else{		
				if( strlen( szTempAddChar ) != 0 )
				{
					strcat_s( szTempAddChar, "\r\n" );
				}
				if( strstr( szTempChar, "\r\n" ) != NULL )
				{
					strncat_s( szTempAddChar, szTempChar, strlen(szTempChar) - 2 );
				}else{
					strncat_s( szTempAddChar, szTempChar, strlen(szTempChar) - 1 );
				}
				
			}
			continue;
		}

		if( bAddAnswer )
		{
			if( _stricmp( szSectionChar, "<TEXT>" ) == 0 )
			{
				sprintf_s( szTempChar, "</ANSWER>�� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}

			if( _stricmp( szSectionChar, "</ANSWER>" ) == 0 )
			{
				bAddAnswer = FALSE;
				if( iAnswerNumber != pDialogueSet->GetDialogue2(iNpcNumber)->GetDlgCase(iTextNumber)->GetTalkControl()->GetTalkNum() )
				{
					sprintf_s( szTempChar, "�����Ϳ� �ִ� Answer �������� Import�� Answer ������ �ٸ��ϴ�.\n %d��° ��", iLineNumber );
					MessageBox( szTempChar, "����" );
					fclose( pFile );
					return;
				}

				iAnswerNumber = 0;
				continue;
			}
			ZeroMemory( szTempAddChar, 1024 );
			strncpy_s( szTempAddChar, szTempChar, strlen(szTempChar)-1 );
			SAnswerTextData inputAnswerData;
			inputAnswerData.dwNpcNumber = iNpcNumber;
			inputAnswerData.dwTextNumber = iTextNumber; 
			inputAnswerData.dwAnswerNumber = iAnswerNumber;
			inputAnswerData.answerTalkText	= szTempAddChar;
			npcAnswerTextData.push_back( inputAnswerData );

			iAnswerNumber++;
		}


		// ��ü NPC ��ȭ ���� ����
		if( _stricmp( szSectionChar, "<START>" ) == 0 )
		{
			if( bEndText )
			{
				iNpcNumber++;
				iTextNumber = -1;
				bEndText = FALSE;
				if( iNpcNumber >= (int)pDialogueSet->GetDialogueMap()->size() )
				{
					sprintf_s( szTempChar, "�����Ϳ� �ִ� NPC ��ȭ �������� Import�� NPC ��ȭ ������ �� �����ϴ�.\n %d��° ��", iLineNumber );
					MessageBox( szTempChar, "����" );
					fclose( pFile );
					return;
				}
			}else{
				sprintf_s( szTempChar, "<END>�� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
		}
		// �ؽ�Ʈ ����Ʈ �߰�
		if( _stricmp( szSectionChar, "<TEXT>" ) == 0 && bAddAnswer == FALSE)
		{
			if( bEndText )
			{
				sprintf_s( szTempChar, "<START>�� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
			bAddText = TRUE;
			iTextNumber++;

			sscanf( szTempChar, "%s %s", szSectionChar, szTempAddChar );
			if( iTextNumber + 1 != atoi( szTempAddChar ) )
			{
				sprintf_s( szTempChar, "TEXT ID�� �߸� �Ǿ����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}

			if( iTextNumber >= (int)pDialogueSet->GetDialogue2(iNpcNumber)->GetDlgCaseVec().size() )
			{
				sprintf_s( szTempChar, "�����Ϳ� �ִ� Text �������� Import�� Text ������ �� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
			ZeroMemory( szTempAddChar, 1024 );
		}
		// ��� ����Ʈ �߰�
		if( _stricmp( szSectionChar, "<ANSWER>" ) == 0 &&  bAddText == FALSE  )
		{
			if( pDialogueSet->GetDialogue2(iNpcNumber)->GetDlgCase(iTextNumber)->GetTalkControl() == NULL )
			{
				sprintf_s( szTempChar, "Answer�� �߰��� �� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
			if( bEndText )
			{
				sprintf_s( szTempChar, "<START>�� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
			ZeroMemory( szTempAddChar, 1024 );
			bAddAnswer = TRUE;
			iAnswerNumber = 0;
		}
		// END üũ
		if( _stricmp( szSectionChar, "<END>" ) == 0 )
		{
			if( pDialogueSet->GetDialogue2(iNpcNumber)->GetDlgCaseVec().size() != iTextNumber + 1 )
			{
				sprintf_s( szTempChar, "�����Ϳ� �ִ� ��ȭ ������ Import�� ��ȭ ������ �ٸ��ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}

			if( bEndText == FALSE )
			{
				bEndText = TRUE;
			}else{
				sprintf_s( szTempChar, "<START>�� �����ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
		}
		// ID ���� ������ npc ���� üũ
		if( strcmp( szSectionChar, "<ID>" ) == 0 )
		{
			sscanf( szTempChar, "%s %s", szSectionChar, szTempAddChar );
			if( pDialogueSet->GetDialogue2(iNpcNumber)->GetDlgNID() != atoi(szTempAddChar) )
			{
				sprintf_s( szTempChar, "Import�� ID�� �������� ID�� �ٸ��ϴ�.\n %d��° ��", iLineNumber );
				MessageBox( szTempChar, "����" );
				fclose( pFile );
				return;
			}
		}
	}
	fclose( pFile );

	if( iNpcNumber + 1  != pDialogueSet->GetDialogueMap()->size() )
	{
		sprintf_s( szTempChar, "�����Ϳ� �ִ� NPC ��ȭ ������ Import�� NPC ��ȭ ������ �ٸ��ϴ�.\n %d��° ��", iLineNumber );
		MessageBox( szTempChar, "����" );
		fclose( pFile );
		return;
	}



	int i;
	for( i = 0; i < (int)npcTextData.size(); i++ )
	{
		pDialogueSet->GetDialogue2(npcTextData[i].dwNpcNumber)->GetDlgCase(npcTextData[i].dwTextNumber)->SetBasicTalk(npcTextData[i].TalkText.c_str());
	}


	for( i = 0; i < (int)npcAnswerTextData.size(); i++ )
	{

		int test1 = npcAnswerTextData[i].dwNpcNumber;
		int test2 = npcAnswerTextData[i].dwTextNumber;
		int test3 = npcAnswerTextData[i].dwAnswerNumber;
		string test4 = npcAnswerTextData[i].answerTalkText.c_str();

		pDialogueSet->GetDialogue2(npcAnswerTextData[i].dwNpcNumber)->GetDlgCase(npcAnswerTextData[i].dwTextNumber)->
		  			  GetTalkControl()->GetTalk2(npcAnswerTextData[i].dwAnswerNumber)->SetTalk(npcAnswerTextData[i].answerTalkText.c_str());
	}	
	m_pSheetTab->GetDialogueSet()->DoModified();
	m_pSheetTab->m_NpcActionTable.UpdateItems ();
}


void CNpcActionDlg::OnBnClickedCsvexportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, NULL, 0, szTempChar, 256 );
	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK ������ �ε��ϼ���." );
		return;
	}

	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, szTempChar, 256, NULL, 0 );
	strcat_s( szTempChar, ".csv" );

	FILE *pFile;
	fopen_s(&pFile, szTempChar, "wt");
	if( pFile == NULL )
	{
		MessageBox( "���� ���� ����. CSV ������ ���������� �ݾ��ּ���" );
		return;
	}

	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();
	// ��ü NPC �ؽ�Ʈ
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = pDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_IT found;

	
	// ����� �޴� �̸�
	char szMenuChar[]={"NPCMainID,NPCSubID,TotalAnswerNumber,ConditionLevel,ConditionLevelsize,ConditionClass,ConditionElemental,ConditionElementalSize,ConditionLivingScroe,ConditionLivingScoreSize,ConditionQuestMID,ConditionQuestSID,CondtionDonotQuest,ConditionRequestitemMID,ConditionRequestItemSID,ConditionCompletionQuest,ConditionSchool\n"};
	char szAnswerChar[]={"AnswerID,AnswerMainType,AnswerSubType,Buslistfilename,ExchangeFromItemMID,ExchangeFromItemSID,ExchangeToItemMID,ExchangeToItemSID,StartQuestID1,StartQuestID2,StartQuestID3,StartQuestID4,StartQuestID5,ProgressingQuestMID,ProgressingQuestSID,"};
	char szAnswerConChar[]={"ConditionLevel,ConditionLevelsize,ConditionClass,ConditionElemental,ConditionElementalSize,ConditionLivingScroe,ConditionLivingScoreSize,ConditionQuestMID,ConditionQuestSID,CondtionDonotQuest,ConditionRequestitemMID,ConditionRequestItemSID,ConditionCompletionQuest,ConditionSchool\n"};

	int i = 0, j = 0;

	// ��ü ����Ʈ ������
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;
		for( i = 0; i < (int)pNpcDialogue->GetDlgCaseVec().size(); i++ )
		{
			// �ؽ�Ʈ ���� �ʱ�ȭ
			string strLine;
			ZeroMemory( szTempChar, 256 );
			//strLine.clear();

			// �޴� �̸� ����
			fprintf( pFile, szMenuChar );

			// NPCMainID
			sprintf_s( szTempChar, "%d", pNpcDialogue->GetDlgNID() );
			strLine += szTempChar; strLine += ",";
			// NPCSubID 
			sprintf_s( szTempChar, "%d", i );
			strLine += szTempChar; strLine += ",";		
			// TotalAnswerNumber 
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() != NULL )
			{
				sprintf_s( szTempChar, "%d", pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() );
				strLine += szTempChar; strLine += ",";
			}else{
				strLine += "0"; strLine += ",";
			}

			// Save Condition
			if( pNpcDialogue->GetDlgCase( i )->GetCondition() != NULL )
			{
				pNpcDialogue->GetDlgCase( i )->GetCondition()->SAVECSVFILE( strLine );
			}
			strLine += "\n";
			fprintf( pFile, strLine.c_str() );

			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			// ��� �޴� ����
			fprintf( pFile, szAnswerChar );
			fprintf( pFile, szAnswerConChar );
			CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				ZeroMemory( szTempChar, 128 );
				strLine.clear();

				SNpcTalk* pNpcTalk = (*iter).second;

				// AnswerID
				sprintf_s( szTempChar, "%d", pNpcTalk->m_dwNID );
				strLine += szTempChar; strLine += ",";
				// AnswerMainType
				sprintf_s( szTempChar, "%d", pNpcTalk->m_nACTION );
				strLine += szTempChar; strLine += ",";
				// AnswerSubType
				sprintf_s( szTempChar, "%d", pNpcTalk->m_dwACTION_NO );
				strLine += szTempChar; strLine += ",";
				// Buslistfilename

				if( pNpcTalk->m_strBusFile.size() == 0 )
				{
					strLine += "NULL"; strLine += ",";
				}else{
					strLine += pNpcTalk->m_strBusFile.c_str(); strLine += ",";
				}
				// ExchangeFromItemMID, ExchangeFromItemSID, ExchangeToItemMID, ExchangeToItemSID
				// Type: �⺻���
				// Data: ������ ��ȯ
				// Need Add New Item Con.
				if( pNpcTalk->m_nACTION == 2 && pNpcTalk->m_dwACTION_NO == 5 )
				{
					SNATIVEID nID;
					nID.dwID = pNpcTalk->m_dwACTION_PARAM1;
					sprintf_s( szTempChar, "%d", nID.wMainID );
					strLine += szTempChar; strLine += ",";
					sprintf_s( szTempChar, "%d", nID.wSubID );
					strLine += szTempChar; strLine += ",";

					nID.dwID = pNpcTalk->m_dwACTION_PARAM2;
					sprintf_s( szTempChar, "%d", nID.wMainID );
					strLine += szTempChar; strLine += ",";
					sprintf_s( szTempChar, "%d", nID.wSubID );
					strLine += szTempChar; strLine += ",";
				}else{
					sprintf_s( szTempChar, "NULL,NULL,NULL,NULL" );
					strLine += szTempChar; strLine += ",";
				}
				// StartQuestID 
				// Type: ����Ʈ ����
				// Data: �������
				if( pNpcTalk->m_nACTION == 3 )
				{
					for ( int index = 0; index < MAX_QUEST_START; ++index )
					{
						if( pNpcTalk->m_dwQuestStartID[index] == UINT_MAX )
						{
							strLine += "NULL"; strLine += ",";
						}else{
							sprintf_s( szTempChar, "%d", pNpcTalk->m_dwQuestStartID[index] );
							strLine += szTempChar; strLine += ",";
						}
					}
				}else{
					sprintf_s( szTempChar, "NULL,NULL,NULL,NULL,NULL" );
					strLine += szTempChar; strLine += ",";
				}
				// ProgressingQuestMID, ProgressingQuestSID
				if( pNpcTalk->m_nACTION == 4 )
				{
					sprintf_s( szTempChar, "%d", pNpcTalk->m_dwACTION_PARAM1 );
					strLine += szTempChar; strLine += ",";
					sprintf_s( szTempChar, "%d", pNpcTalk->m_dwACTION_PARAM2 );
					strLine += szTempChar; strLine += ",";
				}else{
					sprintf_s( szTempChar, "NULL,NULL" );
					strLine += szTempChar; strLine += ",";
				}
				// Save Answer Condition
				if( pNpcTalk->GetCondition() != NULL )
					pNpcTalk->GetCondition()->SAVECSVFILE( strLine );
				strLine += "\n";

				fprintf( pFile, strLine.c_str() );
			}
		}
//		strLine += szTempChar;		

	}

	fclose( pFile );

	MessageBox( "Export�� �����߽��ϴ�." );

}

void CNpcActionDlg::OnBnClickedCsvimportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char szTempChar[256];


	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, NULL, 0, szTempChar, 256 );

	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK ������ �ε��ϼ���." );
		return;
	}
	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, szTempChar, 256, NULL, 0 );
	strcat_s( szTempChar, ".csv" );
	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( szTempChar ) )
	{
		MessageBox( "�ͽ���Ʈ�� CSV �����Ͱ� �����ϴ�." );
		return;
	}

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	CStringArray beforeStrArray;
	string	strSection;

	bool	bAddAnswer = FALSE;

	int iNpcNumber = -1;
	int iTextNumber = -1;
	int iAnswerNumber = -1;
	int iLineNumber = 0;

	// �ε��� ������ ����
	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();
	
	while( StrFile.GetNextLine( strLine ) )
	{
		iLineNumber++;

		STRUTIL::StringSeparate( strLine, StrArray );

		strSection = StrArray[0];
		
		if( strSection == "NPCMainID" )
		{
			bAddAnswer = FALSE;
			continue;
		}else if( strSection == "AnswerID" )
		{
			bAddAnswer = TRUE;
			iAnswerNumber = -1;
			beforeStrArray.Copy( StrArray );
			continue;
		}

	
		if( bAddAnswer == FALSE )
		{
			// ����� �ƴ� �⺻ �������� ���
			iNpcNumber  = atoi(StrArray[0]);
			iTextNumber = atoi(StrArray[1]);

			CNpcDialogue* pNpcDialogue = pDialogueSet->GetDialogue(iNpcNumber);
			
			// Condition�� ���� ������ �����Ѵ�.
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition() != NULL )
			{
				if( pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition()->LOADCSVFILE( StrArray, 3, iLineNumber ) == FALSE )
					goto load_error;
			}
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl() != NULL )
			{
				if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl()->GetTalkMap()->size() != atoi(StrArray[2]) )
				{
					sprintf_s( szTempChar, "CSV�� ��� ������ �������� ��� ������ �ٸ��ϴ�.\n %d��° ��", iLineNumber );
					MessageBox( szTempChar, "����" );
					goto load_error;
				}
			}
		}else if( bAddAnswer == TRUE )
		{
			
			char *szAnswerChar[] = { {"AnswerID"}, {"AnswerMainType"}, {"AnswerSubType"}, {"Buslistfilename"}, 
									{"ExchangeFromItemMID"}, {"ExchangeFromItemSID"}, {"ExchangeToItemMID"}, 
									{"ExchangeToItemSID"}, {"StartQuestID"}, {"ProgressingQuestMID"}, {"ProgressingQuestSID" }};
			// ����� ��� 
			int iCsvCur = 0;
			iAnswerNumber++;
			CNpcDialogue* pNpcDialogue = pDialogueSet->GetDialogue(iNpcNumber);
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl() == NULL ) continue;
			SNpcTalk* pNpcTalk = pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->GetTalk2(iAnswerNumber);
			// AnswerID
			pNpcTalk->m_dwNID       = atoi(StrArray[iCsvCur++]);
			// AnswerMainType
			pNpcTalk->m_nACTION		= atoi(StrArray[iCsvCur++]);
			// AnswerSubType
			pNpcTalk->m_dwACTION_NO = atoi(StrArray[iCsvCur++]);
			// Buslistfilename
			if( StrArray[iCsvCur++] != "NULL" )
				pNpcTalk->m_strBusFile	= StrArray[iCsvCur];
			else
				pNpcTalk->m_strBusFile.clear();

			string strTemp;
			// ExchangeFromItemMID, ExchangeFromItemSID, ExchangeToItemMID, ExchangeToItemSID
			if( pNpcTalk->m_nACTION == SNpcTalk::EM_BASIC && pNpcTalk->SNpcTalk::EM_ITEM_TRADE == 5 )
			{
				
				SNATIVEID nID = SNATIVEID(false);

				// ExchangeFromItemMID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{					 
				}else{
					for( int i = 0; i < (int)strTemp.size(); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf_s( szTempChar, "ExchangeFromItemMID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
							MessageBox( szTempChar, "����" );
							goto load_error;
						}
					}
					nID.wMainID = atoi(strTemp.c_str());
				}

				// ExchangeFromItemSID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{
				}else{
					for( int i = 0; i < (int)strTemp.size(); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf_s( szTempChar, "ExchangeFromItemSID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
							MessageBox( szTempChar, "����" );
							goto load_error;
						}
					}
					nID.wSubID = atoi(strTemp.c_str());
				}
				pNpcTalk->m_dwACTION_PARAM1 = nID.dwID;

				nID = SNATIVEID(false);

				// ExchangeToItemMID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{					
				}else{
					for( int i = 0; i < (int)strTemp.size(); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf_s( szTempChar, "ExchangeToItemMID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
							MessageBox( szTempChar, "����" );
							goto load_error;
						}
					}
					nID.wMainID = atoi(strTemp.c_str());
				}

				// ExchangeToItemSID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{					
				}else{
					for( int i = 0; i < (int)strTemp.size(); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf_s( szTempChar, "ExchangeToItemSID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
							MessageBox( szTempChar, "����" );
							goto load_error;
						}
					}
					nID.wSubID = atoi(strTemp.c_str());
				}
				pNpcTalk->m_dwACTION_PARAM2 = nID.dwID;
			}else{
				iCsvCur += 4;
			}
			// StartQuestID
			if( pNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_START )
			{
				string strTemp2 = beforeStrArray[8];
				if( beforeStrArray[8] == "StartQuestID1" || beforeStrArray[8] == " StartQuestID1" )
				{
					for ( int index = 0; index < MAX_QUEST_START; ++index )
					{
						strTemp = StrArray[iCsvCur++];
						if( strTemp == "NULL" || strTemp == " NULL" )
						{
							pNpcTalk->m_dwQuestStartID[index] = UINT_MAX;
							continue;
						}else{	
							for( int i = 0; i < (int)strTemp.size(); i++ )
							{
								if( !isdigit( strTemp[i] ) )
								{
									sprintf_s( szTempChar, "StartQuestID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
									MessageBox( szTempChar, "����" );
									goto load_error;
								}
							}
							pNpcTalk->m_dwQuestStartID[index] = atoi(strTemp.c_str());
						}
					}
				}else if( beforeStrArray[8] == "StartQuestID" || beforeStrArray[8] == " StartQuestID" )
				{
					strTemp = StrArray[iCsvCur++];
					if( strTemp == "NULL" || strTemp == " NULL" )
					{
					}else{	
						for( int i = 0; i < (int)strTemp.size(); i++ )
						{
							if( !isdigit( strTemp[i] ) )
							{
								sprintf_s( szTempChar, "StartQuestID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
								MessageBox( szTempChar, "����" );
								goto load_error;
							}
						}
						pNpcTalk->m_dwQuestStartID[0] = atoi(strTemp.c_str());
					}

				}
			}else{
				iCsvCur += MAX_QUEST_START;
			}
			// ProgressingQuestMID ProgressingQuestSID
			if( pNpcTalk->m_nACTION == SNpcTalk::EM_QUEST_STEP )
			{
				// ProgressingQuestMID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{
					pNpcTalk->m_dwACTION_PARAM1 = UINT_MAX;
				}else{
					for( int i = 0; i < (int)strTemp.size(); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf_s( szTempChar, "ProgressingQuestMID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
							MessageBox( szTempChar, "����" );
							goto load_error;
						}
					}
					pNpcTalk->m_dwACTION_PARAM1 = atoi(strTemp.c_str());
				}
				// ProgressingQuestSID
				strTemp = StrArray[iCsvCur++];
				if( strTemp == "NULL" || strTemp == " NULL" )
				{
					pNpcTalk->m_dwACTION_PARAM2 = UINT_MAX;
				}else{
					for( int i = 0; i < (int)strTemp.size(); i++ )
					{
						if( !isdigit( strTemp[i] ) )
						{
							sprintf_s( szTempChar, "ProgressingQuestSID�� ���ڰ� �� �ֽ��ϴ�.\n %d��° ��", iLineNumber );
							MessageBox( szTempChar, "����" );
							goto load_error;
						}
					}
					pNpcTalk->m_dwACTION_PARAM2 = atoi(strTemp.c_str());
				}

			}else{
				iCsvCur += 2;
			}
			if( pNpcTalk->GetCondition() != NULL )
			{
				if( pNpcTalk->GetCondition()->LOADCSVFILE( StrArray, iCsvCur, iLineNumber ) == FALSE )
					goto load_error;
			}

			pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->AddTalk(pNpcTalk);
            
		}

	
	}
	MessageBox( "Import�� �����߽��ϴ�." );
	return;
load_error:
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	m_pSheetTab->GetDialogueSet()->Load ( szTempChar );
	MessageBox( "Import�� �����߽��ϴ�." );

}


void CNpcActionDlg::OnBnClickedCsvexportButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char szTempChar[256];
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, NULL, 0, szTempChar, 256);
	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK ������ �ε��ϼ���." );
		return;
	}

	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, szTempChar, 256, NULL, 0);
	strcat_s( szTempChar, "_@" );
	strcat_s( szTempChar, ".csv" );
	
	FILE *pFile;
	fopen_s(&pFile, szTempChar, "wt");
	if( pFile == NULL )
	{
		MessageBox( "���� ���� ����. CSV ������ ���������� �ݾ��ּ���" );
		return;
	}

	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();
	// ��ü NPC �ؽ�Ʈ
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = pDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_IT found;


	// ����� �޴� �̸�
	char szMenuChar[] = { "NPCMainID, NPCSubID, ConditionClass\n" };
	char szAnswerChar[] = { "AnswerID, ConditionClass\n" };

	int i = 0, j = 0;

	// ��ü ����Ʈ ������
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;
		for( i = 0; i < (int)pNpcDialogue->GetDlgCaseVec().size(); i++ )
		{
			// �ؽ�Ʈ ���� �ʱ�ȭ
			string strLine;
			ZeroMemory( szTempChar, 256 );
			//strLine.clear();

			// �޴� �̸� ����
			fprintf( pFile, szMenuChar );

			// NPCMainID
			sprintf_s( szTempChar, "%d", pNpcDialogue->GetDlgNID() );
			strLine += szTempChar; strLine += ",";
			// NPCSubID 
			sprintf_s( szTempChar, "%d", i );
			strLine += szTempChar; strLine += ",";		

			// ConditionClass
			if( pNpcDialogue->GetDlgCase( i )->GetCondition() != NULL )
			{
				sprintf_s( szTempChar, "%d", pNpcDialogue->GetDlgCase( i )->GetCondition()->m_dwClass );
				strLine += szTempChar; strLine += ",";
			}
			strLine += "\n";
			fprintf( pFile, strLine.c_str() );

			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			// ��� �޴� ����
			fprintf( pFile, szAnswerChar );
			CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				ZeroMemory( szTempChar, 128 );
				strLine.clear();

				SNpcTalk* pNpcTalk = (*iter).second;

				// AnswerID
				sprintf_s( szTempChar, "%d", pNpcTalk->m_dwNID );
				strLine += szTempChar; strLine += ",";

				// ConditionClass
				if( pNpcTalk->GetCondition() != NULL )
				{
					sprintf_s( szTempChar, "%d", pNpcTalk->GetCondition()->m_dwClass );
					strLine += szTempChar; strLine += ",";
				}
				strLine += "\n";

				fprintf( pFile, strLine.c_str() );
			}
		}	
	}

	fclose( pFile );

	MessageBox( "Export�� �����߽��ϴ�." );
}

void CNpcActionDlg::OnBnClickedCsvimportButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	char szTempChar[256];


	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, NULL, 0, szTempChar, 256);

	if( strlen( szTempChar ) == 0 )
	{
		MessageBox( "NTK ������ �ε��ϼ���." );
		return;
	}
	GetWindowText( szTempChar, 256 );
	_splitpath_s( szTempChar, NULL, 0, NULL, 0, szTempChar, 256, NULL, 0);
	strcat_s( szTempChar, "_@" );
	strcat_s( szTempChar, ".csv" );
	CStringFile StrFile( 10240 );
	if( !StrFile.Open ( szTempChar ) )
	{
		MessageBox( "�ͽ���Ʈ�� CSV �����Ͱ� �����ϴ�." );
		return;
	}

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;
	string	strSection;

	bool	bAddAnswer = FALSE;

	int iNpcNumber = -1;
	int iTextNumber = -1;
	int iAnswerNumber = -1;
	int iLineNumber = 0;

	// �ε��� ������ ����
	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();

	while( StrFile.GetNextLine( strLine ) )
	{
		iLineNumber++;


		STRUTIL::StringSeparate( strLine, StrArray );

		strSection = StrArray[0];

		if( strSection == "NPCMainID" )
		{
			bAddAnswer = FALSE;
			continue;
		}else if( strSection == "AnswerID" )
		{
			bAddAnswer = TRUE;
			iAnswerNumber = -1;
			continue;
		}


		if( bAddAnswer == FALSE )
		{
			// ����� �ƴ� �⺻ �������� ���
			iNpcNumber  = atoi(StrArray[0]);
			iTextNumber = atoi(StrArray[1]);

			CNpcDialogue* pNpcDialogue = pDialogueSet->GetDialogue(iNpcNumber);

			// Condition�� ���� ������ �����Ѵ�.
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition() != NULL )
			{
				pNpcDialogue->GetDlgCase(iTextNumber)->GetCondition()->m_dwClass = atoi(StrArray[2]);					
			}else{
//				goto load_error;
			}
		}else if( bAddAnswer == TRUE )
		{
			// ����� ��� 
			iAnswerNumber++;
			CNpcDialogue* pNpcDialogue = pDialogueSet->GetDialogue(iNpcNumber);
			if( pNpcDialogue->GetDlgCase(iTextNumber)->GetTalkControl() == NULL ) continue;
			SNpcTalk* pNpcTalk = pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->GetTalk2(iAnswerNumber);
			if( pNpcTalk->GetCondition() != NULL )
			{
				pNpcTalk->GetCondition()->m_dwClass = atoi(StrArray[1]);
			}else{
//				goto load_error;
			}

			pNpcDialogue->GetDlgCase( iTextNumber )->GetTalkControl()->AddTalk(pNpcTalk);

		}


	}
	MessageBox( "Import�� �����߽��ϴ�." );
	return;
load_error:
	ZeroMemory( szTempChar, 256 );
	GetWindowText( szTempChar, 256 );
	m_pSheetTab->GetDialogueSet()->Load ( szTempChar );
	MessageBox( "Import�� �����߽��ϴ�." );
}


void CNpcActionDlg::OnBnClickedAllSaveButton()
{
	CString strPath = CNpcDialogueSet::GetPath ();
	CString strFind = strPath + "*.ntk";

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	} 

//	std::fstream streamFILE;
//	streamFILE.open( pInfo.cFileName, std::ios_base::out );
	CString strFile;

	do
	{
		if( m_pSheetTab->GetDialogueSet()->Load( pInfo.cFileName ) )
		{
			m_pSheetTab->GetDialogueSet()->DoModified();
			m_pSheetTab->m_NpcActionTable.UpdateItems   ();

			strFile = strPath + pInfo.cFileName;			
			if ( !m_pSheetTab->GetDialogueSet()->Save(strFile) )	MessageBox ( "error");
		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	FindClose( hContext );

	MessageBox( "Ntk File All Save" );
}

void CNpcActionDlg::OnMenuAllTextExport()
{
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );

	CString strPath = CNpcDialogueSet::GetPath ();
	CString strFind = strPath + "*.ntk";

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	} 

	CString strFile;
	CString strTextFile = strPath + "NpcTalk.txt";

	FILE *pFile;
	fopen_s(&pFile, strTextFile, "wt");
	if ( !pFile ) return;

	do
	{
		if( m_pSheetTab->GetDialogueSet()->Load( pInfo.cFileName ) )
		{
			m_pSheetTab->GetDialogueSet()->DoModified();
			m_pSheetTab->m_NpcActionTable.UpdateItems();

			CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();
			// ��ü NPC �ؽ�Ʈ
			CNpcDialogueSet::DIALOGMAP* pDialogueMap = pDialogueSet->GetDialogueMap();
			CNpcDialogueSet::DIALOGMAP_IT found;

			UINT i;
			// ��ü ����Ʈ
			for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
			{
				CNpcDialogue* pNpcDialogue = (*found).second;
				
				fprintf( pFile, "%s\n", pInfo.cFileName );
				fprintf( pFile, "<START>\n" );
				sprintf_s( szTempChar, "<ID> %d\n\n", pNpcDialogue->GetDlgNID() );
				fprintf( pFile, szTempChar );

				// �ؽ�Ʈ ����Ʈ
				for( i = 0; i < pNpcDialogue->GetDlgCaseVec().size(); i++ )
				{
					sprintf_s( szTempChar, "<TEXT> %d\n", i + 1 );
					fprintf( pFile, szTempChar );
					sprintf_s( szTempChar, "%s\r\n", pNpcDialogue->GetDlgCase( i )->GetBasicTalk().c_str() );
					fprintf( pFile, szTempChar );
					fprintf( pFile, "</TEXT>\n\n" );

					// ��� ����Ʈ
					if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
					if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

					fprintf( pFile, "<ANSWER>\n" );
					CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
					CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
					for ( ; iter!=iter_end; ++iter )
					{
						SNpcTalk* pNpcTalk = (*iter).second;
						sprintf_s( szTempChar, "%s\n", pNpcTalk->GetTalk() );
						fprintf( pFile, szTempChar );
					}
					fprintf( pFile, "</ANSWER>\n\n" );
				}

				fprintf( pFile, "<END>\n\n\n" );
			}
		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	FindClose( hContext );

	fclose( pFile );
}


void CNpcActionDlg::OnMenuAllTextExport2()
{
	CString strPath = CNpcDialogueSet::GetPath ();
	CString strFind = strPath + "*.ntk";

	WIN32_FIND_DATA pInfo;
	lstrcpy( pInfo.cFileName, strFind.GetString() );

	HANDLE hContext = FindFirstFile( strFind.GetString(), &pInfo );
	if( hContext == INVALID_HANDLE_VALUE )
	{
		FindClose( hContext );
		return;
	} 

	CString strFile;

	do
	{
		if( m_pSheetTab->GetDialogueSet()->Load( pInfo.cFileName ) )
		{
			m_pSheetTab->GetDialogueSet()->DoModified();
			m_pSheetTab->m_NpcActionTable.UpdateItems();

			strFile = strPath + pInfo.cFileName;
			strFile = strFile.Left( strFile.ReverseFind('.'));
			strFile += ".txt";

			if ( !TextExport(strFile) ) MessageBox( strFile );

		}
	}
	while( FindNextFile( hContext, &pInfo ) != 0 );

	FindClose( hContext );
}

bool CNpcActionDlg::TextExport( CString strFileName )
{
	char szTempChar[1024];
	ZeroMemory( szTempChar, 1024 );

	FILE* pFile;
	fopen_s(&pFile, strFileName, "wt");
	if ( !pFile ) return false;

	CNpcDialogueSet* pDialogueSet = m_pSheetTab->GetDialogueSet();
	// ��ü NPC �ؽ�Ʈ
	CNpcDialogueSet::DIALOGMAP* pDialogueMap = pDialogueSet->GetDialogueMap();
	CNpcDialogueSet::DIALOGMAP_IT found;

	UINT i;
	// ��ü ����Ʈ
	for( found = pDialogueMap->begin(); found != pDialogueMap->end(); ++found )
	{
		CNpcDialogue* pNpcDialogue = (*found).second;
		
		fprintf( pFile, "<START>\n" );
		sprintf_s( szTempChar, "<ID> %d\n\n", pNpcDialogue->GetDlgNID() );
		fprintf( pFile, szTempChar );

		// �ؽ�Ʈ ����Ʈ
		for( i = 0; i < pNpcDialogue->GetDlgCaseVec().size(); i++ )
		{
			sprintf_s( szTempChar, "<TEXT> %d\n", i + 1 );
			fprintf( pFile, szTempChar );
			sprintf_s( szTempChar, "%s\r\n", pNpcDialogue->GetDlgCase( i )->GetBasicTalk().c_str() );
			fprintf( pFile, szTempChar );
			fprintf( pFile, "</TEXT>\n\n" );

			// ��� ����Ʈ
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl() == NULL ) continue;
			if( pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->size() == 0 ) continue;

			fprintf( pFile, "<ANSWER>\n" );
			CNpcTalkControl::NPCMAP_IT iter     = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->begin();
			CNpcTalkControl::NPCMAP_IT iter_end = pNpcDialogue->GetDlgCase( i )->GetTalkControl()->GetTalkMap()->end();	
			for ( ; iter!=iter_end; ++iter )
			{
				SNpcTalk* pNpcTalk = (*iter).second;
				sprintf_s( szTempChar, "%s\n", pNpcTalk->GetTalk() );
				fprintf( pFile, szTempChar );
			}
			fprintf( pFile, "</ANSWER>\n\n" );
		}

		fprintf( pFile, "<END>\n\n\n" );
	}

	fclose( pFile );

	return true;
}



void CNpcActionDlg::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	SetWin_Enable( this, IDC_CSVIMPORT_BUTTON, false );
	SetWin_Enable( this, IDC_CSVIMPORT_BUTTON2, false );
	SetWin_Enable( this, IDC_ALLSAVE_BUTTON, false );

	EnableMenu( ID_MENUITEM_NEW, FALSE );
	EnableToobar( ID_MENUITEM_NEW, FALSE );

#endif
}
