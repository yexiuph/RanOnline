#include "pch.h"

#include "./Collision.h"
#include "./DxLightMan.h"
#include "./DxEnvironment.h"

#include "./DxStaticMesh.h"
#include "./DxPieceEff.h"
#include "./DxPieceStaticAni.h"
#include "./DxPieceContainer.h"
#include "./DxPieceTexEff.h"

#include "./DxPiece.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//													D	x		P	i	e	c	e
//
//		보면 PDXEFFECTBASE 이	DxStaticAniFrame과 DxPieceEff에 둘다 들어 있다.. 하지만 2개의 동작은 틀리다.
//		DxStaticAniFrame는 같은 것을 써서 동작은 같지만 부하를 덜 먹고,
//		DxPieceEff는 틀린것을 써서 동작은 틀리지만 부하는 많이 먹는다.
// -----------------------------------------------------------------------------------------------------------------------------------------
BOOL	g_bPICK_ALPHA = TRUE;	// 픽킹알파.

DxPiece::DxPiece() :
	m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
	m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vStaticMax(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vStaticMin(FLT_MAX,FLT_MAX,FLT_MAX),
	m_vCenter(0.f,0.f,0.f),
	m_fLength(1.f),
	m_fAlpha(1.f),
	m_bAplhaBlend(FALSE),
	m_fTime(0.f),
	m_fAniTime(0.f),
	m_ElapsedTime(0.f),
	m_pStaticAniFrame(NULL),
	m_pPieceEff(NULL),
	m_pAniTexEff(NULL),
	m_pNext(NULL),
	m_pTreeNext(NULL),
	m_pSortNext(NULL)
{
	m_fAniTime = RANDOM_POS * 100.f * UNITANIKEY_PERSEC;

	D3DXMatrixIdentity( &m_matWorld );
}

DxPiece::~DxPiece()
{
	CleanUp();
}

void DxPiece::CleanUp()
{
	m_pStaticAniFrame = NULL;
	SAFE_DELETE( m_pPieceEff );
	SAFE_DELETE( m_pAniTexEff );
}

void DxPiece::FrameMove( const float fTime, const float fElapsedTime )
{
	m_fTime += fElapsedTime;
	m_fAniTime += fElapsedTime * UNITANIKEY_PERSEC;
	if( m_fTime > 1.0e15f )		m_fTime = 0.f;
	if( m_fAniTime > 1.0e15f )	m_fAniTime = 0.f;

	if( m_pStaticAniFrame )	m_pStaticAniFrame->FrameMove( fElapsedTime );
	if( m_pPieceEff )		m_pPieceEff->FrameMove( fTime, fElapsedTime );
	if( m_pAniTexEff )		m_pAniTexEff->FrameMove( fElapsedTime );
}

void DxPiece::Render( const LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pStaticAniFrame )	return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice );

	// Note : Render
	m_pStaticAniFrame->Render( pd3dDevice, m_matWorld, m_fAniTime, m_pAniTexEff->GetMapTexEff() );

	// Note : Saved of Location Matirx. Because Animation is Change to All Time. Is Alpha ?
	//			위치 매트릭스를 저장 시켜 놓는다. Animation이 계속 바뀌기 때문이다. 알파는 .??
	if( !m_pPieceEff )		return;
	m_pPieceEff->SetMatrix();
}

void DxPiece::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pStaticAniFrame )	return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice );

	// Note : Render
	m_pStaticAniFrame->Render_Alpha( pd3dDevice, m_matWorld, m_pAniTexEff->GetMapTexEff() );
}

void DxPiece::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh )
{
	if( !m_pPieceEff )	return;

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice );

	// Note : Render
	m_pPieceEff->Render( pd3dDevice, m_matWorld, pStaticMesh, pFrameMesh );
}

void DxPiece::Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();
	D3DXMATRIX matWorld = m_matWorld;
	matWorld._42 = matWorld._42 - (vCenter.y*2.f);
	D3DXMatrixMultiply_MIRROR( matWorld );

	// Note : Point Light를 적용시킨다.
	RenderPointLight( pd3dDevice );

	// Note : Render
	if( m_pStaticAniFrame )
	{
		m_pStaticAniFrame->Render( pd3dDevice, matWorld, m_fAniTime, m_pAniTexEff->GetMapTexEff() );
		m_pStaticAniFrame->Render_Alpha( pd3dDevice, matWorld, m_pAniTexEff->GetMapTexEff() );
	}

	// Note : Render
	if( m_pPieceEff )
	{
		m_pPieceEff->Render( pd3dDevice, matWorld, NULL, NULL );
	}
}

