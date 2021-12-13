#include "pch.h"

#include "DxLightMan.h"
#include "dxviewport.h"
#include "DxShadowMap.h"
#include "./TextureManager.h"
#include "./SerialFile.h"

#include "./GLDefine.h"
#include "./GLPeriod.h"
#include "dxweatherman.h"

#include "../[Lib]__EngineSound/Sources/DxSound/DxSoundLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//			비 																	//

const	float	DxEffectRain::RAIN_TURN_TIME	= 40.f;		// 40 초 안에 원하는 바뀐다.
const	float	DxEffectRain::RAIN_WIDTH_SCALE	= 0.15f;

const	DWORD	DxEffectRain::VERSION	=	0x103;
const	DWORD	DxEffectRain::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

DxEffectRain::DxEffectRain() :
	m_fTime(0.0f),
	m_vDirection(D3DXVECTOR3(0.0f,0.0f,0.0f)),
	m_dwFaces(1000),
	m_vColor(D3DCOLOR_ARGB(70,70,70,70)),
	m_fLength(35.0f),				// 빗줄기의 길이
	m_iRadius(600),			// 빗줄기가 떨어지는 최대 반경
	m_fLimit(-50.0f),			// 빗줄기가 떨어지는 깊이 제한
	m_iHeight(150),	
	m_fSpeed(0.8f),
	m_bPrevUse(FALSE),
	m_dwPlayFaces(0),
	m_emRainStats(EMWS_NULL),
	m_fApplyRate(0.f),

	m_pVB(NULL),
	m_pIB(NULL),
	m_pInforRain(NULL),
	m_pTexture(NULL)
{

}

DxEffectRain::~DxEffectRain()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	SAFE_DELETE_ARRAY ( m_pInforRain );
}

HRESULT DxEffectRain::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vDirec, float fPower )
{
	HRESULT hr(S_OK);

	D3DXVECTOR3	vWidth;
	D3DXVECTOR3	vUp ( 0.f, 1.f, 0.f );
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vDirection = DxViewPort::GetInstance().GetLookDir();

	D3DXVec3Cross ( &vWidth, &vUp, &vDirection );
	D3DXVec3Normalize ( &vWidth, &vWidth );
	vWidth = vWidth * RAIN_WIDTH_SCALE;

	m_vDirection = D3DXVECTOR3 ( vDirec.x*fPower, vDirec.y*fPower - 9.8f, vDirec.z*fPower )*m_fSpeed;
	D3DXVec3Normalize ( &m_vDirection, &m_vDirection );


	SAFE_DELETE_ARRAY ( m_pInforRain );
	m_pInforRain = new INFORMATION_RAIN[m_dwFaces];
	for ( DWORD i=0; i<m_dwFaces; i++ )
	{
		m_pInforRain[i].vPos.x = vLookatPt.x + ((RANDOM_POS-0.5f)*m_iRadius) + ((RANDOM_POS-0.5f)*m_iRadius);
		m_pInforRain[i].vPos.y = vLookatPt.y + ( rand()%(m_iHeight*2) );
		m_pInforRain[i].vPos.z = vLookatPt.z + ((RANDOM_POS-0.5f)*m_iRadius) + ((RANDOM_POS-0.5f)*m_iRadius);

		m_pInforRain[i].vDirect.x = RANDOM_POS-0.5f;
		m_pInforRain[i].vDirect.y = -RANDOM_POS;
		m_pInforRain[i].vDirect.z = RANDOM_POS-0.5f;
	}


	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( 4*m_dwFaces*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
	if ( !m_pVB )
	{
		CDebugSet::ToLogFile( "DxEffectRain::InitDeviceObjects() -- CreateVertexBuffer() -- Failed" );
		return S_OK;
	}

	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( 6*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if ( !m_pIB )	
	{
		CDebugSet::ToLogFile( "DxEffectRain::InitDeviceObjects() -- CreateIndexBuffer() -- Failed" );
		return S_OK;
	}

	VERTEX*		pVertex;
	hr = m_pVB->Lock( 0, 0, (VOID**)&pVertex, D3DLOCK_NOOVERWRITE );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectRain::InitDeviceObjects() -- m_pVB->Lock() -- Failed" );
		return E_FAIL;
	}

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		pVertex[(i*4)+0].vPos	= m_pInforRain[i].vPos - vWidth;
		pVertex[(i*4)+0].cColor	= 0x00000000;//m_vColor;
		pVertex[(i*4)+0].vTex	= D3DXVECTOR2 ( 0.f, 0.f );

		pVertex[(i*4)+1].vPos	= m_pInforRain[i].vPos - vWidth + ( m_vDirection * m_fLength );
		pVertex[(i*4)+1].cColor	= m_vColor;
		pVertex[(i*4)+1].vTex	= D3DXVECTOR2 ( 0.f, 1.f );

		pVertex[(i*4)+2].vPos	= m_pInforRain[i].vPos + vWidth;
		pVertex[(i*4)+2].cColor	= 0x00000000;//m_vColor;
		pVertex[(i*4)+2].vTex	= D3DXVECTOR2 ( 1.f, 0.f );

		pVertex[(i*4)+3].vPos	= m_pInforRain[i].vPos + vWidth + ( m_vDirection * m_fLength );
		pVertex[(i*4)+3].cColor	= m_vColor;
		pVertex[(i*4)+3].vTex	= D3DXVECTOR2 ( 0.f, 0.f );
	}
	m_pVB->Unlock ();


	WORD*	pIndices;
	hr = m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectRain::InitDeviceObjects() -- m_pIB->Lock() -- Failed" );
		return E_FAIL;
	}

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		pIndices[(i*6)+0] = (WORD)((i*4)+0);
		pIndices[(i*6)+1] = (WORD)((i*4)+1);
		pIndices[(i*6)+2] = (WORD)((i*4)+2);
		pIndices[(i*6)+3] = (WORD)((i*4)+1);
		pIndices[(i*6)+4] = (WORD)((i*4)+3);
		pIndices[(i*6)+5] = (WORD)((i*4)+2);
	}

	m_pIB->Unlock ();

	m_szRainTex = "Rain.dds";
	TextureManager::LoadTexture ( m_szRainTex.c_str(), pd3dDevice, m_pTexture, 0, 0 );

	//	비소리 사운드 초기화.
	DxSoundLib::GetInstance()->CreateSound ( "rain", "04000328.wav", MAP_SOUND, true );
	DxSoundLib::GetInstance()->CreateSound ( "thunder", "04000349.wav", MAP_SOUND );

	return S_OK;
}

