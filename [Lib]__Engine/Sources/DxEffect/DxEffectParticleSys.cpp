#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./ShaderConstant.h"
#include "./EditMeshs.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectParticleSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------- [정적 맴버 정의 및 초기화] ----------------------------------------------
//
//
const char DxEffectParticleSys::m_strParticle[] =
{
	"vs.1.1 \n"
	""
	"dp4 oPos.x, v0, c22		;Transform vertex position to screen \n"
	"dp4 oPos.y, v0, c23 \n"
	"dp4 oPos.z, v0, c24 \n"
	"dp4 r1.w, v0, c25			;Keep distance to calculate particle size \n"
	"mov oPos.w, r1.w \n"
	"mov oD0, v2 \n"
	"rcp r1.w, r1.w				; 1/w (used to scale vert proportional to distance) \n"
	"mul oPts.x, r1.w, v1.x		; Scale the splash by the animation seed \n"
};

const DWORD DxEffectParticleSys::m_dwParticleVertexShader[] =
{
	0xfffe0101, 0x0005fffe, 0x454c4946, 0x74726170,
	0x656c6369, 0x76766e2e, 0x00000000, 0x0002fffe,
	0x454e494c, 0x00000001, 0x0002fffe, 0x454e494c,
	0x00000002, 0x0002fffe, 0x454e494c, 0x00000003,
	0x0002fffe, 0x454e494c, 0x00000003, 0x00000009,
	0xc0010000, 0x90e40000, 0xa0e40004, 0x0002fffe,
	0x454e494c, 0x00000004, 0x0002fffe, 0x454e494c,
	0x00000004, 0x00000009, 0xc0020000, 0x90e40000,
	0xa0e40005, 0x0002fffe, 0x454e494c, 0x00000005,
	0x0002fffe, 0x454e494c, 0x00000005, 0x00000009,
	0xc0040000, 0x90e40000, 0xa0e40006, 0x0002fffe,
	0x454e494c, 0x00000006, 0x0002fffe, 0x454e494c,
	0x00000006, 0x00000009, 0x80080001, 0x90e40000,
	0xa0e40007, 0x0002fffe, 0x454e494c, 0x00000007,
	0x0002fffe, 0x454e494c, 0x00000007, 0x00000001,
	0xc0080000, 0x80ff0001, 0x0002fffe, 0x454e494c,
	0x00000008, 0x0002fffe, 0x454e494c, 0x00000008,
	0x00000001, 0xd00f0000, 0x90e40002, 0x0002fffe,
	0x454e494c, 0x00000009, 0x0002fffe, 0x454e494c,
	0x00000009, 0x00000006, 0x80080001, 0x80ff0001,
	0x0002fffe, 0x454e494c, 0x0000000a, 0x0002fffe,
	0x454e494c, 0x0000000a, 0x00000005, 0xc0010002,
	0x80ff0001, 0x90000001, 0x0000ffff
};

const DWORD	DxEffectParticleSys::TYPEID		= EFFSINGLE_PARTICE;
const DWORD	DxEffectParticleSys::FLAG		= NULL;
const char	DxEffectParticleSys::NAME[]		= "파티클";

//	Note : FVF 중 D3DFVF_TEX2 는 ( D3DXVECTOR4 Diffuse ) 값의 메모리를 가리키는 것이다.
//
const DWORD DxEffectParticleSys::D3DPARTICLEVERTEX::FVF = D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_TEX2;
const DWORD DxEffectParticleSys::D3DVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

const float	DxEffectParticleSys::MIN_SPEED			= 0.0f;
const float	DxEffectParticleSys::MIN_LIFETIME		= 0.1f;
const float	DxEffectParticleSys::MIN_SIZE			= 0.5f;
const float	DxEffectParticleSys::MIN_GRAVITY		= -5.0f;
const float	DxEffectParticleSys::MIN_ALPHA			= 0.0f;

const float	DxEffectParticleSys::MAX_SPEED			= 250.0f;
const float	DxEffectParticleSys::MAX_LIFETIME		= 10.0f;
const float	DxEffectParticleSys::MAX_SIZE			= 100.0f;
const float	DxEffectParticleSys::MAX_GRAVITY		= 5.0f;
const float	DxEffectParticleSys::MAX_ALPHA			= 1.0f;
const float	DxEffectParticleSys::_SCALEFACT_PAR		= 400.0f;

D3DXVECTOR3	DxEffectParticleSys::START_POS	= D3DXVECTOR3(0.f,0.f,0.f);
D3DXVECTOR3	DxEffectParticleSys::TARGET_POS	= D3DXVECTOR3(0.f,0.f,0.f);

BOOL DxEffectParticleSys:: VAILEDDEVICE				= FALSE;
const DWORD	DxEffectParticleSys::MAX_PARTICLES			= 500;//100;
DWORD DxEffectParticleSys::m_dwPointSpriteSh		= NULL;

DxEffectParticleSys::D3DVERTEX*		DxEffectParticleSys::m_pVertices	= NULL;
LPDIRECT3DINDEXBUFFERQ	DxEffectParticleSys::m_pIBuffer = NULL;

CMemPool<DxEffectParticleSys::PARTICLE> *DxEffectParticleSys::m_pParticlePool = NULL;
CMemPool<DxEffectParticleSys> *DxEffectParticleSys::m_pPool = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectParticleSys::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectParticleSys::m_pDrawRenderStats = NULL;

