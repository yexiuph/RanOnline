#include "pch.h"

#include "./DxDynamicVB.h"
#include "./DxSurfaceTex.h"
#include "./DxEnvironment.h"
#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharBlur.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharBlur::TYPEID			= EMEFFCHAR_BLUR;
DWORD		DxEffCharBlur::VERSION			= 0x0105;
char		DxEffCharBlur::NAME[MAX_PATH]	= "2.캐릭터 잔상";

LPDIRECT3DSTATEBLOCK9		DxEffCharBlur::m_pSavedStateBlock	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharBlur::m_pEffectStateBlock	= NULL;

DxEffCharBlur::VERTEX_UPDOWN_POOL*	DxEffCharBlur::m_pVertexUpDownPool = NULL;
DxEffCharBlur::BLUR_UPDOWN_POOL*	DxEffCharBlur::m_pBlurUpDownPool = NULL;

DxEffCharBlur::DxEffCharBlur(void) :
	DxEffChar(),
	m_dwPrevKeyFrame(0),
	m_dwKeyPointCOUNT(0),
	m_bBaseBlurRender(FALSE),

	m_fTime(0.f),
	m_fElapsedTime(0.0f),
	m_pddsTexture(NULL)
{
	m_fFullLife = 0.f;

	m_vColorUP = D3DCOLOR_ARGB(255,255,255,255);
	m_vColorDOWN = D3DCOLOR_ARGB(0,255,255,255);

	m_fAlpha[0] = 1.f;
	m_fAlpha[1] = 0.f;

	m_dwColor[0] = 0xffffff;
	m_dwColor[1] = 0xffffff;

	D3DXMatrixIdentity( &m_matIdentity );

	memset( m_szTraceUP, 0, sizeof(char)*STRACE_NSIZE );
	memset( m_szTraceDOWN, 0, sizeof(char)*STRACE_NSIZE );
}

DxEffCharBlur::~DxEffCharBlur(void)
{
	// Note : Key Data를 clear 한다.
	ClearKeyFrame();

	// Note : Blur Data를 clear 한다.
	ClearBlurDara();

	CleanUp ();
}

DxEffChar* DxEffCharBlur::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharBlur *pEffChar = new DxEffCharBlur;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharBlur::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	D3DCAPSQ	d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// Note : Base 값
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x40 );

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharBlur::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	

	return S_OK;
}

void DxEffCharBlur::OnCreateDevice_STATIC()
{
	m_pVertexUpDownPool = new CMemPool<VERTEX_UPDOWN>;
	m_pBlurUpDownPool = new CMemPool<BLUR_UPDOWN>;
}

void DxEffCharBlur::OnDestroyDevice_STATIC()
{
	SAFE_DELETE( m_pVertexUpDownPool );
	SAFE_DELETE( m_pBlurUpDownPool );
}

HRESULT DxEffCharBlur::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharBlur::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3	vNormal;

	m_fFullLife = m_fFullLifeTemp;
	m_vColorUP = m_vColorUPTemp;
	m_vColorDOWN = m_vColorDOWNTemp;

	m_fOneDiveFullLife = 1.f / m_fFullLifeTemp;

	m_fAlpha[0] = ((DWORD)(m_vColorUP & 0xff000000) >> 24) / 255.f;;
	m_fAlpha[1] = ((DWORD)(m_vColorDOWN & 0xff000000) >> 24) / 255.f;;
	m_dwColor[0] = m_vColorUP & 0xffffff;
	m_dwColor[1] = m_vColorDOWN & 0xffffff;

	//	Note : Eff Group의 매트릭스 트랜스폼 지정.
	if ( m_szTraceUP[0] == NULL )	return E_FAIL;
	if ( m_szTraceDOWN[0] == NULL )	return E_FAIL;

	//	Note : 텍스쳐 로드
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffCharBlur::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharBlur::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	// Note : Key Data를 clear 한다.
	ClearKeyFrame();

	// Note : Blur Data를 clear 한다.
	ClearBlurDara();

	return S_OK;
}

HRESULT DxEffCharBlur::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffCharBlur::FinalCleanup ()
{

	return S_OK;
}

