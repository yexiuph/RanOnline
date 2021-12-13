// DxEffectReflect.cpp: implementation of the DxEffectReflect class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxSurfaceTex.h"
#include "./DxEnvironment.h"
#include "./SerialFile.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"

#include "./DxEffectReflect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectReflect::TYPEID = DEF_EFFECT_REFLECT;
const DWORD	DxEffectReflect::VERSION = 0x0100;
const char DxEffectReflect::NAME[] = "[ 평면반사효과 (단순) ]";
const DWORD DxEffectReflect::FLAG = _EFF_SINGLE_AFTER_0;

void DxEffectReflect::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(REFLECT_PROPERTY);
	dwVer = VERSION;
}

void DxEffectReflect::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(REFLECT_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

LPDIRECT3DSTATEBLOCK9	DxEffectReflect::m_pSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectReflect::m_pSB_SAVE = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectReflect::DxEffectReflect() :
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),
	m_vCenter(0.f,0.f,0.f),
	m_dwPower(40),
	m_dwColor(0xffffffff)
{
}

DxEffectReflect::~DxEffectReflect()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

HRESULT DxEffectReflect::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0001f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0x88ffffff );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB );
	}

	return S_OK;
}

HRESULT DxEffectReflect::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSB );
	SAFE_RELEASE( m_pSB_SAVE );

	return S_OK;
}

HRESULT DxEffectReflect::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				CreateVB( pd3dDevice, pmsMeshs, pframeCur->matCombined );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectReflect::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectReflect::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pAdaptFrame = NULL;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectReflect::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )	
{ 
	m_dwColor = m_dwPower<<24;
	m_dwColor += 0xffffff;

	// 데이터의 추가 셋팅
	m_vCenter = (m_vMax+m_vMin) * 0.5f;

	return S_OK; 
}

HRESULT DxEffectReflect::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	if( !DxEffectMan::GetInstance().IsRealReflect() )		return S_OK;

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	// Note : StateBlock Capture
	m_pSB_SAVE->Capture();
	m_pSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	m_dwColor );

	if ( DxEnvironment::GetInstance().IsRecentReflect() )
	{
		DxEnvironment::GetInstance().SetReflection( m_vCenter );	//	반사
		RenderReflectionEX ( pd3dDevice, pLandMan );
	}
	else
	{
		DxEnvironment::GetInstance().SetReflection( m_vCenter, 0, 0, m_vMax, m_vMin );	//	반사
		RenderReflection ( pd3dDevice, pLandMan );
	}

	// Note : StateBlock Apply
	m_pSB_SAVE->Apply();

	return S_OK;
}

void DxEffectReflect::D3DXMatrixTexScaleBias( D3DXMATRIX& Out )
{
	//set special texture matrix for shadow mapping
	unsigned int range;
	float fOffsetX = 0.5f + (0.5f / (float)512);
	float fOffsetY = 0.5f + (0.5f / (float)512);

	range = 0xFFFFFFFF >> (32 - 16);

	float fBias    = -0.001f * (float)range;
	Out._11 = 0.5f;		Out._12 = 0.f;		Out._13 = 0.f;			Out._14 = 0.f;
	Out._21 = 0.0f;		Out._22 = -0.5f;	Out._23 = 0.f;			Out._24 = 0.f;
	Out._31 = 0.0f;		Out._32 = 0.f;		Out._33 = (float)range;	Out._34 = 0.f;
	Out._41 = fOffsetX;	Out._42 = fOffsetY;	Out._43 = fBias;		Out._44 = 1.f;
}

HRESULT DxEffectReflect::RenderReflection ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( !DxSurfaceTex::GetInstance().m_pReflectTex )	return S_OK;

	D3DXMATRIX	matReflect, matProj;
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );
	D3DXMatrixMultiply ( &matReflect, &DxEnvironment::GetInstance().GetMatrix_ReflectView(),	&matProj );
	
	D3DXMATRIX matTex;
	D3DXMatrixTexScaleBias( matTex );
	D3DXMatrixMultiply ( &matReflect, &matReflect, &matTex );				// 반사

	pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pReflectTex );

	D3DXMATRIX matVP;
	D3DXMatrixMultiply ( &matVP, &DxEnvironment::GetInstance().GetMatrix_ReflectView(), &matProj );

	DxSetTextureMatrix ( pd3dDevice, 0, matVP );

	pd3dDevice->SetFVF ( D3DREFLECTVERTEX::FVF );
	{
		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();
		if ( pLandMan )	
		{
			m_sReflectAABB.DynamicLoad( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sReflectAABB.Render( pd3dDevice, sCV, TRUE );
		}
		else
		{
			m_sReflectAABB.Render( pd3dDevice, sCV, FALSE );
		}
	}

	DxResetTextureMatrix ( pd3dDevice, 0 );

	return S_OK;
}