HRESULT DxEffectRain::ResetMesh ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	D3DXVECTOR3	vWidth;
	D3DXVECTOR3	vUp ( 0.f, 1.f, 0.f );
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vDirection = DxViewPort::GetInstance().GetLookDir();

	D3DXVec3Cross ( &vWidth, &vUp, &vDirection );
	D3DXVec3Normalize ( &vWidth, &vWidth );
	vWidth = vWidth * RAIN_WIDTH_SCALE;

	SAFE_DELETE_ARRAY ( m_pInforRain );
	m_pInforRain = new INFORMATION_RAIN[m_dwFaces];
	for ( DWORD i=0; i<m_dwFaces; i++ )
	{
		m_pInforRain[i].vPos.x = vLookatPt.x + ((RANDOM_POS-0.5f)*m_iRadius) + ((RANDOM_POS-0.5f)*m_iRadius);
		m_pInforRain[i].vPos.y = vLookatPt.y + ( rand()%(m_iHeight*2) );
		m_pInforRain[i].vPos.z = vLookatPt.z + ((RANDOM_POS-0.5f)*m_iRadius) + ((RANDOM_POS-0.5f)*m_iRadius);

		m_pInforRain[i].vDirect.x = RANDOM_POS-0.5f;
		m_pInforRain[i].vDirect.y = -RANDOM_POS;
		m_pInforRain[i].vDirect.z = RANDOM_POS-0.5f;
	}

	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( 4*m_dwFaces*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
	if ( !m_pVB )
	{
		CDebugSet::ToLogFile( "DxEffectRain::ResetMesh() -- CreateVertexBuffer() -- Failed" );	
		return S_OK;
	}

	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( 6*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if ( !m_pIB )
	{
		CDebugSet::ToLogFile( "DxEffectRain::ResetMesh() -- CreateIndexBuffer() -- Failed" );	
		return S_OK;
	}

	VERTEX*		pVertex;
	hr = m_pVB->Lock( 0, 0, (VOID**)&pVertex, D3DLOCK_NOOVERWRITE );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectRain::ResetMesh() -- m_pVB->Lock() -- Failed" );
		return E_FAIL;
	}

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		pVertex[(i*4)+0].vPos	= m_pInforRain[i].vPos - vWidth;
		pVertex[(i*4)+0].cColor	= 0x00000000;//m_vColor;
		pVertex[(i*4)+0].vTex	= D3DXVECTOR2 ( 0.f, 0.f );

		pVertex[(i*4)+1].vPos	= m_pInforRain[i].vPos - vWidth + ( m_vDirection * m_fLength );
		pVertex[(i*4)+1].cColor	= m_vColor;
		pVertex[(i*4)+1].vTex	= D3DXVECTOR2 ( 0.f, 1.f );

		pVertex[(i*4)+2].vPos	= m_pInforRain[i].vPos + vWidth;
		pVertex[(i*4)+2].cColor	= 0x00000000;//m_vColor;
		pVertex[(i*4)+2].vTex	= D3DXVECTOR2 ( 1.f, 0.f );

		pVertex[(i*4)+3].vPos	= m_pInforRain[i].vPos + vWidth + ( m_vDirection * m_fLength );
		pVertex[(i*4)+3].cColor	= m_vColor;
		pVertex[(i*4)+3].vTex	= D3DXVECTOR2 ( 0.f, 0.f );
	}
	m_pVB->Unlock ();


	WORD*	pIndices;
	hr = m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectRain::ResetMesh() -- m_pIB->Lock() -- Failed" );
		return E_FAIL;
	}

	for( DWORD i=0; i<m_dwFaces; i++ )
	{
		pIndices[(i*6)+0] = (WORD)((i*4)+0);
		pIndices[(i*6)+1] = (WORD)((i*4)+1);
		pIndices[(i*6)+2] = (WORD)((i*4)+2);
		pIndices[(i*6)+3] = (WORD)((i*4)+1);
		pIndices[(i*6)+4] = (WORD)((i*4)+3);
		pIndices[(i*6)+5] = (WORD)((i*4)+2);
	}

	m_pIB->Unlock ();
	
	return S_OK;
}