WORD ConvertRGBToWORD( int wR, int wG, int wB )
{
	// 압축시
	int wSum;
	wR = wR >> 3;
	wG = wG >> 3;
	wB = wB >> 3;
	wSum = (wR<<10) + (wG<<5) + wB;

	CDebugSet::ToLogFile( "%d", wSum );

	CDebugSet::ToLogFile( " %d, %d, %d", wR, wG, wB );

	// 풀경우
	wR = wSum >> 10;
	wG = wSum >> 5;
	wG = wG&0x1f;
	wB = wSum&0x1f;

	CDebugSet::ToLogFile( " %d, %d, %d", wR, wG, wB );
	
	wR = (WORD)( wR * 8.225806f );	//<< 3;
	wG = (WORD)( wG * 8.225806f );	//<< 3;
	wB = (WORD)( wB * 8.225806f );	//<< 3;

	CDebugSet::ToLogFile( " %d, %d, %d \n", wR, wG, wB );

	return 1;
}

HRESULT DxEffCharBlur::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime += fElapsedTime;
	m_fElapsedTime += fElapsedTime;

	// Note ; 스킬로 붙어 있을 경우 골치 아프게 된다.
	if( m_dwFlag&USEALLACTION )
	{
		if ( m_fTime > 4.5f )	SetEnd();
	}

	// Note : 굴절 셋팅이 되어 있을 경우 강제 셋팅한다.
	//			필요하다면 변하게 할 수 있지만.... 쩝.
	if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
	{
		m_fFullLife = 0.6f;
		m_vColorUP = 0xffffffff;
		m_vColorDOWN = 0xffffffff;

		m_fOneDiveFullLife = 1.f / 0.6f;

		m_fAlpha[0] = ((DWORD)(0xff000000) >> 24) / 255.f;;
		m_fAlpha[1] = ((DWORD)(0xff000000) >> 24) / 255.f;;
		m_dwColor[0] = 0xffffff;
		m_dwColor[1] = 0xffffff;
	}
	else
	{
		m_fFullLife = m_fFullLifeTemp;
		m_vColorUP = m_vColorUPTemp;
		m_vColorDOWN = m_vColorDOWNTemp;

		m_fOneDiveFullLife = 1.f / m_fFullLifeTemp;

		m_fAlpha[0] = ((DWORD)(m_vColorUP & 0xff000000) >> 24) / 255.f;;
		m_fAlpha[1] = ((DWORD)(m_vColorDOWN & 0xff000000) >> 24) / 255.f;;
		m_dwColor[0] = m_vColorUP & 0xffffff;
		m_dwColor[1] = m_vColorDOWN & 0xffffff;
	}

	//static BOOL bUse (TRUE);
	//if( bUse )
	//{
	//	ConvertRGBToWORD( 238, 250, 255 );	// 0
	//	ConvertRGBToWORD( 176, 102,  92 );	// 1
	//	ConvertRGBToWORD( 255, 190,  51 );	// 2
	//	ConvertRGBToWORD( 189, 216, 255 );	// 3
	//	ConvertRGBToWORD( 255, 134,  21 );	// 4
	//	ConvertRGBToWORD( 255, 208, 148 );	// 5
	//	ConvertRGBToWORD( 243, 250, 255 );	// 6
	//	ConvertRGBToWORD( 255, 255, 255 );	// 7
	//	ConvertRGBToWORD( 138, 151, 192 );	// 0
	//	ConvertRGBToWORD( 238, 148,  53 );	// 1
	//	ConvertRGBToWORD( 241, 196, 123 );	// 2
	//	ConvertRGBToWORD( 189, 112,  36 );	// 3
	//	ConvertRGBToWORD( 255, 191,  64 );	// 4
	//	ConvertRGBToWORD( 211, 152, 110 );	// 5
	//	ConvertRGBToWORD( 219, 182, 103 );	// 6

	//	bUse = FALSE;
	//}
	
	
	
	/*
	m1_hair_00 	238 250 255
	m1_hair_01	176 102  92
	m1_hair_02	255 190  51
	m1_hair_03	189 216 255
	m1_hair_04	255 134  21
	m1_hair_05	255 208 148

	SHW0001_M	243 250 255
	SHW0002_M	255 255 255


	W1_hair_00	138 151 192
	W1_hair_01	238 148  53
	W1_hair_02	241 196 123
	W1_hair_03	189 112  36
	W1_hair_04	255 191  64

	SHW0001_W	211 152 110
	SHW0002_W	219 182 103
	*/

	
	


	return S_OK;
}

