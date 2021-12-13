// LogDialog.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "DebugSet.h"
#include "Resource.h"

#include "StringFile.h"
#include "LogDialog.h"


// CLogDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CLogDialog, CDialog)
CLogDialog::CLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDialog::IDD, pParent)
{
}

CLogDialog::~CLogDialog()
{
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONNETS, m_listLog);
}


BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
END_MESSAGE_MAP()


// CLogDialog �޽��� ó�����Դϴ�.

BOOL CLogDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listLog.ResetContent ();

	CString strFullPath = CDebugSet::GetPath();
	strFullPath += m_strFile;

	CString StrLine;
	CStringFile StrFile;
	BOOL bOk = StrFile.Open ( strFullPath.GetString() );
	if ( bOk )
	{
		while ( StrFile.GetNextLine ( StrLine ) )
		{
			m_listLog.AddString ( StrLine );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}




