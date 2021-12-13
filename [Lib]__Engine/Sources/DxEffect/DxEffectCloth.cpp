// DxEffectCloth.cpp: implementation of the DxEffectBlur class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./editmeshs.h"
#include "./DxRenderStates.h"
#include "./DxDynamicVB.h"
#include "./SerialFile.h"

#include "./DxEffectCloth.h"

#define DAMPING		2.0f

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const DWORD DxEffectCloth::TYPEID = DEF_EFFECT_CLOTH;
const DWORD	DxEffectCloth::VERSION = 0x00000104;
const char DxEffectCloth::NAME[] = "[ 옷,천막 효과 ]";
const DWORD DxEffectCloth::FLAG = _EFF_REPLACE;

void DxEffectCloth::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(CLOTH_PROPERTY);
	dwVer = VERSION;
}

void DxEffectCloth::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( ( dwVer==VERSION && dwSize==sizeof(CLOTH_PROPERTY) ) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if( ( ( dwVer==0x103 || dwVer==0x102) && dwSize==sizeof(CLOTH_PROPERTY_102_103) ) )
	{
		CLOTH_PROPERTY_102_103	sProp;
		memcpy ( &sProp, pProp, dwSize );

		m_bPlay				= sProp.m_bPlay;
		m_iNodeCount		= sProp.m_iNodeCount;
		m_fCoEfficient		= sProp.m_fCoEfficient;			//탄성 계수    
		m_vWindDirection	= sProp.m_vWindDirection;		//바람의 방향 벡터 
		m_fWindForce		= sProp.m_fWindForce;			//바람의 세기 	
		m_fGravity			= sProp.m_fGravity;				//중력 부분	
		StringCchCopy( m_szTexture, MAX_PATH, sProp.m_szTexture );

		m_bEachRender = TRUE;
	}
	else if ( ( dwVer==0x101 && dwSize==sizeof(CLOTH_PROPERTY_101) ) )
	{
		CLOTH_PROPERTY_101	Prop101;
		memcpy ( &Prop101, pProp, dwSize );

		m_bPlay = Prop101.m_bPlay;
		m_iNodeCount = Prop101.m_iNodeCount;
		m_fCoEfficient = Prop101.m_fCoEfficient;
		m_vWindDirection = Prop101.m_vWindDirection;
		m_vWindDirection = D3DXVECTOR3(1.f,0.f,1.f);
		m_fWindForce = Prop101.m_fWindForce;
		m_fWindForce = 1.5f;
		m_fGravity = Prop101.m_fGravity;
		StringCchCopy( m_szTexture, MAX_PATH, Prop101.m_szTexture );

		m_bEachRender = TRUE;
	}
	else if (  dwVer==0x101 && dwSize==sizeof(CLOTH_PROPERTY_100) )	//	프로퍼티는 0x100, 0x101 이 동일하다.
	{
		CLOTH_PROPERTY_100	Prop100;
		memcpy ( &Prop100, pProp, dwSize );

		m_bPlay = Prop100.m_bPlay;
		m_iNodeCount = Prop100.m_iNodeCount;
		m_fCoEfficient = Prop100.m_fCoEfficient;
		m_vWindDirection = Prop100.m_vWindDirection;
		m_vWindDirection = D3DXVECTOR3(1.f,0.f,1.f);
		m_fWindForce = Prop100.m_fWindForce;
		m_fWindForce = 1.5f;
		m_fGravity = Prop100.m_fGravity;
		StringCchCopy( m_szTexture, MAX_PATH, Prop100.m_szTexture );

		m_bEachRender = TRUE;
	}
	else if (  dwVer==0x100 && dwSize==sizeof(CLOTH_PROPERTY_100) )	//	프로퍼티는 0x100, 0x101 이 동일하다.
	{
		CLOTH_PROPERTY_100	Prop100;
		memcpy ( &Prop100, pProp, dwSize );

		m_bPlay = Prop100.m_bPlay;
		m_iNodeCount = Prop100.m_iNodeCount;
		m_fCoEfficient = Prop100.m_fCoEfficient;
		m_vWindDirection = Prop100.m_vWindDirection;
		m_vWindDirection = D3DXVECTOR3(1.f,0.f,1.f);
		m_fWindForce = Prop100.m_fWindForce;
		m_fWindForce = 1.5f;
		m_fGravity = Prop100.m_fGravity;
		StringCchCopy( m_szTexture, MAX_PATH, Prop100.m_szTexture );

		m_bEachRender = TRUE;
	}

	// Note : 실행.!@
	m_bPlay = true;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectCloth::DxEffectCloth() :
	m_bEachRender(TRUE),
	m_iNodeCount(0),
	m_fCoEfficient(400.0f),
	m_fWindConstant(1.0f),	
	m_vWindDirection(D3DXVECTOR3(1.0f, 0.0f, 1.0f)),
	m_fWindForce(3.0f),	
	m_fGravity(-3.81f),
	m_bAlphaMap(FALSE),
	m_pddsTexture(NULL),
	m_fTime(0.0f),	
	m_iWindMin(0),
	m_bPlay(true),
	m_dwNumVertices(0),
	m_dwNumIndices(0),
	m_dwNumLockIndex(0),
	m_pLockIndex(NULL),
	m_pNodeTable(NULL),
	m_pParticle(NULL),
	m_pTempLockIndex(NULL),
	m_pBaseVertArray(NULL),
	m_pBaseIndexArray(NULL),
	m_pVertArray(NULL),
	m_pIndexArray(NULL),
	m_pIB(NULL),
	m_fSumTime(0.f)
{
	StringCchCopy( m_szTexture, MAX_PATH, "_Wa_water1.bmp" );
	D3DXMatrixIdentity ( &m_matWorld );
}

DxEffectCloth::~DxEffectCloth()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY( m_pBaseVertArray );
	SAFE_DELETE_ARRAY( m_pBaseIndexArray );
	SAFE_DELETE_ARRAY( m_pVertArray );
	SAFE_DELETE_ARRAY( m_pIndexArray );
	SAFE_RELEASE( m_pIB );

	SAFE_DELETE_ARRAY ( m_pLockIndex );
	SAFE_DELETE_ARRAY ( m_pParticle );
	SAFE_DELETE_ARRAY ( m_pNodeTable );

	SAFE_DELETE_ARRAY ( m_pTempLockIndex );
}

HRESULT DxEffectCloth::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectCloth::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectCloth::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
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
				m_matWorld = pframeCur->matCombined;
	//			StringCchCopy( m_szTexture, MAX_PATH, (char*)pmsMeshs->strTextureFiles->GetBuffer() );

				XFileLoad ( pd3dDevice, pmsMeshs->m_pLocalMesh );
			}
			else
			{
				BaseBuffToDynamicBuff ( pd3dDevice );
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

HRESULT DxEffectCloth::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectCloth::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
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

HRESULT DxEffectCloth::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pddsTexture, 0, 0, TRUE );

	if( m_pddsTexture )
	{
		TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( m_szTexture );
		if( emType==TextureManager::EMTT_ALPHA_HARD )			m_bAlphaMap = TRUE;
		else if( emType== TextureManager::EMTT_ALPHA_SOFT )		m_bAlphaMap = TRUE;
		else if( emType== TextureManager::EMTT_ALPHA_SOFT01 )	m_bAlphaMap = TRUE;
		else if( emType== TextureManager::EMTT_ALPHA_SOFT02 )	m_bAlphaMap = TRUE;
		else													m_bAlphaMap = FALSE;
	}

	return S_OK;
}

HRESULT DxEffectCloth::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szTexture, m_pddsTexture );

	return S_OK;
}

HRESULT DxEffectCloth::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fElapsedTime;

	//	Note : 정점을 움직인다.
	MeshUpdate();

	return S_OK;
}

HRESULT DxEffectCloth::MeshUpdate ()
{
	// 파티클 force를 전부 클리어 시킴.
	// force는 파티클의 거리와 질량으로써 결정되어 지기에 누적을 할 필요가 없어진다.
	// 만약 누적을 한다면 어떻게 될까? ^^

	if( m_pParticle == NULL ) return S_OK;	

	D3DXVECTOR3 vTemp;

	if ( m_bPlay && m_pVertArray )
	{
		int i = 0;
		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
			m_pParticle[i].vForce = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );		

		//난수에 의한 Wind의 방향을 전체적으로 준다.
		//wind라기 보다는 노이즈에 가깝다.
		//Wind 부분을 삽입한 부분이다.	
		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
		{
			if( m_pParticle[i].bLocked == FALSE )
			{
				//중력 부분의 적용
				m_pParticle[i].vForce.y = m_fGravity; 

				// 0 ~ 1.0사이 값으로 셋
				// Note: 바람의 세기 부분 
				D3DXVec3Normalize( &vTemp, &m_vWindDirection );
				m_pParticle[i].vForce += vTemp*m_fWindForce*sinf( D3DXToRadian( rand()%90 ) ); 					
					
				//	Note : 바람 모델 수정 
			//	m_pParticle[i].vForce +=  m_Wind.CalculateForce( m_vWindDirection, pSampleVertices[i].vNor, m_fWindForce, m_fWindConstant );						
			}
		}

		// 각 스프링으로 연결된 노드의 파티클 위치, 속도 정보를 이용하여서 Force을 구하는 부분
		D3DXVECTOR3 vPosition1, vPosition2, vVelocity1, vVelocity2, vForce, vDistance;	
		int index1, index2;

		for( i = 0 ; i < m_iNodeCount ; i++ )
		{	
			// 각 스프링의 노드 정보를 이용하여서 힘을 구하는 부분 
			index1 = m_pNodeTable[i].Index1;
			index2 = m_pNodeTable[i].Index2;

			vPosition1 = m_pParticle[ index1 ].vPosition;
			vPosition2 = m_pParticle[ index2 ].vPosition;

			vVelocity1 = m_pParticle[ index1 ].vVelocity;
			vVelocity2 = m_pParticle[ index2 ].vVelocity;

			vForce = m_Spring.CalculateForce( vPosition1, vPosition2, vVelocity1, vVelocity2, m_pNodeTable[i].fLength ); 

			if( !m_pParticle[ index1 ].bLocked )
				m_pParticle[ index1 ].vForce += vForce; //힘의 누적을 시킨다.그래야 서로 연관된 힘을 얻을수 있다.

			if( !m_pParticle[ index2 ].bLocked )
				m_pParticle[ index2 ].vForce += -vForce;	
		}

		//구한 힘을 이용하여서 각 속도 및 거리를 계산하여서 파티클에 최종적인 데이타를 넣어주는 부분 
		//f = ma에서 m을 생략하고 넣어 준다.	
		//float fElpTime = m_fTime; //고정시킴
		//if ( fElpTime > 0.03f )
		//{
		//	fElpTime = 0.03f;
		//}

		//	시간이 너무 작을 경우 노멀 뒤집고 다른 계산 안한다.
		//
		m_fSumTime += m_fTime;
		if ( m_fSumTime <= 0.03f )
		{
			////	Note : 노멀 뒤집기
			//for ( DWORD i=0; i<m_dwNumVertices; i++ )
			//{
			//	m_pVertArray[i].vNor = -m_pVertArray[i].vNor;
			//}

			return S_OK;
		}

		float fElpTime	= 0.03f;
		m_fSumTime		= 0.f;


		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
		{
			m_pParticle[i].vAccelerate = m_pParticle[i].vForce;
			m_pParticle[i].vVelocity += m_pParticle[i].vAccelerate*fElpTime;
			m_pParticle[i].vPosition += m_pParticle[i].vVelocity*fElpTime;

		//	CDebugSet::ToView ( "%f,   %f,   %f", 10, m_pParticle[i].vVelocity.x, m_pParticle[i].vVelocity.y, m_pParticle[i].vVelocity.z );
		}

		//모든 파티클 정점 정보를 Vertex Buffer로 복사한다.
		for( i = 0 ; i < (int)m_dwNumVertices ; i++ )
		{
			m_pVertArray[i].vPos = m_pParticle[ i ].vPosition;						
		}

		ComputeNormal( (int)m_dwNumVertices, m_pVertArray, (int)m_dwNumIndices, m_pIndexArray ); //고라운드 쉐이딩 노말을 계산 하는 부분 			
	}

	return S_OK;
}

HRESULT DxEffectCloth::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	if( !m_pVertArray )		return S_OK;
	if( !m_pIndexArray )	return S_OK;
 
	PROFILE_BEGIN("DxEffectCloth");

	HRESULT hr = S_OK;

	if( !m_pddsTexture )
	{
		TextureManager::GetTexture( m_szTexture, m_pddsTexture );
		if( m_pddsTexture )
		{
			TextureManager::EM_TEXTYPE emType = TextureManager::GetTexType( m_szTexture );
			if( emType==TextureManager::EMTT_ALPHA_HARD )			m_bAlphaMap = TRUE;
			else if( emType== TextureManager::EMTT_ALPHA_SOFT )		m_bAlphaMap = TRUE;
			else if( emType== TextureManager::EMTT_ALPHA_SOFT01 )	m_bAlphaMap = TRUE;
			else if( emType== TextureManager::EMTT_ALPHA_SOFT02 )	m_bAlphaMap = TRUE;
			else													m_bAlphaMap = FALSE;
		}
	}

	if ( m_bAlphaMap )	DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );

	pd3dDevice->SetTexture( 0, m_pddsTexture );

	pd3dDevice->SetTransform ( D3DTS_WORLD, m_pmatLocal );

	DWORD dwCullMode;
	pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );

	{
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );

		// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = m_dwNumVertices*sizeof(VERTEX);
		if( DxDynamicVB::m_sVB_PNT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte )
		{
			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB_PNT.nVertexCount = 0;
			DxDynamicVB::m_sVB_PNT.nOffsetToLock = 0; 
		}

		VERTEX *pVertices;
		hr = DxDynamicVB::m_sVB_PNT.pVB->Lock( DxDynamicVB::m_sVB_PNT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
		if ( FAILED(hr) )	
		{
			CDebugSet::ToLogFile( "DxEffectCloth::Render() -- DxDynamicVB::m_sVB_PNT.pVB->Lock() -- Failed" );
			goto _RETURN;
		}
		memcpy( pVertices, m_pVertArray, dwVertexSizeFULL );
		DxDynamicVB::m_sVB_PNT.pVB->Unlock ();

		pd3dDevice->SetFVF( VERTEX::FVF );
		pd3dDevice->SetStreamSource( 0, DxDynamicVB::m_sVB_PNT.pVB, 0, sizeof(VERTEX) );
		pd3dDevice->SetIndices( m_pIB );

		//	Note : 실제 앞 부분 뿌리기
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PNT.nVertexCount, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB_PNT.nVertexCount += m_dwNumVertices;
		DxDynamicVB::m_sVB_PNT.nOffsetToLock += dwVertexSizeFULL;
	}

	if ( m_bPlay )
	{
		//	Note : 노멀 뒤집기
		for ( DWORD i=0; i<m_dwNumVertices; i++ )
		{
			m_pVertArray[i].vNor = -(m_pVertArray[i].vNor);
		}

		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
		{
			// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
			DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
			DWORD dwVertexSizeFULL = m_dwNumVertices*sizeof(VERTEX);
			if( DxDynamicVB::m_sVB_PNT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte )
			{
				dwFlag = D3DLOCK_DISCARD; 
				DxDynamicVB::m_sVB_PNT.nVertexCount = 0;
				DxDynamicVB::m_sVB_PNT.nOffsetToLock = 0; 
			}

			VERTEX *pVertices;
			hr = DxDynamicVB::m_sVB_PNT.pVB->Lock( DxDynamicVB::m_sVB_PNT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
			if ( FAILED(hr) )	
			{
				CDebugSet::ToLogFile( "DxEffectCloth::Render() -- DxDynamicVB::m_sVB_PNT.pVB->Lock() -- Failed" );
				goto _RETURN;
			}
			memcpy( pVertices, m_pVertArray, dwVertexSizeFULL );
			DxDynamicVB::m_sVB_PNT.pVB->Unlock ();

			pd3dDevice->SetFVF( VERTEX::FVF );
			pd3dDevice->SetStreamSource( 0, DxDynamicVB::m_sVB_PNT.pVB, 0, sizeof(VERTEX) );
			pd3dDevice->SetIndices( m_pIB );

			//	Note : 뒷 부분 뿌리기
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PNT.nVertexCount, 0, m_dwNumVertices, 0, m_dwNumIndices/3 );

			// Note : 값을 더해줌. 
			DxDynamicVB::m_sVB_PNT.nVertexCount += m_dwNumVertices;
			DxDynamicVB::m_sVB_PNT.nOffsetToLock += dwVertexSizeFULL;
		}
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );

		//	Note : 노멀 뒤집기
		for ( DWORD i=0; i<m_dwNumVertices; i++ )
		{
			m_pVertArray[i].vNor = -(m_pVertArray[i].vNor);
		}
	}

	pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );

	if ( m_bAlphaMap )	DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );

	//	Note : 원 나타내기.
	if ( !m_bPlay )
	{
		for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
		{
			WORD k = m_pLockIndex[i];

			EDITMESHS::RENDERSPHERE ( pd3dDevice, m_pVertArray[k].vPos, 1.0f );
		}
	}

_RETURN:

	PROFILE_END("DxEffectCloth");

	return S_OK;
}

void DxEffectCloth::DeleteClothData( int a )
{
	if ( a == 0 )
	{
		SAFE_DELETE_ARRAY( m_pVertArray );
		SAFE_DELETE_ARRAY( m_pIndexArray );
		SAFE_DELETE_ARRAY( m_pBaseVertArray );
		SAFE_DELETE_ARRAY( m_pBaseIndexArray );

		SAFE_RELEASE( m_pIB );
	}
	
	if ( a == 1 )
	{
		SAFE_DELETE_ARRAY( m_pVertArray );
		SAFE_DELETE_ARRAY( m_pIndexArray );

		SAFE_RELEASE( m_pIB );
	}
}

void DxEffectCloth::XFileLoad( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh )
{	
	HRESULT hr(S_OK);
	DeleteClothData( 0 );

	LPDIRECT3DVERTEXBUFFERQ	pVB	= NULL;
	LPDIRECT3DINDEXBUFFERQ	pIB = NULL;

	WORD *pwIndexSrc;
	VERTEX *pVerticesSrc;
	m_dwNumVertices = pLocalMesh->GetNumVertices();//정점의 수
	m_dwNumIndices  = pLocalMesh->GetNumFaces()*3; //인덱스의 수

	SAFE_DELETE_ARRAY( m_pBaseVertArray );
	SAFE_DELETE_ARRAY( m_pBaseIndexArray );
	m_pBaseVertArray = new VERTEX[m_dwNumVertices];
	m_pBaseIndexArray = new WORD[m_dwNumIndices];

	//	Note : 나중을 위하여 VB와 IB를 저장해 놓는다.
	pLocalMesh->GetVertexBuffer( &pVB );
	pLocalMesh->GetIndexBuffer( &pIB );

	hr = pVB->Lock( 0, 0, (VOID**)&pVerticesSrc, NULL );
	if( FAILED(hr) )	
	{
		CDebugSet::ToLogFile( "DxEffectCloth::XFileLoad() -- pVB->Lock() -- Failed" );
		return;
	}

	memcpy( m_pBaseVertArray, pVerticesSrc, sizeof(VERTEX)*m_dwNumVertices ); //정점 정보 복사 

	for( DWORD i = 0 ; i < m_dwNumVertices ; i++ )
	{
		D3DXVec3TransformCoord ( &m_pBaseVertArray[i].vPos, &pVerticesSrc[i].vPos, &m_matWorld );
	}

	pVB->Unlock();

	hr = pIB->Lock( 0, 0, (VOID**)&pwIndexSrc, NULL );
	if( FAILED(hr) )	
	{
		CDebugSet::ToLogFile( "DxEffectCloth::XFileLoad() -- pIB->Lock() -- Failed" );
		return;
	}

	memcpy ( m_pBaseIndexArray, pwIndexSrc, m_dwNumIndices*sizeof(WORD) );
	pIB->Unlock();

	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
	
	CreateVertex( pd3dDevice, m_dwNumVertices, m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray );
	CreateNodeTable( m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray ); //노드 테이블 만들기

	m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 
}

void DxEffectCloth::BaseBuffToDynamicBuff ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
	DeleteClothData( 1 );
	
	CreateVertex( pd3dDevice, m_dwNumVertices, m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray );
	CreateNodeTable( m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray ); //노드 테이블 만들기

	m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	//	Note : 다시 "락" 걸기.
	//
	for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
	{
		WORD k = m_pLockIndex[i];
		m_pParticle[ k ].bLocked = TRUE;
	}
}

