#pragma once

class DxEffProjSpeed
{
protected:
	BOOL	m_bSpeedEffON;

public:
	void	EnableSpeedEff( BOOL bUse );	// �ӵ� ȿ���� ON/OFF

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjSpeed();
	~DxEffProjSpeed();
};