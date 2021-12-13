// DlgItem.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgItem.h"

//#include <strstream>
#include "GLItemMan.h"


// CDlgItem ��ȭ �����Դϴ�.

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


// CDlgItem �޽��� ó�����Դϴ�.

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

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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