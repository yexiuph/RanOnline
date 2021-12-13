#include "pch.h"
#include "DxDynamicVB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxDynamicVB::BIGVB	DxDynamicVB::m_sVB;
DxDynamicVB::BIGVB	DxDynamicVB::m_sVB_PDT;
DxDynamicVB::BIGVB	DxDynamicVB::m_sVB_PNT;
DxDynamicVB::BIGVB	DxDynamicVB::m_sVB_PT;
DxDynamicVB::BIGVB	DxDynamicVB::m_sVB_PWDT;
DxDynamicVB::BIGVB	DxDynamicVB::m_sVB_PDT2;
DxDynamicVB::BIGVB	DxDynamicVB::m_sVB_CHAR;



LPDIRECT3DINDEXBUFFER9	DxDynamicVB::m_pIBuffer = NULL;
DWORD					DxDynamicVB::DYNAMIC_MAX_FACE = 500;



DxDynamicVB& DxDynamicVB::GetInstance()
{
	static DxDynamicVB Instance;
	return Instance;
}

DxDynamicVB::DxDynamicVB(void)
{
}

DxDynamicVB::~DxDynamicVB(void)
{
}

HRESULT DxDynamicVB::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 기본적인 것
	m_sVB.pVB			= NULL;
	m_sVB.nOffsetToLock = 0;
	m_sVB.nVertexCount	= 0;
	m_sVB.dwFVF			= 0;
	m_sVB.nFullByte		= 32 * 10240;	// 32Byte*10240 = 320K-Byte

	// Note : 파티클, 등.. 점을 한번에 많이 사용
	m_sVB_PDT.pVB			= NULL;
	m_sVB_PDT.nOffsetToLock = 0;
	m_sVB_PDT.nVertexCount	= 0;
	m_sVB_PDT.dwFVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	m_sVB_PDT.nFullByte		= D3DXGetFVFVertexSize( m_sVB_PDT.dwFVF ) * 12800;	// 350 k-byte

	// Note : 지면, 천효과 등... 천효과에서 많이 사용.
	m_sVB_PNT.pVB			= NULL;
	m_sVB_PNT.nOffsetToLock	= 0;
	m_sVB_PNT.nVertexCount	= 0;
	m_sVB_PNT.dwFVF			= D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	m_sVB_PNT.nFullByte		= D3DXGetFVFVertexSize( m_sVB_PNT.dwFVF ) * 12800;	// 400 k-byte
	
	// Note : 시퀀스, 지면 등... 1번에 4개의 점 밖에는 사용 안함.
	m_sVB_PT.pVB			= NULL;
	m_sVB_PT.nOffsetToLock	= 0;
	m_sVB_PT.nVertexCount	= 0;
	m_sVB_PT.dwFVF			= D3DFVF_XYZ|D3DFVF_TEX1;
	m_sVB_PT.nFullByte		= D3DXGetFVFVertexSize( m_sVB_PT.dwFVF ) * 256;

	// Note : 일반 화면 효과
	m_sVB_PWDT.pVB				= NULL;
	m_sVB_PWDT.nOffsetToLock	= 0;
	m_sVB_PWDT.nVertexCount		= 0;
	m_sVB_PWDT.dwFVF			= D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE1(1);
	m_sVB_PWDT.nFullByte		= D3DXGetFVFVertexSize( m_sVB_PWDT.dwFVF ) * 256;

	// Note : 일반적인 컬러 알파 있는 Face
	m_sVB_PDT2.pVB				= NULL;
	m_sVB_PDT2.nOffsetToLock	= 0;
	m_sVB_PDT2.nVertexCount		= 0;
	m_sVB_PDT2.dwFVF			= D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
	m_sVB_PDT2.nFullByte		= D3DXGetFVFVertexSize( m_sVB_PDT2.dwFVF ) * 12800;	// 400 k-byte

	// Note : 무조건 캐릭터에만 쓰인다.. 다른곳에서 쓰이면 문제가 생긴다.
	m_sVB_CHAR.pVB				= NULL;
	m_sVB_CHAR.nOffsetToLock	= 0;
	m_sVB_CHAR.nVertexCount		= 0;
	m_sVB_CHAR.dwFVF			= D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	m_sVB_CHAR.nFullByte		= D3DXGetFVFVertexSize( m_sVB_CHAR.dwFVF ) * 12800;	// 400 k-byte
	
	// Note : CreateVertexBuffer
	m_sVB.CreateVB( pd3dDevice );
	m_sVB_PDT.CreateVB( pd3dDevice );
	m_sVB_PNT.CreateVB( pd3dDevice );
	m_sVB_PT.CreateVB( pd3dDevice );
	m_sVB_PWDT.CreateVB( pd3dDevice );
	m_sVB_PDT2.CreateVB( pd3dDevice );
	m_sVB_CHAR.CreateVB( pd3dDevice );

	// 인덱스 버퍼 생성
	pd3dDevice->CreateIndexBuffer ( 6*DYNAMIC_MAX_FACE*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIBuffer, NULL );

	// 인덱스 버퍼 초기화
	WORD*	pIndies;
	m_pIBuffer->Lock( 0, 0, (VOID**)&pIndies, 0 );
	for( DWORD i=0; i<DYNAMIC_MAX_FACE; ++i )
	{
		*pIndies++ = (WORD)(i*4 + 0);
		*pIndies++ = (WORD)(i*4 + 1);
		*pIndies++ = (WORD)(i*4 + 2);
		*pIndies++ = (WORD)(i*4 + 1);
		*pIndies++ = (WORD)(i*4 + 3);
		*pIndies++ = (WORD)(i*4 + 2);
	}
	m_pIBuffer->Unlock();

	return S_OK;
}

