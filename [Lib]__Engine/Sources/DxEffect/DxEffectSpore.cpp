// DxEffectSpore.cpp: implementation of the DxEffectSpore class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./glperiod.h"
#include "./DxVertexFVF.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"

#include "./DxEffectSpore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectSpore::PARTICLEVERTEX::FVF	= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectSpore::TYPEID = DEF_EFFECT_SPORE;
const DWORD	DxEffectSpore::VERSION = 0x00000101;
const char DxEffectSpore::NAME[] = "[ 포자, 반딧불 ]";
const DWORD DxEffectSpore::FLAG = _EFF_SINGLE_AFTER_0;

LPDIRECT3DVERTEXBUFFERQ	DxEffectSpore::m_pSporeVB = NULL;
LPDIRECT3DVERTEXBUFFERQ	DxEffectSpore::m_pFireFlyVB = NULL;
LPDIRECT3DINDEXBUFFERQ	DxEffectSpore::m_pIB = NULL;

BOOL					DxEffectSpore::VAILEDDEVICE = FALSE;
CMemPool<DxEffectSpore::PARTICLE> *DxEffectSpore::m_pParticlePool	= NULL;
CMemPool<DxEffectSpore::FIREFLY> *DxEffectSpore::m_pFireFlyPool		= NULL;

const float			DxEffectSpore::MAX_LIFE			= 10.f;
const DWORD			DxEffectSpore::MAX_VERTEX		= 100;

const float			DxEffectSpore::FIREFLY_SIZE		= 0.3f;							// 처음 사이즈


void DxEffectSpore::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(SPORE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectSpore::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(SPORE_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if ( dwVer==0x00000100 && dwSize==sizeof(SPORE_PROPERTY_100) )
	{
		SPORE_PROPERTY_100	sProp;
		memcpy ( &sProp, pProp, dwSize );

		m_dwFlags		= USESPORE;

		m_nFlat			= sProp.m_nFlat;
		m_nSec			= sProp.m_nSec;

		m_fScale		= sProp.m_fScale;
		m_fSpeed		= sProp.m_fSpeed;

		m_bRotate		= sProp.m_bRotate;
		m_fRotate		= sProp.m_fRotate;

		m_fAlphaRate1	= sProp.m_fAlphaRate1;
		m_fAlphaRate2	= sProp.m_fAlphaRate2;
		m_fAlphaStart	= sProp.m_fAlphaStart;
		m_fAlphaMid1	= sProp.m_fAlphaMid1;
		m_fAlphaMid2	= sProp.m_fAlphaMid2;
		m_fAlphaEnd		= sProp.m_fAlphaEnd;

		m_vColor		= sProp.m_vColor;

		m_vMax			= D3DXVECTOR3 ( sProp.m_fMaxX, sProp.m_fMaxY, sProp.m_fMaxZ );
		m_vMin			= D3DXVECTOR3 ( sProp.m_fMinX, sProp.m_fMinY, sProp.m_fMinZ );

		StringCchCopy( m_szSporeTex,	MAX_PATH, sProp.m_szSporeTex );
		StringCchCopy( m_szFireFlyTex,	MAX_PATH, "_Na_Min.tga" );
	}
}

LPDIRECT3DSTATEBLOCK9	DxEffectSpore::m_pSavedStateBlock(NULL);
LPDIRECT3DSTATEBLOCK9	DxEffectSpore::m_pPointStateBlock(NULL);

LPDIRECT3DSTATEBLOCK9	DxEffectSpore::m_pFireFlySB_S(NULL);
LPDIRECT3DSTATEBLOCK9	DxEffectSpore::m_pFireFlySB_D(NULL);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////6.f/32.f , 0.35f, 31.f/32.f, 0.4f

DxEffectSpore::DxEffectSpore() :
	m_fTime(0.0f),
	m_fElapsedTime(0.0f),

	m_pParticleHead(NULL),
	m_pFireFlyHead(NULL),

	m_fAddSecOne(0.f),
	m_fSecOne(0.f),
	m_nAddNum(0),
	m_dwFireFlyCount(0),

	m_pSporeTex(NULL)
{
}

DxEffectSpore::~DxEffectSpore()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	if ( DxEffectSpore::VAILEDDEVICE )
	{
		DxEffectSpore::PutSleepParticles ( m_pParticleHead );
		m_pParticleHead = NULL;

		DxEffectSpore::PutSleepFireFlys ( m_pFireFlyHead );
		m_pFireFlyHead = NULL;
	}
	else
	{
		SAFE_DELETE(m_pParticleHead);
		SAFE_DELETE(m_pFireFlyHead);
	}
}


