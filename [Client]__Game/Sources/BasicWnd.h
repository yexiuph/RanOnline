// PageWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBasicWnd window

#pragma once

#include "DxLightMan.h"
#include "DxLandMan.h"

class CBasicWnd : public CWnd, public CD3DApplication
{
private:
	int			m_nScreenWinX;
	int			m_nScreenWinY;
	int			m_wndSizeX;
	int			m_wndSizeY;

	HCURSOR		m_hCursorDefault;

private:
	BOOL		m_bCreated;
	std::string m_strTEXTURE;

	UINT_PTR	m_nGGTimer;
	UINT_PTR	m_nGGATimer;
	UINT_PTR	m_nGGA12Timer;

	HANDLE		m_hMutex;

public:
	LPDIRECT3DDEVICEQ GetD3dDevice ()	{ return m_pd3dDevice; }

public:
	HRESULT InitDeviceObjects();

private:
	HRESULT ConfirmDevice( D3DCAPSQ*,DWORD,D3DFORMAT );
	HRESULT OneTimeSceneInit();
	HRESULT CreateObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	HRESULT RenderText();
	HRESULT ReSizeWindow ( int cx, int cy )
	{
		if ( cx < 10 )	cx = 10;
		if ( cy < 10 )	cy = 10;

		m_d3dpp.BackBufferWidth  = cx;
		m_d3dpp.BackBufferHeight = cy;

		return Resize3DEnvironment();
	}

	virtual HRESULT Resize3DEnvironment();

public:
	virtual HRESULT FrameMove3DEnvironment();
	virtual HRESULT Render3DEnvironment();

// Construction
public:
	CBasicWnd();

// Attributes
public:
  CBasicApp* m_pApp;

// Operations
public:
  BOOL Create();
 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBasicWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBasicWnd)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnNetNotify(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
#if _MSC_VER >= 1200
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

/////////////////////////////////////////////////////////////////////////////