HRESULT DxEffectParticleSys::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0), 
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),
	//	D3DVSD_REG(1, D3DVSDT_FLOAT1 ),
	//	D3DVSD_REG(2, D3DVSDT_FLOAT4 ),
	//	D3DVSD_END()
	//};

	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)&m_dwParticleVertexShader, &m_dwPointSpriteSh, 
	//											DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Particle FAILED" );
	//	return E_FAIL;
	//}

	m_pVertices = new D3DVERTEX [ MAX_PARTICLES*4 ];

	// 인덱스 버퍼 생성
	pd3dDevice->CreateIndexBuffer ( 6*MAX_PARTICLES*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIBuffer, NULL );

	// 인덱스 버퍼 초기화
	WORD*	pIndies;
	m_pIBuffer->Lock ( 0, 0, (VOID**)&pIndies, 0 );
	for ( DWORD i=0; i<MAX_PARTICLES; ++i )
	{
		*pIndies++ = (WORD)(i*4 + 0);
		*pIndies++ = (WORD)(i*4 + 1);
		*pIndies++ = (WORD)(i*4 + 2);
		*pIndies++ = (WORD)(i*4 + 1);
		*pIndies++ = (WORD)(i*4 + 3);
		*pIndies++ = (WORD)(i*4 + 2);
	}
	m_pIBuffer->Unlock ();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_BLENDOP,			D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffectParticleSys::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE(m_pIBuffer);
	SAFE_DELETE_ARRAY ( m_pVertices );

	//if ( m_dwPointSpriteSh )			pd3dDevice->DeleteVertexShader ( m_dwPointSpriteSh );

	m_dwPointSpriteSh = NULL;
	
	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectParticleSys::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectParticleSys>;
	m_pParticlePool = new CMemPool<PARTICLE>;
}

void DxEffectParticleSys::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
	SAFE_DELETE(m_pParticlePool);
}

void DxEffectParticleSys::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	float fSize;
	D3DXVECTOR3 vBase;
	D3DXVECTOR3 vPos;
	LPPARTICLE pParticleNode = m_pParticleHead;
	while ( pParticleNode )
	{
		if ( pParticleNode->m_fAge + m_fElapsedTime >= pParticleNode->m_fLifetime ) 
		{
			pParticleNode = pParticleNode->m_pNext;
			continue;
		}

		fSize = pParticleNode->m_fSize*0.5f;
		D3DXVec3TransformCoord( &vBase, &pParticleNode->m_vLocation, &m_matWorld );

		vPos = vBase + D3DXVECTOR3( fSize, fSize, fSize);
		if( vMax.x < vPos.x )	vMax.x = vPos.x;
		if( vMax.y < vPos.y )	vMax.y = vPos.y;
		if( vMax.z < vPos.z )	vMax.z = vPos.z;

		vPos = vBase - D3DXVECTOR3( fSize, fSize, fSize);
		if( vMin.x > vPos.x )	vMin.x = vPos.x;
		if( vMin.y > vPos.y )	vMin.y = vPos.y;
		if( vMin.z > vPos.z )	vMin.z = vPos.z;

		pParticleNode = pParticleNode->m_pNext;
	}

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectParticleSys::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectParticleSys::InvalidateDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szCollTex.c_str(), m_pCollTEX );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectParticleSys::PutSleepParticles ( PARTICLE* pParticles )
{
	if ( !pParticles )	return;

	PARTICLE* pCur = pParticles;
	while ( pCur )
	{
		if( pCur->m_pSingleGroup )
		{
			pCur->m_pSingleGroup->InvalidateDeviceObjects ();
			pCur->m_pSingleGroup->DeleteDeviceObjects ();
			pCur->m_pSingleGroup->FinalCleanup();
			SAFE_DELETE(pCur->m_pSingleGroup);
		}

		pCur = pCur->m_pNext;
	}

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	//
	if( m_pParticlePool ) 
	{
		PARTICLE* pCur = pParticles;
		while ( pCur )
		{
			PARTICLE* pRelease = pCur;
			pCur = pCur->m_pNext;

			pRelease->m_pNext = NULL;
			m_pParticlePool->Release(pRelease);
		}
	}
}

void DxEffectParticleSys::PutSleepParticle ( PARTICLE* pParticle )
{
	if ( !pParticle )	return;

	if( pParticle->m_pSingleGroup )
	{
		pParticle->m_pSingleGroup->InvalidateDeviceObjects ();
		pParticle->m_pSingleGroup->DeleteDeviceObjects ();
		pParticle->m_pSingleGroup->FinalCleanup();
		SAFE_DELETE(pParticle->m_pSingleGroup);
	}

	//	Note : 집어 넣을 파티클 리스트의 끝 노드를 찾는다.
	if( m_pParticlePool ) 
	{
		pParticle->m_pNext = NULL;
		m_pParticlePool->Release(pParticle);
	}
}