void DxEffCharBlur::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	// Note : Key가 바뀌면 기본적으로 저장된 키를 지워준다.
	if( m_dwPrevKeyFrame!=m_dwKeyTime )
	{
		D3DXVECTOR3 vUp(0.f,0.f,0.f);
		D3DXVECTOR3 vDown(0.f,0.f,0.f);

		// Note : 현재 위치를 얻는다. 만약 같다면 이전거랑 생성하지 않는다.
		if( !IsUnionKey( vUp, vDown ) )	goto _RETURN;

		// Note : 초기화.
		m_bBaseBlurRender = FALSE;

		// Note : 처음것 지움
		FrameMoveKeyFrame();

		// 같을 경우에만 잔상을 새로 생성한다. || 모든 액션일 경우에 삽입한다.
		if( (m_CurAniMType==m_SetAniMType) || (m_dwFlag&USEALLACTION) )
		{
			CreateNewKeyFrame( vUp, vDown );	// m_deqKeyPoint 에 넣는다.
			CreateBlur();						// 생성.
		}
		else
		{
			ClearKeyFrame();	// 애니메이션이 바뀌어서 기존 저장된 Key 데이터를 삭제한다.
		}

		FrameMoveBlur();		// 활동.

		// 백업 및 초기화.
		m_dwPrevKeyFrame = m_dwKeyTime;
		m_fElapsedTime = 0.f;
	}


_RETURN:

	m_pSavedStateBlock->Capture();
	m_pEffectStateBlock->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		break;
	case 3:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

		break;			
	case 4:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case 5:
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
		}
		break;
	}

	pd3dDevice->SetTransform( D3DTS_WORLD, &m_matIdentity );

	if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
	{
		DxEnvironment::GetInstance().SetRefractPlayON();	// 활성화
		pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pWaveTex );

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	}
	else
	{
		// Note : Get Texture
		if( !m_pddsTexture )
		{
			TextureManager::GetTexture( m_szTexture, m_pddsTexture );
		}
		pd3dDevice->SetTexture( 0, m_pddsTexture );
	}

	RenderBlur( pd3dDevice );

	m_pSavedStateBlock->Apply();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//																K	e	y
