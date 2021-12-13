// LevelEditorView.cpp : implementation of the CLevelEditorView class
//

#include "pch.h"
#include "LevelEditor.h"

#include "LevelEditorDoc.h"
#include "LevelEditorView.h"
#include "MainFrm.h"

#include "DxEditMat.h"
#include "DxEditBox.h"
#include "DxObjectMRS.h"
#include "DxLandMan.h"
#include "DxFrameMesh.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelEditorView
BOOL CLevelEditorView::m_bRENDQUAD = FALSE;

IMPLEMENT_DYNCREATE(CLevelEditorView, CView)

BEGIN_MESSAGE_MAP(CLevelEditorView, CView)
END_MESSAGE_MAP()

// CLevelEditorView construction/destruction

CLevelEditorView::CLevelEditorView()
	: CD3DApplication()
	, m_bCreated ( FALSE )
	, m_p3dPosMesh ( NULL )
	, m_pDxEditmat ( NULL )
	, m_vCollisionPos ( 0.f, 0.f, 0.f )
{
	// TODO: add construction code here
	m_bUseDepthBuffer	= TRUE;
}

CLevelEditorView::~CLevelEditorView()
{
}

BOOL CLevelEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLevelEditorView drawing

void CLevelEditorView::OnDraw(CDC* /*pDC*/)
{
	CLevelEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CLevelEditorView diagnostics

#ifdef _DEBUG
void CLevelEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CLevelEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLevelEditorDoc* CLevelEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLevelEditorDoc)));
	return (CLevelEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CLevelEditorView message handlers
BOOL CLevelEditorView::SaveGLLandMan ( const char *szFile )
{
	GLGaeaServer::GetInstance().GetRootMap()->SaveFile ( szFile );

	return TRUE;
}

BOOL CLevelEditorView::LoadGLLandMan ( const char *szFile )
{
	CLevelMainFrame* pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();

	pGLLandMan->ReSetMap();
	pGLLandMan->LoadFile ( szFile );

	m_LandMan.CleanUp();

	if ( m_LandMan.LoadFile ( pGLLandMan->GetWldFileName(), GetD3DDevice() ) )
	{
		GLGaeaServer::GetInstance().ClearDropObj();
		m_LandMan.ActiveMap ();
	}

	pFrame->m_DlgLevelEdit.UpdatePage( TRUE );

	return TRUE;
}