//-------------------------------------------- [PARTICLE] ------------------------------------------------------------
//
//
DxEffectParticleSys::PARTICLE::PARTICLE() :
	m_fAge(0.0f),
	m_fSize(1.0f),
	m_bPlay(FALSE),
	m_pSingleGroup(NULL),
	m_pNext(NULL)
{
	m_cColor.x = 1.0f;
	m_cColor.y = 0.0f;
	m_cColor.z = 0.0f;
	m_cColor.w = 0.5f;

	D3DXMatrixIdentity( &m_matRotate );
	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectParticleSys::PARTICLE::~PARTICLE()
{
	if( m_pSingleGroup )
	{
		m_pSingleGroup->InvalidateDeviceObjects ();
		m_pSingleGroup->DeleteDeviceObjects ();
		m_pSingleGroup->FinalCleanup();
		SAFE_DELETE(m_pSingleGroup);
	}

	SAFE_DELETE(m_pNext);
}

void DxEffectParticleSys::PARTICLE::CreateSingleGroup( LPDIRECT3DDEVICEQ pd3dDevice, EFF_PROPGROUP* pPropGroup )
{
	if( m_pSingleGroup )
	{
		m_pSingleGroup->InvalidateDeviceObjects ();
		m_pSingleGroup->DeleteDeviceObjects ();
		m_pSingleGroup->FinalCleanup();
		SAFE_DELETE(m_pSingleGroup);
	}

	m_pSingleGroup = pPropGroup->NEWEFFGROUP();
	if ( !m_pSingleGroup )	return;

	//	Note : Eff Group Create Dev.
	m_pSingleGroup->Create( pd3dDevice );

	m_matWorld._41 = m_vLocation.x;
	m_matWorld._42 = m_vLocation.y;
	m_matWorld._43 = m_vLocation.z;
	m_pSingleGroup->m_matWorld = m_matWorld;
}

inline BOOL DxEffectParticleSys::PARTICLE::FrameMove ( float fElapsedTime, D3DXVECTOR3 *pAttractLoc, D3DXVECTOR3 *pGravity, BOOL bFromTo, DWORD dwFlag )
{
	if( m_pSingleGroup )
	{
		if( m_bPlay )
		{
			m_pSingleGroup->FrameMove( 0.f, fElapsedTime );
		}

		if ( m_pSingleGroup->m_dwAliveCount == 0 )
		{
			m_pSingleGroup->InvalidateDeviceObjects ();
			m_pSingleGroup->DeleteDeviceObjects ();
			m_pSingleGroup->FinalCleanup();
			SAFE_DELETE(m_pSingleGroup);
		}
	}

	if ( m_fAge + fElapsedTime >= m_fLifetime ) 
	{
		if( !m_pSingleGroup )	// 파티클에 속해있는 효과도 삭제 됨.
		{
			m_fAge = -1.0f;
			return FALSE;
		}

		// Note : USENEWEFF_END 이 활성화 되어 있는 경우를 대비해서 넣어준다.
		m_bPlay = TRUE;

		if( dwFlag&USENEWEFF_END_STOP )	// 부모가 움직이는것을 원하지 않는다면 return 한다.
		{
			return TRUE;
		}
	}

	m_fAge += fElapsedTime;

	m_vPrevLocation = m_vLocation;

	if ( m_fAge == fElapsedTime )	m_vLocation += (m_vVelocity*fElapsedTime);
	else							m_vLocation += (m_vVelocity*fElapsedTime) + (m_vForceFull*(1.f-(m_fForceVar*m_fAge)));
	m_vVelocity += ( *pGravity * m_fGravity * fElapsedTime );

	if ( pAttractLoc && m_fAge > 0.03f )
	{
		D3DXVECTOR3 AttractDir = *pAttractLoc - m_vLocation;
		D3DXVec3Normalize ( &AttractDir, &AttractDir );
		m_vVelocity += AttractDir * 25.0f * fElapsedTime;
	}

	//	Note : 지면과 충돌시	- 
	//
	if ( m_bCollision )
	{
		DxLandMan*	pLandMan = DxEffSingleMan::GetInstance().GetLandMan();

		if ( pLandMan )	// 이상														// 처음보는 위치이면 Picking 계산함
		{
			BOOL		bCollision;
			D3DXVECTOR3	vCollision;
			D3DXVECTOR3	vPoint1( m_vLocation.x,	m_vLocation.y+5.f,		m_vLocation.z );		// 뿌려지는 중심 위치 구하기
			D3DXVECTOR3	vPoint2( m_vLocation.x,	m_vLocation.y-100.f,	m_vLocation.z );		// 뿌려지는 중심 위치 구하기
			LPDXFRAME	pDxFrame = NULL;

			if ( m_vVelocity.y < 0.f )
			{
				pLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, TRUE );

				if ( (m_vLocation.y<=vCollision.y) &&  (m_vPrevLocation.y>=vCollision.y))
				{
					m_vVelocity.y = -m_vVelocity.y*0.7f;
					m_vLocation.y += m_vVelocity.y * fElapsedTime;
				}
			}
		}
	}

	if ( bFromTo )		// 어디에서 어디까지 가라는 목표가 있을 경우 사용한다.
	{
		D3DXVECTOR3	vDelta = TARGET_POS - START_POS;
		m_vLocation	= START_POS + (vDelta*(m_fAge/m_fLifetime));
	}

	//	Note : 알파 조정 
	//
	if ( m_fAge < m_fAlphaTime1 )									m_fAlpha += m_fAlphaDelta1 * fElapsedTime;
	else if ( m_fAge >= m_fAlphaTime1 && m_fAge < m_fAlphaTime2)	m_fAlpha += m_fAlphaDelta2 * fElapsedTime;
	else															m_fAlpha += m_fAlphaDelta3 * fElapsedTime;

	if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
	if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

	//	Note : 컬러 조정 
	//
	m_cColor.x += m_cColorDelta.x * fElapsedTime;
	m_cColor.y += m_cColorDelta.y * fElapsedTime;
	m_cColor.z += m_cColorDelta.z * fElapsedTime;
	m_cColor.w = m_fAlpha;

	//	Note : 사이즈, 중력 변화
	//	
	m_fSize += m_fSizeDelta * fElapsedTime;
	if ( m_fSize <= 0.f )	m_fSize = 0.01f;		// 0.f 일 경우 버그 생성 가능성 때문에.

	m_fGravity += m_fGravityDelta * fElapsedTime;

	//	Note : 텍스쳐 회전 적용
	//
	m_fTexRotateAngelSum += m_fTexRotateAngel * fElapsedTime;
	m_vTexScale += m_vTexScaleDelta * fElapsedTime;

	return TRUE;
}