void DxPiece::SetAABBBox()
{
	m_pStaticAniFrame->GetAABBBox( m_vMax, m_vMin );

	m_vStaticMax = m_vMax;
	m_vStaticMin = m_vMin;
}

BOOL DxPiece::IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision )
{
	D3DXMATRIX matInverse;
	D3DXMatrixInverse( &matInverse, NULL, &m_matWorld );

	D3DXVECTOR3 vNewStart = vStart;
	D3DXVECTOR3 vNewEnd = vEnd;
	D3DXVec3TransformCoord( &vNewStart, &vStart, &matInverse );
	D3DXVec3TransformCoord( &vNewEnd, &vEnd, &matInverse );

	BOOL bCollision(FALSE);
	bCollision = m_pStaticAniFrame->IsCollisionLine( vNewStart, vNewEnd, vCollision );

	if( bCollision )
	{
		D3DXVec3TransformCoord( &vEnd, &vNewEnd, &m_matWorld );
		D3DXVec3TransformCoord( &vCollision, &vCollision, &m_matWorld );
	}

	return bCollision;
}

void DxPiece::Save( CSerialFile& SFile )
{
	SFile << m_vMax;		// Maybe It is unnecessary < 필요 없지 싶다. >
	SFile << m_vMin;		// 필요 없지 싶다. DxPieceEdit을 로드시 부르기 때문에..

	SFile.WriteBuffer( &m_matWorld, sizeof(D3DXMATRIX) );
}

void DxPiece::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxPieceEdit* pSrc )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

	SFile.ReadBuffer( &m_matWorld, sizeof(D3DXMATRIX) );

	// Note : 원본이 없을 경우 대비
	if( !pSrc )
	{
		m_vStaticMax = m_vMax;
		m_vStaticMin = m_vMin;

		// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
		D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
		m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
		m_fLength = m_fLength*m_fLength;

		// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
		CheckPointLight();

		return;
	}

	// Note : 얻어온다.
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_fLength = pSrc->GetLength();

	// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
	D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	m_fLength = m_fLength*m_fLength;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

	// Note : 포인터를 얻어옴.
	m_pStaticAniFrame = pSrc->m_pStaticAniFrame;

	m_pPieceEff = new DxPieceEff;
	m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff );

	m_pAniTexEff = new DxPieceTexEff;
	m_pAniTexEff->Clone( pd3dDevice, pSrc->m_pAniTexEff );
}

void DxPiece::Import( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEdit* pSrc, const D3DXMATRIX& matLocal, const D3DXMATRIX& matFrame )
{
	CleanUp();

	m_vMax		= pSrc->GetAABBMax();
	m_vMin		= pSrc->GetAABBMin();
	m_vStaticMax = pSrc->GetStaticMax();
	m_vStaticMin = pSrc->GetStaticMin();
	m_vCenter	= pSrc->GetCenter();
	m_fLength	= pSrc->GetLength();
	D3DXMatrixMultiply( &m_matWorld, &matLocal, &matFrame );
	D3DXMatrixMultiply( &m_matWorld, pSrc->GetMatrixLocal(), &m_matWorld );

	// Note : 자신에 맞도록 StaticAABB와 Center, Length를 바꾼다.
	D3DXMatrixAABBTransform( m_vStaticMax, m_vStaticMin, m_matWorld );
	m_vCenter = (m_vStaticMax+m_vStaticMin)*0.5f;
	m_fLength = m_fLength*m_fLength;

	m_pStaticAniFrame = pSrc->m_pStaticAniFrame;	// 그냥 주소만 주면 됨.

	m_pPieceEff = new DxPieceEff;
	m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff );

	m_pAniTexEff = new DxPieceTexEff;
	m_pAniTexEff->Clone( pd3dDevice, pSrc->m_pAniTexEff );
}

