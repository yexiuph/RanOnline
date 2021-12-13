#pragma once

#include "DxMethods.h"

struct DxFrame;
class DxFrameMesh;
class DxLandMan;
class DxOctree;
class DxSkinChar;

struct DxEffectType;
class DxEffectBase;

class DxPieceEffMan
{
protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	DxEffectType	*pEffectTypeList;

public:
	DxEffectType* GetEffectList ()	{ return pEffectTypeList; }

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffectBase* CreateEffInstance ( DWORD TypeID );
	DxEffectBase* AdaptToDxFrame ( DWORD TypeID, DxFrameMesh *pFrameMesh, DxFrame *pFrame,
		LPDIRECT3DDEVICEQ pd3dDevice, PBYTE pProperty=NULL, DWORD dwSize=0, DWORD dwVer=0, LPDXAFFINEPARTS pAffineParts=NULL );

protected:
	DxPieceEffMan();

public:
	virtual ~DxPieceEffMan();

public:
	static DxPieceEffMan& GetInstance();
};