HRESULT DxEffectRain::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectRain::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectRain::DeleteDeviceObjects ()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );

	SAFE_DELETE_ARRAY ( m_pInforRain );

	TextureManager::ReleaseTexture( m_szRainTex.c_str(), m_pTexture );

	//	사운드 해제
	DxSoundLib::GetInstance()->ReleaseSound ( "rain" );
	DxSoundLib::GetInstance()->ReleaseSound ( "thunder" );

	return S_OK;
}

HRESULT DxEffectRain::PlayThunder()
{
	DxSoundLib::GetInstance()->PlaySound ( "thunder" );
	return S_OK;
}

HRESULT DxEffectRain::StopSound ()
{
	DxSoundLib::GetInstance()->StopSound ( "rain" );
	DxSoundLib::GetInstance()->StopSound ( "thunder" );

	return S_OK;
}

HRESULT DxEffectRain::FrameMove ( float fElapsedTime, D3DXVECTOR3 vDirec, float fPower, BOOL bUse )
{
	HRESULT hr(S_OK);

	if ( !m_pVB )	return S_OK;

	m_fTime += fElapsedTime;

	//	Note : 상태 제어
	//
	if ( (m_emRainStats==EMWS_NULL) && bUse )									// 아무것도 아닌 상태에서 뿌리라는 명령을 내렸을 때
	{
		m_fTime = 0.f;
		m_emRainStats = EMWS_STARTING;
	}
	else if ( (m_emRainStats==EMWS_STARTING) && (m_fTime>=RAIN_TURN_TIME) )	// 비가 내리기 시작하는 중에, 준비 시간이 끝
	{
		m_fTime = 0.f;
		m_emRainStats = EMWS_PLAYING;
	}
	else if ( (m_emRainStats==EMWS_PLAYING) && !bUse )							// 비를 내리는 중에 그만 내리라는 통보를 받음
	{
		m_fTime = 0.f;
		m_emRainStats = EMWS_ENDING;
	}
	else if ( (m_emRainStats==EMWS_ENDING) && (m_fTime>=RAIN_TURN_TIME) )		// 비가 내리다가 완전히 그쳤을 경우
	{
		m_fTime = 0.f;
		m_emRainStats = EMWS_NULL;
	}
	else if ( m_emRainStats==EMWS_NULL )										// 비를 안뿌린다.
	{
		StopSound ();
		return S_OK;
	}

	//	NOTE
	//		비소리를 내라고 요청해야할 자리
	//		...Man()->("RAIN", PLAY);
	DxSoundLib::GetInstance()->PlaySound ( "rain" );

	float	fApplyRate = 0.f;
	if ( m_emRainStats==EMWS_STARTING )			m_fApplyRate = m_fTime/RAIN_TURN_TIME;
	else if ( m_emRainStats==EMWS_PLAYING )		m_fApplyRate = 1.f;
	else if ( m_emRainStats==EMWS_ENDING )		m_fApplyRate = 1.f-(m_fTime/RAIN_TURN_TIME);
	else										m_fApplyRate = 0.f;

	m_dwPlayFaces = (DWORD)(m_dwFaces*m_fApplyRate);		// 뿌려야 하는 비 갯수


	D3DXVECTOR3	vWidth;
	D3DXVECTOR3	vNorDirect;
	D3DXVECTOR3	vUp ( 0.f, 1.f, 0.f );
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt ();
	D3DXVECTOR3	vDirection = DxViewPort::GetInstance().GetLookDir();

	D3DXVec3Cross ( &vWidth, &vUp, &vDirection );
	D3DXVec3Normalize ( &vWidth, &vWidth );
	vWidth = vWidth * RAIN_WIDTH_SCALE;


	VERTEX*		pVertex;

	hr = m_pVB->Lock( 0, 0, (VOID**)&pVertex, 0L );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectRain::FrameMove() -- m_pVB->Lock() -- Failed" );
		return E_FAIL;
	}

	for( int i=0; i<((int)m_dwFaces); i++ )
	{
		m_vDirection = D3DXVECTOR3 ( vDirec.x*fPower, vDirec.y*fPower - 9.8f, vDirec.z*fPower )*m_fSpeed;
		m_vDirection += m_pInforRain[i].vDirect;
		D3DXVec3Normalize ( &vNorDirect, &m_vDirection );


		// 새로운 위치에 빗방울을 생성한다.
		if( m_pInforRain[i].vPos.y <= vLookatPt.y + m_fLimit )
		{
			//Radius = (rand() % 1000)/1000.f - 0.5f;			// -0.500 ~ 0.500 까지의 값이 나오게 해준다.
			//if( Radius == 0.000f )		Radius = 0.5f;

			m_pInforRain[i].vPos.x = vLookatPt.x + ((RANDOM_POS-0.5f)*m_iRadius) + ((RANDOM_POS-0.5f)*m_iRadius);//( rand()%m_iRadius ) - m_iRadius/2 - (m_iHeight * (1.0f - m_vDirection.x/(fPower*m_fSpeed)) );
			m_pInforRain[i].vPos.y = vLookatPt.y + ( rand()%m_iHeight ) + m_iHeight; //(m_iHeight * m_vWind.y);		
			m_pInforRain[i].vPos.z = vLookatPt.z + ((RANDOM_POS-0.5f)*m_iRadius) + ((RANDOM_POS-0.5f)*m_iRadius);//( rand()%m_iRadius ) - m_iRadius/2 - (m_iHeight * (1.0f - m_vDirection.z/(fPower*m_fSpeed)) );

			pVertex[(i*4)+0].vPos	= m_pInforRain[i].vPos - vWidth;
			pVertex[(i*4)+1].vPos	= m_pInforRain[i].vPos - vWidth + ( vNorDirect * m_fLength );
			pVertex[(i*4)+2].vPos	= m_pInforRain[i].vPos + vWidth;
			pVertex[(i*4)+3].vPos	= m_pInforRain[i].vPos + vWidth + ( vNorDirect * m_fLength );
		}
		else
		{
			m_pInforRain[i].vPos	+= m_vDirection;
			pVertex[(i*4)+0].vPos	= m_pInforRain[i].vPos - vWidth;
			pVertex[(i*4)+1].vPos	= m_pInforRain[i].vPos - vWidth + ( vNorDirect * m_fLength );
			pVertex[(i*4)+2].vPos	= m_pInforRain[i].vPos + vWidth;
			pVertex[(i*4)+3].vPos	= m_pInforRain[i].vPos + vWidth + ( vNorDirect * m_fLength );
		}
	}

	m_pVB->Unlock ();

	return S_OK;
}

