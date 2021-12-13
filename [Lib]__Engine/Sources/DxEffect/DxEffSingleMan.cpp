#include "pch.h"
#include "./DxEffSingleMan.h"
#include "./DxEffectParticleSys.h"
#include "./DxEffectGround.h"
#include "./DxEffectMesh.h"
#include "./DxEffectSequence.h"
#include "./DxEffectDecal.h"
#include "./DxEffectCamera.h"
#include "./DxEffectBlurSys.h"
#include "./DxEffectLighting.h"
#include "./DxEffectMoveRotate.h"
#include "./DxEffectSkinMesh.h"
#include "./DxEffectWave.h"
#include "./DxEffectPointLight.h"
#include "./DxEffectMoveTarget.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxEffSingleMan& DxEffSingleMan::GetInstance()
{
	static DxEffSingleMan Instance;
	return Instance;
}

DxEffSingleMan::DxEffSingleMan(void) :
	pEffectTypeList(NULL),
	m_pLandMan(NULL)
{
	RegistType ( DxEffectParticleSys::TYPEID, DxEffectParticleSys::NAME );
	RegistType ( DxEffectGround::TYPEID, DxEffectGround::NAME );
	RegistType ( DxEffectMesh::TYPEID, DxEffectMesh::NAME );
	RegistType ( DxEffectSequence::TYPEID, DxEffectSequence::NAME );
	RegistType ( DxEffectDecal::TYPEID, DxEffectDecal::NAME );
	RegistType ( DxEffectCamera::TYPEID, DxEffectCamera::NAME );
	RegistType ( DxEffectBlurSys::TYPEID, DxEffectBlurSys::NAME );
	RegistType ( DxEffectLightning::TYPEID, DxEffectLightning::NAME );
	RegistType ( DxEffectMoveRotate::TYPEID, DxEffectMoveRotate::NAME );
	RegistType ( DxEffectSkinMesh::TYPEID, DxEffectSkinMesh::NAME );
	RegistType ( DxEffectWave::TYPEID, DxEffectWave::NAME );
	RegistType ( DxEffectPointLight::TYPEID, DxEffectPointLight::NAME );
	RegistType ( DxEffectMoveTarget::TYPEID, DxEffectMoveTarget::NAME );
}

DxEffSingleMan::~DxEffSingleMan(void)
{
	SAFE_DELETE(pEffectTypeList);
}

void DxEffSingleMan::RegistType ( const DWORD TypeID, const char *Name )
{
	GASSERT(Name);

	DxEffSingleType *pCurEff = pEffectTypeList;
	while(pCurEff)
	{
		GASSERT ( pCurEff->TYPEID != TypeID && "TypeID 중복 등록될수 없습니다." );
		GASSERT ( strcmp(pCurEff->NAME,Name) && "NAME가 중복 등록될수 없습니다." );

		pCurEff = pCurEff->pNext;
	}

	//	Note : 새 노드 생성후 대입.
	//
	DxEffSingleType *pNewType = new DxEffSingleType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNext = pEffectTypeList;
	pEffectTypeList = pNewType;
}

HRESULT DxEffSingleMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectParticleSys::CreateDevice ( pd3dDevice );
	DxEffectGround::CreateDevice ( pd3dDevice );
	DxEffectMesh::CreateDevice ( pd3dDevice );
	DxEffectSequence::CreateDevice ( pd3dDevice );
	DxEffectDecal::CreateDevice ( pd3dDevice );
	DxEffectCamera::CreateDevice ( pd3dDevice );
	DxEffectBlurSys::CreateDevice ( pd3dDevice );
	DxEffectLightning::CreateDevice ( pd3dDevice );
	DxEffectMoveRotate::CreateDevice ( pd3dDevice );
	DxEffectSkinMesh::CreateDevice ( pd3dDevice );
	DxEffectWave::CreateDevice ( pd3dDevice );
	DxEffectPointLight::CreateDevice ( pd3dDevice );
	DxEffectMoveTarget::CreateDevice ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingleMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectParticleSys::ReleaseDevice ( pd3dDevice );
	DxEffectGround::ReleaseDevice ( pd3dDevice );
	DxEffectMesh::ReleaseDevice ( pd3dDevice );
	DxEffectSequence::ReleaseDevice ( pd3dDevice );
	DxEffectDecal::ReleaseDevice ( pd3dDevice );
	DxEffectCamera::ReleaseDevice ( pd3dDevice );
	DxEffectBlurSys::ReleaseDevice ( pd3dDevice );
	DxEffectLightning::ReleaseDevice ( pd3dDevice );
	DxEffectMoveRotate::ReleaseDevice ( pd3dDevice );
	DxEffectSkinMesh::ReleaseDevice ( pd3dDevice );
	DxEffectWave::ReleaseDevice ( pd3dDevice );
	DxEffectPointLight::ReleaseDevice ( pd3dDevice );
	DxEffectMoveTarget::ReleaseDevice ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffSingleMan::OneTimeSceneInit()
{
	DxEffectParticleSys::OnInitDevice_STATIC();
	DxEffectGround::OnInitDevice_STATIC();
	DxEffectMesh::OnInitDevice_STATIC();
	DxEffectSequence::OnInitDevice_STATIC();
	DxEffectDecal::OnInitDevice_STATIC();
	DxEffectCamera::OnInitDevice_STATIC();
	DxEffectBlurSys::OnInitDevice_STATIC();
	DxEffectLightning::OnInitDevice_STATIC();
	DxEffectMoveRotate::OnInitDevice_STATIC();
	DxEffectSkinMesh::OnInitDevice_STATIC();
	DxEffectWave::OnInitDevice_STATIC();
	DxEffectPointLight::OnInitDevice_STATIC();
	DxEffectMoveTarget::OnInitDevice_STATIC();

	return S_OK;
}

HRESULT DxEffSingleMan::FinalCleanup()
{
	DxEffectParticleSys::OnDeleteDevice_STATIC();
	DxEffectGround::OnDeleteDevice_STATIC();
	DxEffectMesh::OnDeleteDevice_STATIC();
	DxEffectSequence::OnDeleteDevice_STATIC();
	DxEffectDecal::OnDeleteDevice_STATIC();
	DxEffectCamera::OnDeleteDevice_STATIC();
	DxEffectBlurSys::OnDeleteDevice_STATIC();
	DxEffectLightning::OnDeleteDevice_STATIC();
	DxEffectMoveRotate::OnDeleteDevice_STATIC();
	DxEffectSkinMesh::OnDeleteDevice_STATIC();
	DxEffectWave::OnDeleteDevice_STATIC();
	DxEffectPointLight::OnDeleteDevice_STATIC();
	DxEffectMoveTarget::OnDeleteDevice_STATIC();

	return S_OK;
}


