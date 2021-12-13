#pragma once

class DxBackUpRendTarget
{
protected:
	LPDIRECT3DSURFACEQ	m_pTempSurface;
	LPDIRECT3DSURFACEQ	m_pTempZBuffer;	

public:
	DxBackUpRendTarget (void);
	DxBackUpRendTarget ( LPDIRECT3DDEVICEQ pd3dDevice );

	void RestoreTarget ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	~DxBackUpRendTarget (void);
};