HRESULT DxEffectRain::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pVB || !m_pIB || !m_emRainStats || !m_dwPlayFaces )		return S_OK;

	DWORD dwFVFSize;
	
	dwFVFSize = VERTEX::FVF;
	dwFVFSize = D3DXGetFVFVertexSize ( dwFVFSize );

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );

	pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	//	Note : Setup a material
	//
	D3DMATERIALQ mtrl;
	D3DUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.f );
	pd3dDevice->SetMaterial( &mtrl );

	//	Note : 이전 상태 백업.
	DWORD	dwCullMode, dwLighting;
	DWORD	dwAlphaEnable, dwSrcBlend, dwDestBlend;
	DWORD	dwColorARG1, dwColorARG2, dwColorOP;
	pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &dwLighting );

	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaEnable );
	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );

	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG1,	&dwColorARG1 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLORARG2,	&dwColorARG2 );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );
	{
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	}



	pd3dDevice->SetTexture ( 0, m_pTexture );

	pd3dDevice->SetIndices ( m_pIB );
	pd3dDevice->SetStreamSource ( 0, m_pVB, 0, dwFVFSize );
	pd3dDevice->SetFVF( VERTEX::FVF );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwPlayFaces*4, 0, m_dwPlayFaces*2 );




	pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, dwLighting );

	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaEnable );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	dwColorARG1 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	dwColorARG2 );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );



	return S_OK;
}