//파티클의 갯수 만큼 내려가기에 파티클의 갯수로 한다.
//노드 테이블 수를 리턴한다.
void DxEffectCloth::CreateNodeTable( VERTEX *pVertices, DWORD dwNumIndices, WORD *pIndices )
{	
	NODE *Temp_Node = new NODE[ dwNumIndices ];//임시 노드임
	
	//----------- 인덱스 버퍼에서 3개의 노드로 분리 
	int k = 0;
	int i = 0;
	for( i = 0 ; i < (int)dwNumIndices ; i+=3 )
	{
		for( int j = 0 ; j < 3 ; j++, k++ )
		{
			Temp_Node[k].Index1 = pIndices[i+j];
			Temp_Node[k].Index2 = pIndices[i + (j+1)%3];
		}
	}

	//------------중복 플래그 초기화------------------------
	BOOL *pDuplicate_Table = new BOOL[ dwNumIndices ];
	for( i = 0 ; i < (int)dwNumIndices ; i++ )
			pDuplicate_Table[i] = FALSE;	

	//------------- 2 단계 중복 플래그 부분 검색 -------------------
	int count = 0; //중복 플래그 갯수 카운터 
    for( int m = 0 ; m < (int)dwNumIndices ; m++ )
	{
		for( int n = m + 1 ; n < (int)dwNumIndices ; n++ )
		{
			if( ((Temp_Node[m].Index1 == Temp_Node[n].Index1) && (Temp_Node[m].Index2 == Temp_Node[n].Index2 ))  //괄호에 주의 하자.!!!
				|| ((Temp_Node[m].Index1 == Temp_Node[n].Index2) && (Temp_Node[m].Index2 == Temp_Node[n].Index1 )) )
			{
				pDuplicate_Table[n] = TRUE;
				count++;
			}			
		}
	}

	 //------------------------- 중복 검사 ---------------------------//	 
	SAFE_DELETE_ARRAY ( m_pNodeTable );
	m_pNodeTable = new NODE[ dwNumIndices - count ];
	m_iNodeCount = dwNumIndices - count; //중복 제거한 노드수 

	int index = 0;
	for( int c = 0 ; c < (int)dwNumIndices ; c++ )
	{
		  if( !pDuplicate_Table[c] )
		  {
			  m_pNodeTable[index ] = Temp_Node[c];
			  index++;
		  }
	 }
	     
	 SAFE_DELETE_ARRAY ( Temp_Node );
	 SAFE_DELETE_ARRAY ( pDuplicate_Table );

	 
	 //-------------------------------- 중복 검사 완결 ------------------------------------------//
	 //파티클이 정점과 같은 위치로 되어 있다고 생각하에...파티클의 위치와 정점의 인덱스가 동일함.
	 int index1, index2;
	 D3DXVECTOR3 vLength;
	 for( int i = 0 ; i < m_iNodeCount ; i++ )
	 {
		 index1 = m_pNodeTable[i].Index1;
		 index2 = m_pNodeTable[i].Index2;
		 vLength = pVertices[index2].vPos - pVertices[index1].vPos ; 
		 m_pNodeTable[i].fLength = D3DXVec3Length( &vLength );	 
	 } 
}

void DxEffectCloth::SetParticle(int index, D3DXVECTOR3 &vPosition )
{ 
	 //파티클 초기화 ^^
	 	m_pParticle[ index ].vPosition   = vPosition;
		m_pParticle[ index ].vVelocity   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pParticle[ index ].vAccelerate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pParticle[ index ].vForce      = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pParticle[ index ].bLocked = FALSE;	 	
}

//이 함수는 vertex의 index, vertex buffer를 생성하는 부분이다.
//이 안에서는 파티클도 셋하여 진다. vertex의 정보와 파티클은 같은 데이타를 공유하게 된다.
// cl 화일 로딩 후에 함수를 로딩한다...
BOOL DxEffectCloth::CreateVertex( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwNumVertices, VERTEX* pVertices, DWORD dwNumIndices, WORD* pIndices )
{
	// 파티클을 생성하는 부분이다 ^^
	SAFE_DELETE_ARRAY( m_pParticle );	
	m_pParticle = new PARTICLE[ dwNumVertices ];

	//------------- 버택스 버퍼의 내용으로 파티클 정보를 채우는 부분 -----------//
	SAFE_DELETE_ARRAY( m_pVertArray );
	m_pVertArray = new VERTEX[dwNumVertices];
	memcpy( m_pVertArray, pVertices, sizeof(VERTEX)*dwNumVertices ); //정점 정보 복사 

	//파티클 정점 셋 
	for( int i = 0 ; i < (int)dwNumVertices ; i++ )
	{
		SetParticle( i, m_pVertArray[i].vPos );
	}

	//인덱스 정보 복사 
    //---------- 인덱스 버퍼를 이용하여서 Node 테이블과 노말 테이블을 셋하여 주는 부분 ----------//
	SAFE_DELETE_ARRAY( m_pIndexArray );
	m_pIndexArray = new WORD[dwNumIndices];
	memcpy( m_pIndexArray, pIndices, sizeof(WORD)*dwNumIndices );

	//인덱스 정보 복사 
    //---------- 인덱스 버퍼를 이용하여서 Node 테이블과 노말 테이블을 셋하여 주는 부분 ----------//
	SAFE_RELEASE ( m_pIB );
	if( FAILED( pd3dDevice->CreateIndexBuffer ( dwNumIndices*sizeof(WORD), D3DUSAGE_WRITEONLY,
						D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL ) ) )
	{
		CDebugSet::ToLogFile( "DxEffectCloth::CreateVertex() -- CreateIndexBuffer() -- Failed" );
		return FALSE;
	}

	WORD *pTempIndices;		
	if( FAILED( m_pIB->Lock( 0, 0, (VOID**)&pTempIndices, 0 ) ) )
	{
		CDebugSet::ToLogFile( "DxEffectCloth::CreateVertex() -- pIB->Lock() -- Failed" );
		return FALSE;
	}

	memcpy( pTempIndices, pIndices, sizeof(WORD)*dwNumIndices );

	m_pIB->Unlock();	


	return TRUE;
}