//-------------------------------------------- [클레스 본체] ------------------------------------------------------------
//
//
DxEffectParticleSys::DxEffectParticleSys () :
	m_fEmissionResidue(0.0f),
	m_pTexture(NULL),
	m_pParticleHead(NULL),
	m_vGVelocity(D3DXVECTOR3 (0,0,0)),
	m_vGGravity(D3DXVECTOR3 (0,0,0)),
	m_vGGravityDelta(D3DXVECTOR3(0,0,0)),
	m_vRange(D3DXVECTOR3(0,0,0)),
	m_fRangeRate(0.f),
	m_fRotateAngel(0.f),
	m_bRandomPlay(FALSE),
	m_fRandomPlayTime(0.f),
	m_vPlayTime(0.f,0.f,0.f),
	m_vSleepTime(0.f,0.f,0.f),

	m_fRotateLDelta1(0.f),		// 범위	회전
	m_fRotateLDelta2(0.f),
	m_fRotateLDelta3(0.f),
	m_fRotateLTime1(0.f),
	m_fRotateLTime2(0.f),

	m_uParticlesAlive(0),
	m_vGLocation(0,0,0),
	m_vGPrevLocation(0,0,0),
	m_fRotateAngelSum(0.f),
	m_fRotateLAngelSum(0.f),
	m_fElapsedTimeSum(0.f),
	m_pCollTEX(NULL),

	m_pd3dDevice(NULL)
{
	m_fAlpha			= 0.f;

	m_cColorStart.r		= 0.0f;
	m_cColorStart.g		= 0.0f;
	m_cColorStart.b		= 0.0f;
	m_cColorStart.a		= 0.4f;
			
	m_cColorVar.r		= 0.0f;
	m_cColorVar.g		= 0.0f;
	m_cColorVar.b		= 0.0f;
	m_cColorVar.a		= 0.0f;

	m_cColorEnd.r		= 0.0f;
	m_cColorEnd.g		= 0.0f;
	m_cColorEnd.b		= 0.0f;
	m_cColorEnd.a		= 0.0f;

	m_fSizeStart		= 8.0f;
	m_fSizeVar			= 0.0f;
	m_fSizeEnd			= 18.0f;

	m_fSpeed			= 2.0f;
	m_fSpeedVar			= 0.0f;

	m_fRotateLAngel		= 0.f;
	m_fLength			= 0.f;

	m_fTheta			= 180.0f;
		
	m_fLife				= 2.0f;
	m_fLifeVar			= 0.5f;

	m_fGravityStart		= -0.1f;
	m_fGravityVar		= 0.0f;
	m_fGravityEnd		= 0.0f;

	m_uParticlesPerSec	= 40;

	m_nPower			= 1;
	m_nBlend			= 0;

	m_fGLifeTime		= 0.0f;

	m_uParticlesCreated = 0;

	m_nCol				= 4;			// 행
	m_nRow				= 4;			// 열

	m_nAllSprite		= 16;		// 전체 스프라이트 갯수
	m_fTimeSum			= 0.f;

	m_fElapsedTime = 0.f;

	D3DXMatrixIdentity ( &m_matRotate );
	D3DXMatrixIdentity ( &matPrevComb );

	D3DXMatrixIdentity( &m_matWorld );
	m_nCountR = 0;
	D3DXMatrixIdentity( &m_matRandomR );
}

DxEffectParticleSys::~DxEffectParticleSys ()
{
	CleanUp ();
}

HRESULT DxEffectParticleSys::FinalCleanup()
{
	DxEffectParticleSys::PutSleepParticles ( m_pParticleHead );
	m_pParticleHead = NULL;

	//	Note : 형제 자식 노드.
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

HRESULT DxEffectParticleSys::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3 vDeltaGVel;
	D3DXVECTOR3	vLocal (0.f,9.8f,0.f);

	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fElapsedTime = fElapsedTime;
	m_fTimeSum += fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )	m_dwRunFlag |= EFF_PLY_DONE;


	//	Note : 회전 적용
	//
	if ( (m_dwFlag&USERANGEHIGH) )			m_fRotateAngelSum += m_fRotateAngel * fElapsedTime;

	if ( !( m_dwRunFlag & EFF_PLY_DONE ) )
	{
		//	Note : 위치 이동.
		//
		m_vGPrevLocation = m_vGLocation;
		m_vGLocation += m_vGVelocity * fElapsedTime;


		//	Note : 회전에서 필요, 시간의 누적
		m_fElapsedTimeSum += fElapsedTime;

		//	Note : 속도 변화. [ GRAVITY 문제가 있는거 같음. ]  - [수정필요]
		vDeltaGVel = m_vGGravity*fElapsedTime;

		m_vGVelocity += vDeltaGVel;

		//	Note : 중력 변화.
		m_vGGravity += m_vGGravityDelta * fElapsedTime;
	}

	//	Note : 중력.. 중력. 부모의 회전이 나에게 영향을 받도록 만든다.
	D3DXVec3TransformCoord ( &vLocal, &vLocal, &m_matRotate );

	// Note : 랜덤 Play 셋팅
	if ( m_dwFlag & USERANDOMPLAY )
	{
		if( m_bRandomPlay )
		{
			if( m_fRandomPlayTime > m_vPlayTime.z )
			{
				m_fRandomPlayTime = 0.f;
				m_bRandomPlay = FALSE;

				m_vSleepTime.z = m_vSleepTime.x + m_vSleepTime.y*RANDOM_POS;
			}
		}
		else
		{
			if( m_fRandomPlayTime > m_vSleepTime.z )	
			{
				m_fRandomPlayTime = 0.f;
				m_bRandomPlay = TRUE;

				m_vPlayTime.z = m_vPlayTime.x + m_vPlayTime.y*RANDOM_POS;

				// Note : 효과를 재 Play 할 경우 방향을 바꿔주는 셋팅
				if ( m_dwFlag & USERANDOMDIRECT )
				{
					D3DXMATRIX matRotate;
					if( m_nCountR==0 )
					{
						D3DXMatrixRotationX( &matRotate, (float)rand() );
						D3DXMatrixMultiply( &m_matRandomR, &m_matRandomR, &matRotate );
					}
					else if( m_nCountR==1 )
					{
						D3DXMatrixRotationY( &matRotate, (float)rand() );
						D3DXMatrixMultiply( &m_matRandomR, &m_matRandomR, &matRotate );
					}
					else if( m_nCountR==2 )
					{
						D3DXMatrixRotationZ( &matRotate, (float)rand() );
						D3DXMatrixMultiply( &m_matRandomR, &m_matRandomR, &matRotate );
					}

					++m_nCountR;

					if( m_nCountR==3 )	m_nCountR = 0;
				}
			}
		}

		m_fRandomPlayTime += fElapsedTime;
	}

	//	Note : 파티클 업데이트.
	m_uParticlesAlive = 0;
	D3DXVECTOR3 *pAttractLoc = NULL;
	if (IsAttractive() )
	{
		if ( m_dwFlag&USEPARENTMOVE )		pAttractLoc = &D3DXVECTOR3 ( 0.f, 0.f, 0.f );
		else								pAttractLoc = &(m_pThisGroup->m_vGNowPos);
	}

	BOOL	bFromTo;
	if ( m_dwFlag&USEFROMTO )
	{
		bFromTo		= TRUE;
		START_POS	= m_pThisGroup->m_vGNowPos;
		TARGET_POS	= m_pThisGroup->m_vTargetPos;
	}
	else
	{
		bFromTo		= FALSE;
		START_POS	= D3DXVECTOR3(0.f,0.f,0.f);
		TARGET_POS	= D3DXVECTOR3(0.f,0.f,0.f);
	}

	LPPARTICLE pParticleNode = m_pParticleHead, pParticlePrev = NULL;
	while ( pParticleNode )
	{
		if ( pParticleNode->FrameMove ( m_fElapsedTime, pAttractLoc, &vLocal, bFromTo, m_dwFlag ) )
		{
			m_uParticlesAlive++;

			pParticlePrev = pParticleNode;
			pParticleNode = pParticleNode->m_pNext;
		}
		else
		{
			LPPARTICLE pParticleThis = pParticleNode;

			//	Note : 리스트에서 분리.
			//
			if ( pParticleThis == m_pParticleHead )
			{
				m_pParticleHead = pParticleNode->m_pNext;
				pParticlePrev = NULL;

				pParticleNode = m_pParticleHead;
			}
			else
			{
				pParticlePrev->m_pNext = pParticleNode->m_pNext;

				pParticleNode = pParticlePrev->m_pNext;
			}

			//	Note : 소멸하는 파티클 Pool에 등록.
			PutSleepParticle ( pParticleThis );
		}
	}

	float fParticlesNeeded = m_uParticlesPerSec * m_fElapsedTime + m_fEmissionResidue;
	m_uParticlesCreated = (DWORD) fParticlesNeeded;

	if ( m_uParticlesCreated >= m_uParticlesPerSec )
	{
		m_uParticlesCreated = m_uParticlesPerSec;
	}

	//if ( !( m_dwFlag & USESUPPRESS ) )
	//{
		m_fEmissionResidue = fParticlesNeeded - m_uParticlesCreated;
	//}
	//else
	//{
	//	m_fEmissionResidue = fParticlesNeeded;
	//	m_uParticlesCreated = 0;
	//}

	if ( m_dwRunFlag & EFF_PLY_DONE )
	{
		m_uParticlesCreated = 0;
		if ( m_uParticlesAlive == 0 )		m_dwRunFlag |= EFF_PLY_END;
	}


