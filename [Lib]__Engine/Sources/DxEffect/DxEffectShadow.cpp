// DxEffectShadow.cpp: implementation of the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <algorithm>

#include "./EditMeshs.h"
#include "./Collision.h"
#include "./SerialFile.h"
#include "./glperiod.h"
#include "./DxLightMan.h"
#include "./DxSurfaceTex.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./DxLandMan.h"
#include "./DxShadowMap.h"

#include "./DxEffectShadow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectShadow::TYPEID = DEF_EFFECT_SHADOW;
const DWORD	DxEffectShadow::VERSION = 0x101;
const char	DxEffectShadow::NAME[] = "[ 그림자 효과 - 지면 ]";
const DWORD DxEffectShadow::FLAG = _EFF_SINGLE_AFTER_1;

char		DxEffectShadow::m_szPath[MAX_PATH]	= "";

LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pSavedColorSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pEffectColorSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pSavedFilerSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pEffectFilerSB	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pSavedAddSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pEffectAddSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pSavedBlurSB		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectShadow::m_pEffectBlurSB		= NULL;

void DxEffectShadow::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SHADOW_PROPERTY);
	dwVer = VERSION;
}

void DxEffectShadow::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SHADOW_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if ( dwVer==100 && dwSize==sizeof(SHADOW_PROPERTY_100) )
	{
		SHADOW_PROPERTY_100 sProp;
		memcpy ( &sProp, pProp, dwSize );
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectShadow::DxEffectShadow() :
	m_vViewMax(D3DXVECTOR3(0.f,0.f,0.f)),
	m_vViewMin(D3DXVECTOR3(0.f,0.f,0.f)),
	m_vMax(D3DXVECTOR3(0.f,0.f,0.f)),
	m_vMin(D3DXVECTOR3(0.f,0.f,0.f)),
	m_fDistance(500.f),
	m_pSSDayTex(NULL),
	m_pSSNightTex(NULL),
	m_pFilterTex(NULL),
	m_pThisFrame(NULL),
	m_pUse(NULL),

	m_dwTexSize(512),		// 0-64, 1-128, 2-256, 3-512, 4-1024
	m_fShadowD(0.4f),
	m_fShadowP(0.7f),

	m_dwVertices(0),
	m_fViewMoveX(0.f),
	m_fViewMoveZ(0.f),

	m_pASurface(NULL),
	m_pATexture(NULL),
	m_pBSurface(NULL),
	m_pBTexture(NULL),
	m_pSShadowSurface(NULL),
	m_pSShadowTexture(NULL)
{
	m_dwFlag	|= USEDIRECTSHADOW;

	D3DXMatrixIdentity ( &m_matWorld );

	StringCchCopy( m_szFilter,		MAX_PATH, "StaticShadowFilter.tga" );
	StringCchCopy( m_szTexDayTime,	MAX_PATH, "Test_Day.dds" );
	StringCchCopy( m_szTexNight,	MAX_PATH, "Test_Day.dds" );
}

DxEffectShadow::~DxEffectShadow()
{
	m_pThisFrame = NULL;
	SAFE_DELETE_ARRAY(m_szAdaptFrame);
	SAFE_DELETE_ARRAY ( m_pUse );

	SAFE_RELEASE(m_pATexture);
	SAFE_RELEASE(m_pASurface);

	SAFE_RELEASE(m_pBTexture);
	SAFE_RELEASE(m_pBSurface);

	SAFE_RELEASE(m_pSShadowTexture);
	SAFE_RELEASE(m_pSShadowSurface);
}

HRESULT DxEffectShadow::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				CreateShadow( pd3dDevice, pmsMeshs, pframeCur->matCombined, pframeCur );
			}
			
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectShadow::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
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

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );		//	S/W
}

HRESULT DxEffectShadow::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
//	GASSERT ( pFrame );

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

HRESULT DxEffectShadow::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TFACTOR  );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1);

		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,		D3DTA_TEXTURE  );
		//pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_SELECTARG1);

		//// Enable alpha testing (skips pixels with less than a certain alpha.)
		//if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		//{
		//	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x02 );
		//	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		//}

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedColorSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectColorSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,	D3DCULL_NONE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_BORDER );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedFilerSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectFilerSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,	D3DCULL_NONE );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		pd3dDevice->SetSamplerState ( 1, D3DSAMP_MAGFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_MINFILTER,		D3DTEXF_POINT );
		pd3dDevice->SetSamplerState ( 1, D3DSAMP_MIPFILTER,		D3DTEXF_NONE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,		D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,		D3DTOP_ADD );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedAddSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectAddSB );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,	D3DCULL_NONE );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,		D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedBlurSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectBlurSB );
	}

	return S_OK;
}

