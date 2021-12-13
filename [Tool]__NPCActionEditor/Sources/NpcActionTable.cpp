// NpcActionTable.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "NpcAction.h"
#include "NpcActionTable.h"
#include "SheetWithTab.h"

#include "NpcDialogue.h"
#include ".\npcactiontable.h"
#include "EtcFunction.h"

// CNpcActionTable ��ȭ �����Դϴ�.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CNpcActionTable, CPropertyPage)
CNpcActionTable::CNpcActionTable( LOGFONT logfont )
	: CPropertyPage(CNpcActionTable::IDD)
	, m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CNpcActionTable::~CNpcActionTable()
{
	SAFE_DELETE ( m_pFont );
}

void CNpcActionTable::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACTION, m_ctrlActionTable);
}


BEGIN_MESSAGE_MAP(CNpcActionTable, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_CSVIMPORT_BUTTON, OnBnClickedCsvimportButton)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ACTION, OnNMDblclkListAction)
END_MESSAGE_MAP()


// CNpcActionTable �޽��� ó�����Դϴ�.


BOOL CNpcActionTable::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_ctrlActionTable.SetExtendedStyle (
		m_ctrlActionTable.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	RECT	rect;
	m_ctrlActionTable.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	const int	ColumnCount = 2;	
	char*		ColumnName[ColumnCount] = { "ID", "Basic Conversation" };
	int			ColumnWidthPercent[ColumnCount] = { 6, 94 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlActionTable.InsertColumn (i, &lvcolumn);		
	}

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CNpcActionTable::OnBnClickedButtonInsert()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->ActiveActionPage ( NULL );
}

CNpcDialogueSet*	CNpcActionTable::GetDialogueSet ()
{
	return &m_DialogueSet;
}

void CNpcActionTable::UpdateItems ()
{
	m_ctrlActionTable.DeleteAllItems ();

	CString strTemp;
	CNpcDialogueSet::DIALOGMAP* pMap = GetDialogueSet()->GetDialogueMap ();
	
	CNpcDialogueSet::DIALOGMAP_IT iter = pMap->begin();
	CNpcDialogueSet::DIALOGMAP_IT iter_end = pMap->end();	

	

	for ( int nIndex = 0; iter!=iter_end; ++iter, nIndex++ )
	{
		CNpcDialogue* pDialogue = (*iter).second;

		DWORD dwIndex = pDialogue->FindNonCondition();
		if ( dwIndex==UINT_MAX )	dwIndex = 0;

		CNpcDialogueCase* pDlgCase = pDialogue->GetDlgCase(dwIndex);
		if ( !pDlgCase )
		{
			
			pMap->erase( iter++ );
			--nIndex;
			continue;			
		}

		strTemp.Format ( "%d", pDialogue->GetDlgNID() );
		int nItem = m_ctrlActionTable.InsertItem ( nIndex, strTemp );		
		m_ctrlActionTable.SetItemText ( nIndex, 1, pDlgCase->GetBasicTalk().c_str() );

		m_ctrlActionTable.SetItemData ( nItem, (DWORD_PTR)pDialogue->GetDlgNID() );
	}
}

void CNpcActionTable::OnBnClickedButtonModify()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_ctrlActionTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlActionTable.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlActionTable.GetItemData ( nItem );	
	m_pSheetTab->ActiveActionPage ( GetDialogueSet()->GetDialogue ( NativeID ) );
}

void CNpcActionTable::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_ctrlActionTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlActionTable.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlActionTable.GetItemData ( nItem );

	CNpcDialogue* pDialogue = GetDialogueSet()->GetDialogue ( NativeID );
	
	DWORD dwIndex = pDialogue->FindNonCondition();
	if ( dwIndex==UINT_MAX )	dwIndex = 0;

	CString strBasicTalk;
	CNpcDialogueCase* pDlgCase = pDialogue->GetDlgCase(dwIndex);
	if ( pDlgCase )		strBasicTalk = pDlgCase->GetBasicTalk().c_str();

	CString strTemp;
	strTemp.Format ( "Do you want to delete?\n[%03d] %s", NativeID, strBasicTalk );

    if ( MessageBox ( strTemp, "Question", MB_YESNO | MB_ICONASTERISK ) == IDYES )
	{
		GetDialogueSet()->DelDialogue ( NativeID );
		UpdateItems ();
	}
}

void CNpcActionTable::OnBnClickedButtonCopy()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_ctrlActionTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlActionTable.GetNextSelectedItem ( pos );	
	if ( nItem == -1 )	return;

	DWORD dwNativeID = (DWORD) m_ctrlActionTable.GetItemData ( nItem );
	CNpcDialogue* pDialogue = GetDialogueSet()->GetDialogue ( dwNativeID );
	if ( !pDialogue )	return;

	DWORD dwNID = GetDialogueSet()->MakeDlgNID ();

	CNpcDialogue* pNewDialogue = new CNpcDialogue;
	*pNewDialogue = *pDialogue;
	pNewDialogue->SetDlgNID ( dwNID );

	GetDialogueSet()->AddDialogue ( pNewDialogue );

	UpdateItems ();
}



void CNpcActionTable::OnBnClickedCsvimportButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CNpcActionTable::SetReadMode()
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	const int nSkipNum = 2;
	const int nSkipID[nSkipNum] = { IDC_LIST_ACTION, IDC_BUTTON_MODIFY };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}

#endif
}

void CNpcActionTable::OnNMDblclkListAction(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	if ( pNMIA->iItem == -1 )
	{
		return ;
	}

	DWORD NativeID = (DWORD) m_ctrlActionTable.GetItemData ( pNMIA->iItem );	
	m_pSheetTab->ActiveActionPage ( GetDialogueSet()->GetDialogue ( NativeID ) );
}
