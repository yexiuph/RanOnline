// EmulatorDoc.cpp : CEmulatorDoc 클래스의 구현
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


// CEmulatorDoc 생성/소멸

CEmulatorDoc::CEmulatorDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CEmulatorDoc::~CEmulatorDoc()
{
}

BOOL CEmulatorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CEmulatorDoc serialization

void CEmulatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CEmulatorDoc 진단

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


// CEmulatorDoc 명령
