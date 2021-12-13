// DxEffectNature.cpp: implementation of the DxEffectNature class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"

#include "./DxEffectNature.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectNature::PARTICLE::FVF		= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectNature::PARTICLEPOINT::FVF	= D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_TEX2;
const DWORD DxEffectNature::TYPEID = DEF_EFFECT_NATURE;
const DWORD	DxEffectNature::VERSION = 0x00000101;
const char DxEffectNature::NAME[] = "[ 자연 현상 ]";
const DWORD DxEffectNature::FLAG = NULL;

void DxEffectNature::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(NATURE_PROPERTY);
	dwVer = VERSION;
}

void DxEffectNature::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if( dwVer==VERSION && dwSize==sizeof(NATURE_PROPERTY) )
	{
		memcpy( &m_Property, pProp, dwSize );
	}
	else if( dwVer==0x00000100 && dwSize==sizeof(NATURE_PROPERTY_100) )
	{
		NATURE_PROPERTY_100 sProp;
		memcpy( &sProp, pProp, dwSize );

		m_fRate = sProp.m_fRate;
		m_dwBaseNumber = sProp.m_dwBaseNumber;

		m_fFlowerRate = sProp.m_fFlowerRate;
		m_fFlowerScale = sProp.m_fFlowerScale;

		m_fButterRate = sProp.m_fButterRate;
		m_fButterWidth = sProp.m_fButterWidth;
		m_fButterHeight = sProp.m_fButterHeight;
		m_fButterSpeed = sProp.m_fButterSpeed;
		m_fButterRange = sProp.m_fButterRange;
		m_fButterHeightUP = sProp.m_fButterHeightUP;
		m_fButterHeightDOWN = sProp.m_fButterHeightDOWN;

		m_fBugRate = sProp.m_fBugRate;
		m_iBugCol = sProp.m_iBugCol;
		m_iBugRow = sProp.m_iBugRow;
		m_fBugWidth = sProp.m_fBugWidth;
		m_fBugHeight = sProp.m_fBugHeight;
		m_fBugSpeed = sProp.m_fBugSpeed;
		m_fBugRange = sProp.m_fBugRange;
		m_fBugHeightUP = sProp.m_fBugHeightUP;
		m_fBugHeightDOWN = sProp.m_fBugHeightDOWN;

		StringCchCopy( m_szButterTex, MAX_PATH, sProp.m_szButterTex );
		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );
		StringCchCopy( m_szBugTex, MAX_PATH, sProp.m_szBugTex );

		// Add :  Ver.101 
		StringCchCopy( m_szShadowTex, MAX_PATH, "Circle_Shadow.TGA" );
	}
}

DWORD DxEffectNature::m_dwParticle(NULL);

char DxEffectNature::m_strParticle[] =
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

LPDIRECT3DSTATEBLOCK9	DxEffectNature::m_pSB_Effect		= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectNature::m_pSB_Shadow		= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectNature::m_pSB_Point			= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectNature::m_pSB_Effect_SAVE	= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectNature::m_pSB_Shadow_SAVE	= NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectNature::m_pSB_Point_SAVE	= NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////6.f/32.f , 0.35f, 31.f/32.f, 0.4f

DxEffectNature::DxEffectNature() :
	m_fTime(0.0f),
	m_fElapsedTime(0.0f),
	m_pBaseVert(NULL),					// 	
	m_dwButterNum(0),										// 나비 갯수
	m_pButterVert(NULL),
	m_pButterVB(NULL),
	m_pButterIB(NULL),
	m_pButterShadowVert(NULL),	// 나비 그림자 들
	m_pButterShadowVB(NULL),
	m_pButterShadowIB(NULL),
	m_dwBugNum(0),				// 벌레(디폴트)
	m_pBugVert(NULL),			// 벌레(디폴트)
	m_pBugVB(NULL),				// 벌레(디폴트)
	m_pBugIB(NULL),				// 벌레(디폴트)
	m_fRate(0.3f),					// 전체 비율
	m_dwBaseNumber(0),
	m_fButterRate(5.f),									// 전체에서 나비의 비율
	m_fButterWidth(1.f),
	m_fButterHeight(1.f),
	m_fButterSpeed(6.0f),									// 값은 0에서 10 사이의 값이다.
	m_fButterRange(100.f),	
	m_fButterHeightUP(30.f),
	m_fButterHeightDOWN(10.f),
	m_fBugRate(0.f),			// 벌레(디폴트)
	m_iBugCol(4),				// 벌레(디폴트)
	m_iBugRow(4),				// 벌레(디폴트)
	m_fBugWidth(1.5f),			// 벌레(디폴트)
	m_fBugHeight(1.5f),			// 벌레(디폴트)
	m_fBugSpeed(10.0f),			// 벌레(디폴트)	- 스피드
	m_fBugRange(400.f),			// 벌레(디폴트)
	m_fBugHeightUP(60.f),		// 벌레(디폴트)
	m_fBugHeightDOWN(10.f),		// 벌레(디폴트)
	m_iNowSpriteBug(0),			// 벌레
	m_fTimeBug(0.0f),			// 벌레
	m_fAniTimeBug(0.15f),		// 벌레
	m_AddRand(0),
	m_pButterTex(NULL),
	m_pBugTex(NULL),				// 벌레(디폴트)
	m_pShadowTex(NULL)			
{
	StringCchCopy( m_szButterTex, MAX_PATH, "_Na_Nabi.tga" );
	StringCchCopy( m_szBugTex, MAX_PATH, "_Wa_water1.bmp" );
	StringCchCopy( m_szShadowTex, MAX_PATH, "Circle_Shadow.TGA" );
	memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
}

DxEffectNature::~DxEffectNature()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pButterVert );
	SAFE_DELETE_ARRAY ( m_pButterShadowVert );
	SAFE_DELETE_ARRAY ( m_pBugVert );
	SAFE_DELETE_ARRAY ( m_pBaseVert );

	SAFE_RELEASE ( m_pButterVB );
	SAFE_RELEASE ( m_pButterIB );	
	SAFE_RELEASE ( m_pButterShadowVB );
	SAFE_RELEASE ( m_pButterShadowIB );	
	SAFE_RELEASE ( m_pBugVB );
	SAFE_RELEASE ( m_pBugIB );
}