_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}


void DxEffectParticleSys::SetMatrix ( D3DXMATRIX &matWorld, D3DXMATRIX &matComb )
{
	float		fTime;
	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	D3DXMATRIX matTrans, matEmitter, matRotate, matParent;

	//	Note : 부모 회전 적용된 것과 안된 매트릭스 만들기
	//			그룹의 시작위치가 DTS_WORLD의 이동에 영향을 주어야 한다.	뿌려야 할 위치 매트릭스!!!
	//
	if ( m_dwFlag & USEPARENTROTATEWORLD )	// 월드 에디터 적용..
	{
		matEmitter = matComb;
		matEmitter._41 = m_pThisGroup->m_matWorld._41;
		matEmitter._42 = m_pThisGroup->m_matWorld._42;
		matEmitter._43 = m_pThisGroup->m_matWorld._43;
	}
	else
	{
		D3DXMatrixIdentity ( &matEmitter );
		matEmitter._41 = m_pThisGroup->m_matWorld._41;
		matEmitter._42 = m_pThisGroup->m_matWorld._42;
		matEmitter._43 = m_pThisGroup->m_matWorld._43;
	}

	//	Note : 자기 자신의 매트릭스를 넣어준다.
	//
	matWorld = m_matLocal;
	matWorld._41 = 0.f;
	matWorld._42 = 0.f;
	matWorld._43 = 0.f;

	//	Note : 자기 자신의 회전 적용
	//
	if ( ( m_dwFlag & USERANGEHIGH ) && ( m_dwFlag & USERANGE ) )	
	{
		D3DXMatrixRotationY ( &matRotate, m_fRotateAngelSum*D3DX_PI/180.f );
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
	}


	//	Note : 월드 툴에서 넣어주는 회전(!!!회전!!!)을 셋팅 하기 위하여 이 계산을 한다.
	//
	D3DXMatrixMultiply ( &m_matRotate, &matEmitter, &matWorld );
	m_matRotate._41 = 0.f;
	m_matRotate._42 = 0.f;
	m_matRotate._43 = 0.f;


	// 완전 적용	(상대좌표)	//	빗방울(캐릭), 불(월드)	, 이동을 해도 원본 그상태, 문제점 회전시 빌보드 문제
	if ( (m_dwFlag&USEPARENTMOVE) )			
	{
		matWorld._41 = matComb._41;
		matWorld._42 = matComb._42;
		matWorld._43 = matComb._43;
		matWorld._44 = matComb._44;
	}
	// 적용 안 시킴	(절대좌표)	//							, 이동을 하면 흔들린다.
	else
	{
		matWorld._44 = matComb._44;
	}

	//	Note : 	자신의 위치에서 원점으로 가기위한 보간
	//
	if ( m_dwFlag & USEGOTOCENTER )
	{
		matTrans = m_matLocal;

		fTime = m_fGBeginTime+m_fGLifeTime-m_fGAge;

		vDelta.x = m_matLocal._41/m_fGLifeTime;
		vDelta.y = m_matLocal._42/m_fGLifeTime;
		vDelta.z = m_matLocal._43/m_fGLifeTime;

		matTrans._41 = vDelta.x*fTime;
		matTrans._42 = vDelta.y*fTime;
		matTrans._43 = vDelta.z*fTime;

		D3DXMatrixMultiply ( &matWorld, &matWorld, &matTrans );
	}
}

extern BOOL	g_bEFFtoTEX;