HRESULT DxEffectSpore::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 디퓨즈 사용

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,   D3DTA_DIFFUSE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pPointStateBlock );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 디퓨즈 사용

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pFireFlySB_S );
		else			pd3dDevice->EndStateBlock( &m_pFireFlySB_D );
	}

	SAFE_RELEASE ( m_pSporeVB );
	SAFE_RELEASE ( m_pFireFlyVB );
	SAFE_RELEASE ( m_pIB );

	pd3dDevice->CreateVertexBuffer ( 4*MAX_VERTEX*sizeof(PARTICLEVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									PARTICLEVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pSporeVB, NULL );

	pd3dDevice->CreateVertexBuffer ( 4*MAX_VERTEX*sizeof(PARTICLEVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									PARTICLEVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pFireFlyVB, NULL );

	pd3dDevice->CreateIndexBuffer ( 6*MAX_VERTEX*sizeof(WORD), D3DUSAGE_WRITEONLY, 
									D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL  );

	PARTICLEVERTEX*	pVertices;
	m_pSporeVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<MAX_VERTEX; i++ )
	{
		pVertices[i*4+0].t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+2].t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+3].t = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pSporeVB->Unlock ();

	m_pFireFlyVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<MAX_VERTEX; i++ )
	{
		pVertices[i*4+0].t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+2].t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+3].t = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pFireFlyVB->Unlock ();


	WORD*	pIndices;
	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	for ( DWORD i=0; i<MAX_VERTEX; i++ )
	{	
		*pIndices++ = (WORD)((i*4)+0);
		*pIndices++ = (WORD)((i*4)+1);
		*pIndices++ = (WORD)((i*4)+2);

		*pIndices++ = (WORD)((i*4)+1);
		*pIndices++ = (WORD)((i*4)+3);
		*pIndices++ = (WORD)((i*4)+2);
	}
	m_pIB->Unlock ();


	m_pParticlePool = new CMemPool<PARTICLE>;
	m_pFireFlyPool = new CMemPool<FIREFLY>;

	VAILEDDEVICE = TRUE;

	return S_OK;
}

HRESULT DxEffectSpore::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pPointStateBlock );
	SAFE_RELEASE( m_pFireFlySB_S );
	SAFE_RELEASE( m_pFireFlySB_D );

	SAFE_RELEASE ( m_pSporeVB );
	SAFE_RELEASE ( m_pFireFlyVB );
	SAFE_RELEASE ( m_pIB );

	SAFE_DELETE(m_pParticlePool);
	SAFE_DELETE(m_pFireFlyPool);

	VAILEDDEVICE = FALSE;

	return S_OK;
}

HRESULT DxEffectSpore::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
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
				SetBoundBox ( pmsMeshs->m_pLocalMesh, &pframeCur->matCombined );
			}

			ResetEdit ();

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return S_OK;
}

HRESULT DxEffectSpore::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT DxEffectSpore::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

void DxEffectSpore::PutSleepParticles ( PARTICLE* pParticles )
{
	if ( !pParticles )	return;

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	//
	if ( VAILEDDEVICE && m_pParticlePool ) 
	{
		PARTICLE* pCur = pParticles;
		while ( pCur )
		{
			PARTICLE* pRelease = pCur;
			pCur = pCur->pNext;

			pRelease->pNext = NULL;
			m_pParticlePool->Release(pRelease);
		}
	}
}

void DxEffectSpore::PutSleepParticle ( PARTICLE* pParticle )
{
	if ( !pParticle )	return;

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	//
	if ( VAILEDDEVICE && m_pParticlePool ) 
	{
		pParticle->pNext = NULL;
		m_pParticlePool->Release(pParticle);
	}
}

void DxEffectSpore::PutSleepFireFlys ( FIREFLY* pFireFlys )
{
	if ( !pFireFlys )	return;

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	//
	if ( VAILEDDEVICE && m_pFireFlyPool ) 
	{
		FIREFLY* pCur = pFireFlys;
		while ( pCur )
		{
			FIREFLY* pRelease = pCur;
			pCur = pCur->pNext;

			pRelease->pNext = NULL;
			m_pFireFlyPool->Release(pRelease);
		}
	}
}

void DxEffectSpore::PutSleepFireFly ( FIREFLY* pFireFly )
{
	if ( !pFireFly )	return;

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	//
	if ( VAILEDDEVICE && m_pFireFlyPool ) 
	{
		pFireFly->pNext = NULL;
		m_pFireFlyPool->Release(pFireFly);
	}
}

