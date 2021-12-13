// DlgMapSelect.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "QuestEdit.h"
#include "QuestEditDlg.h"
#include "DlgMapSelect.h"
#include ".\dlgmapselect.h"


// CDlgMapSelect 대화 상자입니다.

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


// CDlgMapSelect 메시지 처리기입니다.

BOOL CDlgMapSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
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

		//strName.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgMapSelect::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nidMAP = SNATIVEID(false);
	OnCancel();
}