HRESULT DxEffectNature::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0), 
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),
	//	D3DVSD_REG(1, D3DVSDT_FLOAT1 ),
	//	D3DVSD_REG(2, D3DVSDT_FLOAT4 ),
	//	D3DVSD_END()
	//};

	//LPD3DXBUFFER pCode = NULL;
	//hr = D3DXAssembleShader( m_strParticle, (UINT)strlen( m_strParticle ), 0, NULL, &pCode, NULL );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Nature FAILED" );
	//	return E_FAIL;
	//}

	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwParticle, 
	//											DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Nature FAILED" );
	//	return E_FAIL;
	//}

	//SAFE_RELEASE(pCode);


	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		// Enable alpha testing (skips pixels with less than a certain alpha.)
		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Effect_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Effect );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_ZERO );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_SRCCOLOR );

		// Enable alpha testing (skips pixels with less than a certain alpha.)
		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Shadow_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Shadow );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );

		pd3dDevice->SetRenderState ( D3DRS_POINTSPRITEENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_POINTSCALEENABLE,	TRUE );
		//pd3dDevice->SetRenderState ( D3DRS_POINTSIZE_MIN,		FtoDW(0.0f) );
		//pd3dDevice->SetRenderState ( D3DRS_POINTSCALE_A,		FtoDW(0.00f) );
		//pd3dDevice->SetRenderState ( D3DRS_POINTSCALE_B,		FtoDW(0.00f) );
		//pd3dDevice->SetRenderState ( D3DRS_POINTSCALE_C,		FtoDW(1.00f) );

		// Enable alpha testing (skips pixels with less than a certain alpha.)
		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Point_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Point );
	}

	return S_OK;
}

HRESULT DxEffectNature::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
//	if ( m_dwParticle )	pd3dDevice->DeleteVertexShader ( m_dwParticle );

	SAFE_RELEASE( m_pSB_Effect );
	SAFE_RELEASE( m_pSB_Shadow );
	SAFE_RELEASE( m_pSB_Point );
	SAFE_RELEASE( m_pSB_Effect_SAVE );
	SAFE_RELEASE( m_pSB_Shadow_SAVE );
	SAFE_RELEASE( m_pSB_Point_SAVE );

	return S_OK;
}

HRESULT DxEffectNature::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
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
				CreateBaseMesh( pd3dDevice, pmsMeshs, pframeCur->matCombined );
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return S_OK;
}

HRESULT DxEffectNature::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT DxEffectNature::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szButterTex, pd3dDevice, m_pButterTex, 0, 0, TRUE );

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szBugTex, pd3dDevice, m_pBugTex, 0, 0, TRUE );

	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szShadowTex, pd3dDevice, m_pShadowTex, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffectNature::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szButterTex,	m_pButterTex );
	TextureManager::ReleaseTexture( m_szBugTex,		m_pBugTex );
	TextureManager::ReleaseTexture( m_szShadowTex,	m_pShadowTex );

	return S_OK;
}

HRESULT DxEffectNature::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime	= fTime;
	m_fElapsedTime	+= fElapsedTime;
	m_fTimeBug		+= fElapsedTime;

	return S_OK;
}

HRESULT DxEffectNature::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	HRESULT hr = S_OK;

	if ( m_fElapsedTime > 0.03f )
	{
		if ( m_pBugVB )
			UpdateBug ( pd3dDevice );
		if ( m_pButterVB )
			UpdateButterFly ( pd3dDevice );

		m_fElapsedTime = 0.0f;
	}

	// Get Texture 
	if( !m_pButterTex )	 TextureManager::GetTexture( m_szButterTex, m_pButterTex );
	if( !m_pBugTex )	 TextureManager::GetTexture( m_szBugTex, m_pBugTex );
	if( !m_pShadowTex )	 TextureManager::GetTexture( m_szShadowTex, m_pShadowTex );


	if ( m_pBugVB )
	{
		DWORD dwFVFSize;
		DWORD dwVerts, dwFaces;

		dwVerts = m_dwBugNum*4;
		dwFaces = m_dwBugNum*2;
		
		dwFVFSize = PARTICLE::FVF;
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );		


		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );


		// Note : StateBlock Capture
		m_pSB_Effect_SAVE->Capture();
		m_pSB_Effect->Apply();

		pd3dDevice->SetIndices ( m_pBugIB );
		pd3dDevice->SetStreamSource ( 0, m_pBugVB, 0, dwFVFSize );
		pd3dDevice->SetFVF ( PARTICLE::FVF );

		pd3dDevice->SetTexture( 0, m_pBugTex );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces );

		// Note : StateBlock Apply
		m_pSB_Effect_SAVE->Apply();
	}

	if ( m_pButterVB )
	{
		DWORD dwFVFSize;
		DWORD dwVerts, dwFaces;

		dwVerts = m_dwButterNum*6;
		dwFaces = m_dwButterNum*4;
		
		dwFVFSize = PARTICLE::FVF;
		dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );	


		D3DXMATRIX	matIdentity;
		D3DXMatrixIdentity ( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );


		// Note : StateBlock Capture
		m_pSB_Effect_SAVE->Capture();
		m_pSB_Effect->Apply();


		pd3dDevice->SetIndices ( m_pButterIB );
		pd3dDevice->SetStreamSource ( 0, m_pButterVB, 0, dwFVFSize );
		pd3dDevice->SetFVF ( PARTICLE::FVF );

		pd3dDevice->SetTexture( 0, m_pButterTex );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces );

		// Note : StateBlock Apply
		m_pSB_Effect_SAVE->Apply();





		//	Note : 그림자
		dwVerts = m_dwButterNum*4;
		dwFaces = m_dwButterNum*2;

		// Note : StateBlock Capture
		m_pSB_Shadow_SAVE->Capture();
		m_pSB_Shadow->Apply();

		pd3dDevice->SetIndices ( m_pButterShadowIB );
		pd3dDevice->SetStreamSource ( 0, m_pButterShadowVB, 0, dwFVFSize );
		pd3dDevice->SetFVF ( PARTICLE::FVF );

		pd3dDevice->SetTexture( 0, m_pShadowTex );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, dwVerts, 0, dwFaces );

		// Note : StateBlock Apply
		m_pSB_Shadow_SAVE->Apply();

	}

	return S_OK;
}