void DxPiece::CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc )
{
	CleanUp();

	m_vMax		= pSrc->m_vMax;
	m_vMin		= pSrc->m_vMin;
	m_vStaticMax = pSrc->m_vStaticMax;
	m_vStaticMin = pSrc->m_vStaticMin;
	m_vCenter	= pSrc->m_vCenter;
	m_fLength	= pSrc->m_fLength;
	m_matWorld	= pSrc->m_matWorld;

	// Note : 자신과 맞닿아 있는 빛을 체크를 한다.
	CheckPointLight();

	m_pStaticAniFrame = pSrc->m_pStaticAniFrame;	// 그냥 주소만 주면 됨.

	m_pPieceEff = new DxPieceEff;
	m_pPieceEff->Clone( pd3dDevice, pSrc->m_pPieceEff );

	m_pAniTexEff = new DxPieceTexEff;
	m_pAniTexEff->Clone( pd3dDevice, pSrc->m_pAniTexEff );
}

void DxPiece::CheckPointLight()
{
	DxLightMan::GetInstance()->SetPiecePointIndex( m_vCenter, m_fLength, m_vecPointIndex );
}

void DxPiece::RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : Light가 꺼져 있을 경우는 체크를 하지 않는다.

	// Note : 2개만 적용시키자.
	for( DWORD i=0; i<2; ++i )
	{
		if( m_vecPointIndex.size() > i )
		{
			D3DLIGHT9* pLight = DxLightMan::GetInstance()->GetPointLight( m_vecPointIndex[i] );
			if( pLight )
			{
				pd3dDevice->SetLight( i+1, pLight );
				pd3dDevice->LightEnable( i+1, TRUE );

				continue;
			}
		}

		// Note : 안 걸리면 무조건 조명은 끈다.
		pd3dDevice->LightEnable( i+1, FALSE );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e		T	r	e	e
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxPieceNode::CleanUp()
{
	//SAFE_DELETE( m_pPiece );

	DxPiece* pDel;
	DxPiece* pCur = m_pPiece;
	while( pCur )
	{
		pDel = pCur;
		pCur = pCur->m_pTreeNext;

		SAFE_DELETE( pDel );
	}

	if( m_pLeftChild )	m_pLeftChild->CleanUp();
	if( m_pRightChild )	m_pRightChild->CleanUp();
}

void DxPieceNode::Save( CSerialFile& SFile )
{
	SFile << m_vMax;
	SFile << m_vMin;

	// Note : 갯수 얻기
	int nCount = 0;
	DxPiece* pCur = m_pPiece;
	while( pCur )
	{
		++nCount;
		pCur = pCur->m_pTreeNext;
	}
	SFile << nCount;

	pCur = m_pPiece;
	while( pCur )
	{
		pCur->Save( SFile );
		pCur = pCur->m_pTreeNext;
	}

	if( m_pLeftChild )
	{
		SFile << (BOOL)TRUE;
		m_pLeftChild->Save( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}
	if( m_pRightChild )	
	{
		SFile << (BOOL)TRUE;
		m_pRightChild->Save( SFile );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}
}

void DxPieceNode::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxPieceEdit* pSrc )
{
	SFile >> m_vMax;
	SFile >> m_vMin;

	int nCount = 0;
	SFile >> nCount;
	for( int i=0; i<nCount; ++i )
	{
		DxPiece* pNew = new DxPiece;
		pNew->Load( pd3dDevice, SFile, pSrc );

		// Tree List 연결
		pNew->m_pTreeNext = m_pPiece;
		m_pPiece = pNew;
	}

	BOOL bExist = FALSE;
	SFile >> bExist;
	if( bExist )
	{
		m_pLeftChild = new DxPieceNode;
		m_pLeftChild->Load( pd3dDevice, SFile, pSrc );
	}

	SFile >> bExist;
	if( bExist )
	{
		m_pRightChild = new DxPieceNode;
		m_pRightChild->Load( pd3dDevice, SFile, pSrc );
	}
}

//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		Q	u	i	c	k		S	o	r	t
//----------------------------------------------------------------------------------------------------------------------
DWORD								DxPieceQuickSort::m_dwArraySizeONLY = 0L;
DWORD								DxPieceQuickSort::m_dwPieceCountONLY = 0L;
DxPieceQuickSort::PIECEDISTANCE*	DxPieceQuickSort::m_pArrayPieceONLY = NULL;

DxPieceQuickSort::DxPieceQuickSort() :
	m_fElapsedTime(0.f),
	m_pArrayPiece(NULL),
	m_dwArraySize(0),
	m_dwPieceCount(0)
{
}

DxPieceQuickSort::~DxPieceQuickSort()
{
	m_dwPieceCount = 0;
	m_dwArraySize = 0;
	SAFE_DELETE_ARRAY( m_pArrayPiece );

	m_dwPieceCountONLY = 0;
	m_dwArraySizeONLY = 0;
	SAFE_DELETE_ARRAY( m_pArrayPieceONLY );
}

void DxPieceQuickSort::FrameMove( const float fTime, const float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;

	for( DWORD i=0; i<m_dwPieceCount; ++i )
	{
		m_pArrayPiece[i].m_pPiece->FrameMove( fTime, fElapsedTime );
	}
}

void DxPieceQuickSort::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( DWORD i=0; i<m_dwPieceCount; ++i )
	{
		m_pArrayPiece[i].m_pPiece->Render( pd3dDevice );
	}
}