HRESULT DxEffectSpore::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szSporeTex, pd3dDevice, m_pSporeTex, 0, 0 );

	TextureManager::LoadTexture ( m_szFireFlyTex, pd3dDevice, m_pFireFlyTex, 0, 0 );

	m_vFireFly_Color_D = m_vFireFly_Color_2 - m_vFireFly_Color_1;

	ResetEdit ();

	return S_OK;
}

HRESULT DxEffectSpore::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szSporeTex, m_pSporeTex );
	TextureManager::ReleaseTexture( m_szFireFlyTex, m_pFireFlyTex );

	return S_OK;
}

HRESULT DxEffectSpore::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime	= fTime;
	m_fElapsedTime	+= fElapsedTime;
	m_fAddSecOne += fElapsedTime;

	return S_OK;
}

HRESULT DxEffectSpore::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	while ( m_fAddSecOne > m_fSecOne && m_fSecOne > 0 )
	{
		//	생성
		if ( m_dwFlags & USESPORE )
		{
			CreateParticle ();
		}

		//if ( !GLPeriod::GetInstance().IsOffLight()&&(m_dwFlags&USEFIREFLY) )	// 밤에만 나오도록 설정
		//{
		//	if ( m_dwFireFlyCount < MAX_VERTEX )
		//	{
		//		CreateFireFly ();
		//		++m_dwFireFlyCount;
		//	}			
		//}

		m_fAddSecOne -= m_fSecOne;
	}

	if ( m_fElapsedTime > 0.03f )
	{
		Update ( pd3dDevice );
		//UpdateFireFly ( pd3dDevice );//	ERROR : 오류가 있어서 임시로 막음.

		m_fElapsedTime = 0.0f;
	}

	RenderSpore ( pd3dDevice );
	//RenderFireFly ( pd3dDevice );//	ERROR : 오류가 있어서 임시로 막음.

	return S_OK;
}

HRESULT DxEffectSpore::RenderSpore ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pSporeVB )	return S_OK;

	PROFILE_BEGIN("DxEffectSpore");

	DWORD dwFVFSize;
	DWORD dwFaces;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	//	Note : 이전 상태 백업.
	m_pSavedStateBlock->Capture();

	//	Note : 랜더링 상태 조정.
	m_pPointStateBlock->Apply();

	{
		dwFVFSize = PARTICLEVERTEX::FVF;
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

		pd3dDevice->SetIndices ( m_pIB );
		pd3dDevice->SetStreamSource ( 0, m_pSporeVB, 0, dwFVFSize );
		pd3dDevice->SetFVF ( PARTICLEVERTEX::FVF );

		pd3dDevice->SetTexture( 0, m_pSporeTex );

		dwFaces = 0;

		D3DXVECTOR3			vLocal;
		D3DXMATRIX			matLocal;
		float				fScale;
		DWORD				dwDiffuse;
		D3DXVECTOR3&		vFromPt = DxViewPort::GetInstance().GetFromPt();
		PARTICLEVERTEX*		pVertices;
		PARTICLE*			pParticle = m_pParticleHead;

		if ( m_bRotate )	// 회전 한다.
		{
			fScale	=	m_fScale*0.7f;		//8

			m_pSporeVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
			while ( pParticle )
			{
				matLocal = DxBillboardLookAt ( &pParticle->vNowPos, DxViewPort::GetInstance().GetMatView() );

				vLocal = D3DXVECTOR3( fScale*sinf(pParticle->fTexRotate), fScale*cosf(pParticle->fTexRotate), 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+0].p, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale*sinf(pParticle->fTexRotate+(D3DX_PI/2.f)), fScale*cosf(pParticle->fTexRotate+(D3DX_PI/2.f)), 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+1].p, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale*sinf(pParticle->fTexRotate+(D3DX_PI*1.5f)), fScale*cosf(pParticle->fTexRotate+(D3DX_PI*1.5f)), 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+2].p, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale*sinf(pParticle->fTexRotate+D3DX_PI), fScale*cosf(pParticle->fTexRotate+D3DX_PI), 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+3].p, &vLocal, &matLocal );

				dwDiffuse	=	(((int)(pParticle->Diffuse.w*255))<<24) + (((int)(pParticle->Diffuse.x*255))<<16) +
								(((int)(pParticle->Diffuse.y*255))<<8) + (int)(pParticle->Diffuse.z*255);

				pVertices[dwFaces*4+0].d = dwDiffuse;
				pVertices[dwFaces*4+1].d = dwDiffuse;
				pVertices[dwFaces*4+2].d = dwDiffuse;
				pVertices[dwFaces*4+3].d = dwDiffuse;

				pParticle = pParticle->pNext;
				dwFaces++;

				if ( dwFaces >= MAX_VERTEX )
				{
					m_pSporeVB->Unlock ();
					pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwFaces*4, 0, dwFaces*2 );

					goto _RETURN;				
				}
			}
			m_pSporeVB->Unlock ();
		}

		else			// 회전 안 한다.
		{
			fScale	=	m_fScale*0.4f;		//5

			m_pSporeVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
			while ( pParticle )
			{
				matLocal = DxBillboardLookAt ( &pParticle->vNowPos, DxViewPort::GetInstance().GetMatView() );

				vLocal = D3DXVECTOR3( -fScale, fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+0].p, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale, fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+1].p, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( -fScale, -fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+2].p, &vLocal, &matLocal );
				vLocal = D3DXVECTOR3( fScale, -fScale, 0.f );
				D3DXVec3TransformCoord ( &pVertices[dwFaces*4+3].p, &vLocal, &matLocal );

				dwDiffuse	=	(((int)(pParticle->Diffuse.w*255))<<24) + (((int)(pParticle->Diffuse.x*255))<<16) +
								(((int)(pParticle->Diffuse.y*255))<<8) + (int)(pParticle->Diffuse.z*255);

				pVertices[dwFaces*4+0].d = dwDiffuse;
				pVertices[dwFaces*4+1].d = dwDiffuse;
				pVertices[dwFaces*4+2].d = dwDiffuse;
				pVertices[dwFaces*4+3].d = dwDiffuse;

				pParticle = pParticle->pNext;
				dwFaces++;

				if ( dwFaces >= MAX_VERTEX )
				{
					m_pSporeVB->Unlock ();
					pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwFaces*4, 0, dwFaces*2 );

					goto _RETURN;				
				}
			}
			m_pSporeVB->Unlock ();
		}

		if ( dwFaces )		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwFaces*4, 0, dwFaces*2 );
	}

