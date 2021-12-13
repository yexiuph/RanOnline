// SearchDialog.cpp : 구현 파일입니다.
//

#include "pch.h"
#include <vector>
#include "ItemEdit.h"
#include "SearchDialog.h"
#include ".\searchdialog.h"


// CSearchDialog 대화 상자입니다.

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


// CSearchDialog 메시지 처리기입니다.
int CSearchDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

BOOL CSearchDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SearchReportList.SetExtendedStyle ( m_SearchReportList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );


	m_SearchReportList.InsertColumn( 0, "MID", LVCFMT_LEFT, 40 );
	m_SearchReportList.InsertColumn( 1, "SID", LVCFMT_LEFT, 40 );
	m_SearchReportList.InsertColumn( 2, "이름", LVCFMT_LEFT, 200 );
	m_SearchReportList.InsertColumn( 3, "값", LVCFMT_LEFT, 200 );

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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSearchDialog::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnClose();
}

void CSearchDialog::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CSearchDialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_iSelectCount = m_SearchReportList.getselect
	POSITION	pos = m_SearchReportList.GetFirstSelectedItemPosition ();
	m_iSelectCount = m_SearchReportList.GetNextSelectedItem ( pos );	
	if( m_iSelectCount == -1 ) 
	{
		MessageBox( "아이템을 선택하세요.", "알림" );
		return;
	}

	OnOK();
}

void CSearchDialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	m_iSelectCount = m_SearchReportList.GetSelectedCount();
	OnCancel();
}
