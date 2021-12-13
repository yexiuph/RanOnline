// Terrain0.cpp: implementation of the DxEffectWater class.
//
//	UPDATE [02.07.10] : ���� �� �ڵ� ����ȭ �۾�.
//	UPDATE [02.07.19] : �Ķ��Ÿ Ȯ�� (m_fWaveCycle), tu, tv �� ���κ�����
//					���� �޽� ũ�⿡ �°� ������.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./Collision.h"

#include "./DxEffectWater.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectWater::D3DWATERVERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectWater::TYPEID = DEF_EFFECT_WATER;
const DWORD	DxEffectWater::VERSION = 0x00000101;
const char DxEffectWater::NAME[] = "[ �� ȿ�� 1 ]";
const DWORD DxEffectWater::FLAG = _EFF_REPLACE_AFTER;

LPDIRECT3DSTATEBLOCK9	DxEffectWater::m_pSavedStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectWater::m_pDrawStateBlock = NULL;

void DxEffectWater::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(WATER_PROPERTY_101);
	dwVer = VERSION;
}

void DxEffectWater::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(WATER_PROPERTY_101) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	
	if ( dwVer==0x00000100 && dwSize==sizeof(WATER_PROPERTY_100))
	{
		WATER_PROPERTY_100 Property100;
		memcpy ( &Property100, pProp, dwSize );

		m_Property.m_col			= Property100.m_col;
		m_Property.m_d3dFillMode	= Property100.m_d3dFillMode;
		m_Property.m_Depth			= Property100.m_Depth;
		m_Property.m_DiffuseColor	= Property100.m_DiffuseColor;
		m_Property.m_fWaveCycle		= Property100.m_fWaveCycle;
		m_Property.m_HeightChange	= Property100.m_HeightChange;
		m_Property.m_row			= Property100.m_row;
		m_Property.m_SizeX			= Property100.m_SizeX;
		m_Property.m_SizeZ			= Property100.m_SizeZ;
		m_Property.m_Velocity		= Property100.m_Velocity;

		StringCchCopy( m_Property.m_szTexture, MAX_PATH, Property100.m_szTexture );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectWater::DxEffectWater () :
	m_d3dFillMode(D3DFILL_SOLID),
	m_col(10),
	m_row(10),
	m_fWaveCycle(3.0f),
	m_Depth(1.75f),
	m_Velocity(2.5f),
	m_HeightChange(0.08f),
	m_DiffuseColor(D3DCOLOR_COLORVALUE(0.7f,0.7f,0.7f,0.8f)),
	m_CellSizeX(10.0f),
	m_CellSizeZ(10.0f),
	m_SizeX(100.0f),
	m_SizeZ(100.0f),
	m_distance(0.0f),
	m_pActiveHeightArray(NULL),
	m_pddsTexture(NULL),
	m_pVB(NULL),
	m_pIB(NULL),
	m_dwNumIndices(NULL),
	m_dwNumVertices(NULL)
{
	StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water3" );
	UseAffineMatrix ();
	D3DXMatrixIdentity ( &m_matFrameComb );
}

DxEffectWater::~DxEffectWater ()
{
	CleanUp();

	//SAFE_DELETE_ARRAY(m_szAdaptFrame);
}

void DxEffectWater::SetTexture ( const char* szName )
{
	//	Note : ���� �ؽ��İ� �ε�Ǿ� ���� ��� ����.
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	//	Note : �ؽ����� �б� ������ �����Ѵ�.
	StringCchCopy( m_szTexture, MAX_PATH, szName );
	TextureManager::LoadTexture ( m_szTexture, m_pd3dDevice, m_pddsTexture, 0, 0 );
}

//	Note : DxFrame �� ȿ���� ���� ��쿡 ���ȴ�.
//
HRESULT DxEffectWater::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	m_pAdaptFrame = pFrame;
	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectWater::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	m_pAdaptFrame = NULL;
	m_matFrameComb = pFrame->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectWater::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	m_dwNumVertices = (m_col+1)*(m_row+1);
	m_pActiveHeightArray = new float[m_dwNumVertices];
	if ( m_pActiveHeightArray == NULL )		return ERROR_OUTOFMEMORY;

	memset( m_pActiveHeightArray, 0, sizeof(float)*m_dwNumVertices );

	return hr;
}

//	Note: ���ؽ��� �ε��� ���۸� ����, ���� ����Ÿ �Է�
//
HRESULT DxEffectWater::CreateVertex ()
{
	HRESULT hr = S_OK;

	//	Note: +1���Ǵ� ������ �ϳ��� ������ �ΰ��� �������� �̷�����⿡ 
	//
	m_dwNumVertices = (m_col+1)*(m_row+1);
	m_dwNumIndices = m_col*m_row*6;

	//	Note: ���ؽ� ���� ���� 
	//
	hr = m_pd3dDevice->CreateVertexBuffer ( m_dwNumVertices*sizeof(D3DWATERVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,
									D3DWATERVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );
	if( FAILED(hr) )	return hr;

	D3DWATERVERTEX *pVertices;
	hr = m_pVB->Lock ( 0, 0, (VOID**)&pVertices, NULL );
	if( FAILED(hr) )	return hr;

	for ( int z = 0 ; z <= m_row ; z++ )
	for ( int x = 0 ; x <= m_col ; x++ )
	{
		//	Note: �޸� ��ǥ ���� ������ ����� ���Ͽ��� ����
		//
		pVertices->vertex.x = (float)-m_CellSizeX*m_col*0.5f + m_CellSizeX*x ;
		pVertices->vertex.y = 0.0f;
		pVertices->vertex.z = (float)-m_CellSizeZ*m_row*0.5f + m_CellSizeZ*z;
			  
		pVertices->tu = (float)x/(float)m_row;	//�ϳ��� �ؽ��ĸ� �÷��� ��ü�� ����
		pVertices->tv = (float)z/(float)m_col;	//�ϳ��� �ؽ��ĸ� �÷��� ��ü�� ���� 
		pVertices->diffuse = m_DiffuseColor;
	
		pVertices++;
	}

	m_pVB->Unlock();

	//	Note: �ε��� ���� ����
	//
	WORD *pIndices;
	hr = m_pd3dDevice->CreateIndexBuffer ( m_dwNumIndices*sizeof(WORD), D3DUSAGE_WRITEONLY,
						D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( FAILED(hr) )	return hr;

	hr = m_pIB->Lock ( 0, m_dwNumIndices*sizeof(WORD), (VOID**)&pIndices, 0 );
	if( FAILED(hr) )	return hr;

	//	Note: �ε��� ���� �ε��� 
	//		�� �κ��� �޸𸮸� �＼���ϱ� ���Ͽ��� �ϴ� x, y �������� .
	//		��, x + (���� ������ +1 )*y �� ���� ���� �ȴ� .	
	//		�ε��� ���۴� ������ ������ �ð� �������� �ǰ� �ؾ� �Ѵ�. 
	for( DWORD z1 = 0 ; z1 < (DWORD)m_row ; z1++ )
	for( DWORD x1 = 0 ; x1 < (DWORD)m_col ; x1++ )
	{
		DWORD vtx = x1 + z1*(m_col+1);
		*pIndices++ = (WORD)(vtx+0);
		*pIndices++ = (WORD)(vtx+(m_col+1));
		*pIndices++ = (WORD)(vtx+1);
		*pIndices++ = (WORD)(vtx+1);
		*pIndices++ = (WORD)(vtx+0+(m_col+1));
		*pIndices++ = (WORD)(vtx+1+(m_col+1));                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
	}

	m_pIB->Unlock();

	return hr;
}

HRESULT DxEffectWater::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pDrawStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectWater::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pDrawStateBlock );

	return S_OK;
}

HRESULT DxEffectWater::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

	//	Note : �ؽ����� �б� ������ �����Ѵ�.
	//
	TextureManager::LoadTexture ( m_szTexture, m_pd3dDevice, m_pddsTexture, 0, 0 );

	hr = CreateVertex();
	if( FAILED(hr) )	return hr;

	memset(&m_Material, 0, sizeof(D3DMATERIALQ));
	m_Material.Diffuse.r = 1.0f;
	m_Material.Diffuse.g = 1.0f;
	m_Material.Diffuse.b = 1.0f;
	m_Material.Ambient = m_Material.Specular = m_Material.Diffuse;

	return TRUE;
}

HRESULT DxEffectWater::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectWater::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectWater::DeleteDeviceObjects ()
{
	HRESULT hr  = S_OK;

	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );

	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return hr;
}

HRESULT DxEffectWater::FinalCleanup ()
{
	SAFE_DELETE_ARRAY(m_pActiveHeightArray);
	return S_OK;
}

//	Note : ������ ���� ������ �ʿ� ���� �κ��� �����ϰ�
//		������ ���� �������� ���� �氨�� ���� ���� �ܼ��ϰ� �����Ѵ�.
//
HRESULT DxEffectWater::FrameMove ( float fTime, float fElapsedTime )
{
	m_distance += fElapsedTime*(m_Velocity*2.0f);

	return S_OK;
}

//	Note : ������ ���� ������ ����ϴ� �κ� 
//		�̺κ��� Water ���� ���̵��� ���� �κ��� �����ϱ� �ٶ� 
//
inline float DxEffectWater::CalculateDisplacement ( float Height )
{
	float angle = (float) atan ( Height );
	float beamangle = (float) asin ( sin(angle)/1.333f );	//���̶� 1.33f���ȴ�.
	return (float) ( tan (beamangle) * (Height*m_Depth) );
}

//	Note : ���̿� ���� u, v��ǥ�� ���ϴ� �κ��̴� 
//		normal���͸� �ִ� �κ��̴�.
//
HRESULT DxEffectWater::ApplyHeightArrayToVB ()
{	
	HRESULT hr = S_OK;
	D3DWATERVERTEX *pVertices;

	int xplus1,  zplus1;
	int x, z;

	hr = m_pVB->Lock ( 0, 0, (VOID**)&pVertices, NULL );
	if( FAILED(hr) )	return hr;

	float fSize = max(m_SizeX,m_SizeZ);
	float fTermSizeX, fTermSizeZ;

	fTermSizeX = m_SizeX / m_col / fSize;
	fTermSizeZ = m_SizeZ / m_row / fSize;

	for ( x = 0 ; x <= m_col ; x++ )
	for ( z = 0 ; z <= m_row ; z++ )
	{			
		pVertices[z*(m_col+1)+x].vertex.y = m_pActiveHeightArray[(z*(m_col+1))+x];
		
		//---------- u, v��ǥ�� �����ϴ� �κ� --------------//		
		// ��ǥ ���������̸� �˾Ƴ��� �κ� 
		// x, z������ y �����ν� tu, tv ���� �����Ѵ�.
		float x_diff = ( x == m_col) ? 0 : m_pActiveHeightArray[z*(m_col+1)+x+1] - m_pActiveHeightArray[z*(m_col+1)+x];
		float z_diff = ( z == m_row) ? 0 : m_pActiveHeightArray[(z+1)*(m_col+1)+x] - m_pActiveHeightArray[z*(m_col+1)+x];

		float x_disp = CalculateDisplacement( x_diff );
		float z_disp = CalculateDisplacement( z_diff );

		float tu, tv;

		if( x_diff < 0 )
		{
			if( z_diff < 0 )
			{
				tu = (float)x*fTermSizeX - x_disp;
				tv = (float)z*fTermSizeZ - z_disp;
			}
			else
			{
				tu = (float)x*fTermSizeX - x_disp;
				tv = (float)z*fTermSizeZ + z_disp;
			}
		}
		else
		{
			if( z_diff < 0 )
			{
				tu = (float)x*fTermSizeX + x_disp;
				tv = (float)z*fTermSizeZ - z_disp;
			}
			else
			{
				tu = (float)x*fTermSizeX + x_disp;
				tv = (float)z*fTermSizeZ + z_disp;
			}
		}

		if ( tu < 0.0f ) tu = 0.0f;
		if ( tu > 1.0f ) tu = 1.0f;
		if ( tv < 0.0f ) tv = 0.0f;
		if ( tv > 1.0f ) tv = 1.0f;

		//	Note : �ؽ��� U,V ��.
		//
		pVertices[z*(m_col+1) + x].tu = tu * 2.0f;
		pVertices[z*(m_col+1) + x].tv = tv * 2.0f;

	 	//	Note : �븻 ���� ���.
		//
		xplus1 = (x == m_col) ? m_col-1 : x+1;
		zplus1 = (z == m_row) ? m_row-1 : z+1;

		D3DXVECTOR3 &v1 = pVertices[z*(m_col+1)+x].vertex;
		D3DXVECTOR3 &v2 = pVertices[z*(m_col+1)+xplus1].vertex;
		D3DXVECTOR3 &v3 = pVertices[zplus1*(m_col+1)+x].vertex;

		D3DXVECTOR3 vNorm1 = v2 - v1;
		D3DXVECTOR3 vNorm2 = v3 - v1;

		D3DXVec3Cross ( &vNorm1, &vNorm2, &vNorm1 );
		D3DXVec3Normalize ( &vNorm1, &vNorm1 );

		//	Note : �븻 ����.
		//
		pVertices[z*(m_col+1)+x].normal = vNorm1;
	}

	m_pVB->Unlock();

	return hr;
}	