_RETURN:

	//	Note : 이전상태로 복원.
	m_pSavedStateBlock->Apply();

	PROFILE_END("DxEffectSpore");

	return S_OK;
}

HRESULT DxEffectSpore::RenderFireFly ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pFireFlyVB )	return S_OK;

	PROFILE_BEGIN("DxEffectFireFly");

	DWORD dwFVFSize;
	DWORD dwFaces;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );


	m_pFireFlySB_S->Capture();
	m_pFireFlySB_D->Apply();

	{
		dwFVFSize = PARTICLEVERTEX::FVF;
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

		pd3dDevice->SetIndices ( m_pIB );
		pd3dDevice->SetStreamSource ( 0, m_pFireFlyVB, 0, dwFVFSize );
		pd3dDevice->SetFVF ( PARTICLEVERTEX::FVF );

		pd3dDevice->SetTexture( 0, m_pFireFlyTex );

		dwFaces = 0;

		D3DXVECTOR3			vLocal;
		D3DXMATRIX			matLocal;
		float				fScale;
		DWORD				dwDiffuse;
		D3DXVECTOR3&		vFromPt = DxViewPort::GetInstance().GetFromPt();
		PARTICLEVERTEX*		pVertices;
		FIREFLY*			pParticle = m_pFireFlyHead;


		m_pFireFlyVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
		while ( pParticle )
		{
			fScale = pParticle->fSize;

			matLocal = DxBillboardLookAt ( &pParticle->vNowPos, DxViewPort::GetInstance().GetMatView() );

			vLocal = D3DXVECTOR3( -fScale, fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[dwFaces*4+0].p, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fScale, fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[dwFaces*4+1].p, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( -fScale, -fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[dwFaces*4+2].p, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fScale, -fScale, 0.f );
			D3DXVec3TransformCoord ( &pVertices[dwFaces*4+3].p, &vLocal, &matLocal );

			dwDiffuse	=	(((int)(pParticle->vColor.w))<<24) + (((int)(pParticle->vColor.x))<<16) +
							(((int)(pParticle->vColor.y))<<8) + (int)(pParticle->vColor.z);

			pVertices[dwFaces*4+0].d = dwDiffuse;
			pVertices[dwFaces*4+1].d = dwDiffuse;
			pVertices[dwFaces*4+2].d = dwDiffuse;
			pVertices[dwFaces*4+3].d = dwDiffuse;

			pParticle = pParticle->pNext;
			dwFaces++;

			if ( dwFaces >= MAX_VERTEX )
			{
				m_pFireFlyVB->Unlock ();
				pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwFaces*4, 0, dwFaces*2 );

				goto _RETURN;
			}
		}
		m_pFireFlyVB->Unlock ();

		if ( dwFaces )		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwFaces*4, 0, dwFaces*2 );
	}

