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
const char DxEffectNature::NAME[] = "[ �ڿ� ���� ]";
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
	m_dwButterNum(0),										// ���� ����
	m_pButterVert(NULL),
	m_pButterVB(NULL),
	m_pButterIB(NULL),
	m_pButterShadowVert(NULL),	// ���� �׸��� ��
	m_pButterShadowVB(NULL),
	m_pButterShadowIB(NULL),
	m_dwBugNum(0),				// ����(����Ʈ)
	m_pBugVert(NULL),			// ����(����Ʈ)
	m_pBugVB(NULL),				// ����(����Ʈ)
	m_pBugIB(NULL),				// ����(����Ʈ)
	m_fRate(0.3f),					// ��ü ����
	m_dwBaseNumber(0),
	m_fButterRate(5.f),									// ��ü���� ������ ����
	m_fButterWidth(1.f),
	m_fButterHeight(1.f),
	m_fButterSpeed(6.0f),									// ���� 0���� 10 ������ ���̴�.
	m_fButterRange(100.f),	
	m_fButterHeightUP(30.f),
	m_fButterHeightDOWN(10.f),
	m_fBugRate(0.f),			// ����(����Ʈ)
	m_iBugCol(4),				// ����(����Ʈ)
	m_iBugRow(4),				// ����(����Ʈ)
	m_fBugWidth(1.5f),			// ����(����Ʈ)
	m_fBugHeight(1.5f),			// ����(����Ʈ)
	m_fBugSpeed(10.0f),			// ����(����Ʈ)	- ���ǵ�
	m_fBugRange(400.f),			// ����(����Ʈ)
	m_fBugHeightUP(60.f),		// ����(����Ʈ)
	m_fBugHeightDOWN(10.f),		// ����(����Ʈ)
	m_iNowSpriteBug(0),			// ����
	m_fTimeBug(0.0f),			// ����
	m_fAniTimeBug(0.15f),		// ����
	m_AddRand(0),
	m_pButterTex(NULL),
	m_pBugTex(NULL),				// ����(����Ʈ)
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
	//	Note : �ؽ����� �б� ������ �����Ѵ�.
	//
	TextureManager::LoadTexture ( m_szButterTex, pd3dDevice, m_pButterTex, 0, 0, TRUE );

	//	Note : �ؽ����� �б� ������ �����Ѵ�.
	//
	TextureManager::LoadTexture ( m_szBugTex, pd3dDevice, m_pBugTex, 0, 0, TRUE );

	//	Note : �ؽ����� �б� ������ �����Ѵ�.
	//
	TextureManager::LoadTexture ( m_szShadowTex, pd3dDevice, m_pShadowTex, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffectNature::DeleteDeviceObjects ()
{
	//	Note : ���� �ؽ��İ� �ε�Ǿ� ���� ��� ����.
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





		//	Note : �׸���
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

		//	Note : ���� ���� ���� ���� �ִ°��� ���ٸ�.. �̰���..
		if ( ((vLength.x*vLength.x)+(vLength.y*vLength.y)+(vLength.z*vLength.z)) < 1.f )	
		{
			m_pBugVert[i].iState	= NATURE_WAIT;
			dwBase = (int)(sinf(m_fTime)*m_dwBaseNumber*11+m_AddRand) % m_dwBaseNumber;
			D3DXVECTOR3 vTempLength = m_pBugVert[i].vTargetPos - m_pBaseVert[dwBase].p;

			// ���� ���� �����Ų��. 
			if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < m_fBugRange*m_fBugRange && 
					sinf(m_fTime+i) > 0.9f )	// 100.f
			{
				if ( m_fBugSpeed > 10.f )	// 0���� ũ�� 10���� �۴�.
					m_fBugSpeed = 10.f;
				float Speed = 10.f - m_fBugSpeed;
				m_pBugVert[i].vTargetPos = m_pBaseVert[dwBase].p;
				m_pBugVert[i].iState		= NATURE_UPFLY;
				m_pBugVert[i].iLeft		= ((int)m_fTime+i)%4 + 1;
				m_pBugVert[i].iRight		= ((int)m_fTime+i)%4 + 1;
				m_pBugVert[i].fSpeed		= (float)(((int)m_fTime+i)%4) + Speed;

				if ( m_pBugVert[i].fSpeed < 1.f )
					m_pBugVert[i].fSpeed = 1.f;

				D3DXVECTOR3 vDirec = m_pBugVert[i].vTargetPos - m_pBugVert[i].vNowPos;	// ����
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


				//	Note : ���� ���� �� �Ÿ��� ª�� �Ÿ���� �ɾ.. �� ����.
				//
				if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < 2500.f )	// 50.f
				{
					m_pBugVert[i].iState		= NATURE_WALK;
				}
			}
		}

		//	Note : ���� �ɾ ������ �Ÿ��� ª�ٸ�
		//
		else if ( m_pBugVert[i].iState == NATURE_WALK )
		{
			D3DXVec3Normalize ( &vLength, &vLength );

			m_pBugVert[i].vNowPos += vLength/m_pBugVert[i].fSpeed;
		}

		//	Note : �Ÿ� �ִ�...... ���� ����.����
		//
		else
		{
			// ��ü�� ��ǥ ������ ����� ���� ��
			if ( ((vLength.x*vLength.x)+(vLength.z*vLength.z)) < 64.f )		// �̰� 8.f �̴�.	// ���񿡼��� �̰� ��� ������.
			{
				D3DXVec3Normalize ( &vLength, &vLength );

				m_pBugVert[i].vNowPos += vLength*0.3f;
				m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/10.f;
				m_pBugVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/10.f;
				m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/10.f;

				m_pBugVert[i].iState = NATURE_LANDING;
			}

			// ��ü�� ��ǥ ������ ���� �Ÿ��� ���� ��
			else
			{
				// �ʹ� ���ƿ�
				if (m_pBugVert[i].vNowPos.y > m_pBugVert[i].vTargetPos.y + m_fBugHeightUP )	// �ְ� ����
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pBugVert[i].vNowPos += vLength*0.8f;
					m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/m_pBugVert[i].fSpeed;
					m_pBugVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pBugVert[i].fSpeed - 0.3f;
					m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/m_pBugVert[i].fSpeed;

					m_pBugVert[i].iState = NATURE_DOWNFLY;
				}
				// �ʹ� ���ƿ�
				else if ( m_pBugVert[i].vNowPos.y < m_pBugVert[i].vTargetPos.y + m_fBugHeightDOWN )	// �ּ� ����
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pBugVert[i].vNowPos += vLength*0.8f;
					m_pBugVert[i].vNowPos.x += sinf(m_fTime*m_pBugVert[i].iLeft+m_AddRand)/m_pBugVert[i].fSpeed;
					m_pBugVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pBugVert[i].fSpeed + 0.3f;
					m_pBugVert[i].vNowPos.z += sinf((m_fTime+1)*m_pBugVert[i].iRight+m_AddRand)/m_pBugVert[i].fSpeed;

					m_pBugVert[i].iState = NATURE_UPFLY;
				}
				// �����׿�.. ^^
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

	//	Note : ��������Ʈ
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


	//	Note : ������ �� �Ǿ� �ִ� ���� ������ ���� �ش�. ���⼭ ��������Ʈ�� �ؾ� �ϳ� ~?
	//
	PARTICLE*	pVertices;
	m_pBugVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	for( DWORD i=0; i<m_dwBugNum; i++ )
	{
		//	Note : �ڱ� �ڸ��� �ɾ� ���� ��...
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

		//	Note : �ɾ�� �� ��Ȳ �� ��,...
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


		//	Note : ���� ���� ���� ���� �ִ°��� ���ٸ�.. �̰���..
		if ( ((vLength.x*vLength.x)+(vLength.y*vLength.y)+(vLength.z*vLength.z)) < 1.f )	
		{
			m_pButterVert[i].iState	= NATURE_WAIT;
			dwBase = (int)(sinf(m_fTime)*m_dwBaseNumber*11+m_AddRand) % m_dwBaseNumber;
			D3DXVECTOR3 vTempLength = m_pButterVert[i].vTargetPos - m_pBaseVert[dwBase].p;

			// ���� ���� �����Ų��. 
			if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < m_fButterRange*m_fButterRange && 
					sinf(m_fTime+i) > 0.9f )	// 100.f
			{
				if ( m_fButterSpeed > 10.f )	// 0���� ũ�� 10���� �۴�.
					m_fButterSpeed = 10.f;
				float Speed = 10.f - m_fButterSpeed;
				m_pButterVert[i].vTargetPos		= m_pBaseVert[dwBase].p;
				m_pButterVert[i].vTargetPos.y	+= 0.1f;								// 0.1 �� �÷� �ش�.
				m_pButterVert[i].vPrevPos		= m_pButterVert[i].vNowPos;
				m_pButterVert[i].iState			= NATURE_UPFLY;
				m_pButterVert[i].iLeft			= ((int)m_fTime+i)%4 + 1;
				m_pButterVert[i].iRight			= ((int)m_fTime+i)%4 + 1;
				m_pButterVert[i].fSpeed			= (float)(((int)m_fTime+i)%4) + Speed;

				D3DXVECTOR3	vBaseVector3 = m_pButterVert[i].vTargetPos - m_pButterVert[i].vPrevPos;
				m_pButterVert[i].fBaseLenth = D3DXVec3Length ( &vBaseVector3 );

				if ( m_pButterVert[i].fSpeed < 1.f )
					m_pButterVert[i].fSpeed = 1.f;

				D3DXVECTOR3 vDirec = m_pButterVert[i].vTargetPos - m_pButterVert[i].vNowPos;	// ����
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


				//	Note : ���� ���� �� �Ÿ��� ª�� �Ÿ���� �ɾ.. �� ����.
				//
				if ( ((vTempLength.x*vTempLength.x)+(vTempLength.z*vTempLength.z)) < 0.f )	// 50.f
				{
					m_pButterVert[i].iState		= NATURE_WALK;
				}
			}
		}

		//	Note : ���� �ɾ ������ �Ÿ��� ª�ٸ�
		//
		else if ( m_pButterVert[i].iState == NATURE_WALK )
		{
			D3DXVec3Normalize ( &vLength, &vLength );

			m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
		}

		//	Note : �Ÿ� �ִ�...... ���� ����.����
		//
		else
		{
			// ��ü�� ��ǥ ������ ����� ���� ��
			if ( ((vLength.x*vLength.x)+(vLength.z*vLength.z)) < 64.f )		// �̰� 8.f �̴�.	// ���񿡼��� �̰� ��� ������.
			{
				D3DXVec3Normalize ( &vLength, &vLength );

				m_pButterVert[i].vNowPos += vLength/10.f;
				m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/10.f;
				m_pButterVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/10.f;
				m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/10.f;

				m_pButterVert[i].iState = NATURE_LANDING;
			}

			// ��ü�� ��ǥ ������ ���� �Ÿ��� ���� ��
			else
			{
				// �ʹ� ���ƿ�
				if (m_pButterVert[i].vNowPos.y > m_pButterVert[i].vTargetPos.y + m_fButterHeightUP )	// �ְ� ����
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pButterVert[i].fSpeed - 0.3f;
					m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/m_pButterVert[i].fSpeed;

					m_pButterVert[i].iState = NATURE_DOWNFLY;
				}
				// �ʹ� ���ƿ�
				else if ( m_pButterVert[i].vNowPos.y < m_pButterVert[i].vTargetPos.y + m_fButterHeightDOWN )	// �ּ� ����
				{
					D3DXVec3Normalize ( &vLength, &vLength );

					m_pButterVert[i].vNowPos += vLength/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.x += sinf(m_fTime*m_pButterVert[i].iLeft+m_AddRand)/m_pButterVert[i].fSpeed;
					m_pButterVert[i].vNowPos.y += sinf(m_fTime+i+m_AddRand)/m_pButterVert[i].fSpeed + 0.3f;
					m_pButterVert[i].vNowPos.z += sinf((m_fTime+1)*m_pButterVert[i].iRight+m_AddRand)/m_pButterVert[i].fSpeed;

					m_pButterVert[i].iState = NATURE_UPFLY;
				}
				// �����׿�.. ^^
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


	//	Note : ������ �� �Ǿ� �ִ� ���� ������ ���� �ش�. ���⼭ ��������Ʈ�� �ؾ� �ϳ� ~?
	//
	PARTICLE*	pVertices;
	m_pButterVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	for( DWORD i=0; i<m_dwButterNum; i++ )
	{
		//	Note : �ڱ� �ڸ��� �ɾ� ���� ��...
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

		//	Note : �ɾ�� �� ��Ȳ �� ��,...
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

			//	���̴� ����. ���� ..
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


	//	Note : �׸���
	//
	m_pButterShadowVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	
	for( DWORD i=0; i<m_dwButterNum; i++ )
	{
		// ���� ���
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


	//	Note : LocalMesh, OctreeMesh �� �� �ϳ��� ����.
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
		area += (m_fRate*0.01f)*CalcSurfaceArea(Array);	// 1���� ������ ����ؼ� ����� ���� ����.
		//area += m_fRainRate;							// 1��� - ������ ����� ����.

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
			if ( Fullarea  > dwRainVert )			// �ִ� �� ������ ���� �ʵ��� ����
			{
				m_pBaseVert[dwRainVert].p.x = Array[0]->p.x + s.x*r1 + t.x*r2;							// 1 ���� ��ó�� Vertex�� ����
				m_pBaseVert[dwRainVert].p.y = Array[0]->p.y + s.y*r1 + t.y*r2;
				m_pBaseVert[dwRainVert].p.z = Array[0]->p.z + s.z*r1 + t.z*r2;
			}

			area -= 1.0f;dwRainVert++;
		}
	}

	//	Note : LocalMesh, OctreeMesh �� �� �ϳ��� ����.
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
	m_dwBaseNumber = dwRainVert;	// ��ü �� ���� 

	//	Note : ���� ��Ʈ ���� �� �� �� 
	//
	for( DWORD i=0; i<m_dwBaseNumber; i++ )
	{
		D3DXVec3TransformCoord ( &m_pBaseVert[ i ].p, &m_pBaseVert[ i ].p, &matWorld );
	}

	CreateBugMesh		( pd3dDevice );	// ���� (����Ʈ)
	CreateButterMesh	( pd3dDevice );	// ����


	return S_OK;
}