HRESULT DxEffectNature::UpdateBug ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 &vFromPt	= DxViewPort::GetInstance().GetFromPt ();
	D3DXVECTOR3 &vLookatPt	= DxViewPort::GetInstance().GetLookatPt ();
	D3DXMATRIX matBillboard = BillboardLookAt ( vLookatPt, vFromPt );

	D3DXVECTOR3	vLength;
	DWORD		dwBase;

	for( DWORD i=0; i<m_dwBugNum; i++ )
	{
		vLength = (m_pBugVert[i].vTargetPos - m_pBugVert[i].vNowPos );

		//	Note : 가고 싶은 곳과 지금 있는곳이 같다면.. 이것을..
		if ( ((vLength.x*vLength.x)+(vLength.y*vLength.y)+(vLength.z*vLength.z)) < 1.f )	
		{
			m_pBugVert[i].iState	= NATURE_WAIT;
			dwBase = (int)(sinf(m_fTime)*m_dwBaseNumber*11+m_AddRand) % m_dwBaseNumber;
			D3DXVECTOR3 vTempLength = m_pBugVert[i].vTargetPos - m_pBaseVert[dwBase].p;

			// 작을 때만 실행시킨다. 
			if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < m_fBugRange*m_fBugRange && 
					sinf(m_fTime+i) > 0.9f )	// 100.f
			{
				if ( m_fBugSpeed > 10.f )	// 0보다 크고 10보다 작다.
					m_fBugSpeed = 10.f;
				float Speed = 10.f - m_fBugSpeed;
				m_pBugVert[i].vTargetPos = m_pBaseVert[dwBase].p;
				m_pBugVert[i].iState		= NATURE_UPFLY;
				m_pBugVert[i].iLeft		= ((int)m_fTime+i)%4 + 1;
				m_pBugVert[i].iRight		= ((int)m_fTime+i)%4 + 1;
				m_pBugVert[i].fSpeed		= (float)(((int)m_fTime+i)%4) + Speed;

				if ( m_pBugVert[i].fSpeed < 1.f )
					m_pBugVert[i].fSpeed = 1.f;

				D3DXVECTOR3 vDirec = m_pBugVert[i].vTargetPos - m_pBugVert[i].vNowPos;	// 방향
				vDirec.y = 0.0f;
				D3DXVec3Normalize ( &vDirec, &vDirec );
				D3DXVECTOR3	vInit = D3DXVECTOR3 ( 0.f, 0.f, 1.f );
				float Dot3 = D3DXVec3Dot ( &vDirec, &vInit );

				if ( vDirec.x > 0.f )
				{
					Dot3 -= 1.0f;
					Dot3 /= -2.f;
				}
				else
				{
					Dot3 += 1.0f;
					Dot3 /= 2.f;
					Dot3 += 1.0f;
				}


				//	Note : 만약 가야 할 거리가 짧은 거리라면 걸어서.. 기어서 간다.
				//
				if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < 2500.f )	// 50.f
				{
					m_pBugVert[i].iState		= NATURE_WALK;
				}
			}
		}

		//	Note : 만약 걸어갈 정도로 거리가 짧다면
		//
		else if ( m_pBugVert[i].iState == NATURE_WALK )
		{
			D3DXVec3Normalize ( &vLength, &vLength );

			m_pBugVert[i].vNowPos += vLength/m_pBugVert[i].fSpeed;
		}

		//	Note : 거리 멀다...... 날라 가라.ㅋㅋ
		//
		else
		{
			// 물체가 목표 지점에 가까워 졌을 때
			if ( ((vLength.x*vLength.x)+(vLength.z*vLength.z)) < 64.f )		// 이건 8.f 이다.	// 나비에서는 이건 사용 안하지.
			{
				D3DXVec3Normalize ( &vLength, &vLength );

				m_pBugVert[i].vNowPos += vLength*0.3f;
				m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/10.f;
				m_pBugVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/10.f;
				m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/10.f;

				m_pBugVert[i].iState = NATURE_LANDING;
			}

			// 물체가 목표 지점과 아직 거리가 있을 때
			else
			{
				// 너무 높아요
				if (m_pBugVert[i].vNowPos.y > m_pBugVert[i].vTargetPos.y + m_fBugHeightUP )	// 최고 높이
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pBugVert[i].vNowPos += vLength*0.8f;
					m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/m_pBugVert[i].fSpeed;
					m_pBugVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pBugVert[i].fSpeed - 0.3f;
					m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/m_pBugVert[i].fSpeed;

					m_pBugVert[i].iState = NATURE_DOWNFLY;
				}
				// 너무 낮아요
				else if ( m_pBugVert[i].vNowPos.y < m_pBugVert[i].vTargetPos.y + m_fBugHeightDOWN )	// 최소 높이
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pBugVert[i].vNowPos += vLength*0.8f;
					m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/m_pBugVert[i].fSpeed;
					m_pBugVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pBugVert[i].fSpeed + 0.3f;
					m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/m_pBugVert[i].fSpeed;

					m_pBugVert[i].iState = NATURE_UPFLY;
				}
				// 괜찮네요.. ^^
				else
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pBugVert[i].vNowPos += vLength*0.8f;
					m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/m_pBugVert[i].fSpeed;
					m_pBugVert[i].vNowPos.y += sinf((m_fTime+i+m_AddRand)*6)/m_pBugVert[i].fSpeed;
					m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/m_pBugVert[i].fSpeed;
				}



				if ( m_pBugVert[i].iState == NATURE_UPFLY )
				{
					m_pBugVert[i].vNowPos.y += 0.1f;
				}
				else if  ( m_pBugVert[i].iState == NATURE_DOWNFLY )
				{
					m_pBugVert[i].vNowPos.y -= 0.1f;
				}
			}

			if ( m_pBugVert[i].vNowPos.y < m_pBugVert[i].vTargetPos.y )
				m_pBugVert[i].vNowPos.y = m_pBugVert[i].vTargetPos.y;
		}

		//m_AddRand++;

		if ( m_AddRand > 30000 )
			m_AddRand = 0;
	}

	//	Note : 스프라이트
	//
	int iSpriteNum = m_iBugCol * m_iBugRow;

	if ( m_fTimeBug > m_fAniTimeBug )
	{
		m_iNowSpriteBug += 1;
		m_fTimeBug = 0.0f;
	}

	if ( m_iNowSpriteBug >= iSpriteNum/2 )
	{
		m_iNowSpriteBug = 0;
	}


	//	Note : 셋팅이 다 되어 있는 것을 빌보드 시켜 준다. 여기서 스프라이트도 해야 하나 ~?
	//
	PARTICLE*	pVertices;
	m_pBugVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	for( DWORD i=0; i<m_dwBugNum; i++ )
	{
		//	Note : 자기 자리에 앉아 있을 때...
		//
		if ( m_pBugVert[i].iState == NATURE_WAIT )	
		{
			pVertices->p = D3DXVECTOR3 ( -m_fBugWidth, m_fBugHeight*2.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = m_iNowSpriteBug / (float)m_iBugCol;
			pVertices->t.y = (m_iNowSpriteBug / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fBugWidth, m_fBugHeight*2.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = (m_iNowSpriteBug+1) / (float)m_iBugCol;
			pVertices->t.y = (m_iNowSpriteBug / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( -m_fBugWidth, 0.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = m_iNowSpriteBug / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+m_iBugRow) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fBugWidth, 0.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = (m_iNowSpriteBug+1) / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+m_iBugRow) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;
		}

		//	Note : 걸어가야 할 상황 일 때,...
		//
		else if ( m_pBugVert[i].iState == NATURE_WALK )
		{
			pVertices->p = D3DXVECTOR3 ( -m_fBugWidth, m_fBugHeight*2.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = m_iNowSpriteBug / (float)m_iBugCol;
			pVertices->t.y = (m_iNowSpriteBug / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fBugWidth, m_fBugHeight*2.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = (m_iNowSpriteBug+1) / (float)m_iBugCol;
			pVertices->t.y = (m_iNowSpriteBug / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( -m_fBugWidth, 0.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = m_iNowSpriteBug / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+m_iBugRow) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fBugWidth, -0.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = (m_iNowSpriteBug+1) / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+m_iBugRow) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;
		}

		else
		{
			pVertices->p = D3DXVECTOR3 ( -m_fBugWidth, m_fBugHeight*2.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = m_iNowSpriteBug / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+(iSpriteNum/2)) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fBugWidth, m_fBugHeight*2.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = (m_iNowSpriteBug+1) / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+(iSpriteNum/2)) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( -m_fBugWidth, 0.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = m_iNowSpriteBug / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+(iSpriteNum/2)+m_iBugRow) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fBugWidth, 0.f, 0.0f );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matBillboard );
			pVertices->p += m_pBugVert[i].vNowPos;
			pVertices->t.x = (m_iNowSpriteBug+1) / (float)m_iBugCol;
			pVertices->t.y = ((m_iNowSpriteBug+(iSpriteNum/2)+m_iBugRow) / m_iBugRow) / (float)m_iBugRow;
			pVertices++;
		}
	}

	m_pBugVB->Unlock ();


	return S_OK;
}


