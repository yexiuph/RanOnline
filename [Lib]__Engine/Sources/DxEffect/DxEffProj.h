#pragma once

#include "./DxEffProjGetItem.h"

// Note : �����, ����, ������ Get, ��, ���� ��...  ���� �� �ִ�.
class DxEffProjMan
{
protected:
	DxEffProjGetItem	m_sGetItemEFF;

public:
	void EnableGetItemEFF( BOOL bUse )		{ m_sGetItemEFF.EnableEFF( bUse ); }

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnDestroyDevice();

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffProjMan();
	~DxEffProjMan();

public:
	static DxEffProjMan& GetInstance();
};