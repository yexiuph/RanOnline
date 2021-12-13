// LevelEditorView.h : interface of the CLevelEditorView class
//


#pragma once

#include "d3dapp.h"

#include "DxMethods.h"
#include "DxEditBox.h"
#include "DxObjectMRS.h"
#include "DxLandMan.h"

class DxFrameMesh;
class DxEditMat;

class CLevelEditorDoc;

enum CUR_TYPE
{
	CUR_SEL_MOBSCH		= 0x008C,
	CUR_NONE			= 0x8003,
	CUR_SELECT			= 0x8004,
	CUR_MOVE			= 0x8005,
	CUR_ROTATE			= 0x8006,
	CUR_SCALE			= 0x8007,
	CUR_EDIT_BOX		= 0x800C,
};

class CLevelEditorView : public CView, public CD3DApplication
{
protected: // create from serialization only
	CLevelEditorView();
	DECLARE_DYNCREATE(CLevelEditorView)

// Attributes
public:
	CLevelEditorDoc* GetDocument() const;

public:
	static BOOL			m_bRENDQUAD;
	static DxLandMan	m_LandMan;

	BOOL				m_bCreated;

	DxFrameMesh*		m_p3dPosMesh;
	DxEditMat*			m_pDxEditmat;

	DxEditBox			m_EditBox;
	DxObjectMRS			m_sObjectMRS;

	D3DXVECTOR3			m_vCollisionPos;			

private:
	CUR_TYPE			m_CurType;

// Operations
public:
	virtual void OnInitialUpdate();

	void	ActiveEditBox ( D3DXVECTOR3 vMax, D3DXVECTOR3 vMin );
	void	ActiveEditMatrix ( DXAFFINEMATRIX *pAffineMatrix );
	void	ActiveEditMatrix2 ( D3DXMATRIX *matWorld );

	void	DeActiveEditBox () { m_CurType = CUR_NONE; }
	void	DeActiveEditMatrix ();

	void	CurMove();
	void	CurSelect();

	void	SetCurType ( CUR_TYPE cType ) { m_CurType = cType; }
	void	SetCurEditType ( DWORD EditType );

	void	SetActive ( BOOL bActive );

	D3DXVECTOR3*	GetCollisionPos() { return &m_vCollisionPos; }
	DxEditBox*		GetEditBox() { return &m_EditBox; }

	LPDIRECT3DDEVICEQ GetD3DDevice () { return m_pd3dDevice; }

	BOOL	SaveGLLandMan ( const char* szFile );
	BOOL	LoadGLLandMan ( const char* szFile );

private:
	HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT CreateObjects();
	HRESULT InitDeviceObjects();	
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	HRESULT RenderText();
	void	RenderLandEffectState();

	HRESULT ReSizeWindow ( int cx, int cy )
	{
		if ( cx < 10 )	cx = 10;
		if ( cy < 10 )	cy = 10;

		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

		return Resize3DEnvironment();
	}

protected:
	virtual void PostNcDestroy();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual HRESULT FrameMove3DEnvironment();
	virtual HRESULT Render3DEnvironment();

protected:

// Implementation
public:
	virtual ~CLevelEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in LevelEditorView.cpp
inline CLevelEditorDoc* CLevelEditorView::GetDocument() const
   { return reinterpret_cast<CLevelEditorDoc*>(m_pDocument); }
#endif