//------------------------------------------------------------------------------------------------------------------------------------------------
BOOL DxEffCharBlur::IsUnionKey( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown )
{
	// Note : 현재의 위치를 얻는다.
	HRESULT hr(S_OK);
	D3DXVECTOR3 vNormal(0.f,0.f,0.f);
	if ( m_pCharPart )
	{
		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceUP, vUp, vNormal );
		if ( FAILED(hr) )	return FALSE;

		hr = m_pCharPart->CalculateVertexInflu ( m_szTraceDOWN, vDown, vNormal );
		if ( FAILED(hr) )	return FALSE;
	}
	else if ( m_pSkinPiece )
	{
		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceUP, vUp, vNormal );
		if ( FAILED(hr) )	return FALSE;

		hr = m_pSkinPiece->CalculateVertexInflu ( m_szTraceDOWN, vDown, vNormal );
		if ( FAILED(hr) )	return FALSE;
	}

	if( !m_deqKeyPoint.empty() )
	{
		// 같은 위치일 경우 값을 넣지 않는다.
		if( m_deqKeyPoint[0]->sPos[0].x == vUp.x &&
			m_deqKeyPoint[0]->sPos[0].y == vUp.y &&
			m_deqKeyPoint[0]->sPos[0].z == vUp.z )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void DxEffCharBlur::CreateNewKeyFrame( D3DXVECTOR3& vUp, D3DXVECTOR3& vDown )
{
	// Note : 값을 넣는다.
	VERTEX_UPDOWN* pNew = m_pVertexUpDownPool->New();
	pNew->sPos[0] = vUp;
	pNew->sPos[1] = vDown;

	m_deqKeyPoint.push_front( pNew );
	++m_dwKeyPointCOUNT;
}

void DxEffCharBlur::FrameMoveKeyFrame()
{
	VERTEX_UPDOWN* pVector(NULL);
	while( m_deqKeyPoint.size() >= 4 )
	{
		pVector = m_deqKeyPoint.back();

		m_pVertexUpDownPool->Release( pVector );
		//SAFE_DELETE( pVector );

		m_deqKeyPoint.pop_back();
		--m_dwKeyPointCOUNT;
	}
}

void DxEffCharBlur::ClearKeyFrame()
{
	if( !m_pVertexUpDownPool )	return;

	for( DWORD i=0; i<m_deqKeyPoint.size(); ++i )
	{
		m_pVertexUpDownPool->Release( m_deqKeyPoint[i] );
	}
	m_deqKeyPoint.clear();

	m_dwKeyPointCOUNT = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//															B	l	u	r
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBlur::CreateBlur()
{
	float fAlpha(1.f);
	int nNUM(0);
	BLUR_UPDOWN* pNew(NULL);
	BLUR_UPDOWN* pNew1(NULL);

	// Key의 갯수에 따라서 생성을 틀리게 함.
	switch( m_dwKeyPointCOUNT ) 
	{
	case 2:
		m_bBaseBlurRender = TRUE;
		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos[0];
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos[1];
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos[0];
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos[1];
		break;


	case 3:
		m_bBaseBlurRender = TRUE;
		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos[0];
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos[1];
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos[0];
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos[1];

		// Note : 데이터 삽입.
		pNew = m_pBlurUpDownPool->New();
		pNew->fLife = m_fFullLife;
		pNew->sVertex[0].vPos = m_deqKeyPoint[2]->sPos[0];
		pNew->sVertex[1].vPos = m_deqKeyPoint[2]->sPos[1];
		m_deqBlurPoint.push_back( pNew );

		pNew1 = m_pBlurUpDownPool->New();
		pNew1->fLife = m_fFullLife;
		pNew1->sVertex[0].vPos = m_deqKeyPoint[1]->sPos[0];
		pNew1->sVertex[1].vPos = m_deqKeyPoint[1]->sPos[1];
		m_deqBlurPoint.push_back( pNew1 );

		break;


	case 4:
		m_bBaseBlurRender = TRUE;
		m_sBaseBlur[0].vPos = m_deqKeyPoint[0]->sPos[0];
		m_sBaseBlur[1].vPos = m_deqKeyPoint[0]->sPos[1];
		m_sBaseBlur[2].vPos = m_deqKeyPoint[1]->sPos[0];
		m_sBaseBlur[3].vPos = m_deqKeyPoint[1]->sPos[1];

		// Note : 중간 단계 셋팅.
		nNUM = SeparateNumOfLine();
		switch( nNUM )
		{
		case 0:
			break;

		case 1:
			DxInsertBlurPoint( 0.5f );
			break;

		case 2:
			DxInsertBlurPoint( 0.66f );
			DxInsertBlurPoint( 0.33f );
			break;

		case 3:
			DxInsertBlurPoint( 0.75f );
			DxInsertBlurPoint( 0.5f );
			DxInsertBlurPoint( 0.25f );
			break;

		case 4:
			DxInsertBlurPoint( 0.8f );
			DxInsertBlurPoint( 0.6f );
			DxInsertBlurPoint( 0.4f );
			DxInsertBlurPoint( 0.2f );
			break;

		case 5:
			DxInsertBlurPoint( 0.83f );
			DxInsertBlurPoint( 0.66f );
			DxInsertBlurPoint( 0.5f );
			DxInsertBlurPoint( 0.33f );
			DxInsertBlurPoint( 0.17f );
			break;

		case 6:
			DxInsertBlurPoint( 0.858f );
			DxInsertBlurPoint( 0.715f );
			DxInsertBlurPoint( 0.572f );
			DxInsertBlurPoint( 0.429f );
			DxInsertBlurPoint( 0.286f );
			DxInsertBlurPoint( 0.143f );
			break;

		case 7:
			fAlpha = 1.f;
			DxInsertBlurPoint( 0.875f );
			DxInsertBlurPoint( 0.75f );
			DxInsertBlurPoint( 0.625f );
			DxInsertBlurPoint( 0.5f );
			DxInsertBlurPoint( 0.375f );
			DxInsertBlurPoint( 0.25f );
			DxInsertBlurPoint( 0.125f );
			break;

		case 15:
			fAlpha = 1.f;					fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );	fAlpha -= 0.0625f;
			DxInsertBlurPoint( fAlpha );
			break;

		default:
			CDebugSet::ToLogFile( "SeparateNumOfLine()에서 넘어오는 값 문제" );
		};

		// Note : 최종 점 셋팅.
		pNew = m_pBlurUpDownPool->New();
		pNew->fLife = m_fFullLife;
		pNew->sVertex[0].vPos = m_deqKeyPoint[1]->sPos[0];
		pNew->sVertex[1].vPos = m_deqKeyPoint[1]->sPos[1];
		m_deqBlurPoint.push_back( pNew );		

		break;

	default:
		m_bBaseBlurRender = FALSE;
		break;
	};
}

int DxEffCharBlur::SeparateNumOfLine()
{
	D3DXVECTOR3 vLine[2];
	vLine[0] = m_deqKeyPoint[0]->sPos[0] - m_deqKeyPoint[2]->sPos[0];
	vLine[1] = m_deqKeyPoint[1]->sPos[0] - m_deqKeyPoint[3]->sPos[0];

	D3DXVec3Normalize( &vLine[0], &vLine[0] );
	D3DXVec3Normalize( &vLine[1], &vLine[1] );

	float fDot = D3DXVec3Dot( &vLine[0], &vLine[1] );

	int nData(0);
	if( fDot > 0.97f )			nData = 0;
	else if( fDot > 0.93f )		nData = 1;
	else if( fDot > 0.89f )		nData = 2;
	else if( fDot > 0.83f )		nData = 3;
	else if( fDot > 0.75f )		nData = 4;
	else if( fDot > 0.55f )		nData = 5;
	else if( fDot > 0.3f )		nData = 6;
	else if( fDot > 0.0f )		nData = 7;
	else						nData = 15;

	// 캐릭터 디테일이 낮다면 굴절을 많이 하지 않는다.
	if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_SPEC )
	{
		if( nData==0 )			nData = 0;
		else if( nData==1 )		nData = 0;
		else if( nData==2 )		nData = 1;
		else if( nData==3 )		nData = 2;
		else if( nData==4 )		nData = 3;
		else if( nData==5 )		nData = 4;
		else if( nData==6 )		nData = 5;
		else if( nData==7 )		nData = 6;
		else if( nData==15 )	nData = 7;
	}
	else if( DxEffectMan::GetInstance().GetSkinDetail() == SKD_NONE )
	{
		if( nData==0 )			nData = 0;
		else if( nData==1 )		nData = 0;
		else if( nData==2 )		nData = 1;
		else if( nData==3 )		nData = 1;
		else if( nData==4 )		nData = 2;
		else if( nData==5 )		nData = 2;
		else if( nData==6 )		nData = 3;
		else if( nData==7 )		nData = 3;
		else if( nData==15 )	nData = 4;
	}

	return nData;
}