void DxPieceQuickSort::RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
	{
		m_pArrayPiece[i].m_pPiece->RenderAlpha( pd3dDevice );
	}
}

void DxPieceQuickSort::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh )
{
	for( int i=(int)(m_dwPieceCount-1); i>=0; --i )
	{
		m_pArrayPiece[i].m_pPiece->RenderEff( pd3dDevice, pStaticMesh, pFrameMesh );
	}
}

void DxPieceQuickSort::FrameMoveONLY( const float fTime, const float fElapsedTime )
{
	for( DWORD i=0; i<m_dwPieceCountONLY; ++i )
	{
		m_pArrayPieceONLY[i].m_pPiece->FrameMove( fTime, fElapsedTime );
	}
}

void DxPieceQuickSort::RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh )
{
	// Note : SetRenderState
	DWORD dwAlphaBlendEnable;
	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlendEnable );

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,	D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	D3DMATERIALQ sMaterial;
	D3DUtil_InitMaterial( sMaterial, 1.f, 1.f, 1.0f, 1.f );
	switch( DxMeshes::g_emMeshMeterial )
	{
	case EM_MESH_MATERIAL:
		// Note : Change for Alpha 0.5f
		D3DMATERIALQ sMaterial;
		sMaterial.Diffuse.r = sMaterial.Diffuse.g = sMaterial.Diffuse.b = 1.f;
		sMaterial.Ambient.r = sMaterial.Ambient.g = sMaterial.Ambient.b = 1.f;
		sMaterial.Specular.a = sMaterial.Specular.r = sMaterial.Specular.g = sMaterial.Specular.b = 0.f;
		sMaterial.Emissive.a = sMaterial.Emissive.r = sMaterial.Emissive.g = sMaterial.Emissive.b = 0.f;
		break;
	case EM_MESH_MATERIAL_RED:
		sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
		sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
		sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
		pd3dDevice->SetMaterial( &sMaterial );
		break;
	};

	DxPiece* pPiece(NULL);
	for( int i=(int)(m_dwPieceCountONLY-1); i>=0; --i )
	{
		pPiece = m_pArrayPieceONLY[i].m_pPiece;

		if( pPiece->m_bAplhaBlend )
		{
			sMaterial.Diffuse.a = sMaterial.Ambient.a = pPiece->m_fAlpha;
			pd3dDevice->SetMaterial( &sMaterial );

			pPiece->Render( pd3dDevice );
			pPiece->RenderAlpha( pd3dDevice );
			pPiece->RenderEff( pd3dDevice, pStaticMesh, pFrameMesh );
		}
	}

	switch( DxMeshes::g_emMeshMeterial )
	{
	case EM_MESH_MATERIAL:
		// Note : ReSet Material
		sMaterial.Diffuse.a = sMaterial.Ambient.a = 1.f;
		pd3dDevice->SetMaterial( &sMaterial );
		break;
	case EM_MESH_MATERIAL_RED:
		sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
		sMaterial.Ambient.g = sMaterial.Diffuse.g = 1.f;
		sMaterial.Ambient.b = sMaterial.Diffuse.b = 1.f;
		pd3dDevice->SetMaterial( &sMaterial );
		break;
	};

	// Note : SetRenderState
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlendEnable );
}

