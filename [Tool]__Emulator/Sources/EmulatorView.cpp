// EmulatorView.cpp : CEmulatorView Ŭ������ ����
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
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_COMMAND(ID_APP_PLAY, OnAppPlay)
END_MESSAGE_MAP()

// CEmulatorView ����/�Ҹ�

CEmulatorView::CEmulatorView() :
	CD3DApplication(),
	m_bCreated(FALSE),
	m_hCursorDefault(NULL)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
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

// CEmulatorView �׸���

void CEmulatorView::OnDraw(CDC* /*pDC*/)
{
	CEmulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CEmulatorView �μ�

BOOL CEmulatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CEmulatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CEmulatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CEmulatorView ����

#ifdef _DEBUG
void CEmulatorView::AssertValid() const
{
	CView::AssertValid();
}

void CEmulatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEmulatorDoc* CEmulatorView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEmulatorDoc)));
	return (CEmulatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CEmulatorView �޽��� ó����
