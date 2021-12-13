#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./Collision.h"
#include "./DxEffectMan.h"

#include "./DxEffectWater2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectWater2::D3DWATER2VERTEX::FVF = D3DFVF_XYZ| D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 ;
const DWORD DxEffectWater2::TYPEID = DEF_EFFECT_WATER2; //Note: DxEffectDefine.h에 정의 되어 있다.
const DWORD DxEffectWater2::VERSION = 0x00000100;									  	
const char DxEffectWater2::NAME[] = "[ 물 효과 2 ]";
const DWORD DxEffectWater2::FLAG = _EFF_REPLACE_AFTER;


DWORD	DxEffectWater2::m_dwEffReflect;

char DxEffectWater2::m_strEffReflect[] =
{
	"vs.1.0	;\n" //

	//;transform position
	//;----------------------------------------------------------------------------
	//;	Vertex 를 뿌린다.~~~~~ㅋㅋ
	"m4x4 oPos, v0, c22	;\n"

	//;----------------------------------------------------------------------------
	//;	Vertex 를 World 좌표로 이동 시킨다.

	// 버텍스 좌표를 World 좌표로.
	"m4x3 r0, v0, c10	;\n"
	"mov r0.w, v0.w		;\n"

	//;----------------------------------------------------------------------------
	//		노멀벡터를 단위 벡터로.		---		r9 에 등록

	// 노멀벡터를 가져온다.
	"m3x3 r9, v3, c10	;\n"

	// 단위 벡터로 만들어 준다.
	"dp3 r9.w, r9, r9	\n"	//
	"rsq r9.w, r9.w		\n"	//
	"mul r9, r9, r9.w	\n"	//

	// 카메라 부터 버텍스까지의 단위 벡터를 얻는다.
	"sub r1, r0, c8		\n"	// 거리 계산.
	"dp3 r3.w, r1, r1	\n"	// 단위 벡터 만들기 시작.
	"rsq r3.w, r3.w		\n"	//
	"mul r5, r1, r3.w	\n"	//

	"mov oD0.xyz, c28.zzz		\n"
	"mov oD0.w, c29.x		\n"

	//	Texture 설정.
	//
	"dp3 r4, r5, r9				\n"	// r4 = r3 (내적) 노멀벡터r9
	"add r4, r4, r4				\n"	// r4 = 2 * r4
	"mad oT0.xyz, r9, -r4, r5	\n"	// r3 - ( r4 ) * r9
	"mov oT0.w, c28.z			\n"	// c1.x = 1.0f
	
};

LPDIRECT3DSTATEBLOCK9	DxEffectWater2::m_pSavedStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectWater2::m_pDrawStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectWater2::m_pReflectSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectWater2::m_pReflectSB = NULL;

void DxEffectWater2::GetProperty( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp  = (PBYTE) &m_Property;
	dwSize = sizeof(WATER2_PROPERTY);
	dwVer  = VERSION;
}

void DxEffectWater2::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if( dwVer == VERSION && dwSize == sizeof(WATER2_PROPERTY) )
	{
		memcpy( &m_Property, pProp, dwSize );
	}

}


DxEffectWater2::DxEffectWater2(void) :
	m_d3dFillMode( D3DFILL_SOLID ),
	m_nCol( 10 ),
	m_nRow( 10 ),
	m_fSize( 10.0f ),
	m_pddsTexture( NULL ),
	m_pWaterFactor( NULL ),
    m_fWaveCycle( 9.0f ),
    m_fDepth( 1.75f ),
    m_tu( 0.0f ),
	m_tv( 0.0f ),
	m_fRefractionRate( 0.01f ), //굴정률 반영 비율 설정 변수 
	m_fHeightMax( 0.5f ),
	m_fHeightMin( -0.2f ),
	m_fTextureMoveVelocity( 0.001f ),
	m_fHeightSpeed(1.0f),
	m_vTextureDirection( D3DXVECTOR3( 1.0f, 0.0f, 1.0f ) ),
	m_bCreate( FALSE ),
	m_clrDiffuseMin( D3DXCOLOR( 0.3f, 0.3f, 0.8f, 0.5f ) ),
	m_clrDiffuseMax( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) ),
	m_pVB( NULL ),
	m_pIB( NULL ),
	m_dwNumIndices( NULL ),
	m_dwNumVertices( NULL ),
	m_fElapsedTimed( 0.0f ),	
	m_bReflectionMode( FALSE ),
	m_pCubeTexture(NULL),								// 반사
	m_nPlayType( 0 ),
	m_fReflectionAlpha( 0.3f )
{
	//Note: 기본 텍스쳐 셋
	StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water3" );
	
	
	StringCchCopy( m_szEnvTexture1, MAX_PATH, "_Env_lobbyxpos.bmp" );	// 반사
	StringCchCopy( m_szEnvTexture2, MAX_PATH, "_Env_lobbyxneg.bmp" );	// 반사
	StringCchCopy( m_szEnvTexture3, MAX_PATH, "_Env_lobbyypos.bmp" );	// 반사
	StringCchCopy( m_szEnvTexture4, MAX_PATH, "_Env_lobbyyneg.bmp" );	// 반사
	StringCchCopy( m_szEnvTexture5, MAX_PATH, "_Env_lobbyzpos.bmp" );	// 반사
	StringCchCopy( m_szEnvTexture6, MAX_PATH, "_Env_lobbyzneg.bmp" );	// 반사

	UseAffineMatrix();
	D3DXMatrixIdentity ( &m_matFrameComb );

	
}

DxEffectWater2::~DxEffectWater2(void)
{
	CleanUp();
}


//Note: 텍스쳐 셋하는 함수 
void DxEffectWater2::SetTexture( const char *filename )
{
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	m_pddsTexture = NULL;
	StringCchCopy( m_szTexture, MAX_PATH, filename );
	TextureManager::LoadTexture( m_szTexture, m_pd3dDevice, m_pddsTexture, 0, 0 );
}


//Note: 모든 메모리와 디바이스 관련 해제하는 부분
//      이 부분은 CleanUp()과 내용이 같다.
//	DxEffectBase()
//		+------- CleanUp()
//					+------ InvalidateDeviceObjects()
//					+------ DeleteDeviceObjects()
//					+------ FinalCleanUp()       

HRESULT DxEffectWater2::AdaptToDxFrame( DxFrame *pFrame, LPDIRECT3DDEVICEQ	pd3dDevice )
{
	if( !pFrame ) return S_OK;

	m_pAdaptFrame				= pFrame;
	m_matFrameComb				= pFrame->matCombined;

	this->pLocalFrameEffNext	= pFrame->pEffect;
	pFrame->pEffect = this;

	GASSERT( pFrame->szName );
	if( pFrame->szName )
	{
		SAFE_DELETE_ARRAY( m_szAdaptFrame );
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[ nStrLen ];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectWater2::AdaptToEffList( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pFrame );

	m_pAdaptFrame = NULL;
	m_matFrameComb = pFrame->matCombined;

	if( pFrame->szName )
	{
		SAFE_DELETE_ARRAY( m_szAdaptFrame );
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[ nStrLen ];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}


HRESULT DxEffectWater2::OneTimeSceneInit()
{
	HRESULT hr = S_OK;

	m_dwNumVertices = (m_nCol+1)*(m_nRow+1); 
	m_dwNumIndices = m_nCol*m_nRow*6;
	m_pWaterFactor = new WATERFACTOR[ m_dwNumVertices ];
	if( m_pWaterFactor == NULL ) return ERROR_OUTOFMEMORY;

	memset( m_pWaterFactor, 0, sizeof(m_pWaterFactor)*m_dwNumVertices );

	return hr;
}

HRESULT DxEffectWater2::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

	//Note: 텍스쳐 
	TextureManager::LoadTexture( m_szTexture, m_pd3dDevice, m_pddsTexture, 0, 0 );

	hr = Init( pd3dDevice, m_nRow, m_nCol, m_nPlayType, m_vTextureDirection );
	if( FAILED(hr) ) return hr;

	memset( &m_Material, 0, sizeof(D3DMATERIALQ));
	m_Material.Diffuse.r = 1.0f;
	m_Material.Diffuse.g = 1.0f;
	m_Material.Diffuse.b = 1.0f;
	m_Material.Ambient   = m_Material.Specular = m_Material.Diffuse;

	//// Note	: Shader 에서 쓰이는 Cube Texture 만들기				// 반사
	////
	//const char* szSkyTex[6] =
	//{
	//	m_szEnvTexture1, m_szEnvTexture2, m_szEnvTexture3,
	//	m_szEnvTexture4, m_szEnvTexture5, m_szEnvTexture6
	//};

	//TextureManager::LoadCubeTexture ( szSkyTex, pd3dDevice, m_pCubeTexture, 0, 0 );


	return S_OK;
}

HRESULT DxEffectWater2::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LPD3DXBUFFER pCode = NULL; 

	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),		//D3DVSDE_POSITION,  0  
	//	D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		//D3DVSDE_
	//	D3DVSD_REG(5, D3DVSDT_D3DCOLOR ),		//D3DVSDE_
	//	D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		//D3DVSDE_
	//	D3DVSD_END()
	//};

	//hr = D3DXAssembleShader( m_strEffReflect, (UINT)strlen( m_strEffReflect ), 0, NULL, &pCode, NULL );
	//if( FAILED(hr) )	return E_FAIL;

	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)pCode->GetBufferPointer(), &m_dwEffReflect, 
	//											DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Water2 FAILED" );
	//	return E_FAIL;
	//}

	//SAFE_RELEASE(pCode);



	for( UINT which = 0 ; which < 2 ; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//pd3dDevice->SetRenderState( D3DRS_FILLMODE, m_d3dFillMode );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); //diffuse값을 이용해서 알파블렌딩의 농도를 결정하게 된다, 

		if( which == 0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else				pd3dDevice->EndStateBlock( &m_pDrawStateBlock );
	}

	for( UINT which = 0 ; which < 2 ; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//pd3dDevice->SetRenderState( D3DRS_FILLMODE, m_d3dFillMode );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE ); //diffuse값을 조정함으로써 어느정도 색상을 바꿀수 있도록 한다.
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); //diffuse값을 이용해서 알파블렌딩의 농도를 결정하게 된다, 

		if( which == 0 )	pd3dDevice->EndStateBlock( &m_pReflectSavedSB );
		else				pd3dDevice->EndStateBlock( &m_pReflectSB );
	}

	return S_OK;
}

HRESULT DxEffectWater2::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//if ( m_dwEffReflect )	pd3dDevice->DeleteVertexShader ( m_dwEffReflect );

	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pDrawStateBlock );

	SAFE_RELEASE( m_pReflectSavedSB );
	SAFE_RELEASE( m_pReflectSB );

	return S_OK;
}

HRESULT DxEffectWater2::DeleteDeviceObjects()
{
	HRESULT hr = S_OK;

	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );	

	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	////	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	////
	//if ( m_pCubeTexture )
	//	TextureManager::ReleaseTexture ( m_pCubeTexture );
	//m_pCubeTexture = NULL;	

	//m_pCubeTexture->Release ();
	
	return hr;
}


HRESULT DxEffectWater2::FinalCleanup()
{
	SAFE_DELETE_ARRAY( m_pWaterFactor );	

	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );

	return S_OK;
}

//Note: 정점의 싸이즈를 다시 셋하는 부분 
void DxEffectWater2::SetSize(float size)
{
	m_fSize = size;

	WATER2VERTEX *pVertices;
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVertices, NULL ) ) )
	{
		MessageBox( NULL, "버텍스 버퍼 락 실패", "error", MB_OK | MB_ICONERROR );
		return ;
	}

	for( int z = 0 ; z <= m_nRow ; z++ )
	{
		for( int x = 0 ; x <= m_nCol ; x++ )
		{
			//Note: 메모리 좌표 계산과 유사한 방법에 의하여서 계산됨 
			pVertices->vertex.x = (float)-size*m_nCol*0.5f + size*x ;			
			pVertices->vertex.z = (float)-size*m_nRow*0.5f + size*z;			 
			pVertices++;
		}
	}

	m_pVB->Unlock();
}

//Note: 초기화 함수 
HRESULT DxEffectWater2::Init( LPDIRECT3DDEVICEQ pd3dDevice, int nRow, int nCol, int nPlayType, D3DXVECTOR3 vDirection )
{
	//Note: +1씩되는 이유는 하나의 선분은 두개의 정점으로 이루어지기에 
	m_nCol = nCol;
	m_nRow = nRow;	
	m_pd3dDevice = pd3dDevice;
	m_vTextureDirection = vDirection;


	//Note: 버텍스 버퍼 생성 
	SAFE_RELEASE ( m_pVB );
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices*sizeof(D3DWATER2VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
													D3DWATER2VERTEX::FVF,  D3DPOOL_SYSTEMMEM, &m_pVB, NULL ) ))
	{
		MessageBox( NULL, "버텍스 버퍼 생성 실패", "error", MB_OK | MB_ICONERROR );
		return E_FAIL;
	}

	WATER2VERTEX* pVertices, *temp;
	
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVertices, NULL ) ) )
	{
		MessageBox( NULL, "버텍스 버퍼 락 실패", "error", MB_OK | MB_ICONERROR );
		return E_FAIL;
	}

	temp = pVertices;

	for( int z = 0 ; z <= m_nRow ; z++ )
	{
		for( int x = 0 ; x <= m_nCol ; x++ )
		{
			//Note: 메모리 좌표 계산과 유사한 방법에 의하여서 계산됨 
			pVertices->vertex.x = (float)-m_fSize*m_nCol*0.5f + m_fSize*x ;
			pVertices->vertex.y = 0.0f;
			pVertices->vertex.z = (float)-m_fSize*m_nRow*0.5f + m_fSize*z;
			  
			pVertices->tu = (float)x/(float)m_nRow; //하나의 텍스쳐를 늘려서 전체에 입힘
			pVertices->tv = (float)z/(float)m_nCol; //하나의 텍스쳐를 늘려서 전체에 입힘 
			pVertices->diffuse = D3DCOLOR_RGBA( 255, 255, 255, 255 );		
			
			m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition = pVertices->vertex;
			m_pWaterFactor[ x + z*(m_nCol+1) ].vVelocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );			
			m_pWaterFactor[ x + z*(m_nCol+1) ].tu0 =  pVertices->tu;
			m_pWaterFactor[ x + z*(m_nCol+1) ].tv0 =  pVertices->tv;				
			pVertices++;
		}
	}	

	SetNormal( temp ) ;
	m_pVB->Unlock();

	//Note: 물의 종류에 따라서 속도를 제어하는 부분 

	switch( nPlayType )
	{
		case 0:
				float fTempX, fX; 
				
				fTempX = D3DX_PI*((m_fWaveCycle)/(float)(m_nCol*m_nRow));
				for( int z = 0 ; z <= m_nRow ; z++ )
				{
					for( int x = 0 ; x <= m_nCol ; x++ )
					{
						fX = (x + z*(m_nCol+1))*fTempX;
						m_pWaterFactor[ x + z*(m_nCol+1)].vVelocity.y = sinf(fX)*m_fHeightSpeed + (float)rand()/(float)RAND_MAX*0.25f;
					}
				}
				break;

				//Note: 모이는 물결
		case 1:
				float xr, zr, rr;
				for( int z = 0 ; z <= m_nRow ; z++ )
				{
					for( int x = 0 ; x <= m_nCol ; x++ )
					{
						xr = (float)fabs( m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition.x - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.x );
						zr = (float)fabs( m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition.z - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.z );
						rr = sqrtf( xr*xr + zr*zr );
						 
						//Note: 0.01f를 곱해주는 이유는 상태적인 속도를 맞춰주기 위해서..
						m_pWaterFactor[ x + z*(m_nCol + 1) ].vVelocity.y = rr*m_fHeightSpeed*0.01f; //0.05f정도, 
					}
				}
				break;

				//Note: 퍼지는 물결
		case 2 :
				float xrr, zrr, rrr;
				float fMaxLength = (float)fabs( m_pWaterFactor[0].vPosition.x - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.x );

				for( int z = 0 ; z <= m_nRow ; z++ )
				{
					for( int x = 0 ; x <= m_nCol ; x++ )
					{
						xrr = (float)fabs( m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition.x - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.x );
						zrr = (float)fabs( m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition.z - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.z );
						xrr = fMaxLength - xrr;
						zrr = fMaxLength - zrr;

						rrr = sqrtf( xrr*xrr + zrr*zrr );
						 
						//Note: 0.01f를 곱해주는 이유는 상태적인 속도를 맞춰주기 위해서..
						m_pWaterFactor[ x + z*(m_nCol + 1) ].vVelocity.y = rrr*m_fHeightSpeed*0.01f; //0.05f정도, 
					}
				}

				break;

				//속도 보간 부분인데.. 별차이가 없어서 삭제 (참고)
				/*int zminus, zplus, xminus, xplus;
				float fHeight;
				for( z = 0 ; z <= m_nRow ; z++ )
				{
					for( int x = 0 ; x <= m_nCol ; x++ )
					{
						zminus = (z-1) < 0 ? z : z-1;
						zplus  = (z+1) > m_nRow ? m_nRow : z+1 ;
						xminus = (x-1) < 0 ? x : x-1;
						xplus  = (x+1) > m_nCol ? m_nCol : x+1;

						fHeight = ( m_pWaterFactor[ z*(m_nCol+1) + xminus ].vVelocity.y 
									+ m_pWaterFactor[ z*(m_nCol+1) + xplus ].vVelocity.y
									+ m_pWaterFactor[ zminus*(m_nCol+1) + x ].vVelocity.y 
									+ m_pWaterFactor[ zplus*(m_nCol+1) + x ].vVelocity.y )*0.25f;
						m_pWaterFactor[ x + z*(m_nCol + 1) ].vVelocity.y = fHeight;
					}
				}
				*/
				

	}

	//Note: 인덱스 버퍼 생성
	WORD *pIndices;
	SAFE_RELEASE ( m_pIB );
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( m_dwNumIndices*sizeof(WORD), D3DUSAGE_WRITEONLY,
						D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL )))
	{
		MessageBox( NULL, "인데스 버퍼 생성 에러", "인덱스 버퍼 에러", MB_OK );
		return E_FAIL;
	}

	if( FAILED( m_pIB->Lock( 0, m_dwNumIndices*sizeof(WORD), (VOID**)&pIndices, 0 ) ) )
	{
		MessageBox( NULL, "인덱스 버퍼 Lock 에러", "지형 에러", MB_OK );
		return E_FAIL;
	}
	
	for( DWORD z1 = 0 ; z1 < (DWORD)m_nRow ; z1++ )
	{
		for( DWORD x1 = 0 ; x1 < (DWORD)m_nCol ; x1++ )
		{
			DWORD vtx = x1 + z1*(m_nCol+1);
			*pIndices++ = (WORD)(vtx+0);
			*pIndices++ = (WORD)(vtx+(m_nCol+1));
			*pIndices++ = (WORD)(vtx+1);
			*pIndices++ = (WORD)(vtx+1);
			*pIndices++ = (WORD)(vtx+0+(m_nCol+1));
			*pIndices++ = (WORD)(vtx+1+(m_nCol+1));                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
		}

	}
	m_pIB->Unlock();	
	m_bCreate = TRUE;

	//Note: 물의 텍스쳐 방향 정하는 부분 	
	D3DXVec3Normalize( &m_vTextureDirection, &m_vTextureDirection );

	return S_OK;
}

//Note: 물의 굴절률 계산 하는 부분 
float DxEffectWater2::CalculateDisplacement( float Height )
{
	float angle = (float)atan( Height );
	float beamangle = (float)asin(sin(angle)/1.333f ); //물이라서 1.33f가된다.
	return (float)(tan(beamangle)*(Height+m_fDepth)); 	
}

//Note: m_fConstantHeight 변수에 따라서 전체 높이 값을 조정할수 있다.
HRESULT DxEffectWater2::FrameMove( float fTime, float fElapsedTime )
{		
	m_fElapsedTimed = fElapsedTime;	

	return S_OK;
}


//Note: 물효과 렌더링 옵션 적용 함수 이다. 
void DxEffectWater2::ApplyWaterRenderOption(void)
{
	if( !m_bCreate ) return ;

	int xplus, xminus, zplus, zminus;
	D3DXVECTOR3 n1, n2, n3, n4, n5, n6;
	D3DXVECTOR3 v1, v2,v3, v4, v5, v6, normal;
	WATER2VERTEX *pVertices;
	float		fRefraction;		   //Note: 높이에 의한 굴절값
	D3DXVECTOR3 vAcceleration; //Note: 가속도 변수 
	float		fDiffuseRate;		   //Note: 높이에 따른 diffuse 값 비율 
			
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVertices, NULL ) ) )
	{
		MessageBox( NULL, "버텍스 버퍼 락 실패", "error", MB_OK | MB_ICONERROR );
		return ;
	}
			
	for( int z = 0 ; z <= m_nRow ; z++ )
	{		
		
		for( int x = 0 ; x <= m_nCol ; x++ )
		{	
			//Note: 에니메이션을 하는 부분 //
			WATERFACTOR *pWaterFactor;
			pWaterFactor = &m_pWaterFactor[ x + z*( m_nCol + 1 ) ];
			pWaterFactor->vPosition += pWaterFactor->vVelocity * m_fElapsedTimed ;
			
			D3DXVECTOR3 vPosition;
			vPosition = pWaterFactor->vPosition ;

			//Note: 일단 Y값에 대한 것만 처리한다.
			if( (vPosition.y < m_fHeightMin ) || (vPosition.y > m_fHeightMax ) )
			{
				pWaterFactor->vPosition.y -= pWaterFactor->vVelocity.y*m_fElapsedTimed;				
				pWaterFactor->vVelocity.y = -1.0f*pWaterFactor->vVelocity.y;	
				pVertices[z*(m_nCol+1) + x].vertex.y = pWaterFactor->vPosition.y;								
			}
						
			pVertices[z*(m_nCol+1) + x].vertex = pWaterFactor->vPosition;			

			//Note: 굴절률 적용 부분 
			fRefraction = CalculateDisplacement( (float)fabs(pVertices[z*(m_nCol+1)+x].vertex.y) );

			//Note: 방향에 따른 텍스쳐의 이동 속도 설정 부분 
			//      물의 흐름은 방향만 있으면 되기에 Normalization을 해주어야 한다. 
			m_tu += -1.0f*m_vTextureDirection.x*( m_fTextureMoveVelocity*m_fElapsedTimed); 
			m_tv += -1.0f*m_vTextureDirection.z*( m_fTextureMoveVelocity*m_fElapsedTimed );

			//Note: 텍스쳐가 경계를 나가면 처리 부분 
			if( m_tu > 1.0f ) m_tu -= 1.0f;
			if( m_tu < -1.0f ) m_tu += 1.0f;
			if( m_tv > 1.0f ) m_tv -= 1.0f;
			if( m_tv < -1.0f ) m_tv += 1.0f;
			
			//Note: 텍스쳐의 tu, tv값의 로테이션 보정 및 굴정율 반영 부분  
			pVertices[z*(m_nCol+1) + x].tu = pWaterFactor->tu0 + m_tu + fRefraction*m_fRefractionRate;
			pVertices[z*(m_nCol+1) + x].tv = pWaterFactor->tv0 + m_tv + fRefraction*m_fRefractionRate;

			//Note: 높이에 따른 diffuse 값 설정
			fDiffuseRate = (( pWaterFactor->vPosition.y - m_fHeightMin ) / (float)fabs( m_fHeightMax - m_fHeightMin )) ;
			D3DXCOLOR clrDiffuse;
			D3DXColorLerp( &clrDiffuse, &m_clrDiffuseMin, &m_clrDiffuseMax, fDiffuseRate );
						
			pVertices[z*(m_nCol+1) + x].diffuse = clrDiffuse;	

									
			//Note: 고라운드 쉐이딩 부분 
			zminus = (z-1) < 0 ? z : z-1;
			zplus  = (z+1) > m_nRow ? m_nRow : z+1 ;
			xminus = (x-1) < 0 ? x : x-1;
			xplus  = (x+1) > m_nCol ? m_nCol : x+1;

			v1 = pVertices[ zminus*(m_nCol+1) + x ].vertex - pVertices[ z*(m_nCol+1) + x ].vertex;
			v2 = pVertices[ zminus*(m_nCol+1) + xplus ].vertex - pVertices[ z*(m_nCol+1) + x].vertex;
			v3 = pVertices[ z*(m_nCol+1) + xplus].vertex - pVertices[ z*(m_nCol+1) + x].vertex;
			v4 = pVertices[ zplus*(m_nCol+1) + x].vertex - pVertices[ z*(m_nCol+1) + x].vertex;
			v5 = pVertices[ zplus*(m_nCol+1) + xminus].vertex - pVertices[ z*(m_nCol+1) + x].vertex;
			v6 = pVertices[ z*(m_nCol+1) + xminus].vertex - pVertices[ z*(m_nCol+1) + x].vertex;

			D3DXVec3Cross( &n1, &v2, &v1 );	
			D3DXVec3Cross( &n2, &v3, &v2 );	
			D3DXVec3Cross( &n3, &v4, &v3 );	
			D3DXVec3Cross( &n4, &v5, &v4 );	
			D3DXVec3Cross( &n5, &v6, &v5 );	
            D3DXVec3Cross( &n6, &v1, &v6 );	

			normal =  n1+n2+n3+n4+n5+n6 ;
			D3DXVec3Normalize( &normal, &normal ); //이 정규화를 해주어야 만다 
			pVertices[z*(m_nCol+1) + x].normal = normal;			
		}
	}		
	
	m_pVB->Unlock();

}

//Note: 렌더링 시에 환경 매핑과 일반 매핑 모두 구분하여서 셋한다. 
HRESULT DxEffectWater2::Render( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectWater2");
	HRESULT hr = S_OK;

	//Note: 렌더링 옵션을 그대로 적용한다.
	ApplyWaterRenderOption(); 

	//Note: 트렌스 폼을 설정한다.
	//      오브젝트의 로컬좌표에서 월드 좌표로 변환시켜주는 부분이다.
	//      pFrame로 부터 월드에 위치할수 있는 메트릭스를 가져온다.
	D3DXMATRIXA16 matCombined, matIdentity;
	if( pFrame ) m_matFrameComb = pFrame->matCombined;
	D3DXMatrixMultiply( &matCombined, m_pmatLocal, &m_matFrameComb );
	hr = m_pd3dDevice->SetTransform( D3DTS_WORLD, &matCombined );
	if( FAILED(hr) )  return hr;	

	//Note: 이전 상태 백업
	m_pSavedStateBlock->Capture();
	m_pDrawStateBlock->Apply();

	pd3dDevice->SetMaterial( &m_Material );
	m_pd3dDevice->SetTexture( 0, m_pddsTexture );	

	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(WATER2VERTEX) );
	m_pd3dDevice->SetFVF( D3DWATER2VERTEX::FVF );	
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

	m_pSavedStateBlock->Apply();

	//이 부분은 다시 활성화 되어야 할것 같다.
	if( m_bReflectionMode )
	{
		/*

		//	Note : 허상 뿌린다.
		//
		D3DXVECTOR3	vMax, vMin, vCenter;
		vMax.x = m_nCol*m_fSize*0.5f;
		vMax.y = m_fHeightMax;
		vMax.z = m_nRow*m_fSize*0.5f;

		vMin.x = -vMax.x;
		vMin.y = m_fHeightMin;
		vMin.z = -vMax.z;

		vCenter = (vMax+vMin)*0.5f;
		D3DXVec3TransformCoord ( &vCenter, &vCenter, &matCombined );
		D3DXVec3TransformCoord ( &vMax, &vMax,	&matCombined );
		D3DXVec3TransformCoord ( &vMin, &vMin,	&matCombined );

		m_pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
		DxEffectMan::GetInstance().UpdateReflection ( pd3dDevice, pFrameMesh, pLandMan, vCenter, vMax, vMin );	//	반사

		//	Note : 매트릭스를 만든다.
		//
		D3DXMATRIX	matView, matProj, texScaleBiasMat, matDirect;

		//set special texture matrix for shadow mapping
		unsigned int range;
		float fOffsetX = 0.5f + (0.5f / (float)800);
		float fOffsetY = 0.5f + (0.5f / (float)600);

		//	H/W
		//	if ( DxEffectMan::GetInstance().GetUseZTexture() )
				range = 0xFFFFFFFF >> (32 - 24);//m_bitDepth)
		// S/W
		//	else if ( !(DxEffectMan::GetInstance().GetUseZTexture() ) )
		//		range = 0xFFFFFFFF >> (32 - 16);//m_bitDepth)

		float fBias    = -0.001f * (float)range;
		texScaleBiasMat._11 = 0.5f;		texScaleBiasMat._12 = 0.f;		texScaleBiasMat._13 = 0.f;			texScaleBiasMat._14 = 0.f;
		texScaleBiasMat._21 = 0.0f;		texScaleBiasMat._22 = -0.5f;	texScaleBiasMat._23 = 0.f;			texScaleBiasMat._24 = 0.f;
		texScaleBiasMat._31 = 0.0f;		texScaleBiasMat._32 = 0.f;		texScaleBiasMat._33 = (float)range;	texScaleBiasMat._34 = 0.f;
		texScaleBiasMat._41 = fOffsetX;	texScaleBiasMat._42 = fOffsetY;	texScaleBiasMat._43 = fBias;		texScaleBiasMat._44 = 1.f;

		pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
		pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );

		D3DXMatrixMultiply ( &matDirect, &m_matWorld,	&DxEffectMan::GetInstance().GetMatrix_ReflectView() );
		D3DXMatrixMultiply ( &matDirect, &matDirect,	&matProj );
		D3DXMatrixMultiply ( &matDirect, &matDirect,	&texScaleBiasMat);				// 반사

		//	Note : 뿌린다.
		//
		m_pd3dDevice->SetTransform ( D3DTS_WORLD, &matCombined );
		//m_pd3dDevice->GetTransform ( D3DTS_WORLD, &matWorld );
		//matTemp		= matWorld;
		//matTemp._42 -= 0.1f;
		//m_pd3dDevice->SetTransform ( D3DTS_WORLD, &matTemp );

		m_pd3dDevice->SetTexture( 0, DxEffectMan::GetInstance().GetReflectionTexture() );

		m_pReflectSavedSB->Capture();
		m_pReflectSB->Apply();

		m_pd3dDevice->SetIndices( m_pIB, 0 );
		m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(WATER2VERTEX) );
		m_pd3dDevice->SetFVF( m_dwEffReflect );
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

		m_pReflectSavedSB->Apply();

		//		m_pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

		//-------------------------------------------------------------------------------------------------------

		//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
		//
		D3DXMATRIX	matWorld, matView, matProj, matWVP;

		matView = DxViewPort::GetInstance().GetMatView();
		matProj = DxViewPort::GetInstance().GetMatProj();

		matCombined._42 += 0.03f;		// 약간 띠어 줌.. 나중에 문제 생길 수 있음
		D3DXMatrixTranspose( &matWorld, &matCombined );
		pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );

		D3DXMatrixMultiply ( &matWVP, &matCombined, &matView );
		D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
		D3DXMatrixTranspose( &matWVP, &matWVP );
		pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLDVIEWPROJ, &matWVP,	4 );

		m_pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+1,	&D3DXVECTOR4( m_fReflectionAlpha, 0.4f, 0.6f, 0.8f ),	1 );

		//Note: 이전 상태 백업
		m_pReflectSavedSB->Capture();
		m_pReflectSB->Apply();

		m_pd3dDevice->SetTexture( 0, m_pCubeTexture );

		m_pd3dDevice->SetIndices( m_pIB, 0 );
		m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(WATER2VERTEX) );
		m_pd3dDevice->SetFVF( m_dwEffReflect );	
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

		//Note: 이전 상태 복원
		m_pReflectSavedSB->Apply();
		*/
	}

	PROFILE_END( "DxEffectWater2" );	

	return S_OK;
}


HRESULT DxEffectWater2::CloneData( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pSrcEffect );
	DxEffectWater2 *pEffWaterSrc = (DxEffectWater2 *)pSrcEffect;

	m_matFrameComb = pEffWaterSrc->m_pAdaptFrame->matCombined;

	return S_OK;
}

void DxEffectWater2::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax.x = m_nCol*m_fSize*0.5f;
	vMax.y = m_fHeightMax;
	vMax.z = m_nRow*m_fSize*0.5f;

	vMin.x = -vMax.x;
	vMin.y = m_fHeightMin;
	vMin.z = -vMax.z;

	D3DXMATRIXA16	matCombined;
	D3DXMatrixMultiply( &matCombined, m_pmatLocal, &m_matFrameComb );

	COLLISION::TransformAABB( vMax, vMin, matCombined );
}


BOOL DxEffectWater2::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXMATRIXA16  matCombined;
	D3DXMatrixMultiply( &matCombined, m_pmatLocal, &m_matFrameComb );

	D3DXVECTOR3  vCenter;
	vCenter.x = matCombined._41;
	vCenter.y = matCombined._42;
	vCenter.z = matCombined._43;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}

//고라운드 쉐이딩 노말값 계산하여 주는 함수
void DxEffectWater2::SetNormal( WATER2VERTEX *pVertices )
{
	int xplus, xminus, yplus, yminus;
	D3DXVECTOR3 n1, n2, n3, n4, n5, n6;
	D3DXVECTOR3 v1, v2,v3, v4, v5, v6, normal;
	
	for( int y = 0 ; y <= m_nRow ; y++ )
	{
		for( int x = 0 ; x <= m_nCol ; x++ )
		{
			yminus = (y-1) < 0 ? y : y-1;
			yplus  = (y+1) > m_nRow ? m_nRow : y+1 ;
			xminus = (x-1) < 0 ? x : x-1;
			xplus  = (x+1) > m_nCol ? m_nCol : x+1;

			v1 = pVertices[ yminus*(m_nCol+1) + x ].vertex - pVertices[ y*(m_nCol+1) + x ].vertex;
			v2 = pVertices[ yminus*(m_nCol+1) + xplus ].vertex - pVertices[ y*(m_nCol+1) + x].vertex;
			v3 = pVertices[ y*(m_nCol+1) + xplus].vertex - pVertices[ y*(m_nCol+1) + x].vertex;
			v4 = pVertices[ yplus*(m_nCol+1) + x].vertex - pVertices[ y*(m_nCol+1) + x].vertex;
			v5 = pVertices[ yplus*(m_nCol+1) + xminus].vertex - pVertices[ y*(m_nCol+1) + x].vertex;
			v6 = pVertices[ y*(m_nCol+1) + xminus].vertex - pVertices[ y*(m_nCol+1) + x].vertex;

			D3DXVec3Cross( &n1, &v2, &v1 );	
			D3DXVec3Cross( &n2, &v3, &v2 );	
			D3DXVec3Cross( &n3, &v4, &v3 );	
			D3DXVec3Cross( &n4, &v5, &v4 );	
			D3DXVec3Cross( &n5, &v6, &v5 );	
            D3DXVec3Cross( &n6, &v1, &v6 );	

			normal =  n1+n2+n3+n4+n5+n6 ;
			D3DXVec3Normalize( &normal, &normal ); //이 정규화를 해주어야 만다 

			pVertices[y*(m_nCol+1) + x].normal = normal;
		}
	}
}