void DxPieceQuickSort::SetArrayMaxSize( DWORD dwCount )
{
	if( dwCount > m_dwArraySize )
	{
		SAFE_DELETE_ARRAY( m_pArrayPiece );

		m_dwArraySize = dwCount;
		m_pArrayPiece = new PIECEDISTANCE[m_dwArraySize];
	}

	if( dwCount > m_dwArraySizeONLY )
	{
		SAFE_DELETE_ARRAY( m_pArrayPieceONLY );

		m_dwArraySizeONLY = dwCount;
		m_pArrayPieceONLY = new PIECEDISTANCE[m_dwArraySizeONLY];
	}
}

void DxPieceQuickSort::Reset()
{
	m_dwPieceCount = 0;
}

void DxPieceQuickSort::InitPickAlpha()
{
	m_dwPieceCountONLY = 0;
}

void DxPieceQuickSort::InsertData( DxPiece* pPiece, DxPiece* pPiecePick, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength )
{
	float fTempLength(0.f);
	D3DXVECTOR3 vDir(0.f,0.f,0.f);

	DxPiece* pCur = pPiece;
	while( pCur )
	{
		vDir = pCur->m_vCenter - vFromPt;
		fTempLength = vDir.x*vDir.x + vDir.y*vDir.y + vDir.z*vDir.z;

		if( g_bPICK_ALPHA )
		{
			if( fLength > (fTempLength - pCur->m_fLength) )
			{
				// Note : Pick Check를 한다.
				if ( COLLISION::IsCollisionLineToAABB( vFromPt, vLookatPt, pCur->m_vStaticMax, pCur->m_vStaticMin ) )
				{
					if( pCur->m_fAlpha > 0.4f )	pCur->m_fAlpha -= m_fElapsedTime*0.6f;	// 1초에 0.6f가 줄어든다.
					else						pCur->m_fAlpha = 0.4f;

					pCur->m_bAplhaBlend = TRUE;
				}
				else if( pCur->m_bAplhaBlend )
				{
					if( pCur->m_fAlpha < 1.f ) 
					{
						pCur->m_fAlpha += m_fElapsedTime*0.6f;	// 1초에 0.6f가 늘어난다.
					}
					if( pCur->m_fAlpha > 1.f )
					{
						pCur->m_bAplhaBlend = FALSE;
						pCur->m_fAlpha = 1.f;
					}
				}
			}
			else if( pCur->m_bAplhaBlend )
			{
				if( pCur->m_fAlpha < 1.f ) 
				{
					pCur->m_fAlpha += m_fElapsedTime*0.6f;	// 1초에 0.6f가 늘어난다.
				}
				if( pCur->m_fAlpha > 1.f )
				{
					pCur->m_bAplhaBlend = FALSE;
					pCur->m_fAlpha = 1.f;
				}
			}
		}

		// Note : Pick AlphaBlending      
		if( pCur->m_bAplhaBlend )
		{
			m_pArrayPieceONLY[m_dwPieceCountONLY].m_fDistance = fTempLength;
			m_pArrayPieceONLY[m_dwPieceCountONLY].m_pPiece = pCur;
			++m_dwPieceCountONLY;
		}
		else
		{
			m_pArrayPiece[m_dwPieceCount].m_fDistance = fTempLength;
			m_pArrayPiece[m_dwPieceCount].m_pPiece = pCur;
			++m_dwPieceCount;
		}

		pCur = pCur->m_pTreeNext;
	}
}

void DxPieceQuickSort::QuickSortProcess()
{
	DxQuickSort( m_pArrayPiece, 0, m_dwPieceCount-1 );
	DxQuickSort( m_pArrayPieceONLY, 0, m_dwPieceCountONLY-1 );
}

void DxPieceQuickSort::DxQuickSort( PIECEDISTANCE pArray[], int ilo, int ihi ) 
{
    float pivot;	// pivot value for partitioning array
    int ulo, uhi;	// indices at ends of unpartitioned region
    int ieq;		// least index of array entry with value equal to pivot
 //   int tempEntry;	// temporary entry used for swapping

    if (ilo >= ihi) return;

    // Select a pivot value.
	pivot = pArray[(ilo + ihi)/2].m_fDistance;
    // Initialize ends of unpartitioned region and least index of entry
    // with value equal to pivot.
    ieq = ulo = ilo;
    uhi = ihi;
    // While the unpartitioned region is not empty, try to reduce its size.
    while( ulo <= uhi ) 
	{
		if( pArray[uhi].m_fDistance > pivot ) 
		{
			// Here, we can reduce the size of the unpartitioned region and
			// try again.
			uhi--;
		} 
		else 
		{
			// Here, A[uhi] <= pivot, so swap entries at indices ulo and
			// uhi.
			//tempEntry = pArray[ulo];
			//pArray[ulo] = pArray[uhi];
			//pArray[uhi] = tempEntry;

			Swap( pArray[uhi], pArray[ulo] );

			// After the swap, A[ulo] <= pivot.
			if (pArray[ulo].m_fDistance < pivot) 
			{
				// Swap entries at indices ieq and ulo.
				//tempEntry = pArray[ieq];
				//pArray[ieq] = pArray[ulo];
				//pArray[ulo] = tempEntry;
				Swap( pArray[ieq], pArray[ulo] );

				// After the swap, A[ieq] < pivot, so we need to change
				// ieq.
				ieq++;
				// We also need to change ulo, but we also need to do
				// that when A[ulo] = pivot, so we do it after this if
				// statement.
			}
			// Once again, we can reduce the size of the unpartitioned
			// region and try again.
			ulo++;
		}
    }

    // Now, all entries from index ilo to ieq - 1 are less than the pivot
    // and all entries from index uhi to ihi + 1 are greater than the
    // pivot.  So we have two regions of the array that can be sorted
    // recursively to put all of the entries in order.
    DxQuickSort( pArray, ilo, ieq - 1 );
    DxQuickSort( pArray, uhi + 1, ihi );
}