HRESULT DxEffectNature::UpdateButterFly ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3	vLength;
	DWORD		dwBase;

	for( DWORD i=0; i<m_dwButterNum; i++ )
	{
		vLength = (m_pButterVert[i].vTargetPos - m_pButterVert[i].vNowPos );


		//	Note : 가고 싶은 곳과 지금 있는곳이 같다면.. 이것을..
		if ( ((vLength.x*vLength.x)+(vLength.y*vLength.y)+(vLength.z*vLength.z)) < 1.f )	
		{
			m_pButterVert[i].iState	= NATURE_WAIT;
			dwBase = (int)(sinf(m_fTime)*m_dwBaseNumber*11+m_AddRand) % m_dwBaseNumber;
			D3DXVECTOR3 vTempLength = m_pButterVert[i].vTargetPos - m_pBaseVert[dwBase].p;

			// 작을 때만 실행시킨다. 
			if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < m_fButterRange*m_fButterRange && 
					sinf(m_fTime+i) > 0.9f )	// 100.f
			{
				if ( m_fButterSpeed > 10.f )	// 0보다 크고 10보다 작다.
					m_fButterSpeed = 10.f;
				float Speed = 10.f - m_fButterSpeed;
				m_pButterVert[i].vTargetPos		= m_pBaseVert[dwBase].p;
				m_pButterVert[i].vTargetPos.y	+= 0.1f;								// 0.1 만 올려 준다.
				m_pButterVert[i].vPrevPos		= m_pButterVert[i].vNowPos;
				m_pButterVert[i].iState			= NATURE_UPFLY;
				m_pButterVert[i].iLeft			= ((int)m_fTime+i)%4 + 1;
				m_pButterVert[i].iRight			= ((int)m_fTime+i)%4 + 1;
				m_pButterVert[i].fSpeed			= (float)(((int)m_fTime+i)%4) + Speed;

				D3DXVECTOR3	vBaseVector3 = m_pButterVert[i].vTargetPos - m_pButterVert[i].vPrevPos;
				m_pButterVert[i].fBaseLenth = D3DXVec3Length ( &vBaseVector3 );

				if ( m_pButterVert[i].fSpeed < 1.f )
					m_pButterVert[i].fSpeed = 1.f;

				D3DXVECTOR3 vDirec = m_pButterVert[i].vTargetPos - m_pButterVert[i].vNowPos;	// 방향
				vDirec.y = 0.0f;
				D3DXVec3Normalize ( &vDirec, &vDirec );
				D3DXVECTOR3	vInit = D3DXVECTOR3 ( 0.f, 0.f, 1.f );
				float Dot3 = D3DXVec3Dot ( &vDirec, &vInit );

				if ( vDirec.x > 0.f )
				{
					Dot3 -= 1.0f;
					Dot3 /= -2.f;
				}
				else
				{
					Dot3 += 1.0f;
					Dot3 /= 2.f;
					Dot3 += 1.0f;
				}

				m_pButterVert[i].fDot3		= Dot3;


				//	Note : 만약 가야 할 거리가 짧은 거리라면 걸어서.. 기어서 간다.
				//
				if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < 0.f )	// 50.f
				{
					m_pButterVert[i].iState		= NATURE_WALK;
				}
			}
		}

		//	Note : 만약 걸어갈 정도로 거리가 짧다면
		//
		else if ( m_pButterVert[i].iState == NATURE_WALK )
		{
			D3DXVec3Normalize ( &vLength, &vLength );

			m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
		}

		//	Note : 거리 멀다...... 날라 가라.ㅋㅋ
		//
		else
		{
			// 물체가 목표 지점에 가까워 졌을 때
			if ( ((vLength.x*vLength.x)+(vLength.z*vLength.z)) < 64.f )		// 이건 8.f 이다.	// 나비에서는 이건 사용 안하지.
			{
				D3DXVec3Normalize ( &vLength, &vLength );

				m_pButterVert[i].vNowPos += vLength/10.f;
				m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/10.f;
				m_pButterVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/10.f;
				m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/10.f;

				m_pButterVert[i].iState = NATURE_LANDING;
			}

			// 물체가 목표 지점과 아직 거리가 있을 때
			else
			{
				// 너무 높아요
				if (m_pButterVert[i].vNowPos.y > m_pButterVert[i].vTargetPos.y + m_fButterHeightUP )	// 최고 높이
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pButterVert[i].fSpeed - 0.3f;
					m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/m_pButterVert[i].fSpeed;

					m_pButterVert[i].iState = NATURE_DOWNFLY;
				}
				// 너무 낮아요
				else if ( m_pButterVert[i].vNowPos.y < m_pButterVert[i].vTargetPos.y + m_fButterHeightDOWN )	// 최소 높이
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pButterVert[i].fSpeed + 0.3f;
					m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/m_pButterVert[i].fSpeed;

					m_pButterVert[i].iState = NATURE_UPFLY;
				}
				// 괜찮네요.. ^^
				else
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.y += sinf((m_fTime+i+m_AddRand)*6)/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/m_pButterVert[i].fSpeed;
				}



				if ( m_pButterVert[i].iState == NATURE_UPFLY )
				{
					m_pButterVert[i].vNowPos.y += 0.1f;
				}
				else if  ( m_pButterVert[i].iState == NATURE_DOWNFLY )
				{
					m_pButterVert[i].vNowPos.y -= 0.1f;
				}
			}

			if ( m_pButterVert[i].vNowPos.y < m_pButterVert[i].vTargetPos.y )
				m_pButterVert[i].vNowPos.y = m_pButterVert[i].vTargetPos.y;
		}

		//m_AddRand++;

		if ( m_AddRand > 30000 )
			m_AddRand = 0;
	}


	//	Note : 셋팅이 다 되어 있는 것을 빌보드 시켜 준다. 여기서 스프라이트도 해야 하나 ~?
	//
	PARTICLE*	pVertices;
	m_pButterVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	for( DWORD i=0; i<m_dwButterNum; i++ )
	{
		//	Note : 자기 자리에 앉아 있을 때...
		//
		if ( m_pButterVert[i].iState == NATURE_WAIT )	
		{
			D3DXMATRIX	matRot;
			D3DXMatrixRotationY ( &matRot, m_pButterVert[i].fDot3*D3DX_PI );

			float	fTempX = (sinf(m_fTime+i)+m_fButterWidth)*0.66f;

			pVertices->p = D3DXVECTOR3 ( -fTempX, m_fButterWidth, m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 (-fTempX, m_fButterWidth, -m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( 0.f, 0.f, m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( 0.f, 0.f, -m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( fTempX, m_fButterWidth, m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( fTempX, m_fButterWidth, -m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;
		}

		//	Note : 걸어가야 할 상황 일 때,...
		//
		else if ( m_pButterVert[i].iState == NATURE_WALK )
		{
			D3DXMATRIX	matRot;
			D3DXMatrixRotationY ( &matRot, m_pButterVert[i].fDot3*D3DX_PI );

			float	fTempX = (sinf(m_fTime+i)+m_fButterWidth) * (m_fButterWidth/(m_fButterWidth+1)) ;

			pVertices->p = D3DXVECTOR3 ( -fTempX, m_fButterWidth, m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 (-fTempX, m_fButterWidth, -m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( 0.f, 0.f, m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( 0.f, 0.f, -m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( fTempX, m_fButterWidth, m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( fTempX, m_fButterWidth, -m_fButterHeight);
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;
		}

		else
		{
			D3DXMATRIX	matRot;
			D3DXMatrixRotationY ( &matRot, m_pButterVert[i].fDot3*D3DX_PI );

			//	꺽이는 각도. 조절 ..
			//
			float fTempX = cosf(m_fTime*(m_pButterVert[i].fSpeed+8.f)+i);
			fTempX = fTempX * 0.8f;
			if ( fTempX >= 0)	fTempX += 0.2f;
			else				fTempX -= 0.2f;

			float fTempY = sinf(m_fTime*(m_pButterVert[i].fSpeed+8.f)+i);
			fTempY = fTempY * 0.8f;
			if ( fTempY >= 0)	fTempY += 0.2f;
			else				fTempY -= 0.2f;



			pVertices->p = D3DXVECTOR3 ( -m_fButterWidth*fTempX, fTempY*m_fButterWidth, m_fButterHeight );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( -m_fButterWidth*fTempX, fTempY*m_fButterWidth, -m_fButterHeight );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( 0.f, 0.f, m_fButterHeight );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( 0.f, 0.f, -m_fButterHeight );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fButterWidth*fTempX, fTempY*m_fButterWidth, m_fButterHeight );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_fButterWidth*fTempX, fTempY*m_fButterWidth, -m_fButterHeight );
			D3DXVec3TransformCoord ( &pVertices->p, &pVertices->p, &matRot );
			pVertices->p += m_pButterVert[i].vNowPos;
			pVertices++;
		}
	}

	m_pButterVB->Unlock ();


	//	Note : 그림자
	//
	m_pButterShadowVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	
	for( DWORD i=0; i<m_dwButterNum; i++ )
	{
		// 비율 계산
		D3DXVECTOR3	vNowVector3 = m_pButterVert[i].vTargetPos - m_pButterVert[i].vNowPos;
		float fNowLenth = D3DXVec3Length ( &vNowVector3 );

		float fRate = 1.0f - fNowLenth/m_pButterVert[i].fBaseLenth;
		float fHeight = ((m_pButterVert[i].vTargetPos.y - m_pButterVert[i].vPrevPos.y)*fRate)+0.1f;

		pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x-(m_fButterWidth*0.7f), m_pButterVert[i].vPrevPos.y+fHeight, m_pButterVert[i].vNowPos.z+(m_fButterHeight*0.7f));
		pVertices++;

		pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x+(m_fButterWidth*0.7f), m_pButterVert[i].vPrevPos.y+fHeight, m_pButterVert[i].vNowPos.z+(m_fButterHeight*0.7f));
		pVertices++;

		pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x-(m_fButterWidth*0.7f), m_pButterVert[i].vPrevPos.y+fHeight, m_pButterVert[i].vNowPos.z-(m_fButterHeight*0.7f));
		pVertices++;

		pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x+(m_fButterWidth*0.7f), m_pButterVert[i].vPrevPos.y+fHeight, m_pButterVert[i].vNowPos.z-(m_fButterHeight*0.7f));
		pVertices++;
	}

	m_pButterShadowVB->Unlock ();

	return S_OK;
}



// This function computes the surface area of a triangle
float DxEffectNature::CalcSurfaceArea(VERTEX* pFV[3])
{
	float s;
	float len[3];

	for (DWORD i=0;i<3;i++)
	{
		float x,y,z;
		x = pFV[(i+1)%3]->p.x - pFV[i]->p.x;
		y = pFV[(i+1)%3]->p.y - pFV[i]->p.y;
		z = pFV[(i+1)%3]->p.z - pFV[i]->p.z;
		len[i] = (float)sqrt(x*x+y*y+z*z);
	}
	
	s = (len[0]+len[1]+len[2])/2.0f;
	return (float)sqrt(s*(s-len[0])*(s-len[1])*(s-len[2]));
}


HRESULT DxEffectNature::CreateBaseMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs, D3DXMATRIX matWorld )
{
	VERTEX*		Array[3];
	float		area=0;
	DWORD		Fullarea=0;

	VERTEX*		pVertices;
	WORD*		pIndices;
	DWORD		Faces;
	WORD*		pIndicesCur;


	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		Faces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

		pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
		pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	}
	else
	{
		Faces		= pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;

		pmsMeshs->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pVertices );
		pmsMeshs->m_pDxOctreeMesh->LockIndexBuffer( (VOID**)&pIndices );
	}


	pIndicesCur = pIndices;

	for(DWORD i=0; i<Faces; i++)
	{
		Array[0] = &pVertices[pIndicesCur[i*3+0]];
		Array[1] = &pVertices[pIndicesCur[i*3+1]];
		Array[2] = &pVertices[pIndicesCur[i*3+2]];
		area += (m_fRate*0.01f)*CalcSurfaceArea(Array);
	}

	Fullarea = (DWORD)area;
	SAFE_DELETE_ARRAY (	m_pBaseVert );
	m_pBaseVert = new BASEVERTEX[Fullarea];
	area = 0;
	DWORD		dwRainVert = 0;

	for( DWORD j=0; j<Faces; j++ )
	{
		Array[0] = &pVertices[pIndicesCur[j*3+0]];
		Array[1] = &pVertices[pIndicesCur[j*3+1]];
		Array[2] = &pVertices[pIndicesCur[j*3+2]];
		area += (m_fRate*0.01f)*CalcSurfaceArea(Array);	// 1면의 면적을 계산해서 빗방울 갯수 정함.
		//area += m_fRainRate;							// 1면당 - 생성할 빗방울 갯수.

		// Here we create random vertices in the triangle. The randomness is defines
		// based on 2 edge vector of the triangle. We compute a ratio r1 and r2 which represent a fraction
		// of the edge. If we compute the point as pt = r1*v1 + r2*v2 and we satisfy r1+r2 <= 1.0, the point
		// will be within the triangle.
		while (area>1.0f)
		{
			float r1,r2;
			D3DXVECTOR3 s,t;
			// Create new vertex
			r1 = ((float)(rand()%1000)) / 1000.0f;
			r2 = 1.0f-r1;
			r2 = r2 * ((rand()%1000) / 1000.0f);
			s.x = Array[1]->p.x - Array[0]->p.x;
			s.y = Array[1]->p.y - Array[0]->p.y;
			s.z = Array[1]->p.z - Array[0]->p.z;
			t.x = Array[2]->p.x - Array[0]->p.x;
			t.y = Array[2]->p.y - Array[0]->p.y;
			t.z = Array[2]->p.z - Array[0]->p.z;
			if ( Fullarea  > dwRainVert )			// 최대 점 갯수를 넘지 않도록 조정
			{
				m_pBaseVert[dwRainVert].p.x = Array[0]->p.x + s.x*r1 + t.x*r2;							// 1 정점 근처에 Vertex를 생성
				m_pBaseVert[dwRainVert].p.y = Array[0]->p.y + s.y*r1 + t.y*r2;
				m_pBaseVert[dwRainVert].p.z = Array[0]->p.z + s.z*r1 + t.z*r2;
			}

			area -= 1.0f;dwRainVert++;
		}
	}

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
		pmsMeshs->m_pLocalMesh->UnlockIndexBuffer();
	}
	else
	{
		pmsMeshs->m_pDxOctreeMesh->UnlockVertexBuffer ( );
		pmsMeshs->m_pDxOctreeMesh->UnlockIndexBuffer ( );
	}
	m_dwBaseNumber = dwRainVert;	// 전체 점 갯수 

	//	Note : 월드 매트 릭스 곱 한 거 
	//
	for( DWORD i=0; i<m_dwBaseNumber; i++ )
	{
		D3DXVec3TransformCoord ( &m_pBaseVert[ i ].p, &m_pBaseVert[ i ].p, &matWorld );
	}

	CreateBugMesh		( pd3dDevice );	// 벌레 (디폴트)
	CreateButterMesh	( pd3dDevice );	// 나비


	return S_OK;
}

