// paddoc.cpp : implementation of the CPadDoc class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "pch.h"
#include "superpad.h"
#include "paddoc.h"
#include "paditem.h"
#include "linkitem.h"
#include "paddoc.h"
#include "../[Lib]__Engine/Sources/Common/StringFile.h"
#include "../[Lib]__Engine/Sources/Common/Rijndael.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CPadDoc

IMPLEMENT_DYNCREATE(CPadDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CPadDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CPadDoc)
	ON_COMMAND(ID_VIEW_UPDATENOW, OnViewUpdatenow)
	ON_COMMAND(ID_CANCEL_INPLACE, OnCancelInplace)
//	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
//	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPadDoc delegation to CEditView

CPadDoc::CPadDoc()
{
}

CPadDoc::~CPadDoc()
{
}

void CPadDoc::DeleteContents()
{
	COleServerDoc::DeleteContents();
	if (m_viewList.IsEmpty())
		return;
	CEditView* pView = (CEditView*)m_viewList.GetHead();
	ASSERT_KINDOF(CEditView, pView);
	pView->DeleteContents();
}

void CPadDoc::Serialize(CArchive& ar)
{
	CEditView* pView = (CEditView*)m_viewList.GetHead();
	ASSERT_KINDOF(CEditView, pView);
	pView->SerializeRaw(ar);
}

COleServerItem* CPadDoc::OnGetEmbeddedItem()
{
	CEmbeddedItem* pItem = new CEmbeddedItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

COleServerItem* CPadDoc::OnGetLinkedItem(LPCTSTR lpszItemName)
{
	CPadLinkItem *pItem =
		(CPadLinkItem*)COleServerDoc::OnGetLinkedItem(lpszItemName);
	if (pItem == NULL)
		pItem = new CPadLinkItem(this, lpszItemName);

	ASSERT_VALID(pItem);
	return pItem;
}

/////////////////////////////////////////////////////////////////////////////

void CPadDoc::OnViewUpdatenow()
{
	UpdateAllItems(NULL);
}

// Note: both the server and the container should have a keyboard method
//  of deactivating an active in-place item.

void CPadDoc::OnCancelInplace()
{
	if (IsInPlaceActive())
		OnDeactivateUI(FALSE);
}

void CPadDoc::SetSelection(int nBeg, int nEnd)
{
	CEditView* pView = (CEditView*)m_viewList.GetHead();
	ASSERT_KINDOF(CEditView, pView);
	pView->GetEditCtrl().SetSel(nBeg, nEnd);
}

BOOL CPadDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!COleServerDoc::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO: Add specialized writing code here.
	char ext[_MAX_EXT] = {0};
	_splitpath_s( lpszPathName, NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT );

	CString strExt(ext);
	strExt.MakeUpper();

	// TODO: Add new supported extensions ex. ( .LUA,.INI & etc. )
	if( strExt != _T(".CHARSET") && strExt != _T(".CHARCLASS") &&
		strExt != _T(".CLASSCONST") && strExt != _T(".CROWSALE") &&
		strExt != _T(".GENITEM") && strExt != _T(".BIN") )
	{
		// Fixed rubbish question - YeXiuPH - 03-25-2020
		if( IDNO == AfxMessageBox( "Would you like to decode?", MB_YESNO ) )
			return TRUE;
	}

	CStringFile StrFile;
	if ( !StrFile.Open ( lpszPathName, TRUE ) ) // Open and decode string file class.
		return FALSE;

	CString strTemp, strLine;
	while ( StrFile.GetNextLine(strLine) ) // Read one line from a file.
	{
		strLine += "\r\n";
		strTemp += strLine;
	}

	strTemp.SetAt(strTemp.GetLength()-2, NULL); // Remove the last \ r \ n.
	// output to edit view
	reinterpret_cast<CEditView*>(m_viewList.GetHead())->SetWindowText(strTemp.GetString());

	return TRUE;
}

BOOL CPadDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add specialized code here and / or call base classes.
	char ext[_MAX_EXT] = {0};
	_splitpath_s( lpszPathName, NULL, 0, NULL, 0, NULL, 0, ext, _MAX_EXT );

	CString strExt(ext);
	strExt.MakeUpper();

	if( strExt != _T(".CHARSET") && strExt != _T(".CHARCLASS") &&
		strExt != _T(".CLASSCONST") && strExt != _T(".CROWSALE") &&
		strExt != _T(".GENITEM") && strExt != _T(".BIN") )
	{
		// Fixed rubbish question - YeXiuPH - 03-25-2020
		if( IDNO == AfxMessageBox( "Would you like to encode?", MB_YESNO ) )
			return COleServerDoc::OnSaveDocument(lpszPathName);
	}

	
	CString strTemp;
	// Read the text in EditView.
	reinterpret_cast<CEditView*>(m_viewList.GetHead())->GetWindowText(strTemp);

	CRijndael oRijndael;
	oRijndael.Initialize( CRijndael::sm_Version[CRijndael::VERSION-1].c_str(), CRijndael::sm_KeyLength[CRijndael::VERSION-1], CRijndael::VERSION_DATE, CRijndael::VERSION);

	int nLen = oRijndael.GetEncryptStringLength( strTemp );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strTemp.GetString(), szBuffer, strTemp.GetLength() );
	int nVersion = CRijndael::VERSION;

	CFile file;
	file.Open( lpszPathName, CFile::modeCreate|CFile::modeWrite );	// Open file
	file.Write( &nVersion, sizeof(int) );							// Write version
	file.Write( szBuffer, nLen );		// Write version
	file.Close();

	delete [] szBuffer;

	SetModifiedFlag(FALSE);
	return TRUE;
}