_RETURN:
	//	Note : 이전상태로 복원.
	m_pFireFlySB_S->Apply();

	PROFILE_END("DxEffectFireFly");

	return S_OK;
}

HRESULT DxEffectSpore::ResetEdit ()
{
	m_fAlpha = m_fAlphaStart;

	m_fAlphaTime1 = MAX_LIFE*m_fAlphaRate1*0.01f;
	m_fAlphaTime2 = MAX_LIFE*m_fAlphaRate2*0.01f;

	m_fAlphaLDelta1 = (m_fAlphaMid1-m_fAlphaStart)/m_fAlphaTime1;
	m_fAlphaLDelta2 = (m_fAlphaMid2-m_fAlphaMid1)/(m_fAlphaTime2-m_fAlphaTime1);
	m_fAlphaLDelta3 = (m_fAlphaEnd-m_fAlphaMid2)/(MAX_LIFE-m_fAlphaTime2);

	m_fSecOne = 1.f/(float)m_nSec;

	return S_OK;
}

HRESULT DxEffectSpore::CreateParticle ()
{
	int nRandHeight = 0;

	//	Note : 평지일때와 경사진 곳일 때의 셋팅
	//
	if ( m_nFlat == 0 )		nRandHeight = rand()%10-10;	//	-10 ~ 0
	else					nRandHeight = rand()%30-20;	//	-20 ~ 10


	// 보는 시점
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vPos;
	vPos = D3DXVECTOR3( (float)((rand()%400)-200)+vLookatPt.x, vLookatPt.y+nRandHeight, (float)((rand()%400)-200)+vLookatPt.z );

	//	범위 안에 있다면 새로운 것을 생성한다. 
	//
	if ( vPos.x < m_vMax.x && vPos.x > m_vMin.x && vPos.z < m_vMax.z && vPos.z > m_vMin.z )
	{
		GASSERT(m_pParticlePool);

		PARTICLE*		pParticle = NULL;

		//	Note : 새로운 파티클 할당.
		//
		pParticle = m_pParticlePool->New();

		//	Note : 파티클 활성 리스트에 등록.
		//
		pParticle->pNext = m_pParticleHead;
		m_pParticleHead = pParticle;

		pParticle->vNowPos		= D3DXVECTOR3 ( vPos.x, vPos.y, vPos.z );
		pParticle->fPrevY		= 0.f;
		pParticle->iState		= 0;							// 무조건 올라가는 상태
		pParticle->nRand		= m_nAddNum;
		pParticle->iLeft		= ((int)m_fTime+pParticle->nRand)%4 + 1;
		pParticle->iRight		= ((int)m_fTime+pParticle->nRand)%4 + 1;
		pParticle->fSpeed		= m_fSpeed;
		pParticle->fLife		= 0.f;
		pParticle->fTexRotate	= 0.f;
		pParticle->Diffuse.w	= m_fAlphaStart;
		pParticle->Diffuse.x	= m_vColor.x*DIV_1_255;
		pParticle->Diffuse.y	= m_vColor.y*DIV_1_255;
		pParticle->Diffuse.z	= m_vColor.z*DIV_1_255;

		m_nAddNum++;

		if ( m_nAddNum >= INT_MAX)	// 오버
			m_nAddNum = 0;
	}


	return S_OK;
}

