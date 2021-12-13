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
const DWORD DxEffectWater2::TYPEID = DEF_EFFECT_WATER2; //Note: DxEffectDefine.h�� ���� �Ǿ� �ִ�.
const DWORD DxEffectWater2::VERSION = 0x00000100;									  	
const char DxEffectWater2::NAME[] = "[ �� ȿ�� 2 ]";
const DWORD DxEffectWater2::FLAG = _EFF_REPLACE_AFTER;


DWORD	DxEffectWater2::m_dwEffReflect;

char DxEffectWater2::m_strEffReflect[] =
{
	"vs.1.0	;\n" //

	//;transform position
	//;----------------------------------------------------------------------------
	//;	Vertex �� �Ѹ���.~~~~~����
	"m4x4 oPos, v0, c22	;\n"

	//;----------------------------------------------------------------------------
	//;	Vertex �� World ��ǥ�� �̵� ��Ų��.

	// ���ؽ� ��ǥ�� World ��ǥ��.
	"m4x3 r0, v0, c10	;\n"
	"mov r0.w, v0.w		;\n"

	//;----------------------------------------------------------------------------
	//		��ֺ��͸� ���� ���ͷ�.		---		r9 �� ���

	// ��ֺ��͸� �����´�.
	"m3x3 r9, v3, c10	;\n"

	// ���� ���ͷ� ����� �ش�.
	"dp3 r9.w, r9, r9	\n"	//
	"rsq r9.w, r9.w		\n"	//
	"mul r9, r9, r9.w	\n"	//

	// ī�޶� ���� ���ؽ������� ���� ���͸� ��´�.
	"sub r1, r0, c8		\n"	// �Ÿ� ���.
	"dp3 r3.w, r1, r1	\n"	// ���� ���� ����� ����.
	"rsq r3.w, r3.w		\n"	//
	"mul r5, r1, r3.w	\n"	//

	"mov oD0.xyz, c28.zzz		\n"
	"mov oD0.w, c29.x		\n"

	//	Texture ����.
	//
	"dp3 r4, r5, r9				\n"	// r4 = r3 (����) ��ֺ���r9
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
	m_fRefractionRate( 0.01f ), //������ �ݿ� ���� ���� ���� 
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
	m_pCubeTexture(NULL),								// �ݻ�
	m_nPlayType( 0 ),
	m_fReflectionAlpha( 0.3f )
{
	//Note: �⺻ �ؽ��� ��
	StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water3" );
	
	
	StringCchCopy( m_szEnvTexture1, MAX_PATH, "_Env_lobbyxpos.bmp" );	// �ݻ�
	StringCchCopy( m_szEnvTexture2, MAX_PATH, "_Env_lobbyxneg.bmp" );	// �ݻ�
	StringCchCopy( m_szEnvTexture3, MAX_PATH, "_Env_lobbyypos.bmp" );	// �ݻ�
	StringCchCopy( m_szEnvTexture4, MAX_PATH, "_Env_lobbyyneg.bmp" );	// �ݻ�
	StringCchCopy( m_szEnvTexture5, MAX_PATH, "_Env_lobbyzpos.bmp" );	// �ݻ�
	StringCchCopy( m_szEnvTexture6, MAX_PATH, "_Env_lobbyzneg.bmp" );	// �ݻ�

	UseAffineMatrix();
	D3DXMatrixIdentity ( &m_matFrameComb );

	
}

DxEffectWater2::~DxEffectWater2(void)
{
	CleanUp();
}


//Note: �ؽ��� ���ϴ� �Լ� 
void DxEffectWater2::SetTexture( const char *filename )
{
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	m_pddsTexture = NULL;
	StringCchCopy( m_szTexture, MAX_PATH, filename );
	TextureManager::LoadTexture( m_szTexture, m_pd3dDevice, m_pddsTexture, 0, 0 );
}


//Note: ��� �޸𸮿� ����̽� ���� �����ϴ� �κ�
//      �� �κ��� CleanUp()�� ������ ����.
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

	//Note: �ؽ��� 
	TextureManager::LoadTexture( m_szTexture, m_pd3dDevice, m_pddsTexture, 0, 0 );

	hr = Init( pd3dDevice, m_nRow, m_nCol, m_nPlayType, m_vTextureDirection );
	if( FAILED(hr) ) return hr;

	memset( &m_Material, 0, sizeof(D3DMATERIALQ));
	m_Material.Diffuse.r = 1.0f;
	m_Material.Diffuse.g = 1.0f;
	m_Material.Diffuse.b = 1.0f;
	m_Material.Ambient   = m_Material.Specular = m_Material.Diffuse;

	//// Note	: Shader ���� ���̴� Cube Texture �����				// �ݻ�
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
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); //diffuse���� �̿��ؼ� ���ĺ����� �󵵸� �����ϰ� �ȴ�, 

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

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE ); //diffuse���� ���������ν� ������� ������ �ٲܼ� �ֵ��� �Ѵ�.
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE ); //diffuse���� �̿��ؼ� ���ĺ����� �󵵸� �����ϰ� �ȴ�, 

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

	////	Note : ���� �ؽ��İ� �ε�Ǿ� ���� ��� ����.
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