HRESULT DxEffectNature::CreateBugMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	// 나비 만들기 
	//
	m_dwBugNum = (int)(m_fBugRate*0.01*m_dwBaseNumber);		// 나비 갯수 정하기

	if ( m_dwBugNum > 0)
	{
		SAFE_DELETE_ARRAY (	m_pBugVert );
		m_pBugVert = new PARTICLE_BUG[m_dwBugNum];


		SAFE_RELEASE ( m_pBugVB );

		pd3dDevice->CreateVertexBuffer ( 4*m_dwBugNum*sizeof(PARTICLE), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_MANAGED, &m_pBugVB, NULL  );

		if ( !m_pBugVB )		// 너무 값이 크면 아무것도 안나오게 해 준다.
			return S_OK;

		PARTICLE*	pVertices;
		m_pBugVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

		DWORD		dwBase;
		D3DXVECTOR3	vVel;
		for( DWORD i=0; i<m_dwBugNum; i++ )
		{
			if ( m_fBugSpeed > 10.f )	// 0보다 크고 10보다 작다.
				m_fBugSpeed = 10.f;
			float Speed = 10.f - m_fBugSpeed;

			dwBase = (int)(i*11) % m_dwBaseNumber;
			m_pBugVert[i].vNowPos		= m_pBaseVert[ dwBase ].p;
			m_pBugVert[i].vTargetPos	= m_pBaseVert[ dwBase ].p;
			m_pBugVert[i].iState		= NATURE_WAIT;
			m_pBugVert[i].iLeft			= ((int)m_fTime+i)%4 + 1;
			m_pBugVert[i].iRight		= ((int)m_fTime+i)%4 + 1;
			m_pBugVert[i].fSpeed		= (float)(((int)m_fTime+i)%4) + Speed;


			pVertices->p = D3DXVECTOR3 ( m_pBugVert[i].vNowPos.x-m_fBugWidth, m_pBugVert[i].vNowPos.y+m_fBugWidth, m_pBugVert[i].vNowPos.z+m_fBugHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pBugVert[i].vNowPos.x-m_fBugWidth, m_pBugVert[i].vNowPos.y+m_fBugWidth, m_pBugVert[i].vNowPos.z-m_fBugHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.f, 1.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pBugVert[i].vNowPos.x+m_fBugWidth, m_pBugVert[i].vNowPos.y+m_fBugWidth, m_pBugVert[i].vNowPos.z+m_fBugHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 1.f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pBugVert[i].vNowPos.x+m_fBugWidth, m_pBugVert[i].vNowPos.y+m_fBugWidth, m_pBugVert[i].vNowPos.z-m_fBugHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 1.f, 1.1f );
			pVertices++;
		}

		m_pBugVB->Unlock ();



		SAFE_RELEASE ( m_pBugIB );

		hr = pd3dDevice->CreateIndexBuffer( m_dwBugNum *6* sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
										D3DPOOL_MANAGED, &m_pBugIB, NULL );
		if (FAILED(hr))		return S_OK;

		WORD *pIndices; 
		hr = m_pBugIB->Lock( 0, 0,(VOID**)&pIndices, 0 );
		if (FAILED(hr))		return S_OK;

		for(DWORD i=0;i<m_dwBugNum;i++)
		{
			*pIndices++ = (WORD)(i*4+0);
			*pIndices++ = (WORD)(i*4+1);
			*pIndices++ = (WORD)(i*4+2);

   			*pIndices++ = (WORD)(i*4+1);
			*pIndices++ = (WORD)(i*4+3);
			*pIndices++ = (WORD)(i*4+2);
		}
		m_pBugIB->Unlock();

	}

	return S_OK;
}


