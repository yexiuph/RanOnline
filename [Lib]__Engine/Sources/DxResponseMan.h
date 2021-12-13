#ifndef DXRESPONSEMAN_H_
#define DXRESPONSEMAN_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------[DxResponseMan]
//
//	�������� ��ġ Ŭ�������� ����, ������ ��Ƽ� ó���ϱ� ���ؼ� �����.
//	

class DxResponseMan
{
protected:
	char m_szFullPath[MAX_PATH];
	
protected:
	CWnd*				m_pWndApp;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	LPDIRECT3DQ			m_pD3D;
	D3DCAPSQ			m_d3dCaps;
	D3DSURFACE_DESC		m_d3dsdBackBuffer;

public:
	HRESULT SetRenderState();

public:
	HRESULT DoInterimClean( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT OneTimeSceneInit(
        const char* szPath,
		CWnd *pWndApp,
		CString strFontSys,
		DWORD dwLang=0,
		CString strFontName="Tahoma" );

	HRESULT InitDeviceObjects(
		LPDIRECT3DDEVICEQ pd3dDevice,
		BOOL bCAPTUREMOUSE=FALSE );

	HRESULT RestoreDeviceObjects();

	HRESULT InvalidateDeviceObjects();

	HRESULT DeleteDeviceObjects();

	HRESULT FinalCleanup();

	HRESULT FrameMove(
		float fTime,
		float fElapsedTime,
		BOOL bDefWin,
		BOOL bGame=FALSE );
	//HRESULT Render ();

	LRESULT MsgProc( MSG* pMsg );

protected:
	DxResponseMan(void);

public:
	~DxResponseMan(void);

public:
	static DxResponseMan& GetInstance();
};

namespace LOADINGSTEP
{
	void RESET();
	void SETSTEP( int nStep );
	int GETSTEP();
};

#endif // DXRESPONSEMAN_H_