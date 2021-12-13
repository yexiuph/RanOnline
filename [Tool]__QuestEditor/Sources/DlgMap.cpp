// DlgMap.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "QuestEdit.h"
#include "DlgMap.h"

//#include <strstream>
#include "QuestEditDlg.h"


// CDlgMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMap, CDialog)
CDlgMap::CDlgMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMap::IDD, pParent),
	m_nidMAP(false)
{
}

CDlgMap::~CDlgMap()
{
}

void CDlgMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAP, m_listMAP);
}


BEGIN_MESSAGE_MAP(CDlgMap, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgMap �޽��� ó�����Դϴ�.

BOOL CDlgMap::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR szName[128] = {0};

	GLMapList::FIELDMAP_ITER iter = CQuestEditDlg::m_sMapList.GetMapList().begin();
	GLMapList::FIELDMAP_ITER iter_end = CQuestEditDlg::m_sMapList.GetMapList().end();
	for ( ; iter!=iter_end; ++iter )
	{
		const SMAPNODE &sNODE = (*iter).second;

		//std::strstream strName;
		//strName << "[" << sNODE.sNativeID.wMainID << "/" << sNODE.sNativeID.wSubID << "] ";
		//strName << sNODE.strMapName.c_str() << std::ends;

		_snprintf_s( szName, 128, "[%u/%u] %s", sNODE.sNativeID.wMainID, sNODE.sNativeID.wSubID, sNODE.strMapName.c_str() );

		int nIndex = m_listMAP.AddString ( szName );
		m_listMAP.SetItemData ( nIndex, sNODE.sNativeID.dwID );

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgMap::OnBnClickedOk()
{
	int nIndex = m_listMAP.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidMAP.dwID = (DWORD) m_listMAP.GetItemData ( nIndex );

	OnOK();
}

void CDlgMap::OnBnClickedCancel()
{
	m_nidMAP = SNATIVEID(false);

	OnCancel();
}