HRESULT DxEffectNature::CreateButterMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	// 나비 만들기 
	//
	m_dwButterNum = (int)(m_fButterRate*0.01f*m_dwBaseNumber);		// 나비 갯수 정하기

	if ( m_dwButterNum > 0)
	{
		SAFE_DELETE_ARRAY (	m_pButterVert );
		m_pButterVert = new PARTICLE_BUTTERFLY[m_dwButterNum];


		SAFE_RELEASE ( m_pButterVB );

		pd3dDevice->CreateVertexBuffer ( 6*m_dwButterNum*sizeof(PARTICLE), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_MANAGED, &m_pButterVB, NULL );

		if ( !m_pButterVB )		// 너무 값이 크면 아무것도 안나오게 해 준다.
			return S_OK;

		PARTICLE*	pVertices;
		m_pButterVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

		DWORD		dwBase;
		D3DXVECTOR3	vVel;
		for( DWORD i=0; i<m_dwButterNum; i++ )
		{
			if ( m_fButterSpeed > 10.f )	// 0보다 크고 10보다 작다.
				m_fButterSpeed = 10.f;
			float Speed = 10.f - m_fButterSpeed;

			dwBase = (int)(i*11) % m_dwBaseNumber;
			m_pButterVert[i].vNowPos		= m_pBaseVert[ dwBase ].p;
			m_pButterVert[i].vNowPos.y		+= 0.1f;
			m_pButterVert[i].vTargetPos		= m_pBaseVert[ dwBase ].p;
			m_pButterVert[i].vTargetPos.y	+= 0.1f;
			m_pButterVert[i].vPrevPos		= m_pBaseVert[ dwBase ].p;
			m_pButterVert[i].vPrevPos.y		+= 0.1f;
			m_pButterVert[i].iState			= NATURE_WAIT;
			m_pButterVert[i].iLeft			= ((int)m_fTime+i)%4 + 1;
			m_pButterVert[i].iRight			= ((int)m_fTime+i)%4 + 1;
			m_pButterVert[i].fSpeed			= (float)(((int)m_fTime+i)%4) + Speed;
			m_pButterVert[i].fDot3			= 0.0f;


			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x-m_fButterWidth, m_pButterVert[i].vNowPos.y+m_fButterWidth, m_pButterVert[i].vNowPos.z+m_fButterHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x-m_fButterWidth, m_pButterVert[i].vNowPos.y+m_fButterWidth, m_pButterVert[i].vNowPos.z-m_fButterHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.f, 1.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x, m_pButterVert[i].vNowPos.y, m_pButterVert[i].vNowPos.z+m_fButterHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.5f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x, m_pButterVert[i].vNowPos.y, m_pButterVert[i].vNowPos.z-m_fButterHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.5f, 1.0f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x+m_fButterWidth, m_pButterVert[i].vNowPos.y+m_fButterWidth, m_pButterVert[i].vNowPos.z+m_fButterHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 1.f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x+m_fButterWidth, m_pButterVert[i].vNowPos.y+m_fButterWidth, m_pButterVert[i].vNowPos.z-m_fButterHeight);
			pVertices->d = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 1.f, 1.1f );
			pVertices++;
		}

		m_pButterVB->Unlock ();



		SAFE_RELEASE ( m_pButterIB );

		hr = pd3dDevice->CreateIndexBuffer( m_dwButterNum *12* sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
										D3DPOOL_MANAGED, &m_pButterIB, NULL );
		if (FAILED(hr))		return S_OK;

		WORD *pIndices; 
		hr = m_pButterIB->Lock( 0, 0,(VOID**)&pIndices, 0 );
		if (FAILED(hr))		return S_OK;

		for(DWORD i=0;i<m_dwButterNum;i++)
		{
			*pIndices++ = (WORD)(i*6+0);
			*pIndices++ = (WORD)(i*6+1);
			*pIndices++ = (WORD)(i*6+2);

   			*pIndices++ = (WORD)(i*6+1);
			*pIndices++ = (WORD)(i*6+3);
			*pIndices++ = (WORD)(i*6+2);

			*pIndices++ = (WORD)(i*6+2);
			*pIndices++ = (WORD)(i*6+3);
			*pIndices++ = (WORD)(i*6+4);

   			*pIndices++ = (WORD)(i*6+3);
			*pIndices++ = (WORD)(i*6+5);
			*pIndices++ = (WORD)(i*6+4);
		}
		m_pButterIB->Unlock();


		//	Note : 그림자 만들기
		//
		SAFE_RELEASE ( m_pButterShadowVB );

		pd3dDevice->CreateVertexBuffer ( 4*m_dwButterNum*sizeof(PARTICLE), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_MANAGED, &m_pButterShadowVB, NULL );

		if ( !m_pButterShadowVB )		// 너무 값이 크면 아무것도 안나오게 해 준다.
			return S_OK;

		m_pButterShadowVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

		for( DWORD i=0; i<m_dwButterNum; i++ )
		{
			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x-(m_fButterWidth/2.f), m_pButterVert[i].vNowPos.y, m_pButterVert[i].vNowPos.z+(m_fButterHeight/2.f));
			pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.0f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x+(m_fButterWidth/2.f), m_pButterVert[i].vNowPos.y, m_pButterVert[i].vNowPos.z+(m_fButterHeight/2.f));
			pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 0.0f, 1.0f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x-(m_fButterWidth/2.f), m_pButterVert[i].vNowPos.y, m_pButterVert[i].vNowPos.z-(m_fButterHeight/2.f));
			pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 1.f, 0.f );
			pVertices++;

			pVertices->p = D3DXVECTOR3 ( m_pButterVert[i].vNowPos.x+(m_fButterWidth/2.f), m_pButterVert[i].vNowPos.y, m_pButterVert[i].vNowPos.z-(m_fButterHeight/2.f));
			pVertices->d = D3DCOLOR_ARGB ( 100, 255, 255, 255 );
			pVertices->t = D3DXVECTOR2 ( 1.f, 1.f );
			pVertices++;
		}

		m_pButterShadowVB->Unlock ();

		SAFE_RELEASE ( m_pButterShadowIB );

		hr = pd3dDevice->CreateIndexBuffer( m_dwButterNum *6* sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
										D3DPOOL_MANAGED, &m_pButterShadowIB, NULL );
		if (FAILED(hr))		return S_OK;

		hr = m_pButterShadowIB->Lock( 0, 0,(VOID**)&pIndices, 0 );
		if (FAILED(hr))		return S_OK;

		for(DWORD i=0;i<m_dwButterNum;i++)
		{
			*pIndices++ = (WORD)(i*4+0);
			*pIndices++ = (WORD)(i*4+1);
			*pIndices++ = (WORD)(i*4+2);

   			*pIndices++ = (WORD)(i*4+1);
			*pIndices++ = (WORD)(i*4+3);
			*pIndices++ = (WORD)(i*4+2);
		}
		m_pButterShadowIB->Unlock();

	}

	return S_OK;
}