//Note: ������ ����� �ٽ� ���ϴ� �κ� 
void DxEffectWater2::SetSize(float size)
{
	m_fSize = size;

	WATER2VERTEX *pVertices;
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVertices, NULL ) ) )
	{
		MessageBox( NULL, "���ؽ� ���� �� ����", "error", MB_OK | MB_ICONERROR );
		return ;
	}

	for( int z = 0 ; z <= m_nRow ; z++ )
	{
		for( int x = 0 ; x <= m_nCol ; x++ )
		{
			//Note: �޸� ��ǥ ���� ������ ����� ���Ͽ��� ���� 
			pVertices->vertex.x = (float)-size*m_nCol*0.5f + size*x ;			
			pVertices->vertex.z = (float)-size*m_nRow*0.5f + size*z;			 
			pVertices++;
		}
	}

	m_pVB->Unlock();
}

//Note: �ʱ�ȭ �Լ� 
HRESULT DxEffectWater2::Init( LPDIRECT3DDEVICEQ pd3dDevice, int nRow, int nCol, int nPlayType, D3DXVECTOR3 vDirection )
{
	//Note: +1���Ǵ� ������ �ϳ��� ������ �ΰ��� �������� �̷�����⿡ 
	m_nCol = nCol;
	m_nRow = nRow;	
	m_pd3dDevice = pd3dDevice;
	m_vTextureDirection = vDirection;


	//Note: ���ؽ� ���� ���� 
	SAFE_RELEASE ( m_pVB );
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_dwNumVertices*sizeof(D3DWATER2VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
													D3DWATER2VERTEX::FVF,  D3DPOOL_SYSTEMMEM, &m_pVB, NULL ) ))
	{
		MessageBox( NULL, "���ؽ� ���� ���� ����", "error", MB_OK | MB_ICONERROR );
		return E_FAIL;
	}

	WATER2VERTEX* pVertices, *temp;
	
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVertices, NULL ) ) )
	{
		MessageBox( NULL, "���ؽ� ���� �� ����", "error", MB_OK | MB_ICONERROR );
		return E_FAIL;
	}

	temp = pVertices;

	for( int z = 0 ; z <= m_nRow ; z++ )
	{
		for( int x = 0 ; x <= m_nCol ; x++ )
		{
			//Note: �޸� ��ǥ ���� ������ ����� ���Ͽ��� ���� 
			pVertices->vertex.x = (float)-m_fSize*m_nCol*0.5f + m_fSize*x ;
			pVertices->vertex.y = 0.0f;
			pVertices->vertex.z = (float)-m_fSize*m_nRow*0.5f + m_fSize*z;
			  
			pVertices->tu = (float)x/(float)m_nRow; //�ϳ��� �ؽ��ĸ� �÷��� ��ü�� ����
			pVertices->tv = (float)z/(float)m_nCol; //�ϳ��� �ؽ��ĸ� �÷��� ��ü�� ���� 
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

	//Note: ���� ������ ���� �ӵ��� �����ϴ� �κ� 

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

				//Note: ���̴� ����
		case 1:
				float xr, zr, rr;
				for( int z = 0 ; z <= m_nRow ; z++ )
				{
					for( int x = 0 ; x <= m_nCol ; x++ )
					{
						xr = (float)fabs( m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition.x - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.x );
						zr = (float)fabs( m_pWaterFactor[ x + z*(m_nCol+1) ].vPosition.z - m_pWaterFactor[m_nCol/2 + m_nRow/2*(m_nCol+1)].vPosition.z );
						rr = sqrtf( xr*xr + zr*zr );
						 
						//Note: 0.01f�� �����ִ� ������ �������� �ӵ��� �����ֱ� ���ؼ�..
						m_pWaterFactor[ x + z*(m_nCol + 1) ].vVelocity.y = rr*m_fHeightSpeed*0.01f; //0.05f����, 
					}
				}
				break;

				//Note: ������ ����
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
						 
						//Note: 0.01f�� �����ִ� ������ �������� �ӵ��� �����ֱ� ���ؼ�..
						m_pWaterFactor[ x + z*(m_nCol + 1) ].vVelocity.y = rrr*m_fHeightSpeed*0.01f; //0.05f����, 
					}
				}

				break;

				//�ӵ� ���� �κ��ε�.. �����̰� ��� ���� (����)
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

	//Note: �ε��� ���� ����
	WORD *pIndices;
	SAFE_RELEASE ( m_pIB );
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( m_dwNumIndices*sizeof(WORD), D3DUSAGE_WRITEONLY,
						D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL )))
	{
		MessageBox( NULL, "�ε��� ���� ���� ����", "�ε��� ���� ����", MB_OK );
		return E_FAIL;
	}

	if( FAILED( m_pIB->Lock( 0, m_dwNumIndices*sizeof(WORD), (VOID**)&pIndices, 0 ) ) )
	{
		MessageBox( NULL, "�ε��� ���� Lock ����", "���� ����", MB_OK );
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

	//Note: ���� �ؽ��� ���� ���ϴ� �κ� 	
	D3DXVec3Normalize( &m_vTextureDirection, &m_vTextureDirection );

	return S_OK;
}