void	DxEffectRain::SaveSet ( CSerialFile &SFile )
{
	DWORD	dwSize = sizeof(RAIN_PROPERTY);
	SFile << (DWORD) VERSION;
	SFile << dwSize;

	SFile.WriteBuffer ( &m_Property, sizeof(RAIN_PROPERTY) );
}

void	DxEffectRain::LoadSet ( CSerialFile &SFile )
{
	DWORD	dwBuffSize;
	DWORD	dwVer;

	SFile >> dwVer;
	SFile >> dwBuffSize;

	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer ( &m_Property, sizeof(RAIN_PROPERTY) );
	}
	else if ( dwVer == 0x102 )
	{
		RAIN_PROPERTY_101	sProp;
		SFile.ReadBuffer ( &sProp, sizeof(RAIN_PROPERTY_101) );
		m_dwFaces = sProp.m_dwFaces;
		m_fLength = 35.f;//sProp.m_fLength;
		m_fLimit = sProp.m_fLimit;
		m_fSpeed = 0.8f;//sProp.m_fSpeed;
		m_iHeight = 150;//sProp.m_iHeight;
		m_iRadius = 600;//sProp.m_iRadius;
		m_vColor = D3DCOLOR_ARGB(70,70,70,70);
	}
	else if ( dwVer == 0x101 )
	{
		RAIN_PROPERTY_101	sProp;
		SFile.ReadBuffer ( &sProp, sizeof(RAIN_PROPERTY_101) );
		m_dwFaces = sProp.m_dwFaces;
		m_fLength = 35.f;//sProp.m_fLength;
		m_fLimit = sProp.m_fLimit;
		m_fSpeed = 0.8f;//sProp.m_fSpeed;
		m_iHeight = 150;//sProp.m_iHeight;
		m_iRadius = 600;//sProp.m_iRadius;
		m_vColor = D3DCOLOR_ARGB(70,70,70,70);
	}
	else
	{
		dwBuffSize -= sizeof(DWORD);
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
	}
}












//													
//
const DWORD		DxEffectCloud::CLOUDSHADOW::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
LPDIRECT3DSTATEBLOCK9	DxEffectCloud::m_pSavedCloudSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectCloud::m_pEffectCloudSB = NULL;

const	DWORD	DxEffectCloud::VERSION	=	0x100;

DxEffectCloud::DxEffectCloud() :
	m_bUse(FALSE),
	m_fWidth(20.f),
	m_pddsTexture(NULL)
{
	StringCchCopy( m_szTexture, MAX_PATH, "eff02.tga" );
}

DxEffectCloud::~DxEffectCloud()
{
}

HRESULT DxEffectCloud::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0 );

	for ( DWORD i=0; i<1; i++ )
	{
		m_CloudShadow[i*4+0].vPos = D3DXVECTOR3 ( -m_fWidth, 0.f, m_fWidth );
		m_CloudShadow[i*4+1].vPos = D3DXVECTOR3 ( m_fWidth, 0.f, m_fWidth );
		m_CloudShadow[i*4+2].vPos = D3DXVECTOR3 ( -m_fWidth, 0.f, -m_fWidth );
		m_CloudShadow[i*4+3].vPos = D3DXVECTOR3 ( m_fWidth, 0.f, -m_fWidth );

		m_CloudShadow[i*4+0].vColor = 0xffffffff;
		m_CloudShadow[i*4+1].vColor = 0xffffffff;
		m_CloudShadow[i*4+2].vColor = 0xffffffff;
		m_CloudShadow[i*4+3].vColor = 0xffffffff;

		m_CloudShadow[i*4+0].vTex = D3DXVECTOR2 ( 0.f, 0.f );
		m_CloudShadow[i*4+1].vTex = D3DXVECTOR2 ( 1.f, 0.f );
		m_CloudShadow[i*4+2].vTex = D3DXVECTOR2 ( 0.f, 1.f );
		m_CloudShadow[i*4+3].vTex = D3DXVECTOR2 ( 1.f, 1.f );
	}

	for ( DWORD i=0; i<1; i++ )
	{
		m_vCumulateTex[0]		= D3DXVECTOR2 ( 0.0f, 0.0f );
		m_vCumulateTex[1]		= D3DXVECTOR2 ( 0.0f, 0.0f );
		m_vCumulateTex[2]		= D3DXVECTOR2 ( 0.0f, 0.0f );
		m_vCumulateTex[3]		= D3DXVECTOR2 ( 0.0f, 0.0f );
	}

	return S_OK;
}