//고라운드 쉐이딩 노말값 계산하여 주는 함수
//한점과 연결된 모든 정점의 인덱스를 이용하여서 노말을 구한다. 
void  DxEffectCloth::ComputeNormal(int nVertices,  VERTEX *pVertices, int nIndices, WORD *pIndices  )
{
	D3DXVECTOR3 v1, v2;
	D3DXVECTOR3 vNormal;
	int i;

	// 법선의 누적으로 인한 초기화를 해주는 부분 
	// 삼각형의 두 선분은 두선분의 길이로 구하는 것이다.
	for( i = 0 ; i < nVertices ; i++ )
		 pVertices[i].vNor = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	//각 삼각형의 법선을 구하는 부분
    for( i = 0 ; i < nIndices ; i+= 3 )
	{		
		for( int j = 0; j < 3 ; j++ )
		{
			v1 = pVertices[ pIndices[ (i+1+j)%3 + i ] ].vPos - pVertices[ pIndices[i+j] ].vPos ;
			v2 = pVertices[ pIndices[ (i+2+j)%3 + i ] ].vPos - pVertices[ pIndices[i+j] ].vPos ;
			D3DXVec3Cross( &vNormal, &v1, &v2 );

			pVertices[ pIndices[i+j] ].vNor += vNormal;		
		}
	}

	// Normalize 하는 부분 
	for( i = 0 ; i < nVertices ; i++ )
		D3DXVec3Normalize( &pVertices[ i ].vNor, &pVertices[ i ].vNor );	
}


inline float DxEffectCloth::Random( int nMin, int nMax )
{
	srand( nMax );	
	return (float)(((rand()%nMax)+nMin)%nMax);
}

void DxEffectCloth::SetParticleLock( int index , BOOL bLook )
{
	if ( (m_pParticle == NULL) )	return;
	if ( WORD(index) >= m_dwNumVertices )	return;

	if ( bLook )
	{
		for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			if ( m_pLockIndex[i] == index ) return;

		WORD* pNewLockIndex = new WORD[m_dwNumLockIndex+1];
		
		if ( m_pLockIndex && m_dwNumLockIndex>0 )
			memcpy ( pNewLockIndex, m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );
		
		pNewLockIndex[m_dwNumLockIndex] = index;

		SAFE_DELETE_ARRAY ( m_pLockIndex );

		m_dwNumLockIndex++;
	
		m_pLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pLockIndex, pNewLockIndex, sizeof(WORD)*m_dwNumLockIndex );

		SAFE_DELETE_ARRAY ( pNewLockIndex );
	}
	else
	{
		BOOL bLockIndex = FALSE;
		for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			if ( m_pLockIndex[i] == index ) bLockIndex = TRUE;

		if ( !bLockIndex )	return;

		if ( m_dwNumLockIndex > 1 )
		{
			WORD* pNewLockIndex = new WORD[m_dwNumLockIndex-1];

			DWORD j=0;
			for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			{
				if ( m_pLockIndex[i] != index )
					pNewLockIndex[j++] = m_pLockIndex[i];
			}

			SAFE_DELETE_ARRAY ( m_pLockIndex );

			m_dwNumLockIndex--;

			m_pLockIndex = new WORD[m_dwNumLockIndex];
			memcpy ( m_pLockIndex, pNewLockIndex, sizeof(WORD)*m_dwNumLockIndex );

			SAFE_DELETE_ARRAY ( pNewLockIndex );
		}
		else
		{
			m_dwNumLockIndex = 0;
			SAFE_DELETE_ARRAY ( m_pLockIndex );
		}
	}

	m_pParticle[ index ].bLocked = bLook;
}

void DxEffectCloth::SetIndexLock()
{
	SAFE_DELETE_ARRAY ( m_pTempLockIndex );

	if ( m_dwNumLockIndex )
	{
		m_pTempLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pTempLockIndex, m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );
	}
}

void DxEffectCloth::GetIndexLock()
{
	SAFE_DELETE_ARRAY ( m_pLockIndex );

	if ( m_dwNumLockIndex )
	{
		m_pLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pLockIndex, m_pTempLockIndex, sizeof(WORD)*m_dwNumLockIndex );
	}
	SAFE_DELETE_ARRAY ( m_pTempLockIndex );
}

char* DxEffectCloth::GetStrList( DWORD i )
{
	static char szStrEdit[256];

	WORD k = m_pLockIndex[i];
	StringCchPrintf( szStrEdit, 256, "[%d] %4.2f, %4.2f, %4.2f", k, m_pVertArray[k].vPos.x, m_pVertArray[k].vPos.y, m_pVertArray[k].vPos.z );

	return szStrEdit;
}

DWORD DxEffectCloth::GetdwVertex( DWORD i )
{
	WORD k = m_pLockIndex[i];

	return k;
}

