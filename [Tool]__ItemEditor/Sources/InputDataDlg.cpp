// InputDataDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ItemEdit.h"
#include "InputDataDlg.h"
#include ".\inputdatadlg.h"
#include "EtcFunction.h"


// CInputDataDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CInputDataDlg, CDialog)
CInputDataDlg::CInputDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDataDlg::IDD, pParent)
	, m_bNum ( false )
	, m_dwNum ( 1 )
	, m_fRate ( 0.0f )
{
}

CInputDataDlg::~CInputDataDlg()
{
}

void CInputDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputDataDlg, CDialog)
END_MESSAGE_MAP()


// CInputDataDlg 메시지 처리기입니다.

BOOL CInputDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if ( m_bNum )
	{
		m_strItemName += " Num : ";
		SetWin_Text( this, IDC_ITEM_STATIC, m_strItemName );
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_dwNum );
	}
	else
	{
		m_strItemName += " Rate : ";
		SetWin_Text( this, IDC_ITEM_STATIC, m_strItemName );
		SetWin_Num_float ( this, IDC_EDIT_NUM, m_fRate );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInputDataDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	

	if ( m_bNum )
	{
		m_dwNum = GetWin_Num_int ( this, IDC_EDIT_NUM );
		if ( m_dwNum <= 0 ) 
		{
			MessageBox ( " Num Error " );
			return;
		}

	}
	else
	{
		m_fRate = GetWin_Num_float ( this, IDC_EDIT_NUM );		
		if ( m_fRate <= 0.0f || m_fRate > 100.0f ) 
		{
			MessageBox ( " Rate Error " );
			return;
		}
		m_dwNum = (DWORD)(m_fRate * 100.0f);
	}

	CDialog::OnOK();
}