HRESULT DxEffectSpore::Update ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	바람 방향 추가
	D3DXVECTOR3 *vDirctWind = DxWeatherMan::GetInstance()->GetWindDirec ();

	//	Update
	PARTICLE* pParticlePrev = NULL;
	PARTICLE* pParticle = m_pParticleHead;
	while ( pParticle )
	{
		// 소멸
		if ( MAX_LIFE < pParticle->fLife || 
			(pParticle->vNowPos.x > m_vMax.x || pParticle->vNowPos.x < m_vMin.x || pParticle->vNowPos.z > m_vMax.z || pParticle->vNowPos.z < m_vMin.z) )
		{
			PARTICLE* pParticleThis = pParticle;

			//	Note : 리스트에서 분리.
			//
			if ( pParticle == m_pParticleHead )
			{
				m_pParticleHead = pParticle->pNext;
				pParticlePrev = NULL;

				pParticle = m_pParticleHead;
			}
			else
			{
				pParticlePrev->pNext = pParticle->pNext;

				pParticle = pParticlePrev->pNext;
			}

			//	Note : 소멸하는 파티클 Pool에 등록.
			//
			PutSleepParticle ( pParticleThis );

			continue;
		}

		//	스스르..
		//
		if ( pParticle->iState == 1 )
		{
			pParticle->vNowPos.x += ( sinf((m_fTime+pParticle->nRand)*0.5f)*0.1f + vDirctWind->x*0.3f ) * pParticle->fSpeed;
			pParticle->vNowPos.y += ( sinf((m_fTime+pParticle->nRand)*0.35f)*0.03f + sinf((m_fTime+pParticle->nRand+1)*0.11f)*0.03f ) * pParticle->fSpeed;
			pParticle->vNowPos.z += ( sinf((m_fTime+pParticle->nRand+1)*0.3f)*0.1f + vDirctWind->z*0.3f ) * pParticle->fSpeed;


			// 올라가는 형태로 변신
			if ( pParticle->vNowPos.y >= pParticle->fPrevY )
				pParticle->iState		= 0;
		}

		// 지금은 올라가고 있다..
		//
		else
		{
			pParticle->vNowPos.x += ( sinf((m_fTime+pParticle->nRand)*0.5f)*0.1f + vDirctWind->x*0.3f ) * pParticle->fSpeed;
			pParticle->vNowPos.y += ( sinf((m_fTime+pParticle->nRand)*0.35f)*0.03f + sinf((m_fTime+pParticle->nRand+1)*0.11f)*0.03f ) * pParticle->fSpeed;
			pParticle->vNowPos.z += ( sinf((m_fTime+pParticle->nRand+1)*0.3f)*0.1f + vDirctWind->z*0.3f ) * pParticle->fSpeed;

			pParticle->vNowPos.y += 0.05f;

			// 자기 스스로 노는 형태
			if ( pParticle->vNowPos.y >= 10.f + pParticle->fPrevY )
				pParticle->iState		= 1;
		}

		// 같이 해당
		pParticle->fLife += m_fElapsedTime;

		pParticle->fTexRotate += m_fRotate*m_fElapsedTime;

		// 알파값 변화
		if ( pParticle->fLife < m_fAlphaTime1 )
			pParticle->Diffuse.w += m_fAlphaLDelta1*m_fElapsedTime;
		else if ( pParticle->fLife >= m_fAlphaTime1 && pParticle->fLife < m_fAlphaTime2 )
			pParticle->Diffuse.w += m_fAlphaLDelta2*m_fElapsedTime;
		else
			pParticle->Diffuse.w += m_fAlphaLDelta3*m_fElapsedTime;

		pParticlePrev = pParticle;
		pParticle = pParticle->pNext;
	}

	return S_OK;
}

HRESULT DxEffectSpore::CreateFireFly ()
{
	D3DXVECTOR3	vPos;
	D3DXVECTOR3	vDirect;
	float		fRandHeight;

	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();

	fRandHeight = (RANDOM_POS*25.f) - 5.f;		//	-5.f ~ 15.f
	vPos		= D3DXVECTOR3( (float)((rand()%400)-200)+vLookatPt.x, vLookatPt.y+fRandHeight, (float)((rand()%400)-200)+vLookatPt.z );
	vDirect		= D3DXVECTOR3( RANDOM_POS-0.5f, RANDOM_POS-0.5f, RANDOM_POS-0.5f );

	D3DXVec3Normalize ( &vDirect, &vDirect );

	//	범위 안에 있다면 새로운 것을 생성한다. 
	//
	if ( vPos.x < m_vMax.x && vPos.x > m_vMin.x && vPos.z < m_vMax.z && vPos.z > m_vMin.z )
	{
		GASSERT(m_pFireFlyPool);

		FIREFLY*		pParticle = NULL;

		//	Note : 새로운 파티클 할당.
		//
		pParticle = m_pFireFlyPool->New();

		//	Note : 파티클 활성 리스트에 등록.
		//
		pParticle->pNext = m_pFireFlyHead;
		m_pFireFlyHead = pParticle;

		pParticle->vNowPos		= vPos;									// 초기 위치 설정
		pParticle->fLife		= 0.f;									// 생명 초기화
		pParticle->fChange		= 0.f;									// 방향 초기화 해주는 타이밍
		pParticle->fAplha		= 0.f;									// 알파값 셋팅
		pParticle->fAplhaTime	= 2.f + RANDOM_POS*5.f ;
		pParticle->fSize		= 0.f;									// 크기 초기화
		pParticle->fSpeed		= 3.f + RANDOM_POS*3.f ;				// 속도 정해줌
		pParticle->vColor		= D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.f );	// 색은 FrameMove 때 바뀜
		pParticle->vDirect		= vDirect;								// 방향 정해줌
	}

	return S_OK;
}