void DxEffectCloth::GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	D3DXVECTOR3 vBoxMax;
	D3DXVECTOR3 vBoxMin;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	float fSize;

	// Note: x, y 평면상에서의 최대, 최소를 찾아준다.
	PBYTE pbPoints = (PBYTE)m_pBaseVertArray;
	COLLISION::CalculateBoundingBox( &matIdentity, vBoxMax, vBoxMin,
		pbPoints, m_dwNumVertices, VERTEX::FVF );

	//	Note:	z 좌표에 대해서 길이를 구하여서 증감, 감소를 해줌으로 해주면 박스의 최대 최소를 구할수 있다.
	//			위 충돌 부분이 한 평면에 대해서 가져 오는 것이기에 같은 좌표가 존재한다.
	//			그 좌표가 늘려야 하는 좌표가 되는 것이다.	
	vMin	= vBoxMin;
	vMax	= vBoxMax;	

	if( vBoxMax.x == vBoxMin.x ) 
	{	
		fSize = vBoxMax.y - vBoxMin.y;
		
		// Note: 바람과 평면의 방향이 다르면 바람의 세기 만큼 그쪽 방향을 느려주어서 충돌 박스를 만들어 준다.
		if( D3DXVec3Dot( &vBoxMin, &m_vWindDirection ) < 0 )
		{
			vMin.y -= m_fWindForce*0.3f;
			vMax.y += m_fWindForce*0.3f;

			vMin.z -= m_fWindForce*0.3f;
			vMax.z += m_fWindForce*0.3f;
		}            
		
		vMin.x -= fSize*m_fWindForce*0.2f; 
		vMax.x += fSize*m_fWindForce*0.2f; 
		
			//	Note: y를 고정 해서 사용되는 경우에 
	}else if( vBoxMax.y == vBoxMin.y ) 
	{	
		fSize = vBoxMax.z - vBoxMin.z;

		// Note: 바람과 평면의 방향이 다르면 바람의 세기 만큼 그쪽 방향을 늘려주어서 충돌 박스를 만들어 준다.
		if( D3DXVec3Dot( &vBoxMin, &m_vWindDirection ) < 0 )
		{
			vMin.x -= m_fWindForce*0.3f;
			vMax.x += m_fWindForce*0.3f;

			vMin.z -= m_fWindForce*0.3f;
			vMax.z += m_fWindForce*0.3f;
		}            

		vMin.y -= fSize*0.7f;
		vMax.y += fSize*0.7f;

	}else if( vBoxMax.z == vBoxMin.z ) 
	{	
		fSize = vBoxMax.y - vBoxMin.y;

		// Note: 바람과 같은 방향이면 
		if( D3DXVec3Dot( &vBoxMin, &m_vWindDirection ) < 0 )
		{
			vMin.x -= m_fWindForce*0.3f;
			vMax.x += m_fWindForce*0.3f;

			vMin.y -= m_fWindForce*0.3f;
			vMax.y += m_fWindForce*0.3f;
		}	
		
		vMin.z -= fSize*m_fWindForce*0.2f; 
		vMax.z += fSize*m_fWindForce*0.2f;
	};
}

BOOL DxEffectCloth::IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3 vBoxMax;
	D3DXVECTOR3 vBoxMin;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	PBYTE pbPoints = (PBYTE)m_pBaseVertArray;
	COLLISION::CalculateBoundingBox ( &matIdentity, vBoxMax, vBoxMin,
		pbPoints, m_dwNumVertices, VERTEX::FVF );

	D3DXVECTOR3 vCenter = ( vBoxMax + vBoxMin ) / 2.0f;

	return COLLISION::IsWithInPoint(vDivMax,vDivMin,vCenter);
}

void DxEffectCloth::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(VERTEX)*m_dwNumVertices
			+sizeof(DWORD)+sizeof(WORD)*m_dwNumIndices
			+sizeof(DWORD)+sizeof(WORD)*m_dwNumLockIndex
			+sizeof(int)+sizeof(NODE)*m_iNodeCount;

	SFile << m_dwNumVertices;
	SFile.WriteBuffer( m_pBaseVertArray, sizeof(VERTEX)*m_dwNumVertices );

	SFile << m_dwNumIndices;
	SFile.WriteBuffer ( m_pBaseIndexArray, sizeof(WORD)*m_dwNumIndices );

	SFile << m_dwNumLockIndex;
	if ( m_dwNumLockIndex )
		SFile.WriteBuffer ( m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );

	// Note : Ver 0x101 에서 추가 저장되는 것.
	//
	SFile << m_iNodeCount;
	SFile.WriteBuffer ( m_pNodeTable, sizeof(NODE)*m_iNodeCount );
}

void DxEffectCloth::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	VERTEX*	pClothVertex;
	WORD*			pIndices;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	SFile >> dwBuffSize;
	pClothVertex = new VERTEX[dwBuffSize];
	SFile.ReadBuffer( pClothVertex, sizeof(VERTEX)*dwBuffSize );

	if ( dwBuffSize==m_dwNumVertices && dwBuffSize >= 1 )
	{
		memcpy ( m_pBaseVertArray, pClothVertex, sizeof(VERTEX)*dwBuffSize );
	}
	
	SAFE_DELETE_ARRAY(pClothVertex);



	SFile >> dwBuffSize;
	pIndices = new WORD[dwBuffSize];
	SFile.ReadBuffer ( pIndices, sizeof(WORD)*dwBuffSize );

	if ( dwBuffSize==m_dwNumIndices && dwBuffSize >= 1 )
	{
		memcpy ( m_pBaseIndexArray, pIndices, sizeof(WORD)*dwBuffSize );
	}

	SAFE_DELETE_ARRAY(pIndices);



	SFile >> m_dwNumLockIndex;
	SAFE_DELETE_ARRAY(m_pLockIndex);
	m_pLockIndex = new WORD[m_dwNumLockIndex];
	SFile.ReadBuffer ( m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );

	// Note : Ver 0x101 에서 추가 저장된 것 로드..
	//
	if ( dwVer >= 0x101 )
	{
		SFile >> m_iNodeCount;
		SAFE_DELETE_ARRAY(m_pNodeTable);
		m_pNodeTable = new NODE[m_iNodeCount];
		SFile.ReadBuffer ( m_pNodeTable, sizeof(NODE)*m_iNodeCount );
	}

	//	Note : 스프링 설정.
	//
	m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
	//
	if ( pd3dDevice )
	{
		if ( dwVer==0x100 )
		{
			BaseBuffToDynamicBuff ( pd3dDevice );
		}
		else
		{
			//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
			CreateVertex( pd3dDevice, m_dwNumVertices, m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray );

			//	Note : 파티클 "락" 걸기.
			//
			for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			{
				WORD k = m_pLockIndex[i];
				m_pParticle[ k ].bLocked = TRUE;
			}
		}
	}
	
}

