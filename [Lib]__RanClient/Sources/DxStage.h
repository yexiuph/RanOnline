#ifndef DXSTAGE_H_
#define DXSTAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Dependency/NetGlobal/s_NetGlobal.h"

class DxStage
{
protected:
	char				m_szAppPath[MAX_PATH];
	HWND				m_hWnd;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	virtual HRESULT SetActive ( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd, WORD wWidth, WORD wHeight )
	{
		HRESULT hr;

		hr = OneTimeSceneInit ( hWnd, wWidth, wHeight, m_szAppPath );
		if (FAILED(hr) )	return hr;

		hr = InitDeviceObjects ( pd3dDevice );
		if (FAILED(hr) )	return hr;

		hr = RestoreDeviceObjects ();
		if (FAILED(hr) )	return hr;

		return S_OK;
	}

	virtual HRESULT DeActive ()
	{
		HRESULT hr;

		hr = InvalidateDeviceObjects ();
		if (FAILED(hr) )	return hr;

		hr = DeleteDeviceObjects ();
		if (FAILED(hr) )	return hr;

		hr = FinalCleanup ();
		if (FAILED(hr) )	return hr;

		return S_OK;
	}

public:
	virtual HRESULT OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath )	{ return S_OK; }
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects ()								{ return S_OK; }
	virtual HRESULT InvalidateDeviceObjects ()							{ return S_OK; }
	virtual HRESULT DeleteDeviceObjects ()								{ return S_OK; }
	virtual HRESULT FinalCleanup ()										{ return S_OK; }

public:
	virtual HRESULT FrameMove ( float m_fTime, float m_fElapsedTime )	{ return S_OK; }
	virtual HRESULT Render ()											{ return S_OK; }

	virtual void MsgProcess ( NET_MSG_GENERIC* nmg ) = 0;
	virtual HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight )			{ return S_OK; }

public:
	DxStage () :
		m_hWnd(NULL),
		m_pd3dDevice(NULL)
	{
	}
	~DxStage ()
	{
	}
};

#endif // DXSTAGE_H_