HRESULT DxEffectShadow::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedColorSB );
	SAFE_RELEASE( m_pEffectColorSB );
	SAFE_RELEASE( m_pSavedFilerSB );
	SAFE_RELEASE( m_pEffectFilerSB );
	SAFE_RELEASE( m_pSavedAddSB );
	SAFE_RELEASE( m_pEffectAddSB );
	SAFE_RELEASE( m_pSavedBlurSB );
	SAFE_RELEASE( m_pEffectBlurSB );
	
	return S_OK;
}

HRESULT DxEffectShadow::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 문자가 같을 경우 0가 나온다.		다른 경우 참이다.		틀려야 배경이 바뀐다.
	//
	if ( strcmp ( m_szTexDayTime, m_szTexNight ) )	m_dwFlag |= USECHANGESHADOW;		// 이 옵션은 저장과는 별로 상관 없다.
	else											m_dwFlag &= ~USECHANGESHADOW;

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	if ( m_dwFlag & USESTATICSHADOW )
	{
		TextureManager::LoadTexture ( m_szTexDayTime, pd3dDevice, m_pSSDayTex, 0, 0 );

		if ( m_pSSDayTex )
		{
			D3DSURFACE_DESC pDesc;
			m_pSSDayTex->GetLevelDesc ( 0, &pDesc );
			m_dwTexSize = pDesc.Width;
		}

		TextureManager::LoadTexture ( m_szTexNight, pd3dDevice, m_pSSNightTex, 0, 0 );

		if ( m_pSSNightTex )
		{
			D3DSURFACE_DESC pDesc;
			m_pSSNightTex->GetLevelDesc ( 0, &pDesc );
			m_dwTexSize = pDesc.Width;
		}
	}

	//	Note : 지형 그림자를 저장 시켜 놓는다.
	if ( m_pSSDayTex )
	{
		D3DSURFACE_DESC pDesc;
		m_pSSDayTex->GetLevelDesc ( 0, &pDesc );
		m_dwTexSize = pDesc.Width;
	}
	float fMicroMove = 0.5f/m_dwTexSize;

	D3DXVECTOR3 vMin, vMax;
	vMin = m_vViewMin;
	vMax = m_vViewMax;
	vMin.x = vMin.x + m_fViewMoveX;
	vMax.x = vMax.x + m_fViewMoveX;
	vMin.z = vMin.z + m_fViewMoveZ;
	vMax.z = vMax.z + m_fViewMoveZ;

	m_sShadowAABB.MakeStaticShadowUV ( vMin, vMax, fMicroMove );	//NEW


	return S_OK;
}

HRESULT DxEffectShadow::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectShadow::InvalidateDeviceObjects ()
{
	SAFE_RELEASE(m_pATexture);
	SAFE_RELEASE(m_pASurface);

	SAFE_RELEASE(m_pBTexture);
	SAFE_RELEASE(m_pBSurface);

	SAFE_RELEASE(m_pSShadowTexture);
	SAFE_RELEASE(m_pSShadowSurface);

	return S_OK;
}

HRESULT DxEffectShadow::DeleteDeviceObjects ()
{
	HRESULT hr  = S_OK;

	TextureManager::ReleaseTexture( m_szTexDayTime, m_pSSDayTex );
	TextureManager::ReleaseTexture( m_szTexNight, m_pSSNightTex );

	return hr;
}