HRESULT DxEffectNature::CreateBugMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	// ���� ����� 
	//
	m_dwBugNum = (int)(m_fBugRate*0.01*m_dwBaseNumber);		// ���� ���� ���ϱ�

	if ( m_dwBugNum > 0)
	{
		SAFE_DELETE_ARRAY (	m_pBugVert );
		m_pBugVert = new PARTICLE_BUG[m_dwBugNum];


		SAFE_RELEASE ( m_pBugVB );

		pd3dDevice->CreateVertexBuffer ( 4*m_dwBugNum*sizeof(PARTICLE), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_MANAGED, &m_pBugVB, NULL  );

		if ( !m_pBugVB )		// �ʹ� ���� ũ�� �ƹ��͵� �ȳ����� �� �ش�.
			return S_OK;

		PARTICLE*	pVertices;
		m_pBugVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

		DWORD		dwBase;
		D3DXVECTOR3	vVel;
		for( DWORD i=0; i<m_dwBugNum; i++ )
		{
			if ( m_fBugSpeed > 10.f )	// 0���� ũ�� 10���� �۴�.
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

	// ���� ����� 
	//
	m_dwButterNum = (int)(m_fButterRate*0.01f*m_dwBaseNumber);		// ���� ���� ���ϱ�

	if ( m_dwButterNum > 0)
	{
		SAFE_DELETE_ARRAY (	m_pButterVert );
		m_pButterVert = new PARTICLE_BUTTERFLY[m_dwButterNum];


		SAFE_RELEASE ( m_pButterVB );

		pd3dDevice->CreateVertexBuffer ( 6*m_dwButterNum*sizeof(PARTICLE), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_MANAGED, &m_pButterVB, NULL );

		if ( !m_pButterVB )		// �ʹ� ���� ũ�� �ƹ��͵� �ȳ����� �� �ش�.
			return S_OK;

		PARTICLE*	pVertices;
		m_pButterVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

		DWORD		dwBase;
		D3DXVECTOR3	vVel;
		for( DWORD i=0; i<m_dwButterNum; i++ )
		{
			if ( m_fButterSpeed > 10.f )	// 0���� ũ�� 10���� �۴�.
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


		//	Note : �׸��� �����
		//
		SAFE_RELEASE ( m_pButterShadowVB );

		pd3dDevice->CreateVertexBuffer ( 4*m_dwButterNum*sizeof(PARTICLE), D3DUSAGE_WRITEONLY, 
										0, D3DPOOL_MANAGED, &m_pButterShadowVB, NULL );

		if ( !m_pButterShadowVB )		// �ʹ� ���� ũ�� �ƹ��͵� �ȳ����� �� �ش�.
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