void DxEffCharBlur::DxInsertBlurPoint( float fRate )
{
	D3DXVECTOR3 vPos1(0.f,0.f,0.f);
	D3DXVec3CatmullRom( &vPos1, &m_deqKeyPoint[0]->sPos[0], &m_deqKeyPoint[1]->sPos[0], 
								&m_deqKeyPoint[2]->sPos[0], &m_deqKeyPoint[3]->sPos[0], fRate );

	D3DXVECTOR3 vPos2(0.f,0.f,0.f);
	D3DXVec3CatmullRom( &vPos2, &m_deqKeyPoint[0]->sPos[1], &m_deqKeyPoint[1]->sPos[1], 
								&m_deqKeyPoint[2]->sPos[1], &m_deqKeyPoint[3]->sPos[1], fRate );

	BLUR_UPDOWN* pNew = m_pBlurUpDownPool->New();
	pNew->fLife = m_fFullLife;
	pNew->sVertex[0].vPos = vPos1;
	pNew->sVertex[1].vPos = vPos2;
	m_deqBlurPoint.push_back( pNew );
}

void DxEffCharBlur::FrameMoveBlur()
{
	// 시간을 흘리고,,
	DWORD dwSize = (DWORD)m_deqBlurPoint.size();
	for( DWORD i=0; i<dwSize; ++i )
	{
		m_deqBlurPoint[i]->fLife -= m_fElapsedTime;
	}

	// 시간 다 된 것은 지워준다.
	// 총 시간은 동일하게 들어가기 때문에 큐형식으로 지운다.
	BLUR_UPDOWN* pCur(NULL);
	while( !m_deqBlurPoint.empty() )
	{
		pCur = m_deqBlurPoint.front();

		if( pCur->fLife > 0.f )	goto _RETURN;

		//SAFE_DELETE( pCur );
		m_pBlurUpDownPool->Release( pCur );
		m_deqBlurPoint.pop_front();
	}

_RETURN:

	D3DVIEWPORT9* pViewPort(NULL);
	D3DXMATRIX* pmatView(NULL);
	D3DXMATRIX* pmatProj(NULL);
	float fOneDivWidth(0.f);
	float fOneDivHeight(0.f);
	float fAddWidth(0.f);
	float fAddHeight(0.f);

	if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
	{
		pViewPort = &DxViewPort::GetInstance().GetViewPort();
		pmatView = &DxViewPort::GetInstance().GetMatView();
		pmatProj = &DxViewPort::GetInstance().GetMatProj();

		if( pViewPort->Width < 1 )	return;
		if( pViewPort->Height < 1 )	return;

		fOneDivWidth = 1.f / pViewPort->Width;
		fOneDivHeight = 1.f / pViewPort->Height;

		// 회전
		float fSin = sinf(m_fTime)*0.01f;
		float fCos = cosf(m_fTime)*0.01f;

		fAddWidth = fCos + fSin;
		fAddHeight = fCos - fSin;
	}

	// Note : 
	dwSize = (DWORD)m_deqBlurPoint.size();
	float fTemp(0.f);
	float fOneDivSize(0.f);

	if( m_bBaseBlurRender )
	{
		fOneDivSize = 1.f / (dwSize+1);	// Base 잔상 때문에. 1을 더해주었다.
	}
	else if( dwSize )
	{
		fOneDivSize = 1.f / dwSize;
	}

	// Note : 기본 잔상.
	int nCountUV(0);
	if( m_bBaseBlurRender )
	{
		m_sBaseBlur[0].dwColor = m_vColorUP;
		m_sBaseBlur[1].dwColor = m_vColorDOWN;

		if( m_deqBlurPoint.empty() )
		{
			m_sBaseBlur[2].dwColor = m_dwColor[0];
			m_sBaseBlur[3].dwColor = m_dwColor[1];
		}
		else
		{
			fTemp = m_deqBlurPoint.back()->fLife * m_fOneDiveFullLife;
			fTemp = fTemp*255.f;

			m_sBaseBlur[2].dwColor = (DWORD)(fTemp*m_fAlpha[0]) << 24;
			m_sBaseBlur[3].dwColor = (DWORD)(fTemp*m_fAlpha[1]) << 24;
			m_sBaseBlur[2].dwColor += m_dwColor[0];
			m_sBaseBlur[3].dwColor += m_dwColor[1];
		}

		
		if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			D3DXVec3Project( &vPos, &m_sBaseBlur[0].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[0].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[1].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[1].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[2].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[2].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_sBaseBlur[3].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_sBaseBlur[3].vTex = D3DXVECTOR2( vPos.x, vPos.y );
		}
		else
		{
			m_sBaseBlur[0].vTex = D3DXVECTOR2( 0.f, 0.f );
			m_sBaseBlur[1].vTex = D3DXVECTOR2( 0.f, 1.f );
			m_sBaseBlur[2].vTex = D3DXVECTOR2( fOneDivSize, 0.f );
			m_sBaseBlur[3].vTex = D3DXVECTOR2( fOneDivSize, 1.f );
		}

		++nCountUV;	// 누적 블러를 위해서 넣어둔다.
	}

	// Note : 디퓨즈 값을 변화 시킨다.	
	for( DWORD i=0; i<dwSize; ++i, ++nCountUV )
	{
		fTemp = m_deqBlurPoint[i]->fLife * m_fOneDiveFullLife *255.f;

		m_deqBlurPoint[i]->sVertex[0].dwColor = (DWORD)(fTemp*m_fAlpha[0]) << 24;
		m_deqBlurPoint[i]->sVertex[1].dwColor = (DWORD)(fTemp*m_fAlpha[1]) << 24;
		m_deqBlurPoint[i]->sVertex[0].dwColor += m_dwColor[0];
		m_deqBlurPoint[i]->sVertex[1].dwColor += m_dwColor[1];

		if( (m_dwFlag & USEREFRACT) && DxSurfaceTex::GetInstance().IsEnable() )
		{
			D3DXVECTOR3 vPos(0.f,0.f,0.f);
			D3DXVec3Project( &vPos, &m_deqBlurPoint[i]->sVertex[0].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_deqBlurPoint[i]->sVertex[0].vTex = D3DXVECTOR2( vPos.x, vPos.y );

			D3DXVec3Project( &vPos, &m_deqBlurPoint[i]->sVertex[1].vPos, pViewPort, pmatProj, pmatView, &m_matIdentity );
			vPos.x = vPos.x*fOneDivWidth + fAddWidth;
			vPos.y = vPos.y*fOneDivHeight + fAddHeight;
			m_deqBlurPoint[i]->sVertex[1].vTex = D3DXVECTOR2( vPos.x, vPos.y );
		}
		else
		{
			fTemp = nCountUV*fOneDivSize;
			m_deqBlurPoint[i]->sVertex[0].vTex = D3DXVECTOR2( fTemp, 0.f );
			m_deqBlurPoint[i]->sVertex[1].vTex = D3DXVECTOR2( fTemp, 1.f );
		}
	}
}

void DxEffCharBlur::ClearBlurDara()
{
	if( !m_pBlurUpDownPool )	return;

	for( DWORD i=0; i<m_deqBlurPoint.size(); ++i )
	{
		m_pBlurUpDownPool->Release( m_deqBlurPoint[i] );
	}
	m_deqBlurPoint.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//															R	E	N	D	E	R
//------------------------------------------------------------------------------------------------------------------------------------------------
void DxEffCharBlur::RenderBlur( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	pd3dDevice->SetIndices ( NULL );

	// Note : 기본 잔상.
	if( m_bBaseBlurRender )
	{
		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = sizeof(VERTEXCOLORTEX)*4;
		if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
		{
			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
			DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
		}

		VERTEXCOLORTEX *pVertices;
		hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		if ( FAILED(hr) )	return;

		for( DWORD i=0; i<4; ++i )
		{
			pVertices[i] = m_sBaseBlur[i];
		}

		DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

		pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(VERTEXCOLORTEX) );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, 2 );

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB_PDT.nVertexCount += 4;
		DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
	}
	
	// Note : 남는 잔상.
	if( !m_deqBlurPoint.empty() )
	{
		DWORD dwBlurPointSize = (DWORD)m_deqBlurPoint.size();
		if( dwBlurPointSize <= 1 )	return;
		DWORD dwVerticesNUM = dwBlurPointSize * 2;

		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = sizeof(VERTEXCOLORTEX)*dwVerticesNUM;
		if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
		{
			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
			DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
		}

		VERTEXCOLORTEX *pVertices;
		hr = DxDynamicVB::m_sVB_PDT.pVB->Lock ( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		if ( FAILED(hr) )	return;

		for( DWORD i=0; i<dwBlurPointSize; ++i )
		{
			pVertices[i*2+0] = m_deqBlurPoint[i]->sVertex[0];
			pVertices[i*2+1] = m_deqBlurPoint[i]->sVertex[1];
		}

		DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

		pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(VERTEXCOLORTEX) );

		pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, dwVerticesNUM-2 );

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB_PDT.nVertexCount += dwVerticesNUM;
		DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//												L	O	A	D		&&		 S	A	V	E
//------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT	DxEffCharBlur::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;		

	SFile >> dwVer;
	SFile >> dwSize;

	char szIMAGE[4][STRACE_NSIZE] =
	{
		STRACE_IMAGE01,
		STRACE_IMAGE02,
		STRACE_IMAGE11,
		STRACE_IMAGE12,
	};
	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(EFFCHAR_PROPERTY_BLUR) );

		//	Blur 삭제 버그로 인한 A/S
		std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
		std::string::size_type idx = strSrcName.find("effskin");
		if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// 일치하는 것이 있다.

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x104 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		EFFCHAR_PROPERTY_BLUR_104	pProperty;
		SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_104) );

		m_Property.m_SetAniMType		= pProperty.m_SetAniMType;
		m_Property.m_SetAniSType		= pProperty.m_SetAniSType;

		m_Property.m_nBlend				= pProperty.m_nBlend;
		m_Property.m_dwFlag				= pProperty.m_dwFlag;
	
		m_Property.m_vColorUPTemp			= pProperty.m_vColorUPTemp;
		m_Property.m_vColorDOWNTemp			= pProperty.m_vColorDOWNTemp;

		StringCchCopy( m_Property.m_szTexture,		TEXTURE_LENGTH,		pProperty.m_szTexture ); 
		StringCchCopy( m_Property.m_szTraceUP,		STRACE_NSIZE,		pProperty.m_szTraceUP);
		StringCchCopy( m_Property.m_szTraceDOWN,	STRACE_NSIZE,		pProperty.m_szTraceDOWN);

		//	Blur 삭제 버그로 인한 A/S
		std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
		std::string::size_type idx = strSrcName.find("effskin");
		if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// 일치하는 것이 있다.


		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x103 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		EFFCHAR_PROPERTY_BLUR_103	pProperty;
		SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_103) );

		m_Property.m_SetAniMType		= pProperty.m_SetAniMType;
		m_Property.m_SetAniSType		= pProperty.m_SetAniSType;

		m_Property.m_nBlend				= pProperty.m_nBlend;
	
		m_Property.m_vColorUPTemp			= pProperty.m_vColorUPTemp;
		m_Property.m_vColorDOWNTemp			= pProperty.m_vColorDOWNTemp;

		StringCchCopy( m_Property.m_szTexture,		TEXTURE_LENGTH,		pProperty.m_szTexture ); 
		StringCchCopy( m_Property.m_szTraceUP,		STRACE_NSIZE,		pProperty.m_szTraceUP);
		StringCchCopy( m_Property.m_szTraceDOWN,	STRACE_NSIZE,		pProperty.m_szTraceDOWN);

		//	Blur 삭제 버그로 인한 A/S
		std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
		std::string::size_type idx = strSrcName.find("effskin");
		if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// 일치하는 것이 있다.


		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x102 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		EFFCHAR_PROPERTY_BLUR_102	pProperty;
		SFile.ReadBuffer ( &pProperty, sizeof(EFFCHAR_PROPERTY_BLUR_102) );

		m_Property.m_SetAniMType		= pProperty.m_SetAniMType;
		m_Property.m_SetAniSType		= pProperty.m_SetAniSType;

		m_Property.m_nBlend				= pProperty.m_nBlend;
		
		StringCchCopy(m_Property.m_szTraceUP,	STRACE_NSIZE, szIMAGE[pProperty.m_dwTraceIndexUP]);
		StringCchCopy(m_Property.m_szTraceDOWN,	STRACE_NSIZE, szIMAGE[pProperty.m_dwTraceIndexDOWN]);
		m_Property.m_vColorUPTemp			= pProperty.m_vColorUPTemp;
		m_Property.m_vColorDOWNTemp			= pProperty.m_vColorDOWNTemp;
		StringCchCopy( m_Property.m_szTexture, TEXTURE_LENGTH, pProperty.m_szTexture ); 

		//	Blur 삭제 버그로 인한 A/S
		std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
		std::string::size_type idx = strSrcName.find("effskin");
		if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// 일치하는 것이 있다.

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x101 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		EFFCHAR_PROPERTY_BLUR_101	m_Property101;
		SFile.ReadBuffer ( &m_Property101, sizeof(EFFCHAR_PROPERTY_BLUR_101) );

		m_Property.m_nBlend				= m_Property101.m_nBlend;
		
		StringCchCopy(m_Property.m_szTraceUP,	STRACE_NSIZE, szIMAGE[m_Property101.m_dwTraceIndexUP]);
		StringCchCopy(m_Property.m_szTraceDOWN,	STRACE_NSIZE, szIMAGE[m_Property101.m_dwTraceIndexDOWN]);
		m_Property.m_vColorUPTemp			= m_Property101.m_vColorUPTemp;
		m_Property.m_vColorDOWNTemp			= m_Property101.m_vColorDOWNTemp;
		StringCchCopy( m_Property.m_szTexture, TEXTURE_LENGTH, m_Property101.m_szTexture ); 

		//	Ver.102 에 추가 된 것들
		m_Property.m_SetAniMType		= AN_ATTACK;
		m_Property.m_SetAniSType		= AN_SUB_NONE;

		//	Blur 삭제 버그로 인한 A/S
		std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
		std::string::size_type idx = strSrcName.find("effskin");
		if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// 일치하는 것이 있다.

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else if ( dwVer == 0x100 )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		EFFCHAR_PROPERTY_BLUR_100	m_Property100;
		SFile.ReadBuffer ( &m_Property100, sizeof(EFFCHAR_PROPERTY_BLUR_100) );

		m_Property.m_nBlend				= m_Property100.m_nBlend;
		StringCchCopy(m_Property.m_szTraceUP,	STRACE_NSIZE, szIMAGE[m_Property100.m_dwTraceIndexUP]);
		StringCchCopy(m_Property.m_szTraceDOWN,	STRACE_NSIZE, szIMAGE[m_Property100.m_dwTraceIndexDOWN]);
		m_Property.m_vColorUPTemp			= m_Property100.m_vColorUPTemp;
		m_Property.m_vColorDOWNTemp			= m_Property100.m_vColorDOWNTemp;
		StringCchCopy( m_Property.m_szTexture, TEXTURE_LENGTH, m_Property100.m_szTexture ); 

		//	Ver.102 에 추가 된 것들
		m_Property.m_SetAniMType		= AN_ATTACK;
		m_Property.m_SetAniSType		= AN_SUB_NONE;

		//	Blur 삭제 버그로 인한 A/S
		std::string strSrcName = ((DxEffChar*)this)->GetFileName(); // "effskin"
		std::string::size_type idx = strSrcName.find("effskin");
		if ( idx != std::string::npos )		m_dwFlag |= USEALLACTION;	// 일치하는 것이 있다.

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		GASSERT ( 0 && "DxEffCharBlur에서 파일을 제대로 읽지 못하였습니다." );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharBlur::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