//Note: ���� ������ ��� �ϴ� �κ� 
float DxEffectWater2::CalculateDisplacement( float Height )
{
	float angle = (float)atan( Height );
	float beamangle = (float)asin(sin(angle)/1.333f ); //���̶� 1.33f���ȴ�.
	return (float)(tan(beamangle)*(Height+m_fDepth)); 	
}

//Note: m_fConstantHeight ������ ���� ��ü ���� ���� �����Ҽ� �ִ�.
HRESULT DxEffectWater2::FrameMove( float fTime, float fElapsedTime )
{		
	m_fElapsedTimed = fElapsedTime;	

	return S_OK;
}


//Note: ��ȿ�� ������ �ɼ� ���� �Լ� �̴�. 
void DxEffectWater2::ApplyWaterRenderOption(void)
{
	if( !m_bCreate ) return ;

	int xplus, xminus, zplus, zminus;
	D3DXVECTOR3 n1, n2, n3, n4, n5, n6;
	D3DXVECTOR3 v1, v2,v3, v4, v5, v6, normal;
	WATER2VERTEX *pVertices;
	float		fRefraction;		   //Note: ���̿� ���� ������
	D3DXVECTOR3 vAcceleration; //Note: ���ӵ� ���� 
	float		fDiffuseRate;		   //Note: ���̿� ���� diffuse �� ���� 
			
	if( FAILED( m_pVB->Lock( 0, 0, (VOID**)&pVertices, NULL ) ) )
	{
		MessageBox( NULL, "���ؽ� ���� �� ����", "error", MB_OK | MB_ICONERROR );
		return ;
	}
			
	for( int z = 0 ; z <= m_nRow ; z++ )
	{		
		
		for( int x = 0 ; x <= m_nCol ; x++ )
		{	
			//Note: ���ϸ��̼��� �ϴ� �κ� //
			WATERFACTOR *pWaterFactor;
			pWaterFactor = &m_pWaterFactor[ x + z*( m_nCol + 1 ) ];
			pWaterFactor->vPosition += pWaterFactor->vVelocity * m_fElapsedTimed ;
			
			D3DXVECTOR3 vPosition;
			vPosition = pWaterFactor->vPosition ;

			//Note: �ϴ� Y���� ���� �͸� ó���Ѵ�.
			if( (vPosition.y < m_fHeightMin ) || (vPosition.y > m_fHeightMax ) )
			{
				pWaterFactor->vPosition.y -= pWaterFactor->vVelocity.y*m_fElapsedTimed;				
				pWaterFactor->vVelocity.y = -1.0f*pWaterFactor->vVelocity.y;	
				pVertices[z*(m_nCol+1) + x].vertex.y = pWaterFactor->vPosition.y;								
			}
						
			pVertices[z*(m_nCol+1) + x].vertex = pWaterFactor->vPosition;			

			//Note: ������ ���� �κ� 
			fRefraction = CalculateDisplacement( (float)fabs(pVertices[z*(m_nCol+1)+x].vertex.y) );

			//Note: ���⿡ ���� �ؽ����� �̵� �ӵ� ���� �κ� 
			//      ���� �帧�� ���⸸ ������ �Ǳ⿡ Normalization�� ���־�� �Ѵ�. 
			m_tu += -1.0f*m_vTextureDirection.x*( m_fTextureMoveVelocity*m_fElapsedTimed); 
			m_tv += -1.0f*m_vTextureDirection.z*( m_fTextureMoveVelocity*m_fElapsedTimed );

			//Note: �ؽ��İ� ��踦 ������ ó�� �κ� 
			if( m_tu > 1.0f ) m_tu -= 1.0f;
			if( m_tu < -1.0f ) m_tu += 1.0f;
			if( m_tv > 1.0f ) m_tv -= 1.0f;
			if( m_tv < -1.0f ) m_tv += 1.0f;
			
			//Note: �ؽ����� tu, tv���� �����̼� ���� �� ������ �ݿ� �κ�  
			pVertices[z*(m_nCol+1) + x].tu = pWaterFactor->tu0 + m_tu + fRefraction*m_fRefractionRate;
			pVertices[z*(m_nCol+1) + x].tv = pWaterFactor->tv0 + m_tv + fRefraction*m_fRefractionRate;

			//Note: ���̿� ���� diffuse �� ����
			fDiffuseRate = (( pWaterFactor->vPosition.y - m_fHeightMin ) / (float)fabs( m_fHeightMax - m_fHeightMin )) ;
			D3DXCOLOR clrDiffuse;
			D3DXColorLerp( &clrDiffuse, &m_clrDiffuseMin, &m_clrDiffuseMax, fDiffuseRate );
						
			pVertices[z*(m_nCol+1) + x].diffuse = clrDiffuse;	

									
			//Note: ����� ���̵� �κ� 
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
			D3DXVec3Normalize( &normal, &normal ); //�� ����ȭ�� ���־�� ���� 
			pVertices[z*(m_nCol+1) + x].normal = normal;			
		}
	}		
	
	m_pVB->Unlock();

}