HRESULT DxDynamicVB::InvalidateDeviceObjects()
{
	m_sVB.ReleaseVB();
	m_sVB_PDT.ReleaseVB();
	m_sVB_PNT.ReleaseVB();
	m_sVB_PT.ReleaseVB();
	m_sVB_PWDT.ReleaseVB();
	m_sVB_PDT2.ReleaseVB();
	m_sVB_CHAR.ReleaseVB();

	SAFE_RELEASE( m_pIBuffer );

	return S_OK;
}

void DxDynamicVB::FrameMove()
{
	m_sVB.nVertexCount = 0;
	m_sVB.nOffsetToLock = 0;
}

void DxDynamicVB::BIGVB::CreateVB( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->CreateVertexBuffer( nFullByte, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, dwFVF, D3DPOOL_SYSTEMMEM, &pVB, NULL );
}

void DxDynamicVB::BIGVB::ReleaseVB()
{
	SAFE_RELEASE( pVB );
}

#include "DebugSet.h"

//-----------------------------------------------------------------------------------------------------------------------------------
//											O	P	T	M		B	a	s	e
//-----------------------------------------------------------------------------------------------------------------------------------
LPDIRECT3DDEVICEQ		OPTMBase::m_pd3dDevice = NULL;

OPTMBase::OPTMBase(void) :
	m_pSavedSB(NULL),
	m_pDrawSB(NULL)
{
}

OPTMBase::~OPTMBase(void)
{
}

inline void OPTMBase::UserMatrix( D3DXMATRIX* pIdentity, D3DXMATRIX* pWorld, BOOL& bChange )
{
	if( pWorld )
	{
		bChange = TRUE;
		m_pd3dDevice->SetTransform( D3DTS_WORLD, pWorld );
	}
	else if( bChange )
	{
		bChange = FALSE;
		m_pd3dDevice->SetTransform( D3DTS_WORLD, pIdentity );
	}
}

inline void OPTMBase::UserRenderState( D3DRENDERSTATETYPE dwState, RenderState& sBase, const RenderState& sRS )
{
	if( sRS.m_bChange )
	{
		sBase.m_bChange = TRUE;
		m_pd3dDevice->SetRenderState( dwState,	sRS.m_dwValue );
	}
	else if( sBase.m_bChange )
	{
		sBase.m_bChange = FALSE;
		m_pd3dDevice->SetRenderState( dwState,	sBase.m_dwValue );
	}
}

inline void OPTMBase::UserTextureStageState( D3DTEXTURESTAGESTATETYPE dwType, TextureStageState& sBase, const TextureStageState& sTSS )
{
	if( sTSS.m_bChange )
	{
		sBase.m_bChange = TRUE;
		m_pd3dDevice->SetTextureStageState( 0, dwType,	sTSS.m_dwValue );
	}
	else if( sBase.m_bChange )
	{
		sBase.m_bChange = FALSE;
		m_pd3dDevice->SetTextureStageState( 0, dwType,	sBase.m_dwValue );
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		S	e	q	u	e	n	c	e
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleSequence::OPTMSingleSequence(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_bBaseMatrix(TRUE),
	m_pBaseTexture(NULL)
{
}

OPTMSingleSequence::~OPTMSingleSequence(void)
{
}

void OPTMSingleSequence::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseCullMode.m_dwValue			= D3DCULL_CCW;
	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

	m_pDataPool = new CMemPool<DATA>;
}

void OPTMSingleSequence::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleSequence::FrameMove()
{
	// Note : 초기화를 통해서 넓게 넓게 사용하도록 한다.
	DxDynamicVB::GetInstance().m_sVB.nVertexCount = 0;
	DxDynamicVB::GetInstance().m_sVB.nOffsetToLock = 0;

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleSequence::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2 );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB.pVB, 0, 32 );

	int nCount(0);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		if( pCur->bChangeMatrix )
		{
			m_bBaseMatrix = FALSE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &pCur->matWorld );
		}
		else if( !m_bBaseMatrix )
		{
			m_bBaseMatrix = TRUE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		};

		UserRenderState( D3DRS_CULLMODE,			m_sBaseCullMode,			pCur->sCULLMODE );
		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;

		++nCount;
	}

	CDebugSet::ToView( 5, 10, "%d", nCount );

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleSequence::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		G	r	o	u	n	d
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleGround::OPTMSingleGround(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_pBaseTexture(NULL)
{
}

OPTMSingleGround::~OPTMSingleGround(void)
{
}

void OPTMSingleGround::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		////if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		//{
		//	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,	0x80 );
		//	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		//}

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseALPHAREF.m_dwValue			= 0x01;
	m_sBaseLighting.m_dwValue			= FALSE;
	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

	m_pDataPool = new CMemPool<DATA>;
}

