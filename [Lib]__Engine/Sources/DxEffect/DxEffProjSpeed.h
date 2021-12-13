#pragma once

class DxEffProjSpeed
{
protected:
	BOOL	m_bSpeedEffON;

public:
	void	EnableSpeedEff( BOOL bUse );	// 속도 효과의 ON/OFF

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjSpeed();
	~DxEffProjSpeed();
};