//Note: ������ �ÿ� ȯ�� ���ΰ� �Ϲ� ���� ��� �����Ͽ��� ���Ѵ�. 
HRESULT DxEffectWater2::Render( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectWater2");
	HRESULT hr = S_OK;

	//Note: ������ �ɼ��� �״�� �����Ѵ�.
	ApplyWaterRenderOption(); 

	//Note: Ʈ���� ���� �����Ѵ�.
	//      ������Ʈ�� ������ǥ���� ���� ��ǥ�� ��ȯ�����ִ� �κ��̴�.
	//      pFrame�� ���� ���忡 ��ġ�Ҽ� �ִ� ��Ʈ������ �����´�.
	D3DXMATRIXA16 matCombined, matIdentity;
	if( pFrame ) m_matFrameComb = pFrame->matCombined;
	D3DXMatrixMultiply( &matCombined, m_pmatLocal, &m_matFrameComb );
	hr = m_pd3dDevice->SetTransform( D3DTS_WORLD, &matCombined );
	if( FAILED(hr) )  return hr;	

	//Note: ���� ���� ���
	m_pSavedStateBlock->Capture();
	m_pDrawStateBlock->Apply();

	pd3dDevice->SetMaterial( &m_Material );
	m_pd3dDevice->SetTexture( 0, m_pddsTexture );	

	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(WATER2VERTEX) );
	m_pd3dDevice->SetFVF( D3DWATER2VERTEX::FVF );	
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

	m_pSavedStateBlock->Apply();

	//�� �κ��� �ٽ� Ȱ��ȭ �Ǿ�� �Ұ� ����.
	if( m_bReflectionMode )
	{
		/*

		//	Note : ��� �Ѹ���.
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
		DxEffectMan::GetInstance().UpdateReflection ( pd3dDevice, pFrameMesh, pLandMan, vCenter, vMax, vMin );	//	�ݻ�

		//	Note : ��Ʈ������ �����.
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
		D3DXMatrixMultiply ( &matDirect, &matDirect,	&texScaleBiasMat);				// �ݻ�

		//	Note : �Ѹ���.
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

		//	Note : SetFVF�� World�� WorldViewProjection ��� ���� �ֱ�
		//
		D3DXMATRIX	matWorld, matView, matProj, matWVP;

		matView = DxViewPort::GetInstance().GetMatView();
		matProj = DxViewPort::GetInstance().GetMatProj();

		matCombined._42 += 0.03f;		// �ణ ��� ��.. ���߿� ���� ���� �� ����
		D3DXMatrixTranspose( &matWorld, &matCombined );
		pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );

		D3DXMatrixMultiply ( &matWVP, &matCombined, &matView );
		D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
		D3DXMatrixTranspose( &matWVP, &matWVP );
		pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLDVIEWPROJ, &matWVP,	4 );

		m_pd3dDevice->SetVertexShaderConstant ( VSC_CUSTOM+1,	&D3DXVECTOR4( m_fReflectionAlpha, 0.4f, 0.6f, 0.8f ),	1 );

		//Note: ���� ���� ���
		m_pReflectSavedSB->Capture();
		m_pReflectSB->Apply();

		m_pd3dDevice->SetTexture( 0, m_pCubeTexture );

		m_pd3dDevice->SetIndices( m_pIB, 0 );
		m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(WATER2VERTEX) );
		m_pd3dDevice->SetFVF( m_dwEffReflect );	
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

		//Note: ���� ���� ����
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

//����� ���̵� �븻�� ����Ͽ� �ִ� �Լ�
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
			D3DXVec3Normalize( &normal, &normal ); //�� ����ȭ�� ���־�� ���� 

			pVertices[y*(m_nCol+1) + x].normal = normal;
		}
	}
}