void OPTMSingleGround::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleGround::FrameMove()
{
	// Note : 초기화를 통해서 넓게 넓게 사용하도록 한다.
	DxDynamicVB::GetInstance().m_sVB.nVertexCount = 0;
	DxDynamicVB::GetInstance().m_sVB.nOffsetToLock = 0;

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleGround::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB.pVB, 0, 32 );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pd3dDevice->SetFVF( pCur->dwFVF );
		pd3dDevice->SetTransform( D3DTS_WORLD, &pCur->matWorld );

		
		UserRenderState( D3DRS_ALPHAREF,			m_sBaseLighting,			pCur->sALPHAREF );
		UserRenderState( D3DRS_LIGHTING,			m_sBaseLighting,			pCur->sLIGHTING );
		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		// Note : Material Setting
		if( pCur->pMaterial )	pd3dDevice->SetMaterial( pCur->pMaterial );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleGround::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		B	l	u	r	S	y	s
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleBlurSys::OPTMSingleBlurSys(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_pBaseTexture(NULL),
	m_bMatrixChange(FALSE)
{
}

OPTMSingleBlurSys::~OPTMSingleBlurSys(void)
{
}

void OPTMSingleBlurSys::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

	m_pDataPool = new CMemPool<DATA>;
}

void OPTMSingleBlurSys::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleBlurSys::FrameMove()
{
	// Note : 초기화를 통해서 넓게 넓게 사용하도록 한다.
	DxDynamicVB::GetInstance().m_sVB.nVertexCount = 0;
	DxDynamicVB::GetInstance().m_sVB.nOffsetToLock = 0;

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleBlurSys::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2 );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB.pVB, 0, 32 );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		UserMatrix( &matIdentity, pCur->pmatWorld, m_bMatrixChange );

		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleBlurSys::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		P	a	r	t	i	c	l	e
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMSingleParticle::OPTMSingleParticle(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_bBaseMatrix(TRUE),
	m_pBaseTexture(NULL)
{
}

OPTMSingleParticle::~OPTMSingleParticle(void)
{
}

void OPTMSingleParticle::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

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
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_sBaseCullMode.m_dwValue			= D3DCULL_CCW;
	m_sBaseDestBlend.m_dwValue			= D3DBLEND_INVSRCALPHA;
	m_sBaseZWriteEnable.m_dwValue		= FALSE;
	m_sBaseAlphaBlendEnable.m_dwValue	= TRUE;
	m_sBaseColorOP.m_dwValue			= D3DTOP_MODULATE;

	m_pDataPool = new CMemPool<DATA>;
}

void OPTMSingleParticle::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMSingleParticle::FrameMove()
{
	// Note : 초기화를 통해서 넓게 넓게 사용하도록 한다.
	DxDynamicVB::GetInstance().m_sVB.nVertexCount = 0;
	DxDynamicVB::GetInstance().m_sVB.nOffsetToLock = 0;

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleParticle::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2 );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB.pVB, 0, 32 );

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		if( pCur->bChangeMatrix )
		{
			m_bBaseMatrix = FALSE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &pCur->matWorld );
		}
		else if( !m_bBaseMatrix )
		{
			m_bBaseMatrix = TRUE;
			pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
		};

		UserRenderState( D3DRS_CULLMODE,			m_sBaseCullMode,			pCur->sCULLMODE );
		UserRenderState( D3DRS_DESTBLEND,			m_sBaseDestBlend,			pCur->sDESTBLEND );
		UserRenderState( D3DRS_ZWRITEENABLE,		m_sBaseZWriteEnable,		pCur->sZWRITEENABLE );
		UserRenderState( D3DRS_ALPHABLENDENABLE,	m_sBaseAlphaBlendEnable,	pCur->sALPHABLENDENABLE );

		UserTextureStageState( D3DTSS_COLOROP, m_sBaseColorOP, pCur->sCOLOROP );

		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, pCur->nVertexCount, pCur->nFaceCount );

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMSingleParticle::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//							O	P	T	M		S	i	n	g	l	e		P	a	r	t	i	c	l	e
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMCharParticle::OPTMCharParticle(void) :
	m_pDataHead(NULL),
	m_pDataPool(NULL),
	m_pBaseTexture(NULL)
{
}

