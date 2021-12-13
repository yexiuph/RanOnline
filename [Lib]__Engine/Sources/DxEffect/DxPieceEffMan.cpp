#include "pch.h"

#include "./DxFrameMesh.h"

#include "./DxEffectCloth.h"

#include "./DxPieceEffMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxPieceEffMan& DxPieceEffMan::GetInstance()
{
	static DxPieceEffMan Instance;
	return Instance;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxPieceEffMan::DxPieceEffMan() :
	pEffectTypeList(NULL)
{
	RegistType ( DxEffectCloth::TYPEID,			DxEffectCloth::NAME );
}

DxPieceEffMan::~DxPieceEffMan()
{
	SAFE_DELETE(pEffectTypeList);
}

void DxPieceEffMan::RegistType ( const DWORD TypeID, const char *Name )
{
	//	Note : 새 노드 생성후 대입.
	//
	DxEffectType *pNewType = new DxEffectType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNextType = pEffectTypeList;
	pEffectTypeList = pNewType;
}

void DxPieceEffMan::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectCloth::CreateDevice ( pd3dDevice );
}

void DxPieceEffMan::OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectCloth::ReleaseDevice ( pd3dDevice );
}

DxEffectBase* DxPieceEffMan::CreateEffInstance ( DWORD TypeID )
{
	DxEffectBase* pEffectBase = NULL;

	if( DxEffectCloth::TYPEID==TypeID )		pEffectBase = new DxEffectCloth ();

	return pEffectBase;
}

DxEffectBase* DxPieceEffMan::AdaptToDxFrame ( DWORD TypeID, DxFrameMesh *pFrameMesh, DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, 
											 PBYTE pProperty, DWORD dwSize, DWORD dwVer, LPDXAFFINEPARTS pAffineParts )
{
	HRESULT hr;

	//	Note : Effect Instance 생성.
	DxEffectBase* pEffBase = CreateEffInstance( TypeID );
	if ( !pEffBase ) return NULL;

	//	Note : Property, Affine  값을 대입.
	if ( pProperty )	pEffBase->SetProperty( pProperty, dwSize, dwVer );
	if ( pAffineParts && pEffBase->IsUseAffineMatrix() )	pEffBase->SetAffineValue ( pAffineParts );

	pEffBase->AdaptToDxFrame ( pFrame, pd3dDevice );

	hr = pEffBase->Create ( pd3dDevice );
	if ( FAILED(hr) )	goto E_ERROR;

	pFrameMesh->AddEffect( pEffBase );

	if ( pEffBase->GetFlag () & _EFF_REPLACE )
	{
		//	Note : 알파맵 리스트에서 제거.
		pFrameMesh->DelAlphaMapFrame ( pFrame );
	}

	return pEffBase;

E_ERROR:
	pEffBase->CleanUp();
	SAFE_DELETE(pEffBase);
	return NULL;
}