D3DMATRIX DxEffectNature::BillboardLookAt ( D3DXVECTOR3 BillboardPos, D3DXVECTOR3 CarmeraPos )
{
	D3DXMATRIX	Matrix;
	D3DXVECTOR3	View = CarmeraPos - BillboardPos;
	D3DXVec3Normalize ( &View, &View );

	D3DXVECTOR3 WorldUp = D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f );

	float angle = D3DXVec3Dot ( &WorldUp, &View );
	D3DXVECTOR3	Up = WorldUp - angle*View;
	D3DXVec3Normalize ( &Up, &Up );

	D3DXVECTOR3	Right;
	D3DXVec3Cross ( &Right, &Up, &View );

	Matrix._11 = Right.x;	Matrix._21 = Up.x;	Matrix._31 = View.x;
	Matrix._12 = Right.y;	Matrix._22 = Up.y;	Matrix._32 = View.y;
	Matrix._13 = Right.z;	Matrix._23 = Up.z;	Matrix._33 = View.z;

	Matrix._41 = 0.f;//BillboardPos.x;
	Matrix._42 = 0.f;//BillboardPos.y;
	Matrix._43 = 0.f;

	Matrix._14 = 0.0f;
	Matrix._24 = 0.0f;
	Matrix._34 = 0.0f;
	Matrix._44 = 1.0f;

	return Matrix;
}