void DxPieceQuickSort::Swap( PIECEDISTANCE& pData1, PIECEDISTANCE& pData2 )
{
	PIECEDISTANCE pTemp;
	pTemp.m_fDistance = pData1.m_fDistance;
	pTemp.m_pPiece = pData1.m_pPiece;

	pData1.m_fDistance = pData2.m_fDistance;
	pData1.m_pPiece = pData2.m_pPiece;

	pData2.m_fDistance = pTemp.m_fDistance;
	pData2.m_pPiece = pTemp.m_pPiece;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//											N	S		P	I	E	C	E		T	R	E	E
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSPIECETREE
{
	BOOL OptimizeDivide( DxPieceNode* pTree, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin )
	{
		float fLengthX = vMax.x - vMin.x;
		float fLengthY = vMax.y - vMin.y;
		float fLengthZ = vMax.z - vMin.z;
		fLengthX = fabsf(fLengthX);
		fLengthY = fabsf(fLengthY);
		fLengthZ = fabsf(fLengthZ);

		float fHalfX = (vMax.x + vMin.x)*0.5f;
		float fHalfY = (vMax.y + vMin.y)*0.5f;
		float fHalfZ = (vMax.z + vMin.z)*0.5f;

		// 가장 긴 선분을 분할 한다.
		int nDivide = 0;
		if( fLengthX>=fLengthY && fLengthX>=fLengthZ )	nDivide = 1;
		else if( fLengthY>=fLengthZ )					nDivide = 2;
		else											nDivide = 3;

		int nLeft = 0;
		int nRight = 0;
		D3DXVECTOR3 vMaxAABB, vMinAABB;
		DxPiece* pCur = pTree->m_pPiece;
		while( pCur )
		{
			vMaxAABB = pCur->m_vMax;
			vMinAABB = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMaxAABB, vMinAABB, pCur->m_matWorld );

			switch( nDivide )
			{
			case 1:
				if( vMinAABB.x > fHalfX )	++nRight;
				else						++nLeft;
				break;
			case 2:
				if( vMinAABB.y > fHalfY )	++nRight;
				else						++nLeft;
				break;
			case 3:
				if( vMinAABB.z > fHalfZ )	++nRight;
				else						++nLeft;
				break;
			};

			pCur = pCur->m_pTreeNext;
		}

		// 균일하게 분할이 안되어서 그만 만든다.
		if( !nRight || !nLeft )		return FALSE;

		// Note : 작업
		pTree->m_pLeftChild = new DxPieceNode;
		pTree->m_pRightChild = new DxPieceNode;
		DxPiece* pLoop = pTree->m_pPiece;
		while( pLoop )
		{
			pCur = pLoop;				// 현재것 저장
			pLoop = pLoop->m_pTreeNext;	// 다음것 미리 셋팅

			vMaxAABB = pCur->m_vMax;
			vMinAABB = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMaxAABB, vMinAABB, pCur->m_matWorld );

			switch( nDivide )
			{
			case 1:
				if( vMinAABB.x > fHalfX )
				{
					pCur->m_pTreeNext = pTree->m_pRightChild->m_pPiece;
					pTree->m_pRightChild->m_pPiece = pCur;
				}
				else
				{
					pCur->m_pTreeNext = pTree->m_pLeftChild->m_pPiece;
					pTree->m_pLeftChild->m_pPiece = pCur;
				}
				break;
			case 2:
				if( vMinAABB.y > fHalfY )
				{
					pCur->m_pTreeNext = pTree->m_pRightChild->m_pPiece;
					pTree->m_pRightChild->m_pPiece = pCur;
				}
				else
				{
					pCur->m_pTreeNext = pTree->m_pLeftChild->m_pPiece;
					pTree->m_pLeftChild->m_pPiece = pCur;
				}
				break;
			case 3:
				if( vMinAABB.z > fHalfZ )
				{
					pCur->m_pTreeNext = pTree->m_pRightChild->m_pPiece;
					pTree->m_pRightChild->m_pPiece = pCur;
				}
				else
				{
					pCur->m_pTreeNext = pTree->m_pLeftChild->m_pPiece;
					pTree->m_pLeftChild->m_pPiece = pCur;
				}
				break;
			};
		}

		// Note : 분할로써 내부의 값은 나뉘었다.
		pTree->m_pPiece = NULL;

		return TRUE;
	}

	void MakeTreeChild( DxPieceNode* pTree )
	{
		// FALSE 일 경우 더 이상 분할 불가
		if( !OptimizeDivide( pTree, pTree->m_vMax, pTree->m_vMin ) )	return;


		DxPiece* pCur = NULL;
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vMax;
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMaxAABB;
		D3DXVECTOR3 vMinAABB;

		// Left
		vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		pCur = pTree->m_pLeftChild->m_pPiece;
		while( pCur )
		{
			vMax = pCur->m_vMax;
			vMin = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMax, vMin, pCur->m_matWorld );


			if( vMaxAABB.x < vMax.x )	vMaxAABB.x = vMax.x;
			if( vMaxAABB.y < vMax.y )	vMaxAABB.y = vMax.y;
			if( vMaxAABB.z < vMax.z )	vMaxAABB.z = vMax.z;

			if( vMinAABB.x > vMin.x )	vMinAABB.x = vMin.x;
			if( vMinAABB.y > vMin.y )	vMinAABB.y = vMin.y;
			if( vMinAABB.z > vMin.z )	vMinAABB.z = vMin.z;

			pCur = pCur->m_pTreeNext;
		}

		vMaxAABB += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMinAABB -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		pTree->m_pLeftChild->m_vMax = vMaxAABB;
		pTree->m_pLeftChild->m_vMin = vMinAABB;

		MakeTreeChild( pTree->m_pLeftChild );

		// Right
		vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
		pCur = pTree->m_pRightChild->m_pPiece;
		while( pCur )
		{
			vMax = pCur->m_vMax;
			vMin = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMax, vMin, pCur->m_matWorld );

			if( vMaxAABB.x < vMax.x )	vMaxAABB.x = vMax.x;
			if( vMaxAABB.y < vMax.y )	vMaxAABB.y = vMax.y;
			if( vMaxAABB.z < vMax.z )	vMaxAABB.z = vMax.z;

			if( vMinAABB.x > vMin.x )	vMinAABB.x = vMin.x;
			if( vMinAABB.y > vMin.y )	vMinAABB.y = vMin.y;
			if( vMinAABB.z > vMin.z )	vMinAABB.z = vMin.z;


			pCur = pCur->m_pTreeNext;
		}

		vMaxAABB += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMinAABB -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		pTree->m_pRightChild->m_vMax = vMaxAABB;
		pTree->m_pRightChild->m_vMin = vMinAABB;

		MakeTreeChild( pTree->m_pRightChild );
	}

	void MakeTree ( PDXPIECENODE& pTree, DxPiece* pPiece )
	{
		D3DXVECTOR3 vMax;
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMaxAABB = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		D3DXVECTOR3 vMinAABB = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );

		D3DXVECTOR3 vPos;
		DxPiece* pCur = pPiece;
		while( pCur )
		{
			vMax = pCur->m_vMax;
			vMin = pCur->m_vMin;
			D3DXMatrixAABBTransform( vMax, vMin, pCur->m_matWorld );

			if( vMaxAABB.x < vMax.x )	vMaxAABB.x = vMax.x;
			if( vMaxAABB.y < vMax.y )	vMaxAABB.y = vMax.y;
			if( vMaxAABB.z < vMax.z )	vMaxAABB.z = vMax.z;

			if( vMinAABB.x > vMin.x )	vMinAABB.x = vMin.x;
			if( vMinAABB.y > vMin.y )	vMinAABB.y = vMin.y;
			if( vMinAABB.z > vMin.z )	vMinAABB.z = vMin.z;

			pCur->m_pTreeNext = pCur->m_pNext;	// Tree에서 작업을 위하여..
			pCur = pCur->m_pTreeNext;
		}

		// Note : 충돌을 위해서
		vMaxAABB += D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );
		vMinAABB -= D3DXVECTOR3( 0.0001f, 0.0001f, 0.0001f );

		pTree = new DxPieceNode;
		pTree->m_vMax = vMaxAABB;
		pTree->m_vMin = vMinAABB;
		pTree->m_pPiece = pPiece;

		MakeTreeChild( pTree );

		pPiece = NULL;
	}


	void InsertQuickSort( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, DxPieceQuickSort* pSort, DxPiece* pPiecePick,
							const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength )
	{
		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

		if( pTree->m_pPiece )
		{
			pSort->InsertData( pTree->m_pPiece, pPiecePick, vFromPt, vLookatPt, fLength );
		}

		if( pTree->m_pLeftChild )	InsertQuickSort( pd3dDevice, pTree->m_pLeftChild, pSort, pPiecePick, sCV, vFromPt, vLookatPt, fLength );
		if( pTree->m_pRightChild )	InsertQuickSort( pd3dDevice, pTree->m_pRightChild, pSort, pPiecePick, sCV, vFromPt, vLookatPt, fLength );
	}

	void CollisionLine( DxPieceNode* pTree, const CLIPVOLUME &sCV, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, BOOL& bColl )
	{
		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

		DxPiece* pCur = pTree->m_pPiece;
		while( pCur )
		{
			if( pCur->IsCollisionLine( vStart, vEnd, vCollision ) )	bColl = TRUE;
			pCur = pCur->m_pTreeNext;
		}

		if( pTree->m_pLeftChild )	CollisionLine( pTree->m_pLeftChild, sCV, vStart, vEnd, vCollision, bColl );
		if( pTree->m_pRightChild )	CollisionLine( pTree->m_pRightChild, sCV, vStart, vEnd, vCollision, bColl );
	}

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const CLIPVOLUME &sCV )
	{
		if ( !COLLISION::IsCollisionVolume( sCV, pTree->m_vMax, pTree->m_vMin ) )	return;

		if( pTree->m_pPiece )
		{
			pTree->m_pPiece->Render_Reflect( pd3dDevice );
		}

		if( pTree->m_pLeftChild )	Render_Reflect( pd3dDevice, pTree->m_pLeftChild, sCV );
		if( pTree->m_pRightChild )	Render_Reflect( pd3dDevice, pTree->m_pRightChild, sCV );
	}

	void Save( DxPieceNode* pNode, CSerialFile& SFile )
	{
		BOOL bExist = FALSE;
		if( pNode )
		{
			bExist = TRUE;
			SFile << bExist;
		}
		else
		{
			bExist = FALSE;
			SFile << bExist;

			return;
		}

		pNode->Save( SFile );
	}

	void Load( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, CSerialFile& SFile, DxPieceEdit* pSrc )
	{
		BOOL bExist = FALSE;
		SFile >> bExist;
		if( !bExist )	return;

		pNode = new DxPieceNode;
		pNode->Load( pd3dDevice, SFile, pSrc );
	}
}