void DxEffectCloth::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	DWORD dwBuffSize;
	PBYTE pbBuff;

	SFile >> dwBuffSize; //	"읽지 않고 스킵" 할 때 사용되므로 실제 로드에서는 무의미.

	SFile >> m_dwNumVertices;
	SAFE_DELETE_ARRAY( m_pBaseVertArray );
	m_pBaseVertArray = new VERTEX[m_dwNumVertices];
	SFile.ReadBuffer ( m_pBaseVertArray, sizeof(VERTEX)*m_dwNumVertices );

	SFile >> m_dwNumIndices;
	SAFE_DELETE_ARRAY( m_pBaseIndexArray );
	m_pBaseIndexArray = new WORD[m_dwNumIndices];
	SFile.ReadBuffer ( m_pBaseIndexArray, sizeof(WORD)*m_dwNumIndices );

	SFile >> dwBuffSize;
	m_dwNumLockIndex = dwBuffSize;

	if ( m_dwNumLockIndex>0 )
	{
		pbBuff = new BYTE[sizeof(WORD)*dwBuffSize];
		SFile.ReadBuffer ( pbBuff, sizeof(WORD)*dwBuffSize );

		//	Note : 고정된 버텍스(파티클) 지정.
		//
		SAFE_DELETE_ARRAY(m_pLockIndex);
		if ( m_dwNumLockIndex > 0 )
		{
			m_pLockIndex = new WORD[m_dwNumLockIndex];
			memcpy ( m_pLockIndex, pbBuff, sizeof(WORD)*m_dwNumLockIndex );
		}

		SAFE_DELETE_ARRAY(pbBuff);
	}


	// Note : Ver 0x101 에서 추가 저장된 것 로드..
	//
	if ( dwVer >= 0x101 )
	{
		SFile >> m_iNodeCount;
		SAFE_DELETE_ARRAY(m_pNodeTable);
		m_pNodeTable = new NODE[m_iNodeCount];
		SFile.ReadBuffer ( m_pNodeTable, sizeof(NODE)*m_iNodeCount );
	}

	//	Note : 스프링 설정.
	//
    m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
	//
	if ( pd3dDevice )
	{
		if ( dwVer==0x100 )
		{
			BaseBuffToDynamicBuff ( pd3dDevice );
		}
		else
		{
			//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
			CreateVertex( pd3dDevice, m_dwNumVertices, m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray );

			//	Note : 파티클 "락" 걸기.
			//
			for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
			{
				WORD k = m_pLockIndex[i];
				m_pParticle[ k ].bLocked = TRUE;
			}
		}
	}
}

HRESULT DxEffectCloth::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectCloth* pSrcCloth = (DxEffectCloth*) pSrcEffect;

	//	Note : 먼저 초기화.
	DeleteClothData( 0 );

	SAFE_DELETE_ARRAY( m_pBaseVertArray );
	SAFE_DELETE_ARRAY( m_pBaseIndexArray );

	m_dwNumVertices = pSrcCloth->m_dwNumVertices;
	m_dwNumIndices = pSrcCloth->m_dwNumIndices;

	// Property 설정.
	m_bPlay				= pSrcCloth->m_bPlay;
	m_bEachRender		= pSrcCloth->m_bEachRender; 
	m_iNodeCount		= pSrcCloth->m_iNodeCount;
	m_fCoEfficient		= pSrcCloth->m_fCoEfficient;
	m_vWindDirection	= pSrcCloth->m_vWindDirection;
	m_fWindForce		= pSrcCloth->m_fWindForce;
	m_fGravity			= pSrcCloth->m_fGravity;
	StringCchCopy( m_szTexture, MAX_PATH, pSrcCloth->m_szTexture );

	//	Note : Base Vertex Buffer 복제.
	m_pBaseVertArray = new VERTEX[m_dwNumVertices];
	memcpy( m_pBaseVertArray, pSrcCloth->m_pBaseVertArray, sizeof(VERTEX)*m_dwNumVertices );

	//	Note : Base Index Buffer 복제.
	m_pBaseIndexArray = new WORD[m_dwNumIndices];
	memcpy( m_pBaseIndexArray, pSrcCloth->m_pBaseIndexArray, sizeof(WORD)*m_dwNumIndices );


	//	Note : Base VB, IB를 VB, IB 로 업대이트 하기 위해서 필요하다.
	CreateVertex( pd3dDevice, m_dwNumVertices, m_pBaseVertArray, m_dwNumIndices, m_pBaseIndexArray );

	//	Note : 스프링 설정.
    m_Spring.Init( m_fCoEfficient, DAMPING ); //스프링 상수 초기화 

	//	Note : 노드 배열 복사.
	//
	m_iNodeCount = pSrcCloth->GetNodeTableSize ();

	SAFE_DELETE_ARRAY(m_pNodeTable);
	m_pNodeTable = new NODE[m_iNodeCount];
	memcpy ( m_pNodeTable, pSrcCloth->GetNodeTable(), sizeof(NODE)*m_iNodeCount );

	//	Note : 고정된 버텍스(파티클) 복사.
	//
	m_dwNumLockIndex = pSrcCloth->m_dwNumLockIndex;
	if ( pSrcCloth->m_pLockIndex )
	{
		SAFE_DELETE_ARRAY(m_pLockIndex);
		m_pLockIndex = new WORD[m_dwNumLockIndex];
		memcpy ( m_pLockIndex, pSrcCloth->m_pLockIndex, sizeof(WORD)*m_dwNumLockIndex );
	}

	//	Note : 파티클 "락" 걸기.
	//
	for ( DWORD i=0; i<m_dwNumLockIndex; i++ )
	{
		WORD k = m_pLockIndex[i];
		m_pParticle[ k ].bLocked = TRUE;
	}

	return S_OK;
}



