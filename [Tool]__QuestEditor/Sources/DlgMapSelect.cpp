// DlgMapSelect.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "QuestEdit.h"
#include "QuestEditDlg.h"
#include "DlgMapSelect.h"
#include ".\dlgmapselect.h"


// CDlgMapSelect ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgMapSelect, CDialog)
CDlgMapSelect::CDlgMapSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapSelect::IDD, pParent)
{
}

CDlgMapSelect::~CDlgMapSelect()
{
}

void CDlgMapSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAP_SELECT, m_ListMap);
}


BEGIN_MESSAGE_MAP(CDlgMapSelect, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgMapSelect �޽��� ó�����Դϴ�.

BOOL CDlgMapSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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

		int nIndex = m_ListMap.AddString ( szName );
		m_ListMap.SetItemData ( nIndex, sNODE.sNativeID.dwID );

		//strName.freeze( false );	// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgMapSelect::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_ListMap.GetCurSel();
	if ( LB_ERR==nIndex )
	{
		MessageBox ( "The item was not selected.", "Caution", MB_OK );
		return;
	}

	m_nidMAP.dwID = (DWORD) m_ListMap.GetItemData ( nIndex );
	OnOK();
}

void CDlgMapSelect::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nidMAP = SNATIVEID(false);
	OnCancel();
}
