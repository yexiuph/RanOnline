// SearchDialog.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include <vector>
#include "ItemEdit.h"
#include "SearchDialog.h"
#include ".\searchdialog.h"


// CSearchDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CSearchDialog, CDialog)
CSearchDialog::CSearchDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchDialog::IDD, pParent)
{
	m_iSelectCount = 0;
}

CSearchDialog::~CSearchDialog()
{
}

void CSearchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ITEM_SEARCH_LIST, m_SearchReportList);
}

void CSearchDialog::ClearItem()
{
	m_vecSearchItemList.clear();
	m_SearchReportList.DeleteAllItems();
}

void CSearchDialog::InsertReportItem( std::vector<SSearchItemList> vecSearchItemList )
{
	m_vecSearchItemList.clear();
	m_SearchReportList.DeleteAllItems();

	m_vecSearchItemList = vecSearchItemList;
}
BEGIN_MESSAGE_MAP(CSearchDialog, CDialog)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CSearchDialog �޽��� ó�����Դϴ�.
int CSearchDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.

	return 0;
}

BOOL CSearchDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SearchReportList.SetExtendedStyle ( m_SearchReportList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );


	m_SearchReportList.InsertColumn( 0, "MID", LVCFMT_LEFT, 40 );
	m_SearchReportList.InsertColumn( 1, "SID", LVCFMT_LEFT, 40 );
	m_SearchReportList.InsertColumn( 2, "�̸�", LVCFMT_LEFT, 200 );
	m_SearchReportList.InsertColumn( 3, "��", LVCFMT_LEFT, 200 );

	UINT i;
	char szTempChar[32];
	for( i = 0; i < m_vecSearchItemList.size(); i++ )
	{
		sprintf_s( szTempChar, "%d", m_vecSearchItemList[i].MID );
		m_SearchReportList.InsertItem( i, szTempChar, NULL );
		sprintf_s( szTempChar, "%d", m_vecSearchItemList[i].SID );
		m_SearchReportList.SetItemText( i, 1, szTempChar );
		m_SearchReportList.SetItemText( i, 2, m_vecSearchItemList[i].szItemName );
		m_SearchReportList.SetItemText( i, 3, m_vecSearchItemList[i].szItemValue );
	}

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSearchDialog::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CDialog::OnClose();
}

void CSearchDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CSearchDialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	m_iSelectCount = m_SearchReportList.getselect
	POSITION	pos = m_SearchReportList.GetFirstSelectedItemPosition ();
	m_iSelectCount = m_SearchReportList.GetNextSelectedItem ( pos );	
	if( m_iSelectCount == -1 ) 
	{
		MessageBox( "�������� �����ϼ���.", "�˸�" );
		return;
	}

	OnOK();
}

void CSearchDialog::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
//	m_iSelectCount = m_SearchReportList.GetSelectedCount();
	OnCancel();
}