HRESULT DxEffectSpore::UpdateFireFly ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float	fAlpha;
	float	fAlpha_03;
	float	fAlpha_05;
	float	fAlpha_07;

	D3DXVECTOR3	vDirect;
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();

	m_dwFireFlyCount = 0;

	//	Update
	FIREFLY* pParticlePrev = NULL;
	FIREFLY* pParticle = m_pFireFlyHead;
	while ( pParticle )
	{
		if ( (pParticle->vNowPos.x<=vLookatPt.x+200.f) && (pParticle->vNowPos.x>=vLookatPt.x-200.f) &&
			(pParticle->vNowPos.z<=vLookatPt.z+200.f) && (pParticle->vNowPos.z>=vLookatPt.z-200.f) )
		{
		}
		else
		{
			FIREFLY* pParticleThis = pParticle;

			//	Note : 리스트에서 분리.
			//
			if ( pParticle == m_pFireFlyHead )
			{
				m_pFireFlyHead = pParticle->pNext;
				pParticlePrev = NULL;

				pParticle = m_pFireFlyHead;
			}
			else
			{
				pParticlePrev->pNext = pParticle->pNext;

				pParticle = pParticlePrev->pNext;
			}

			//	Note : 소멸하는 파티클 Pool에 등록.
			//
			PutSleepFireFly ( pParticleThis );

			continue;
		}

		// 같이 해당
		pParticle->fLife	+= m_fElapsedTime;
		pParticle->fAplha	+= m_fElapsedTime;
		pParticle->fChange	+= m_fElapsedTime;

		pParticle->vNowPos	+= pParticle->vDirect*pParticle->fSpeed*m_fElapsedTime;

		pParticle->vNowPos.x += sinf( pParticle->fLife ) * 0.1f;		// 곡선 이동
		pParticle->vNowPos.y += sinf( pParticle->fLife ) * 0.1f;
		pParticle->vNowPos.z += sinf( pParticle->fLife ) * 0.1f;

		pParticle->fSize	= FIREFLY_SIZE;

		if ( pParticle->fAplha >= pParticle->fAplhaTime )
		{
			pParticle->fAplha		= 0.f;
			pParticle->fAplhaTime	= 2.f + RANDOM_POS*5.f ;
		}

		fAlpha_03 = pParticle->fAplhaTime*0.3f;
		fAlpha_05 = pParticle->fAplhaTime*0.5f;
		fAlpha_07 = pParticle->fAplhaTime*0.7f;

		if ( pParticle->fAplha >= fAlpha_03 && pParticle->fAplha <= fAlpha_07 )
		{
			fAlpha	= pParticle->fAplha-fAlpha_05;		// 계산 지저분하다.
			fAlpha	/= (fAlpha_05-fAlpha_03);
			fAlpha	*= 0.8f;
			fAlpha	= fabsf(fAlpha);
			fAlpha	= 1.f - fAlpha;
		}
		else
		{
			fAlpha	= 0.2f;
		}

		pParticle->vColor.w	= fAlpha * 255.f;
		pParticle->vColor.x	= m_vFireFly_Color_1.x;
		pParticle->vColor.y	= m_vFireFly_Color_1.y;
		pParticle->vColor.z	= m_vFireFly_Color_1.z;

		if ( pParticle->vNowPos.y < (vLookatPt.y-7.f) )			// 낮으면 위로 올리고
		{
			pParticle->fChange = 0.f;

			vDirect		= pParticle->vDirect;
			vDirect		+= D3DXVECTOR3( RANDOM_POS-0.5f, RANDOM_POS, RANDOM_POS-0.5f );
			D3DXVec3Normalize ( &vDirect, &vDirect );
			pParticle->vDirect		= vDirect;
			pParticle->fSpeed		= 1.f + RANDOM_POS;			
		}
		if ( pParticle->vNowPos.y > (vLookatPt.y+40.f) )		// 높으면 아래로 내리고
		{
			pParticle->fChange = 0.f;

			vDirect		= pParticle->vDirect;
			vDirect		+= D3DXVECTOR3( RANDOM_POS-0.5f, -RANDOM_POS, RANDOM_POS-0.5f );
			D3DXVec3Normalize ( &vDirect, &vDirect );
			pParticle->vDirect		= vDirect;
			pParticle->fSpeed		= 1.f + RANDOM_POS;
		}
		if ( pParticle->fChange > 1.5f )						// 가끔씩은 움직임에 변화를 준데요
		{
			pParticle->fChange = 0.f;

			vDirect		= pParticle->vDirect;
			vDirect		+= D3DXVECTOR3( RANDOM_POS-0.5f, RANDOM_POS-0.5f, RANDOM_POS-0.5f );
			D3DXVec3Normalize ( &vDirect, &vDirect );
			pParticle->vDirect		= vDirect;
			pParticle->fSpeed		= 3.f + RANDOM_POS*3.f ;	
		}

		++m_dwFireFlyCount;

		pParticlePrev = pParticle;
		pParticle = pParticle->pNext;
	}

	return S_OK;
}

