// DlgItem.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgItem.h"

//#include <strstream>
#include "GLItemMan.h"


// CDlgItem 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgItem, CDialog)
CDlgItem::CDlgItem(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItem::IDD, pParent),
	m_nidITEM(false)
{
}

CDlgItem::~CDlgItem()
{
}

void CDlgItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEM, m_listITEM);
}


BEGIN_MESSAGE_MAP(CDlgItem, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgItem 메시지 처리기입니다.

BOOL CDlgItem::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szItem[128] = {0};

	for ( WORD i=0; i<GLItemMan::MAX_MID; ++i )
	for ( WORD j=0; j<GLItemMan::MAX_SID; ++j )
	{
		SITEM* pITEM = GLItemMan::GetInstance().GetItem ( i, j );
		if ( !pITEM )	continue;

		//std::strstream strName;
		//strName << "[" << i << "/" << j << "] ";
		//strName << pITEM->GetName() << std::ends;

		_snprintf_s( szItem, 128, "[%u/%u] %s", i, j, pITEM->GetName() );

		int nIndex = m_listITEM.AddString ( szItem );

		m_listITEM.SetItemData ( nIndex, pITEM->sBasicOp.sNativeID.dwID );

		//strName.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgItem::OnBnClickedOk()
{
	int nIndex = m_listITEM.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidITEM.dwID = (DWORD) m_listITEM.GetItemData ( nIndex );

	OnOK();
}

void CDlgItem::OnBnClickedCancel()
{
	m_nidITEM = SNATIVEID(false);

	OnCancel();
}