HRESULT DxEffectShadow::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectShadow::Render");
	m_pThisFrame = pframeCur;
	DxMeshes*	pmsMeshs = NULL;

	//	Note : m_matLastWVP 이 값을 알아내기 위하여 이것을 한다.
	//
	D3DXMATRIX texMat;

	if ( DxShadowMap::GetInstance().GetShadowDetail() != SHADOW_DISABLE )
	{
		D3DXMATRIX	matLastWVP_D;
		matLastWVP_D	= DxShadowMap::GetInstance().GetMatrix_D ();

		//set special texture matrix for shadow mapping
		unsigned int range;
		float fOffsetX = 0.5f + ( 0.5f/512.f );
		float fOffsetY = 0.5f + ( 0.5f/512.f );

		range = 0xFFFFFFFF >> (32 - 16);//m_bitDepth)

		float fBias    = -0.001f * (float)range;
		D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
									0.0f,     -0.5f,     0.0f,         0.0f,
									0.0f,     0.0f,     (float)range, 0.0f,
									fOffsetX, fOffsetY, fBias,        1.0f );
		
		D3DXMatrixMultiply(&texMat, &matLastWVP_D, &texScaleBiasMat);
	}

	//	Note : Render
	//
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	matIdentity._42 += 0.06f;				// Note : 아주 약간 올려준다.
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();

	// Blending 셋팅
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCCOLOR );

	if ( pLandMan )	
	{
		m_sShadowAABB.DynamicLoad ( pd3dDevice, pLandMan->GetSerialFile(), sCV );

		if( (m_dwFlag&USESTATICSHADOW) && DxEffectMan::GetInstance().GetSSDetail() )
		{
			m_sShadowAABB.RenderStatic( pd3dDevice, sCV, m_pSSDayTex, TRUE );
		}

		if( DxShadowMap::GetInstance().GetShadowDetail()==SHADOW_DISABLE )		goto _RETURN;

		m_sShadowAABB.RenderChar( pd3dDevice, sCV, DxSurfaceTex::GetInstance().m_pShadowTex, texMat, TRUE );
	}
	else
	{
		if( (m_dwFlag&USESTATICSHADOW) && DxEffectMan::GetInstance().GetSSDetail() )
		{
			m_sShadowAABB.RenderStatic( pd3dDevice, sCV, m_pSSDayTex, FALSE );
		}

		if( DxShadowMap::GetInstance().GetShadowDetail()==SHADOW_DISABLE )		goto _RETURN;

		m_sShadowAABB.RenderChar( pd3dDevice, sCV, DxSurfaceTex::GetInstance().m_pShadowTex, texMat, FALSE );
	}

_RETURN:
	PROFILE_END("DxEffectShadow::Render");

	return S_OK;
}

HRESULT DxEffectShadow::CreateShadow( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX	matWorld, DxFrame *pframeCur )
{
	if ( !pframeCur )				return S_OK;
	if ( !pmsMeshs->m_pLocalMesh )	return S_OK;

	D3DXVECTOR3		vPos;
	float			fMicroMove;
	VERTEX*			pVertices;
	WORD*			pindices;

	DWORD dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ();
	DWORD dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ();

	pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pindices );

	m_sShadowAABB.Create ( pd3dDevice, pVertices, pindices, dwFaces, matWorld );

	m_vMin = m_vMax = pVertices[0].vPos;
	for ( DWORD i=0; i<dwVertices; i++ )
	{
		vPos = pVertices[i].vPos;

		m_vMax.x = (vPos.x > m_vMax.x) ? vPos.x : m_vMax.x ;
		m_vMax.y = (vPos.y > m_vMax.y) ? vPos.y : m_vMax.y ;
		m_vMax.z = (vPos.z > m_vMax.z) ? vPos.z : m_vMax.z ;

		m_vMin.x = (vPos.x < m_vMin.x) ? vPos.x : m_vMin.x ;
		m_vMin.y = (vPos.y < m_vMin.y) ? vPos.y : m_vMin.y ;
		m_vMin.z = (vPos.z < m_vMin.z) ? vPos.z : m_vMin.z ;
	}
	D3DXVec3TransformCoord ( &m_vMax, &m_vMax, &pframeCur->matCombined );
	D3DXVec3TransformCoord ( &m_vMin, &m_vMin, &pframeCur->matCombined );

	pmsMeshs->m_pLocalMesh->UnlockIndexBuffer ();
	pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();

	//	Note : 지형 그림자를 저장 시켜 놓는다.
	if ( m_pSSDayTex )
	{
		D3DSURFACE_DESC pDesc;
		m_pSSDayTex->GetLevelDesc ( 0, &pDesc );
		m_dwTexSize = pDesc.Width;
	}
	fMicroMove = 0.5f/m_dwTexSize;
	SetView_Max_Min ( pd3dDevice, pframeCur );

	D3DXVECTOR3 vMin, vMax;
	vMin = m_vViewMin;
	vMax = m_vViewMax;
	vMin.x = vMin.x + m_fViewMoveX;
	vMax.x = vMax.x + m_fViewMoveX;
	vMin.z = vMin.z + m_fViewMoveZ;
	vMax.z = vMax.z + m_fViewMoveZ;

	m_sShadowAABB.MakeStaticShadowUV ( vMin, vMax, fMicroMove );	//NEW

	return S_OK;
}