HRESULT DxEffectSpore::SetBoundBox ( LPD3DXMESH pMesh, D3DXMATRIX *matWorld )
{
	DWORD dwVertices = pMesh->GetNumVertices();

	VERTEX*	pVertices;
	pMesh->LockVertexBuffer ( 0, (VOID**)&pVertices );

	D3DXVECTOR3		pVecter;
	D3DXVec3TransformCoord ( &pVecter, &pVertices[0].p, matWorld );

	m_vMax = m_vMin = pVecter;

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		D3DXVec3TransformCoord ( &pVecter, &pVertices[i].p, matWorld );

		m_vMax.x = (m_vMax.x > pVecter.x) ? m_vMax.x : pVecter.x;
		m_vMin.x = (m_vMin.x > pVecter.x) ? pVecter.x : m_vMin.x;

		m_vMax.y = (m_vMax.y > pVecter.y) ? m_vMax.y : pVecter.y;
		m_vMin.y = (m_vMin.y > pVecter.y) ? pVecter.y : m_vMin.y;

		m_vMax.z = (m_vMax.z > pVecter.z) ? m_vMax.z : pVecter.z;
		m_vMin.z = (m_vMin.z > pVecter.z) ? pVecter.z : m_vMin.z;
	}
	pMesh->UnlockVertexBuffer ();

	return S_OK;
}

void DxEffectSpore::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	COLLISION::TransformAABB( vMax, vMin, matIdentity );
}

BOOL DxEffectSpore::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = 0.f;
	vCenter.y = 0.f;
	vCenter.z = 0.f;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}

HRESULT DxEffectSpore::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectSpore* pEff = (DxEffectSpore*)pSrcEffect;

	SAFE_RELEASE ( m_pSporeVB );
	SAFE_RELEASE ( m_pFireFlyVB );
	SAFE_RELEASE ( m_pIB );

	pd3dDevice->CreateVertexBuffer ( 4*MAX_VERTEX*sizeof(PARTICLEVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									PARTICLEVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pSporeVB, NULL );

	pd3dDevice->CreateVertexBuffer ( 4*MAX_VERTEX*sizeof(PARTICLEVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
									PARTICLEVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pFireFlyVB, NULL  );

	pd3dDevice->CreateIndexBuffer ( 6*MAX_VERTEX*sizeof(WORD), D3DUSAGE_WRITEONLY, 
									D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL  );

	PARTICLEVERTEX*	pVertices;
	m_pSporeVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<MAX_VERTEX; i++ )
	{
		pVertices[i*4+0].t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+2].t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+3].t = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pSporeVB->Unlock ();

	m_pFireFlyVB->Lock ( 0, 0, (VOID**)&pVertices, D3DLOCK_DISCARD );
	for ( DWORD i=0; i<MAX_VERTEX; i++ )
	{
		pVertices[i*4+0].t = D3DXVECTOR2 ( 0.f, 0.f );
		pVertices[i*4+1].t = D3DXVECTOR2 ( 1.f, 0.f );
		pVertices[i*4+2].t = D3DXVECTOR2 ( 0.f, 1.f );
		pVertices[i*4+3].t = D3DXVECTOR2 ( 1.f, 1.f );
	}
	m_pFireFlyVB->Unlock ();


	WORD*	pIndices;
	m_pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	for ( DWORD i=0; i<MAX_VERTEX; i++ )
	{	
		*pIndices++ = (WORD)((i*4)+0);
		*pIndices++ = (WORD)((i*4)+1);
		*pIndices++ = (WORD)((i*4)+2);

		*pIndices++ = (WORD)((i*4)+1);
		*pIndices++ = (WORD)((i*4)+3);
		*pIndices++ = (WORD)((i*4)+2);
	}
	m_pIB->Unlock ();


	SAFE_DELETE(m_pParticlePool);
	SAFE_DELETE(m_pFireFlyPool);

	m_pParticlePool = new CMemPool<PARTICLE>;
	m_pFireFlyPool = new CMemPool<FIREFLY>;

	return S_OK;
}