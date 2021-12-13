#include "pch.h"

#include "./DxTexEffDiffuse.h"
#include "./DxTexEffFlowUV.h"
#include "./DxTexEffRotate.h"
#include "./DxTexEffSpecular.h"

#include "./DxTexEffMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxTexEffMan& DxTexEffMan::GetInstance()
{
	static DxTexEffMan Instance;
	return Instance;
}

// Note : DxTextureEffMan
DxTexEffMan::DxTexEffMan() :
	pTexEffTypeList(NULL)
{
	RegistType( DxTexEffDiffuse::TYPEID,	DxTexEffDiffuse::NAME );
	RegistType( DxTexEffFlowUV::TYPEID,		DxTexEffFlowUV::NAME );
	RegistType( DxTexEffRotate::TYPEID,		DxTexEffRotate::NAME );
	RegistType( DxTexEffSpecular::TYPEID,	DxTexEffSpecular::NAME );
}

DxTexEffMan::~DxTexEffMan()
{
	SAFE_DELETE(pTexEffTypeList);
}

void DxTexEffMan::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxTexEffDiffuse::OnCreateDevice( pd3dDevice );
	DxTexEffFlowUV::OnCreateDevice( pd3dDevice );
	DxTexEffRotate::OnCreateDevice( pd3dDevice );
	DxTexEffSpecular::OnCreateDevice( pd3dDevice );
}

void DxTexEffMan::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxTexEffDiffuse::OnReleaseDevice( pd3dDevice );
	DxTexEffFlowUV::OnReleaseDevice( pd3dDevice );
	DxTexEffRotate::OnReleaseDevice( pd3dDevice );
	DxTexEffSpecular::OnReleaseDevice( pd3dDevice );
}

void DxTexEffMan::RegistType( const DWORD TypeID, const char *Name )
{
	//	Note : 새 노드 생성후 대입.
	DxTexEffType *pNewType = new DxTexEffType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNextType = pTexEffTypeList;
	pTexEffTypeList = pNewType;
}

DxTexEffBase* DxTexEffMan::CreateEffInstance( DWORD TypeID )
{
	DxTexEffBase* pTexEffBase = NULL;

	if( DxTexEffDiffuse::TYPEID == TypeID )			pTexEffBase = new DxTexEffDiffuse();
	else if( DxTexEffFlowUV::TYPEID == TypeID )		pTexEffBase = new DxTexEffFlowUV();
	else if( DxTexEffRotate::TYPEID == TypeID )		pTexEffBase = new DxTexEffRotate();
	else if( DxTexEffSpecular::TYPEID == TypeID )	pTexEffBase = new DxTexEffSpecular();

	return pTexEffBase;
}

DxTexEffBase* DxTexEffMan::AdaptToDxFrame( DWORD TypeID, LPDIRECT3DDEVICEQ pd3dDevice, PBYTE pProperty )
{
	//	Note : Effect Instance 생성.
	DxTexEffBase* pEffBase = CreateEffInstance( TypeID );
	if( !pEffBase ) return NULL;

	//	Note : Property, Affine  값을 대입.
	if( pProperty )		pEffBase->SetProperty( pd3dDevice, pProperty );

	return pEffBase;
}