HRESULT DxEffectWater::Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectWater");

	HRESULT hr = S_OK;

	//	Note: water���� ���� �߿��� �κ����� ���ϸ��̼ǰ� ������ ����� �ڵ�.
	// 
	//	Note : �ð� �κ��� �� �ִٰ� �ٽ� ó���ϵ��� �Ѵ�.
	//	������ �ð� �������� ���ϸ��̼� �Ǵ� ���� �����ؾ� �Ѵ�.
	//
	float angle = 0.0f;
	float temp1 = 0.0f;

	int xminus = 0;
	int zminus = 0;

	//	Note : ���ϸ��̼��� �ٽ� ó������ �����ִ� �κ� 
	//
	if ( m_distance > D3DX_PI*2 )	m_distance -= D3DX_PI*2;
	
	//	Note : angle = 0.0f;
	//
	int x, z;
	float fTermX = D3DX_PI * m_fWaveCycle / (float) m_col;
	float fTermZ = D3DX_PI * m_fWaveCycle / (float) m_row;
	for ( z = 0 ; z <= m_row ; z++ )
	for ( x = 0 ; x <= m_col ; x++ )
	{
		float fX = x * fTermX;
		float fZ = z * fTermZ;

		m_pActiveHeightArray[z*(m_col+1)+x] = cosf ( fX+m_distance+D3DX_PI * sinf(fZ) ) * m_HeightChange;
	}										

	//	Note : ���̰��� ���� tu, tv�� ����
	//
	hr = ApplyHeightArrayToVB ();
	if (FAILED(hr))		return hr;


	//	Note : Ʈ������ ����.
	//
	D3DXMATRIXA16 matCombined;
	if ( pFrame )	m_matFrameComb = pFrame->matCombined;
	D3DXMatrixMultiply ( &matCombined, m_pmatLocal, &m_matFrameComb );

	hr = m_pd3dDevice->SetTransform ( D3DTS_WORLD, &matCombined );
	if (FAILED(hr))		return hr;

	//	Note : ���� ���� ���.
	m_pSavedStateBlock->Capture();

	//	Note : ������ ���� ����.
	m_pDrawStateBlock->Apply();

	pd3dDevice->SetMaterial ( &m_Material );
	m_pd3dDevice->SetTexture ( 0, m_pddsTexture );

	m_pd3dDevice->SetIndices ( m_pIB );
	m_pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(D3DWATERVERTEX) );
	m_pd3dDevice->SetFVF ( D3DWATERVERTEX::FVF );

	m_pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );	

    //	Note : �������·� ����.
	m_pSavedStateBlock->Apply();

	PROFILE_END("DxEffectWater");

	return S_OK;
}

HRESULT DxEffectWater::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pSrcEffect);
	DxEffectWater *pEffWaterSrc = (DxEffectWater*)pSrcEffect;

	m_matFrameComb = pEffWaterSrc->m_pAdaptFrame->matCombined;

	return S_OK;
}

void DxEffectWater::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax.x = m_CellSizeX*m_col*0.5f;
	vMax.y = m_HeightChange;
	vMax.z = m_CellSizeZ*m_row*0.5f;

	vMin = -vMax;

	D3DXMATRIXA16 matCombined;
	D3DXMatrixMultiply ( &matCombined, m_pmatLocal, &m_matFrameComb );

	COLLISION::TransformAABB ( vMax, vMin, matCombined );
}

BOOL DxEffectWater::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXMATRIXA16 matCombined;
	D3DXMatrixMultiply ( &matCombined, m_pmatLocal, &m_matFrameComb );

	D3DXVECTOR3 vCenter;
	vCenter.x = matCombined._41;
	vCenter.y = matCombined._42;
	vCenter.z = matCombined._43;

	return COLLISION::IsWithInPoint(vDivMax,vDivMin,vCenter);
}