void DxEffectParticleSys::NewParticleCreate( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, D3DXVECTOR3 vPos_NowPrev )
{
	GASSERT(m_pParticlePool);

	// Note : Play가 아닐때는 생성을 안한다.
	if ( m_dwFlag & USERANDOMPLAY )
	{
		if( !m_bRandomPlay )
		{
			m_uParticlesCreated = 0;
			return;
		}
	}

	if ( !m_pThisGroup->m_bOut_Particle )	m_uParticlesCreated = 0;

	if( !g_bEFFtoTEX )
	{
		if ( m_uParticlesCreated >= 1 )		m_uParticlesCreated = 1;	// 1 보다 크면 제한 .. 끊김현상 제거
	}

	D3DXMATRIX matTrans, matRotate, matParent;

	int		nBlur = 5;
	if ( m_dwFlag & USEBEZIER_P )
	{
		m_uParticlesCreated *= nBlur;	// 잔상 때.
	}

	//	Note : DONE 상태에서 샐행이 된다.
	//
	LPPARTICLE pParticle = NULL;
	while  ( m_uParticlesCreated > 0 && !(m_dwRunFlag&EFF_PLY_DONE) )
	{
		//	Note : 새로운 파티클 할당.
		pParticle = m_pParticlePool->New();

		//	Note : 파티클 활성 리스트에 등록.
		pParticle->m_pNext = m_pParticleHead;
		m_pParticleHead = pParticle;


		//	Note : 파티클 초기화.
		pParticle->m_fAge = 0.0f;

		pParticle->m_fLifetime = m_fLife + RANDOM_NUM*m_fLifeVar;
		Clamp ( pParticle->m_fLifetime, MIN_LIFETIME, MAX_LIFETIME );

		//	Note : 파티클 알파 초기화
		pParticle->m_fAlphaTime1	= m_fAlphaTime1;
		pParticle->m_fAlphaTime2	= m_fAlphaTime2;

		pParticle->m_fAlpha			= m_fAlpha;
		pParticle->m_fAlphaDelta1	= m_fAlphaDelta1;
		pParticle->m_fAlphaDelta2	= m_fAlphaDelta2;
		pParticle->m_fAlphaDelta3	= m_fAlphaDelta3;

		//	Note : 파티클 컬러 초기화
		pParticle->m_cColor.x = m_cColorStart.r + RANDOM_NUM*m_cColorVar.r;
		pParticle->m_cColor.y = m_cColorStart.g + RANDOM_NUM*m_cColorVar.g;
		pParticle->m_cColor.z = m_cColorStart.b + RANDOM_NUM*m_cColorVar.b;
		pParticle->m_cColor.w = m_fAlpha;

		Clamp ( pParticle->m_cColor.x, 0.0f, 1.0f );
		Clamp ( pParticle->m_cColor.y, 0.0f, 1.0f );
		Clamp ( pParticle->m_cColor.z, 0.0f, 1.0f );

		pParticle->m_cColorDelta.x = ( m_cColorEnd.r - pParticle->m_cColor.x ) / pParticle->m_fLifetime;
		pParticle->m_cColorDelta.y = ( m_cColorEnd.g - pParticle->m_cColor.y ) / pParticle->m_fLifetime;
		pParticle->m_cColorDelta.z = ( m_cColorEnd.b - pParticle->m_cColor.z ) / pParticle->m_fLifetime;

		//	Note : 파티클 사이즈 초기화
		pParticle->m_fSize = m_fSizeStart + RANDOM_NUM*m_fSizeVar;
		Clamp ( pParticle->m_fSize, MIN_SIZE, MAX_SIZE );

		pParticle->m_fSizeDelta = ( m_fSizeEnd - pParticle->m_fSize ) / pParticle->m_fLifetime;

		//	Note : 파티클 중력 초기화
		pParticle->m_fGravity = m_fGravityStart * 9.8f + RANDOM_NUM * m_fGravityVar * 9.8f;
		Clamp ( pParticle->m_fGravity, MIN_GRAVITY*9.8f, MAX_GRAVITY*9.8f );
		pParticle->m_fGravityDelta = ( m_fGravityEnd*9.8f - pParticle->m_fGravity ) / pParticle->m_fLifetime;

		//	Note : 파티클 텍스쳐 회전 초기화
		pParticle->m_fTexRotateAngel = m_fTexRotateAngel + RANDOM_NUM;
		pParticle->m_fTexRotateAngelSum = RANDOM_NUM;

		//	Note : 파티클 시퀀스 시행 초기화
		pParticle->m_nNowSprite		=	0;
		pParticle->m_bSequenceRoop	=	FALSE;

		//	Note : 파티클 텍스쳐 스케일 초기화
		pParticle->m_vTexScale = m_vTexScale;
		pParticle->m_vTexScaleDelta = m_vTexScaleDelta;

		//	Note : 파티클 각각이 충돌을 한다.
		if ( m_dwFlag & USECOLLISION )	pParticle->m_bCollision = TRUE;
		else							pParticle->m_bCollision = FALSE;

		D3DXVECTOR3 tempVelocity = ( m_vGLocation - m_vGPrevLocation ) / m_fElapsedTime;

		//	Note : 초기 생성 위치 지정. 
		pParticle->m_vLocation = m_vGPrevLocation + tempVelocity * RANDOM_NUM * m_fElapsedTime;
		
		//	Note : 생성 범위 지정 
		D3DXVECTOR3 tempLocation (0.f, 1.f, 0.f );

		if ( m_dwFlag & USERANGE )
		{
			tempLocation.x = (m_vRange.x * (rand()%1000)/1000.f) - (m_vRange.x*0.5f);
			tempLocation.y = (m_vRange.y * (rand()%1000)/1000.f) - (m_vRange.y*0.5f);
			tempLocation.z = (m_vRange.z * (rand()%1000)/1000.f) - (m_vRange.z*0.5f);

			//	Note : 원이나 직사각형으로 만들 수 있도록 옵션을. TRUE - 원, FALSE - 직사각형
			//
			if ( m_dwFlag & USESPHERE )
			{
				if ( (tempLocation.x*tempLocation.x) + (tempLocation.z*tempLocation.z) > (m_vRange.x*m_vRange.x/4.f) )
				{
					tempLocation = tempLocation/2.f;
				}

				//	Note : 생성 범위안에 있는 파티클 중 .. 일정범위에 있는 것들이  나오도록 설정할 수 있다.
				//
				if ( m_fRangeRate > 0.f )
				{
					float	fTempLong;

					fTempLong = m_vRange.x/2.f;
					if ( fTempLong < tempLocation.y/2.f ) fTempLong = tempLocation.y;
					if ( fTempLong < tempLocation.z/2.f ) fTempLong = tempLocation.z;	//최고 긴 것을 찾는다.

					// 1. 일정 범위에 있는지 검사		// 바꾸자
					float fTemp1 = sqrtf( (tempLocation.x*tempLocation.x) + (tempLocation.y*tempLocation.y) );
					if ( sqrtf( (fTemp1*fTemp1) + (tempLocation.z*tempLocation.z) ) < fTempLong*m_fRangeRate )
					{
						
						// 2. 시작점 찾기
						float		fRate, fTemp;
						D3DXVECTOR3 tempStart;
						tempStart = (m_vRange)*m_fRangeRate/2.f;

						fRate = fabsf(tempStart.x/tempLocation.x);

						fTemp = fabsf(tempStart.y/tempLocation.y);
						if ( fRate > fTemp ) fRate = fTemp;

						fTemp = fabsf(tempStart.z/tempLocation.z);
						if ( fRate > fTemp ) fRate = fTemp;	// 최소 비율 찾기

						tempStart = tempLocation * fRate;	// 비율로 곱하기

						// 3. 비율로써 새로운 점 찾기
						tempLocation = tempStart + tempLocation*(1-m_fRangeRate);
					}
				}
			}
			else
			{
				//	Note : 생성 범위안에 있는 파티클 중 .. 일정범위에 있는 것들이  나오도록 설정할 수 있다.
				//
				if ( m_fRangeRate > 0.f )
				{
					// 1. 일정 범위에 있는지 검사	// 이것은 직사각형, 육면체만 해당
					if ( m_vRange.x*m_fRangeRate/2.f >= tempLocation.x && -m_vRange.x*m_fRangeRate/2.f <= tempLocation.x &&
						m_vRange.y*m_fRangeRate/2.f >= tempLocation.y && -m_vRange.y*m_fRangeRate/2.f <= tempLocation.y &&
						m_vRange.z*m_fRangeRate/2.f >= tempLocation.z && -m_vRange.z*m_fRangeRate/2.f <= tempLocation.z )
					{
						
						// 2. 시작점 찾기
						float		fRate, fTemp;
						D3DXVECTOR3 tempStart;
						tempStart = (m_vRange)*m_fRangeRate/2.f;

						fRate = fabsf(tempStart.x/tempLocation.x);

						fTemp = fabsf(tempStart.y/tempLocation.y);
						if ( fRate > fTemp ) fRate = fTemp;

						fTemp = fabsf(tempStart.z/tempLocation.z);
						if ( fRate > fTemp ) fRate = fTemp;	// 최소 비율 찾기

						tempStart = tempLocation * fRate;	// 비율로 곱하기

						// 3. 비율로써 새로운 점 찾기
						tempLocation = tempStart + tempLocation*(1-m_fRangeRate);
					}
				}
			}
			pParticle->m_vLocation += tempLocation;
		}

		//	Note : 회전 적용
		if ( m_dwFlag & USEROTATEL )
		{
			D3DXVECTOR3		vCenter ( 0.f, 0.f, 0.f );
			D3DXVECTOR3		vPoint ( 1.f, 0.f, 0.f );		
			D3DXVECTOR3		vTempPoint1 ( -0.01f, 10000.f, 0.f );
			D3DXVECTOR3		vTempPoint2 ( -0.01f, -10000.f, 0.f );
			D3DXPLANE		vPlane;										

			if ( tempVelocity.y <= 0.01f && tempVelocity.y >= -0.01f )		vPoint = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
			else
			{
				D3DXPlaneFromPointNormal ( &vPlane, &vCenter, &tempVelocity );
				vPoint = DxSplit ( &vPlane, &vTempPoint1, &vTempPoint2 );
				D3DXVec3Normalize ( &vPoint, &vPoint );
			}
			
			//	Note : 회전 반지름 - FrameMove
			//
			if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fRotateLTime1)
				m_fLength +=  m_fRotateLDelta1*m_fElapsedTimeSum;
			else if ( m_fGAge >= m_fRotateLTime1 && m_fGAge < m_fRotateLTime2)	
				m_fLength +=  m_fRotateLDelta2*m_fElapsedTimeSum;
			else														
				m_fLength +=  m_fRotateLDelta3*m_fElapsedTimeSum;


			D3DXMATRIX matRotateL;
			vPoint *= m_fLength;
			m_fRotateLAngelSum += m_fRotateLAngel * m_fElapsedTimeSum * 360.f;
			if ( tempVelocity.y > 0.f )	D3DXMatrixRotationAxis ( &matRotateL ,&tempVelocity , m_fRotateLAngelSum*D3DX_PI/180.f );	// 회전 매트릭스 구함
			else						D3DXMatrixRotationAxis ( &matRotateL ,&tempVelocity , m_fRotateLAngelSum*D3DX_PI/180.f + D3DX_PI );	// 회전 매트릭스 구함

			D3DXVec3TransformCoord ( &vCenter, &vPoint ,&matRotateL );

			pParticle->m_vLocation += vCenter;
			m_fElapsedTimeSum = 0.f;
		}

		//	Note : 메쉬 파티클이 방향성을 가지지 않으면 랜덤하게 회전시켜 놓는다.
		if ( (m_dwFlag&USEMESH) && !(m_dwFlag&USEDIRECTION) )
		{
			//D3DXMatrixIdentity ( &matTrans );
			//D3DXMatrixRotationX ( &matRotate, (float)rand() );
			//D3DXMatrixMultiply ( &matTrans, &matTrans, &matRotate );
			//D3DXMatrixRotationY ( &matRotate, (float)rand() );
			//D3DXMatrixMultiply ( &matTrans, &matTrans, &matRotate );
			//D3DXMatrixRotationZ ( &matRotate, (float)rand() );

			D3DXMatrixIdentity ( &matTrans );
			D3DXMatrixRotationY ( &matRotate, (float)rand() );

			D3DXMatrixMultiply ( &pParticle->m_matRotate, &matTrans, &matRotate );
		}

		//	Note : 이동 함 생각 해 볼까 !
		//
		if ( m_dwFlag&USEPARENTMOVE )			// 완전 적용	(상대좌표)	//	빗방울(캐릭)
		{
			matParent = m_matLocal;
			//D3DXVec3TransformCoord ( &pParticle->m_vLocation, &pParticle->m_vLocation, m_pmatLocal );
		}
		else	// 회전만 적용	(절대좌표)	// 불(월드)
		{
		//	matParent = matComb;
		//	matParent._41 = m_pThisGroup->m_matWorld._41;
		//	matParent._42 = m_pThisGroup->m_matWorld._42;
		//	matParent._43 = m_pThisGroup->m_matWorld._43;
			D3DXMatrixMultiply ( &matParent, &m_matLocal, &matComb );//&matParent );

			//D3DXVec3TransformCoord ( &pParticle->m_vLocation, &pParticle->m_vLocation, &matParent );
		}

		//	Note : 회전 함 생각 해 볼까 !
		//
		if ( m_dwFlag & USEPARENTROTATE )
		{
			matRotate = matComb;
			matRotate._41 = 0.f;
			matRotate._42 = 0.f;
			matRotate._43 = 0.f;
			matRotate._44 = 1.f;
			//D3DXMatrixMultiply ( &matParent, &matParent, &matRotate );
			D3DXMatrixMultiply_RM3( matParent, matRotate );
		}

		//	Note : 이동과 회전 적용 최종본
		//
		D3DXVec3TransformCoord ( &pParticle->m_vLocation, &pParticle->m_vLocation, &matParent );

		float fRate = 1.f;

		//	Note : 보간을 위해서 마지막을 장식..
		//
		if ( m_dwFlag & USEBEZIER_P )	fRate = 1.f - (float)(m_uParticlesCreated % nBlur)/(float)(nBlur);

		pParticle->m_vLocation -= m_pThisGroup->m_vGDelta * m_fElapsedTime * fRate;


		pParticle->m_vPrevLocation = m_vGLocation;

		float RandomYaw = RANDOM_POS * 3.141592f * 2.0f;
		float RandomPitch = RANDOM_POS * m_fTheta *  3.141592f / 360.0f;

		if ( m_dwFlag&USEHEIGHTNO )
		{
			pParticle->m_vVelocity.y = 0.f;
			pParticle->m_vVelocity.x = cosf ( RandomYaw );
			pParticle->m_vVelocity.z = sinf ( RandomYaw );
		}
		else
		{
			pParticle->m_vVelocity.y = cosf ( RandomPitch );
			pParticle->m_vVelocity.x = sinf ( RandomPitch ) * cosf ( RandomYaw );
			pParticle->m_vVelocity.z = sinf ( RandomPitch ) * sinf ( RandomYaw );
		}

		//	Note : Ver.101 에 추가된 작업
		//
		pParticle->m_vVelocity += m_vWindDir;
		



		pParticle->m_fSize			*= m_pThisGroup->m_fOut_Scale;		// 밖에서의 스케일 변화 영향
		pParticle->m_fSizeDelta		*= m_pThisGroup->m_fOut_Scale;		// 밖에서의 스케일 변화 영향
		pParticle->m_fGravity		*= m_pThisGroup->m_fOut_Scale;		// 밖에서의 스케일 변화 영향
		pParticle->m_fGravityDelta	*= m_pThisGroup->m_fOut_Scale;		// 밖에서의 스케일 변화 영향
		pParticle->m_vVelocity		*= m_pThisGroup->m_fOut_Scale;		// 밖에서의 스케일 변화 영향




		//	Note : Ver.101 때 작업 - 모이게 시키는 작업
		//
		if ( m_dwFlag & USECENTER )
		{
			D3DXVECTOR3	vLocalPos;
			D3DXVECTOR3	vLocalCenter;

			if ( m_dwFlag&USEPARENTMOVE )
			{
				vLocalCenter = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
				vLocalCenter.y += m_pThisGroup->m_vLocal.y;
			}
			else
			{
				vLocalCenter = m_pThisGroup->m_vGNowPos;
				vLocalCenter.y += m_pThisGroup->m_vLocal.y;
			}

			vLocalPos = (pParticle->m_vLocation-vLocalCenter);

			//	-( (pParticle->m_vLocation-모이는 위치) / pParticle->m_fLifetime );
			pParticle->m_vVelocity = -( vLocalPos / pParticle->m_fLifetime );
			m_fSpeed = 1.f;		// 이것은 거리로 속도를 계산하기 때문에 필요가 없어진다.
			m_fSpeedVar = 0.f;
		}

		//	Note : 중력.. 중력. 부모의 회전이 나에게 영향을 받도록 만든다.
		//
		D3DXVec3TransformCoord ( &pParticle->m_vVelocity, &pParticle->m_vVelocity, &m_matRotate );

		//	Note : 이동방향이 각각의 파티클에게도 영향을 받도록 해 준다.
		//
		if ( !(m_dwFlag&USEPARENTMOVE) )
		{
			float	fAvgFps = 0.03f/m_fElapsedTime;
			if ( fAvgFps > 1.f )	fAvgFps = 1.f;

			if ( m_pThisGroup->m_dwFlag & EFF_CFG_TARGETPOS )		pParticle->m_vForceFull = vPos_NowPrev*0.1f*fAvgFps;
			else if ( m_pThisGroup->m_dwFlag & EFF_CFG_TARGETID )	pParticle->m_vForceFull = vPos_NowPrev*0.5f*fAvgFps;
			else													pParticle->m_vForceFull = m_pThisGroup->m_vDirection;
			pParticle->m_fForceVar = m_fForceVar;
		}
		else
		{
			pParticle->m_vForceFull = D3DXVECTOR3 ( 0.f, 0.f, 0.f );					
			pParticle->m_fForceVar = m_fForceVar;
		}

		float fNewSpeed = m_fSpeed + RANDOM_NUM * m_fSpeedVar;
		fNewSpeed = (fNewSpeed==0.f) ? 0.01f : fNewSpeed;	// 너무 값이 작아서. Mesh 때 버그가 생긴다.

		Clamp ( fNewSpeed, MIN_SPEED, MAX_SPEED );

		pParticle->m_vVelocity *= fNewSpeed;

		m_uParticlesCreated--;

		// Note : EffGroup이 있다면 생성한다.
		if( m_pPropGroup )
		{
			// 무조건 시작할때 부터 뿌린다.
			if( !(m_dwFlag&(USENEWEFF_END|USENEWEFF_END_STOP)) )	pParticle->m_bPlay = TRUE;

			pParticle->CreateSingleGroup( pd3dDevice, m_pPropGroup );
		}
	}

	m_vGPrevLocation = m_vGLocation;
}