HRESULT DxEffectReflect::RenderReflectionEX ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( !DxSurfaceTex::GetInstance().m_pReflectTex )	return S_OK;

	pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pReflectTex );

	D3DXMATRIX matTex, matVP, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );
	D3DXMatrixMultiply ( &matVP, &matView, &matProj );

	DxSetTextureMatrix ( pd3dDevice, 0, matVP );

	pd3dDevice->SetFVF ( D3DREFLECTVERTEX::FVF );
	{
		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();
		if ( pLandMan )	
		{
			m_sReflectAABB.DynamicLoad( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sReflectAABB.Render( pd3dDevice, sCV, TRUE );
		}
		else
		{
			m_sReflectAABB.Render( pd3dDevice, sCV, FALSE );
		}
	}
	DxResetTextureMatrix ( pd3dDevice, 0 );

	return S_OK;
}

HRESULT	DxEffectReflect::CreateVB( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pmsMeshs, D3DXMATRIX& matWorld )
{
	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( !pmsMeshs->m_pLocalMesh )	return S_OK;
		
	DWORD dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ( );
	DWORD dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

	VERTEX*			pSrcVertices;
	WORD*			pSrcIndices;
	pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
	pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );

	m_sReflectAABB.Create( pd3dDevice, (BYTE*)pSrcVertices, pSrcIndices, dwFaces, matWorld, VERTEX::FVF );

	m_vMax = m_vMin = pSrcVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		m_vMax.x = (pSrcVertices[i].vPos.x > m_vMax.x) ? pSrcVertices[i].vPos.x : m_vMax.x ;
		m_vMax.y = (pSrcVertices[i].vPos.y > m_vMax.y) ? pSrcVertices[i].vPos.y : m_vMax.y ;
		m_vMax.z = (pSrcVertices[i].vPos.z > m_vMax.z) ? pSrcVertices[i].vPos.z : m_vMax.z ;

		m_vMin.x = (pSrcVertices[i].vPos.x < m_vMin.x) ? pSrcVertices[i].vPos.x : m_vMin.x ;
		m_vMin.y = (pSrcVertices[i].vPos.y < m_vMin.y) ? pSrcVertices[i].vPos.y : m_vMin.y ;
		m_vMin.z = (pSrcVertices[i].vPos.z < m_vMin.z) ? pSrcVertices[i].vPos.z : m_vMin.z ;
	}

	COLLISION::TransformAABB ( m_vMax, m_vMin, matWorld );

	m_vCenter = (m_vMax+m_vMin) * 0.5f;

	pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();
	pmsMeshs->m_pLocalMesh->UnlockIndexBuffer ();

	return S_OK;
}

void DxEffectReflect::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;
}

BOOL DxEffectReflect::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	return COLLISION::IsWithInPoint(vDivMax,vDivMin,m_vCenter);
}

void DxEffectReflect::SaveBuffer ( CSerialFile &SFile )
{
	SFile.BeginBlock();
	{
		m_sReflectAABB.Save ( SFile );
	}
	SFile.EndBlock();
}

void DxEffectReflect::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	DWORD dwCur = SFile.GetfTell();
	SFile.SetOffSet ( dwCur+dwBuffSize );
	return;
}

void DxEffectReflect::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	if ( VERSION==dwVer )
	{
		m_sReflectAABB.Load ( SFile );
		return;
	}
	else
	{
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

HRESULT DxEffectReflect::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectReflect *pEffSrc = (DxEffectReflect*)pSrcEffect;
	m_sReflectAABB.CloneTree ( pd3dDevice, pEffSrc->m_sReflectAABB.m_pTree );

	return S_OK;
}