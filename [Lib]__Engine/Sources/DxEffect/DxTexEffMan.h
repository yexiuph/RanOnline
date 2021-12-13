#pragma once

#include "DxTexEff.h"

class DxTexEffMan
{
protected:
	void RegistType( const DWORD TypeID, const char *Name );

protected:
	DxTexEffType*	pTexEffTypeList;

public:
	DxTexEffType* GetTexEffList ()	{ return pTexEffTypeList; }

public:
	DxTexEffBase* CreateEffInstance( DWORD TypeID );
	DxTexEffBase* AdaptToDxFrame( DWORD TypeID, LPDIRECT3DDEVICEQ pd3dDevice, PBYTE pProperty );

public:
	void OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxTexEffMan();
	~DxTexEffMan();

public:
	static DxTexEffMan& GetInstance();
};