HRESULT DxEffectCloud::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE	);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE	);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE	);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE	);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedCloudSB );
		else			pd3dDevice->EndStateBlock( &m_pEffectCloudSB );
	}

	return S_OK;
}

HRESULT DxEffectCloud::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedCloudSB );
	SAFE_RELEASE( m_pEffectCloudSB );

	return S_OK;
}

HRESULT DxEffectCloud::DeleteDeviceObjects ()
{

	return S_OK;
}

HRESULT DxEffectCloud::FrameMove ( float fElapsedTime )
{
	if ( !m_bUse ) return S_OK;

	for ( DWORD i=0; i<1; i++ )
	{
		m_vCumulateTex[0]		+= D3DXVECTOR2 ( 0.01f*fElapsedTime, 0.01f*fElapsedTime );
		m_vCumulateTex[1]		+= D3DXVECTOR2 ( 0.01f*fElapsedTime, 0.02f*fElapsedTime );
		m_vCumulateTex[2]		+= D3DXVECTOR2 ( 0.02f*fElapsedTime, 0.01f*fElapsedTime );
		m_vCumulateTex[3]		+= D3DXVECTOR2 ( 0.02f*fElapsedTime, 0.02f*fElapsedTime );
	}
	
	return S_OK;
}

HRESULT DxEffectCloud::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bUse ) return S_OK;

	D3DXVECTOR2	m_vTemp;
	D3DXVECTOR2	m_vTemp2;
	D3DXVECTOR3 &LookatPt = DxViewPort::GetInstance().GetLookatPt ();

	D3DXMATRIX matView, matProj, matIdentity;

	D3DXMatrixIdentity ( &matIdentity );
	matIdentity._41 = LookatPt.x;
	matIdentity._42 = LookatPt.y + 200.f;
	matIdentity._43 = LookatPt.z;
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	m_pSavedCloudSB->Capture();
	m_pEffectCloudSB->Apply();

	pd3dDevice->SetTexture ( 0, m_pddsTexture );

	pd3dDevice->SetFVF ( CLOUDSHADOW::FVF );

	m_CloudShadow[0].vPos = D3DXVECTOR3 ( -2000.f, 0.f, -2000.f );
	m_CloudShadow[1].vPos = D3DXVECTOR3 ( 2000.f, 0.f, -2000.f );
	m_CloudShadow[2].vPos = D3DXVECTOR3 ( -2000.f, 0.f, 2000.f );
	m_CloudShadow[3].vPos = D3DXVECTOR3 ( 2000.f, 0.f, 2000.f );

	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, m_CloudShadow, sizeof(CLOUDSHADOW) );

	m_pSavedCloudSB->Apply();

	return S_OK;
}

void	DxEffectCloud::SaveSet ( CSerialFile &SFile )
{
	DWORD	dwSize = sizeof(CLOUD_PROPERTY);
	SFile << (DWORD) VERSION;
	SFile << dwSize;

	SFile.WriteBuffer ( &m_Property, sizeof(CLOUD_PROPERTY) );
}

void	DxEffectCloud::LoadSet ( CSerialFile &SFile )
{
	DWORD	dwBuffSize;
	DWORD	dwVer;

	SFile >> dwVer;
	SFile >> dwBuffSize;

	if ( dwVer == VERSION )
	{
		SFile.ReadBuffer ( &m_Property, sizeof(CLOUD_PROPERTY) );
	}
	else
	{
		dwBuffSize -= sizeof(DWORD);
		DWORD dwCur = SFile.GetfTell();
		SFile.SetOffSet ( dwCur+dwBuffSize );
	}
}