OPTMCharParticle::~OPTMCharParticle(void)
{
}

void OPTMCharParticle::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	float fBias(-0.00002f);
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,		D3DTOP_DISABLE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	m_pDataPool = new CMemPool<DATA>;
}

void OPTMCharParticle::OnLostDevice()
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;

	SAFE_DELETE( m_pDataPool );
}

void OPTMCharParticle::FrameMove()
{
	// Note : 초기화를 통해서 넓게 넓게 사용하도록 한다.
	DxDynamicVB::GetInstance().m_sVB.nVertexCount = 0;
	DxDynamicVB::GetInstance().m_sVB.nOffsetToLock = 0;

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMCharParticle::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : 디바이스를 얻는다.
	m_pd3dDevice = pd3dDevice;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	m_pBaseTexture = NULL;
	pd3dDevice->SetTexture( 0, NULL );

	pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2 );
	pd3dDevice->SetStreamSource( 0, DxDynamicVB::m_sVB.pVB, 0, 32 );
	pd3dDevice->SetIndices( DxDynamicVB::m_pIBuffer );

	

	DATA* pCur = m_pDataHead;
	while( pCur )
	{
		// Note : 텍스쳐가 같지 않을 경우만 바꾼다.
		if( m_pBaseTexture != pCur->pTex )
		{
			m_pBaseTexture = pCur->pTex;
			pd3dDevice->SetTexture( 0, pCur->pTex );
		}

		DWORD dwState_Depth, dwState_SrcBlend, dwState_DestBlend, dwState_AlphaOp;
		
		pd3dDevice->GetRenderState( D3DRS_DEPTHBIAS, &dwState_Depth );
		pd3dDevice->GetRenderState( D3DRS_SRCBLEND, &dwState_SrcBlend );
		pd3dDevice->GetRenderState( D3DRS_DESTBLEND, &dwState_DestBlend );
		pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP, &dwState_AlphaOp );

		if( pCur->bAlphaTex )
		{
			float fBias(0.0002f);
			pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_DESTALPHA );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		}

		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, pCur->nVertexCount, 0, pCur->nFaceCount*4, 0, pCur->nFaceCount*2 );

		if ( pCur->bAlphaTex ) 
		{
			pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&dwState_Depth) );
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				dwState_SrcBlend );
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			dwState_DestBlend );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwState_AlphaOp );
		}		

		pCur = pCur->pNext;
	}

	m_pSavedSB->Apply();

	// Note : memoryPOOL로 반환한다.
	DATA* pDel(NULL);
	pCur = m_pDataHead;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->pNext;
		m_pDataPool->Release( pDel );
	}

	// Note : 모두 반환. Head도 마찬가지.
	m_pDataHead = NULL;
}

void OPTMCharParticle::InsertData( DATA* pData )
{
	pData->pNext = m_pDataHead;
	m_pDataHead = pData;
}

//-----------------------------------------------------------------------------------------------------------------------------------
//											O	P	T	M		M	a	n	a	g	e	r
//-----------------------------------------------------------------------------------------------------------------------------------
OPTMManager& OPTMManager::GetInstance()
{
	static OPTMManager Instance;
	return Instance;
}

OPTMManager::OPTMManager(void)
{
}

OPTMManager::~OPTMManager(void)
{
}

void OPTMManager::OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sSingleParticle.OnResetDevice( pd3dDevice );
	m_sSingleSequence.OnResetDevice( pd3dDevice );
	m_sSingleGround.OnResetDevice( pd3dDevice );
	m_sSingleBlurSys.OnResetDevice( pd3dDevice );
	m_sCharParticle.OnResetDevice( pd3dDevice );
}

void OPTMManager::OnLostDevice()
{
	m_sSingleParticle.OnLostDevice();
	m_sSingleSequence.OnLostDevice();
	m_sSingleGround.OnLostDevice();
	m_sSingleBlurSys.OnLostDevice();
	m_sCharParticle.OnLostDevice();
}

void OPTMManager::FrameMove()
{
	m_sSingleParticle.FrameMove();
	m_sSingleSequence.FrameMove();
	m_sSingleGround.FrameMove();
	m_sSingleBlurSys.FrameMove();
	m_sCharParticle.FrameMove();
}

void OPTMManager::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sSingleParticle.Render( pd3dDevice );
	m_sSingleSequence.Render( pd3dDevice );
	m_sSingleGround.Render( pd3dDevice );
	m_sSingleBlurSys.Render( pd3dDevice );
	m_sCharParticle.Render( pd3dDevice );
}





