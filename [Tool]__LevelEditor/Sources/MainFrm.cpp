// MainFrm.cpp : implementation of the CLevelMainFrame class
//

#include "pch.h"
#include "LevelEditor.h"

#include "MainFrm.h"

#include "ToolBarEx.h"
#include "CoolDialogBar.h"

#include "LevelEditorView.h"
#include "DlgLevelEdit.h"
#include ".\mainfrm.h"

#include "DxObjectMRS.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelMainFrame

IMPLEMENT_DYNCREATE(CLevelMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CLevelMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_MENU_NONE, OnEditMenuNone)
	ON_COMMAND(ID_EDIT_MENU_SELECT, OnEditMenuSelect)
	ON_COMMAND(ID_EDIT_MENU_MOVE, OnEditMenuMove)
	ON_COMMAND(ID_EDIT_MENU_ROTATE, OnEditMenuRotate)
	ON_COMMAND(ID_EDIT_MENU_SCALE, OnEditMenuScale)
	ON_COMMAND(ID_EDIT_MENU_SELECT_MOBSCH, OnEditMenuSelectMobsch)
	ON_COMMAND(ID_FILE_TOOLBAR_ALL90, OnFileToolbarAll90)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CLevelMainFrame construction/destruction

CLevelMainFrame::CLevelMainFrame()
{
	// TODO: add member initialization code here
}

CLevelMainFrame::~CLevelMainFrame()
{
}


int CLevelMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolBar.CreateComboBox ( m_wndToolBar.m_comboMatrix, 14, ID_FILE_TOOLBAR_COMBOBOX, 65, 100 ) )
	{
		TRACE0("Failed to create combo box in toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.m_comboMatrix.InsertString ( 0, "World" );
	m_wndToolBar.m_comboMatrix.InsertString ( 1, "View" );
	m_wndToolBar.m_comboMatrix.InsertString ( 2, "Local" );
	m_wndToolBar.m_comboMatrix.SetCurSel ( 0 );

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitle ( "Configuration" );

	if (!m_wndDialogBar.Create ( this, &m_DlgLevelEdit, strTitle, IDD_LEVEL_EDIT_DIALOG ) )
	{
		TRACE0("Failed to create combo box in toolbar\n");
		return -1;      // fail to create
	}

	DWORD dwStyle = m_wndDialogBar.GetBarStyle ();
	m_wndDialogBar.SetBarStyle ( dwStyle | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	m_wndDialogBar.EnableDocking ( CBRS_ALIGN_ANY );
	DockControlBar(&m_wndDialogBar, AFX_IDW_DOCKBAR_RIGHT );

	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		m_strAppPath = strAppPath.Left ( strAppPath.ReverseFind ( '\\' ) );
		
		if ( !m_strAppPath.IsEmpty() )
		if ( m_strAppPath.GetAt(0) == '"' )
			m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );
	}

	strcpy_s ( m_szAppPath, m_strAppPath.GetString() );

	return 0;
}

BOOL CLevelMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_MAXIMIZE;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CLevelMainFrame diagnostics

#ifdef _DEBUG
void CLevelMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CLevelMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CLevelMainFrame message handlers
void CLevelMainFrame::OnEditMenuNone()
{
	// TODO: Add your command handler code here
	CLevelEditorView* pView = (CLevelEditorView*)GetActiveView();
	pView->SetCurType ( CUR_NONE );
}

void CLevelMainFrame::OnEditMenuSelect()
{
	// TODO: Add your command handler code here
	CLevelEditorView* pView = (CLevelEditorView*)GetActiveView();
	pView->SetCurType ( CUR_SELECT );
}

void CLevelMainFrame::OnEditMenuMove()
{
	// TODO: Add your command handler code here
	CLevelEditorView* pView = (CLevelEditorView*)GetActiveView();
	pView->SetCurType ( CUR_MOVE );

	if ( pView )
	{
		pView->SetCurEditType ( OBECTMRS_I );
	}
}

void CLevelMainFrame::OnEditMenuRotate()
{
	// TODO: Add your command handler code here
	CLevelEditorView* pView = (CLevelEditorView*)GetActiveView();
	pView->SetCurType ( CUR_ROTATE );

	if ( pView )
	{
		pView->SetCurEditType ( OBECTMRS_M );
	}
}

void CLevelMainFrame::OnEditMenuScale()
{
	// TODO: Add your command handler code here
	CLevelEditorView* pView = (CLevelEditorView*)GetActiveView();
	pView->SetCurType ( CUR_SCALE );

	if ( pView )
	{
		pView->SetCurEditType ( OBECTMRS_R );
	}
}

void CLevelMainFrame::OnEditMenuSelectMobsch()
{
	// TODO: Add your command handler code here
	CLevelEditorView* pView = (CLevelEditorView*)GetActiveView();
	pView->SetCurType ( CUR_SEL_MOBSCH );
}

void CLevelMainFrame::OnFileToolbarAll90()
{
	// TODO: Add your command handler code here
	if ( MessageBox ( "All object will rotate 90. Are you sure you want to process ?", "Caution", MB_YESNO ) == IDYES )
	{
		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		pGLLandMan->SetObjRotate90();
	}
}
