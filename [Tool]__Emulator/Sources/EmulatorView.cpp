// EmulatorView.cpp : CEmulatorView 클래스의 구현
//
#include "pch.h"

#include "Emulator.h"

#include "DxGlobalStage.h"
#include "EmulatorDoc.h"
#include "EmulatorView.h"

#include "GLogicData.h"
#include "GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEmulatorView

IMPLEMENT_DYNCREATE(CEmulatorView, CView)

BEGIN_MESSAGE_MAP(CEmulatorView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_COMMAND(ID_APP_PLAY, OnAppPlay)
END_MESSAGE_MAP()

// CEmulatorView 생성/소멸

CEmulatorView::CEmulatorView() :
	CD3DApplication(),
	m_bCreated(FALSE),
	m_hCursorDefault(NULL)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_bUseDepthBuffer	= TRUE;
	
	GLOGIC::bGLOGIC_PACKFILE = FALSE;
	GLOGIC::bGLOGIC_ZIPFILE = TRUE;
	GLOGIC::bENGLIB_ZIPFILE = TRUE;
}

CEmulatorView::~CEmulatorView()
{
}

BOOL CEmulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	CEmulatorApp *pApp = (CEmulatorApp *) AfxGetApp();

	return CView::PreCreateWindow(cs);
}

// CEmulatorView 그리기

void CEmulatorView::OnDraw(CDC* /*pDC*/)
{
	CEmulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CEmulatorView 인쇄

BOOL CEmulatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CEmulatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CEmulatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CEmulatorView 진단

#ifdef _DEBUG
void CEmulatorView::AssertValid() const
{
	CView::AssertValid();
}

void CEmulatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEmulatorDoc* CEmulatorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEmulatorDoc)));
	return (CEmulatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CEmulatorView 메시지 처리기
