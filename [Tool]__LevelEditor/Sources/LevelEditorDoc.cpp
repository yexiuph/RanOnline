// LevelEditorDoc.cpp : implementation of the CLevelEditorDoc class
//

#include "pch.h"
#include "LevelEditor.h"

#include "LevelEditorDoc.h"

#include "MainFrm.h"
#include "LevelEditorView.h"

#include "GLOGIC.h"

#include "GLGaeaServer.h"
#include "GLLandMan.h"
#include ".\leveleditordoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelEditorDoc

IMPLEMENT_DYNCREATE(CLevelEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CLevelEditorDoc, CDocument)
	ON_COMMAND(ID_FILE_MENU_OPEN, OnFileMenuOpen)
	ON_COMMAND(ID_FILE_MENU_SAVE, OnFileMenuSave)
END_MESSAGE_MAP()


// CLevelEditorDoc construction/destruction

CLevelEditorDoc::CLevelEditorDoc()
{
	// TODO: add one-time construction code here

}

CLevelEditorDoc::~CLevelEditorDoc()
{
}

BOOL CLevelEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CLevelEditorDoc serialization

void CLevelEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CLevelEditorDoc diagnostics

#ifdef _DEBUG
void CLevelEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLevelEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLevelEditorDoc commands
void CLevelEditorDoc::OnCloseDocument()
{
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView*) pFrame->GetActiveView();

	GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
	CString strFileName = pGLLandMan->GetFileName();

	if ( !strFileName.GetLength() )
		strFileName = "untitle.lev";

	strFileName += ".last";

	pView->SaveGLLandMan ( strFileName.GetString() );
	CDocument::OnCloseDocument();
}

void CLevelEditorDoc::OnFileMenuOpen()
{
	// TODO: Add your command handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView*) pFrame->GetActiveView();

	CString szFilter = "Level File (*.Lev)|*.Lev|";

	CFileDialog dlg(TRUE,".Lev","untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName();
		const char* szWriteName = WriteName.GetString();

		GLLandMan* pGLLandMan = GLGaeaServer::GetInstance().GetRootMap();
		pGLLandMan->InvalidateDeviceObjects();
		pGLLandMan->DeleteDeviceObjects();

		GLGaeaServer::GetInstance().CleanUp();

		LPDIRECT3DDEVICEQ pd3dDevice = pView->GetD3DDevice();

		GLGaeaServer::GetInstance().Create4Level ( pd3dDevice );
		GLGaeaServer::GetInstance().SetUpdate ( FALSE );

		pView->LoadGLLandMan ( szWriteName );
		WriteName += ".bak";

		pView->SaveGLLandMan ( WriteName.GetString() );
		pFrame->SetWindowText ( dlg.GetFileName().GetString() );
	}
}

void CLevelEditorDoc::OnFileMenuSave()
{
	// TODO: Add your command handler code here
	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetApp()->m_pMainWnd;
	CLevelEditorView *pView = (CLevelEditorView*) pFrame->GetActiveView();

	CString szFilter = "Level File (*.Lev)|*.Lev|";

	CFileDialog dlg(FALSE,".Lev","untitled",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();

	if ( dlg.DoModal() == IDOK )
	{
		CString WriteName = dlg.GetFileName();
		const char* szWriteName = WriteName.GetString();

		pView->SaveGLLandMan ( szWriteName );
		pFrame->SetWindowText ( dlg.GetFileName().GetString() );
	}
}