void DxEffectShadow::SaveBuffer ( CSerialFile &SFile )
{
	SFile.BeginBlock();
	{
		m_sShadowAABB.Save ( SFile );
	}
	SFile.EndBlock();
}

void DxEffectShadow::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	// 여기서는 로드 해서는 안된다...... 
	// 왜냐면 X 파일을 변경할 수도 있기 때문에 이곳에는 값을 지정해서는 안된다.
	// < X 파일에 속해있는 효과, X 파일이 변하면 자신도 변해야 하기 때문에 >

	if ( VERSION == dwVer)
	{
		SFile >> dwBuffSize;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else
	{
		SFile >> dwBuffSize;
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

void DxEffectShadow::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;

	if ( VERSION == dwVer )
	{
		SFile >> dwBuffSize;
		m_sShadowAABB.Load ( SFile );
		return;
	}
	else if ( VERSION == 0x100 )
	{
		SFile >> dwBuffSize;

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
	else
	{
		SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
		return;
	}
}

HRESULT DxEffectShadow::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectShadow *pEffSrc = (DxEffectShadow*)pSrcEffect;
	m_sShadowAABB.CloneTree (pd3dDevice, pEffSrc->m_sShadowAABB.m_pTree );
	return S_OK;
}

void	DxEffectShadow::SetView_Max_Min ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pFrame )
{
	float fTexWidth(512.f);
	float fTexHeight(512.f);

	if ( m_pSSDayTex )
	{
		D3DSURFACE_DESC pDesc;
		m_pSSDayTex->GetLevelDesc ( 0, &pDesc );
		fTexWidth = (float)pDesc.Width;
		fTexHeight = (float)pDesc.Height;
	}

	//	Note : 카메라와 지형의 거리가 가장 알맞도록 계산함
	//
	m_fDistance = 500.f;
	while ( !CheckViewPort( pd3dDevice, pFrame, m_fDistance, (DWORD)fTexWidth ) )		// 이상함.
	{
		m_fDistance += 100.f;
	}

	//	Note : 평면과 카메라 CV의 만나는 점 구하기.
	//
	D3DXVECTOR3	vViewMax, vViewMin;
	D3DXVECTOR3	vTemp;
	D3DXVECTOR3	vCenter = (m_vMax+m_vMin) * 0.5f;

	D3DXVECTOR3		vUpVec ( 0.f, 1.f, 0.f );
	D3DXVECTOR3		vTempLookatPt	= vCenter + vUpVec*m_fDistance;
	vUpVec = D3DXVECTOR3 ( 0.f, 0.f, -1.f );
	D3DXVECTOR3		vLookatPt	= vCenter;
	D3DXVECTOR3		vFromPt		= vTempLookatPt;
	D3DXVECTOR3		vPosition[4];

	CLIPVOLUME pCV = DxViewPort::GetInstance().ComputeClipVolume ( vCenter, vTempLookatPt, vUpVec, 0.5f, 100000.f, fTexWidth, fTexHeight, D3DX_PI/4 );	// CLIPVOLUME

	float	fNearPlane	= 0.5f;
	float	fFarPlane	= 100000.f;
	float	fFOV		= D3DX_PI/4;

	FLOAT dist, t;
	D3DXVECTOR3 pt[8];
	D3DXVECTOR3 v1, v2, n;
	D3DXVECTOR3 vDir, vUp, vPos;	
	D3DXVECTOR3 vCross;

	vDir = vLookatPt - vFromPt;
	vUp = vUpVec;
	vPos = vFromPt;

	D3DXVec3Normalize ( &vUp, &vUp );
	D3DXVec3Normalize ( &vDir, &vDir );

	D3DXVec3Cross ( &vCross, &vUp, &vDir );
	D3DXVec3Normalize ( &vCross, &vCross );

	D3DXVec3Cross ( &vUp, &vDir, &vCross );
	D3DXVec3Normalize ( &vUp, &vUp );

	for(INT i = 0; i < 8; i++)
	{
		//	Note	:	i의 값이 4이상일때
		//				먼곳에 대한 좌표이다.

		//	(1)
		dist = (i & 0x4) ? fFarPlane : fNearPlane;
		pt[i] = dist * vDir;

		//	(2)
		t = dist * tanf(fFOV/2);
		t = (i & 0x2) ? t : -t;
		pt[i] += vUp * t;
		
		t = dist * tanf(fFOV/2) * fTexWidth/fTexHeight; // take into account screen proportions
		t = (i & 0x1) ? -t : t;
		pt[i] += vCross *t;
		pt[i] = vPos + pt[i];
	}

	//	Note : 면 만들기
	D3DXPLANE	pPlane;
	vPosition[0] = D3DXVECTOR3 ( m_vMax.x, vCenter.y, m_vMin.z );
	vPosition[1] = D3DXVECTOR3 ( m_vMin.x, vCenter.y, m_vMax.z );
	vPosition[2] = D3DXVECTOR3 ( m_vMax.x, vCenter.y, m_vMax.z );
	D3DXPlaneFromPoints ( &pPlane, &vPosition[0], &vPosition[1], &vPosition[2] );

	//	Note : 점 만들기
	vPosition[0] = DxSplit ( &pPlane, &pt[4], &pt[0] );
	vPosition[1] = DxSplit ( &pPlane, &pt[5], &pt[1] );
	vPosition[2] = DxSplit ( &pPlane, &pt[6], &pt[2] );
	vPosition[3] = DxSplit ( &pPlane, &pt[7], &pt[3] );

	vViewMax = vViewMin = vPosition[0];

	vViewMax.x = ( vViewMax.x > vPosition[0].x ) ? vViewMax.x : vPosition[0].x;
	vViewMax.x = ( vViewMax.x > vPosition[1].x ) ? vViewMax.x : vPosition[1].x;
	vViewMax.x = ( vViewMax.x > vPosition[2].x ) ? vViewMax.x : vPosition[2].x;
	vViewMax.x = ( vViewMax.x > vPosition[3].x ) ? vViewMax.x : vPosition[3].x;

	vViewMax.y = ( vViewMax.y > vPosition[0].y ) ? vViewMax.y : vPosition[0].y;
	vViewMax.y = ( vViewMax.y > vPosition[1].y ) ? vViewMax.y : vPosition[1].y;
	vViewMax.y = ( vViewMax.y > vPosition[2].y ) ? vViewMax.y : vPosition[2].y;
	vViewMax.y = ( vViewMax.y > vPosition[3].y ) ? vViewMax.y : vPosition[3].y;

	vViewMax.z = ( vViewMax.z > vPosition[0].z ) ? vViewMax.z : vPosition[0].z;
	vViewMax.z = ( vViewMax.z > vPosition[1].z ) ? vViewMax.z : vPosition[1].z;
	vViewMax.z = ( vViewMax.z > vPosition[2].z ) ? vViewMax.z : vPosition[2].z;
	vViewMax.z = ( vViewMax.z > vPosition[3].z ) ? vViewMax.z : vPosition[3].z;

	vViewMin.x = ( vViewMin.x < vPosition[0].x ) ? vViewMin.x : vPosition[0].x;
	vViewMin.x = ( vViewMin.x < vPosition[1].x ) ? vViewMin.x : vPosition[1].x;
	vViewMin.x = ( vViewMin.x < vPosition[2].x ) ? vViewMin.x : vPosition[2].x;
	vViewMin.x = ( vViewMin.x < vPosition[3].x ) ? vViewMin.x : vPosition[3].x;

	vViewMin.y = ( vViewMin.y < vPosition[0].y ) ? vViewMin.y : vPosition[0].y;
	vViewMin.y = ( vViewMin.y < vPosition[1].y ) ? vViewMin.y : vPosition[1].y;
	vViewMin.y = ( vViewMin.y < vPosition[2].y ) ? vViewMin.y : vPosition[2].y;
	vViewMin.y = ( vViewMin.y < vPosition[3].y ) ? vViewMin.y : vPosition[3].y;

	vViewMin.z = ( vViewMin.z < vPosition[0].z ) ? vViewMin.z : vPosition[0].z;
	vViewMin.z = ( vViewMin.z < vPosition[1].z ) ? vViewMin.z : vPosition[1].z;
	vViewMin.z = ( vViewMin.z < vPosition[2].z ) ? vViewMin.z : vPosition[2].z;
	vViewMin.z = ( vViewMin.z < vPosition[3].z ) ? vViewMin.z : vPosition[3].z;

	m_vViewMax		= vViewMax;
	m_vViewMin		= vViewMin;
	m_vViewMax.y	= vCenter.y;
	m_vViewMin.y	= vCenter.y;
}
