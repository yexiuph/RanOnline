// EmulatorDoc.cpp : CEmulatorDoc Ŭ������ ����
//

#include "pch.h"
#include "Emulator.h"

#include "EmulatorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEmulatorDoc

IMPLEMENT_DYNCREATE(CEmulatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CEmulatorDoc, CDocument)
END_MESSAGE_MAP()


// CEmulatorDoc ����/�Ҹ�

CEmulatorDoc::CEmulatorDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CEmulatorDoc::~CEmulatorDoc()
{
}

BOOL CEmulatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CEmulatorDoc serialization

void CEmulatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CEmulatorDoc ����

#ifdef _DEBUG
void CEmulatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEmulatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CEmulatorDoc ���
