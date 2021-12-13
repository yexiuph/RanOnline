#pragma once

#include "DxFrameMesh.h"

class DxEditBox
{
public:
	enum EMEDIT
	{
		EM_MOVE			= 0,
		EM_SCALE		= 1,
		EM_SELECTSCALE	= 2,

		EM_SIZE			= 3
	};

protected:
	char		m_szPath[MAX_PATH];
	EMEDIT		m_emEditType;
	D3DXVECTOR2	m_vTypePos[EM_SIZE];
	SIZE		m_TypeSize[EM_SIZE];

	DxFrameMesh	m_CurMesh;
	LPDXFRAME	m_pDxFrameSel;

public:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

public:
	void SetPath ( char *szPath )	{ StringCchCopy( m_szPath, MAX_PATH, szPath ); }

public:
	void SetBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEditBox(void);
	~DxEditBox(void);
};
