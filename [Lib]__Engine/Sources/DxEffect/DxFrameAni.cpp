#include "pch.h"

#include <algorithm>
#include "./StlFunctions.h"

#include "./SerialFile.h"
#include "./DxRenderStates.h"
#include "./TextureManager.h"

#include "./DxFrameMesh.h"
#include "./DxTexEffMan.h"
#include "./DxTexEffDiffuse.h"
#include "./DxTexEffSpecular.h"

#include "./DxFrameAni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------------------------------------------
//	Note : 자신이 속해있는 Frame을 알기 위해 Link를 하고, Attribute 를 사용하여 VB,IB 호출횟수를 줄임.
//-------------------------------------------------------------------------------------------------------
DxFrameAttribute::DxFrameAttribute() :
	m_pFrameLink(NULL),
	m_dwVertexStart(0L),
	m_dwVertexCount(0L),
	m_dwFaceStart(0L),
	m_dwFaceCount(0L)
{
}

DxFrameAttribute::~DxFrameAttribute()
{
	CleanUp();

	SAFE_DELETE( m_pNext );
}

void DxFrameAttribute::CleanUp()
{
	m_pFrameLink = NULL;
	m_dwVertexStart = 0L;
	m_dwVertexCount = 0L;
	m_dwFaceStart = 0L;
	m_dwFaceCount = 0L;
}

void DxFrameAttribute::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	pd3dDevice->SetTransform( D3DTS_WORLD, &m_pFrameLink->matCombined );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwVertexStart, m_dwVertexCount, m_dwFaceStart*3, m_dwFaceCount );
}

void DxFrameAttribute::SetFrameAttribute( DxFrame* pFrame, const DWORD& dwVertexStart, const DWORD& dwVertexCount, 
															const DWORD& dwFaceStart, const DWORD& dwFaceCount )							
{
	m_pFrameLink = pFrame;
	m_dwVertexStart = dwVertexStart;
	m_dwVertexCount = dwVertexCount;
	m_dwFaceStart = dwFaceStart;
	m_dwFaceCount = dwFaceCount;
}

void DxFrameAttribute::Save( CSerialFile& SFile )
{
	SFile << m_dwVertexStart;
	SFile << m_dwVertexCount;
	SFile << m_dwFaceStart;
	SFile << m_dwFaceCount;

	if( m_pFrameLink->szName )
	{
		SFile << (BOOL)TRUE;
		UINT nCount = 0;
		nCount = (UINT)strlen(m_pFrameLink->szName)+1;
		SFile << nCount;
		SFile.WriteBuffer( m_pFrameLink->szName, sizeof(char)*nCount );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}
}

void DxFrameAttribute::Load( CSerialFile& SFile, DxAnimationMan* pAniManHead )
{
	CleanUp();

	SFile >> m_dwVertexStart;
	SFile >> m_dwVertexCount;
	SFile >> m_dwFaceStart;
	SFile >> m_dwFaceCount;

	BOOL bExist;
	SFile >> bExist;
	if( bExist )
	{
		UINT nCount = 0;
		SFile >> nCount;
		char* pFrameName = new char[nCount];
		SFile.ReadBuffer( pFrameName, sizeof(char)*nCount );

		m_pFrameLink = pAniManHead->FindFrame( pFrameName );

		SAFE_DELETE_ARRAY( pFrameName );
	}
}


//-------------------------------------------------------------------------------------------------------
//	Note : DxFrameAniVBTEX 안에 있는 struct ANIMESH;
//-------------------------------------------------------------------------------------------------------
DxFrameAniVBTEX::ANIMESH::ANIMESH() :
	m_dwVertices(0L),
	m_pVB(NULL),
	m_dwFaces(0L),
	m_pIB(NULL),
	m_pFrameAttrib(NULL),
	m_pNext(NULL)
{
}

DxFrameAniVBTEX::ANIMESH::~ANIMESH()
{
	CleanUp();

	SAFE_DELETE( m_pNext );	// 밖에서 사용하는 것임. 함부로 건들면 안됨.
}

void DxFrameAniVBTEX::ANIMESH::CleanUp()
{
	m_dwVertices = 0L;
	m_dwFaces = 0L;
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pIB );

	SAFE_DELETE( m_pFrameAttrib );
}

void DxFrameAniVBTEX::ANIMESH::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, VERTEX* pArrayVB, const DWORD& dwVertices, 
											WORD* pArrayIB, const DWORD& dwFaces, DxFrameAttribute* pAttribute )
{
	m_pFrameAttrib = pAttribute;

	m_dwVertices = dwVertices;
	m_dwFaces = dwFaces;

	SAFE_RELEASE( m_pVB );
	pd3dDevice->CreateVertexBuffer( sizeof(VERTEX)*m_dwVertices, 0L, VERTEX::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );
	VERTEX* pVertices;
	m_pVB->Lock( 0L, 0L, (VOID**)&pVertices, 0L );
	memcpy( pVertices, pArrayVB, sizeof(VERTEX)*m_dwVertices );
	m_pVB->Unlock();

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( sizeof(WORD)*m_dwFaces*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	WORD* pIndices;
	m_pIB->Lock( 0L, 0L, (VOID**)&pIndices, 0L );
	memcpy( pIndices, pArrayIB, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock();
}

void DxFrameAniVBTEX::ANIMESH::Save( CSerialFile& SFile )
{
	SFile << m_dwVertices;
	SFile << m_dwFaces;

	VERTEX* pVertices;
	m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	SFile.WriteBuffer( pVertices, sizeof(VERTEX)*m_dwVertices );
	m_pVB->Unlock();

	WORD* pIndices;
	m_pIB->Lock( 0, 0, (VOID**)&pIndices, 0L );
	SFile.WriteBuffer( pIndices, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock();

	UINT nCount = 0;
	DxFrameAttribute* pCur = m_pFrameAttrib;
	while( pCur )
	{
		++nCount;
		pCur = pCur->m_pNext;
	}

	SFile << nCount;
	
	pCur = m_pFrameAttrib;
	while( pCur )
	{
		pCur->Save( SFile );
		pCur = pCur->m_pNext;
	}
}

void DxFrameAniVBTEX::ANIMESH::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAniManHead )
{
	// Note : 깔끔하게 지워준다.
	CleanUp();

	// Note : 
	SFile >> m_dwVertices;
	SFile >> m_dwFaces;

	SAFE_RELEASE( m_pVB );
	pd3dDevice->CreateVertexBuffer( sizeof(VERTEX)*m_dwVertices, 0L, VERTEX::FVF, D3DPOOL_MANAGED, &m_pVB, NULL );
	VERTEX* pVertices;
	m_pVB->Lock( 0L, 0L, (VOID**)&pVertices, 0L );
	SFile.ReadBuffer( pVertices, sizeof(VERTEX)*m_dwVertices );
	m_pVB->Unlock();

	SAFE_RELEASE( m_pIB );
	pd3dDevice->CreateIndexBuffer( sizeof(WORD)*m_dwFaces*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	WORD* pIndices;
	m_pIB->Lock( 0L, 0L, (VOID**)&pIndices, 0L );
	SFile.ReadBuffer( pIndices, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock();

	UINT nCount = 0;
	SFile >> nCount;

	for( UINT i=0; i<nCount; ++i )
	{
		DxFrameAttribute* pCur = new DxFrameAttribute;
		pCur->Load( SFile, pAniManHead );

		pCur->m_pNext = m_pFrameAttrib;
		m_pFrameAttrib = pCur;
	}

	// Note : 음.......... m_pFrameLink 를 설정해 줘야 하는데.. 아직 못하고 있다.. 빨리 하자. ㅡ,.ㅡ;
}


//-------------------------------------------------------------------------------------------------------
//	Note : Texture 중심으로 만들어 진다. VB와 IB를 모았다.
//			Texture 가 같지만 분리 되어 있는 Mesh를 모은것이다.
//-------------------------------------------------------------------------------------------------------
const DWORD DxFrameAniVBTEX::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;

const DWORD DxFrameAniVBTEX::VERSION = 0x0100;

DxFrameAniVBTEX::DxFrameAniVBTEX() :
	m_pArrayVB(NULL),
	m_pArrayIB(NULL),
	m_dwNumVB(0L),
	m_dwFaces(0L),
	m_pAniMesh(NULL),
	m_pTexName(NULL),
	m_pTex(NULL),
	m_emType(TextureManager::EMTT_NORMAL),
	m_dwFlag(0L),
	m_pFrameAttribHead(NULL)
{
}

DxFrameAniVBTEX::~DxFrameAniVBTEX()
{
	CleanUp();
}

void DxFrameAniVBTEX::CleanUp()
{
	m_dwNumVB = 0L;
	m_dwFaces = 0L;
	SAFE_DELETE_ARRAY( m_pArrayVB );
	SAFE_DELETE_ARRAY( m_pArrayIB );
	SAFE_DELETE( m_pFrameAttribHead );

	SAFE_DELETE( m_pAniMesh );
	TextureManager::ReleaseTexture( m_pTexName, m_pTex );
	SAFE_DELETE_ARRAY( m_pTexName );
}

void DxFrameAniVBTEX::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !m_pTex )
	{
		TextureManager::GetTexture( m_pTexName, m_pTex );
		m_emType = TextureManager::GetTexType( m_pTexName );

		if( !m_pTex )	return;
	}
	pd3dDevice->SetTexture( 0, m_pTex );
	
	float fBias = -0.0000f;
	switch( m_emType )
	{
	case TextureManager::EMTT_ALPHA_HARD:
		DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );	
		break;
	case TextureManager::EMTT_ALPHA_SOFT:
		DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
		break;
	case TextureManager::EMTT_ALPHA_SOFT01:
		fBias = -0.0001f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
		break;
	case TextureManager::EMTT_ALPHA_SOFT02:
		fBias = -0.0002f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
		break;
	}

	RenderOnly( pd3dDevice );

	switch( m_emType )
	{
	case TextureManager::EMTT_ALPHA_HARD:
		DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
		break;
	case TextureManager::EMTT_ALPHA_SOFT:
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
		break;
	case TextureManager::EMTT_ALPHA_SOFT01:
		fBias = 0.0000f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
		break;
	case TextureManager::EMTT_ALPHA_SOFT02:
		fBias = 0.0000f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
		break;
	}
}

void DxFrameAniVBTEX::RenderOnly( LPDIRECT3DDEVICEQ pd3dDevice )
{
	ANIMESH* pCur = m_pAniMesh;
	while( pCur )
	{
		pd3dDevice->SetFVF( VERTEX::FVF );
		pd3dDevice->SetStreamSource( 0, pCur->m_pVB, 0, sizeof(VERTEX) );
		pd3dDevice->SetIndices( pCur->m_pIB );

		DxFrameAttribute* pCurAttrib = pCur->m_pFrameAttrib;
		while( pCurAttrib )
		{
			pCurAttrib->Render( pd3dDevice );
			pCurAttrib = pCurAttrib->m_pNext;
		}

		pCur = pCur->m_pNext;
	}
}

void DxFrameAniVBTEX::RenderTexEff( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pmapTexEff )
{
	if( !pmapTexEff )					return;	// TexEff 자체가 없다면 안함.
	if( !(m_dwFlag&DEF_TEXEFF_ALL) )	return;	// 셋팅된 효과가 없다면 안함.

	DxTextureEffMan::MAPTEXEFF_ITER iter = pmapTexEff->find( m_pTexName );
	if( iter!=pmapTexEff->end() )
	{
		DxTexEffBase* pCur = (*iter).second;
		while( pCur )
		{
			switch( pCur->GetTypeID() )
			{
			case DEF_TEXEFF_DIFFUSE:
				((DxTexEffDiffuse*)pCur)->SetRenderBegin( pd3dDevice );
				RenderOnly( pd3dDevice );
				((DxTexEffDiffuse*)pCur)->SetRenderEnd( pd3dDevice );
				break;
			case DEF_TEXEFF_SPECULAR:
				((DxTexEffSpecular*)pCur)->SetRenderBegin( pd3dDevice );
				RenderOnly( pd3dDevice );
				((DxTexEffSpecular*)pCur)->SetRenderEnd( pd3dDevice );
				break;
			};

			pCur = pCur->m_pNext;
		} 
	}
}

void DxFrameAniVBTEX::AddCloneMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, LPD3DXMESH pMesh, 
								   const DWORD& dwVertexStart, const DWORD& dwVertexCount, 
									const DWORD& dwFaceStart, const DWORD& dwFaceCount )
{
	// Note : 점 갯수가 65000가 넘어가면 INDEX64 써야 하므로 좋지 않다.
	//		그래서 일단 VB, IB를 만들어서 자른다.
	if( m_dwNumVB + dwVertexCount > 65000 )
	{
		CreateMesh( pd3dDevice );
	}

	DWORD dwNumPREV = 0;

	if( m_pArrayVB )
	{
		// Note : 임시 복제
		VERTEX* pArrayTEMP = new VERTEX[m_dwNumVB];
		memcpy( pArrayTEMP, m_pArrayVB, sizeof(VERTEX)*m_dwNumVB );

		// Note : Count
		dwNumPREV = m_dwNumVB;
		m_dwNumVB += dwVertexCount;

		// Note : 다시 복원
		SAFE_DELETE_ARRAY( m_pArrayVB );
		m_pArrayVB = new VERTEX[m_dwNumVB];
		memcpy( m_pArrayVB, pArrayTEMP, sizeof(VERTEX)*dwNumPREV );
		SAFE_DELETE_ARRAY( pArrayTEMP );

		// Note : 새로운 데이터 셋팅
		VERTEX* pVertices;
		LPDIRECT3DVERTEXBUFFERQ pVB;
		pMesh->GetVertexBuffer( &pVB );
		pVB->Lock( sizeof(VERTEX)*dwVertexStart, sizeof(VERTEX)*dwVertexCount, (VOID**)&pVertices, D3DLOCK_READONLY );
		memcpy( m_pArrayVB+dwNumPREV, pVertices, sizeof(VERTEX)*dwVertexCount );
		pVB->Unlock();
	}
	else
	{
		dwNumPREV = 0;
		m_dwNumVB = dwVertexCount;

		m_pArrayVB = new VERTEX[m_dwNumVB];
		LPDIRECT3DVERTEXBUFFERQ pVB;
		pMesh->GetVertexBuffer( &pVB );

		VERTEX* pVertices;
		pVB->Lock( sizeof(VERTEX)*dwVertexStart, sizeof(VERTEX)*dwVertexCount, (VOID**)&pVertices, D3DLOCK_READONLY );
		memcpy( m_pArrayVB, pVertices, sizeof(VERTEX)*dwVertexCount );
		pVB->Unlock();
	}

	//		dwNumPREV;		// 현재 Attribute 의 VertexStart
	//		dwVertexCount;	// 현재 Attribute 의 VertexCount

	DWORD dwFacesNUMPREV = 0;

	if( m_pArrayIB )
	{
		// Note : 임시 복제
		WORD* pArrayTEMP = new WORD[m_dwFaces*3];
		memcpy( pArrayTEMP, m_pArrayIB, sizeof(WORD)*m_dwFaces*3 );

		// Note : Count
		dwFacesNUMPREV = m_dwFaces;
		m_dwFaces += dwFaceCount;

		// Note : 다시 복원
		SAFE_DELETE_ARRAY( m_pArrayIB );
		m_pArrayIB = new WORD[m_dwFaces*3];
		memcpy( m_pArrayIB, pArrayTEMP, sizeof(WORD)*dwFacesNUMPREV*3 );
		SAFE_DELETE_ARRAY( pArrayTEMP );

		// Note : 새로운 데이터 셋팅
		WORD* pIndices;
		LPDIRECT3DINDEXBUFFERQ pIB;
		pMesh->GetIndexBuffer( &pIB );
		pIB->Lock( sizeof(WORD)*dwFaceStart*3, sizeof(WORD)*dwFaceCount*3, (VOID**)&pIndices, D3DLOCK_READONLY );

		UINT nCount = 0;
		for( UINT i=dwFacesNUMPREV; i<dwFacesNUMPREV+dwFaceCount; ++i, ++nCount )
		{
			m_pArrayIB[i*3+0] = (WORD)(pIndices[nCount*3+0] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+1] = (WORD)(pIndices[nCount*3+1] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+2] = (WORD)(pIndices[nCount*3+2] - dwVertexStart + dwNumPREV);
		}

		pIB->Unlock();
	}
	else
	{
		dwFacesNUMPREV = 0;
		m_dwFaces = dwFaceCount;

		m_pArrayIB = new WORD[dwFaceCount*3];
		LPDIRECT3DINDEXBUFFERQ pIB;
		pMesh->GetIndexBuffer( &pIB );

		WORD* pIndices;
		pIB->Lock( sizeof(WORD)*dwFaceStart*3, sizeof(WORD)*dwFaceCount*3, (VOID**)&pIndices, D3DLOCK_READONLY );

		UINT nCount = 0;
		for( UINT i=dwFacesNUMPREV; i<dwFacesNUMPREV+dwFaceCount; ++i, ++nCount )
		{
			m_pArrayIB[i*3+0] = (WORD)(pIndices[nCount*3+0] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+1] = (WORD)(pIndices[nCount*3+1] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+2] = (WORD)(pIndices[nCount*3+2] - dwVertexStart + dwNumPREV);
		}

		pIB->Unlock();
	}

	//		dwFacesNUMPREV;	// 현재 Attribute 의 FaceStart
	//		dwFaceCount;	// 현재 Attribute 의 FaceCount

	// Note : 리스트 형식으로 넣기
	DxFrameAttribute* pNew = new DxFrameAttribute;
	pNew->SetFrameAttribute( pFrame, dwNumPREV, dwVertexCount, dwFacesNUMPREV, dwFaceCount );
	pNew->m_pNext =	m_pFrameAttribHead;
	m_pFrameAttribHead = pNew;
}

void DxFrameAniVBTEX::AddCloneMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB,
						const DWORD& dwVertexStart, const DWORD& dwVertexCount, const DWORD& dwFaceStart, const DWORD& dwFaceCount )
{
	// Note : 점 갯수가 65000가 넘어가면 INDEX64 써야 하므로 좋지 않다.
	//		그래서 일단 VB, IB를 만들어서 자른다.
	if( m_dwNumVB + dwVertexCount > 65000 )
	{
		CreateMesh( pd3dDevice );
	}

	DWORD dwNumPREV = 0;

	if( m_pArrayVB )
	{
		// Note : 임시 복제
		VERTEX* pArrayTEMP = new VERTEX[m_dwNumVB];
		memcpy( pArrayTEMP, m_pArrayVB, sizeof(VERTEX)*m_dwNumVB );

		// Note : Count
		dwNumPREV = m_dwNumVB;
		m_dwNumVB += dwVertexCount;

		// Note : 다시 복원
		SAFE_DELETE_ARRAY( m_pArrayVB );
		m_pArrayVB = new VERTEX[m_dwNumVB];
		memcpy( m_pArrayVB, pArrayTEMP, sizeof(VERTEX)*dwNumPREV );
		SAFE_DELETE_ARRAY( pArrayTEMP );

		// Note : 새로운 데이터 셋팅
		VERTEX* pVertices;
		pVB->Lock( sizeof(VERTEX)*dwVertexStart, sizeof(VERTEX)*dwVertexCount, (VOID**)&pVertices, D3DLOCK_READONLY );
		memcpy( &m_pArrayVB[dwNumPREV], pVertices, sizeof(VERTEX)*dwVertexCount );
		pVB->Unlock();
	}
	else
	{
		dwNumPREV = 0;
		m_dwNumVB = dwVertexCount;

		m_pArrayVB = new VERTEX[m_dwNumVB];

		VERTEX* pVertices;
		pVB->Lock( sizeof(VERTEX)*dwVertexStart, sizeof(VERTEX)*dwVertexCount, (VOID**)&pVertices, D3DLOCK_READONLY );
		memcpy( m_pArrayVB, pVertices, sizeof(VERTEX)*dwVertexCount );
		pVB->Unlock();
	}

	//		dwNumPREV;		// 현재 Attribute 의 VertexStart
	//		dwVertexCount;	// 현재 Attribute 의 VertexCount

	DWORD dwFacesNUMPREV = 0;

	if( m_pArrayIB )
	{
		// Note : 임시 복제
		WORD* pArrayTEMP = new WORD[m_dwFaces*3];
		memcpy( pArrayTEMP, m_pArrayIB, sizeof(WORD)*m_dwFaces*3 );

		// Note : Count
		dwFacesNUMPREV = m_dwFaces;
		m_dwFaces += dwFaceCount;

		// Note : 다시 복원
		SAFE_DELETE_ARRAY( m_pArrayIB );
		m_pArrayIB = new WORD[m_dwFaces*3];
		memcpy( m_pArrayIB, pArrayTEMP, sizeof(WORD)*dwFacesNUMPREV*3 );
		SAFE_DELETE_ARRAY( pArrayTEMP );

		// Note : 새로운 데이터 셋팅
		WORD* pIndices;
		pIB->Lock( sizeof(WORD)*dwFaceStart*3, sizeof(WORD)*dwFaceCount*3, (VOID**)&pIndices, D3DLOCK_READONLY );

		UINT nCount = 0;
		for( UINT i=dwFacesNUMPREV; i<dwFacesNUMPREV+dwFaceCount; ++i, ++nCount )
		{
			m_pArrayIB[i*3+0] = (WORD)(pIndices[nCount*3+0] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+1] = (WORD)(pIndices[nCount*3+1] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+2] = (WORD)(pIndices[nCount*3+2] - dwVertexStart + dwNumPREV);
		}

		pIB->Unlock();
	}
	else
	{
		dwFacesNUMPREV = 0;
		m_dwFaces = dwFaceCount;

		m_pArrayIB = new WORD[dwFaceCount*3];

		WORD* pIndices;
		pIB->Lock( sizeof(WORD)*dwFaceStart*3, sizeof(WORD)*dwFaceCount*3, (VOID**)&pIndices, D3DLOCK_READONLY );

		UINT nCount = 0;
		for( UINT i=dwFacesNUMPREV; i<dwFacesNUMPREV+dwFaceCount; ++i, ++nCount )
		{
			m_pArrayIB[i*3+0] = (WORD)(pIndices[nCount*3+0] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+1] = (WORD)(pIndices[nCount*3+1] - dwVertexStart + dwNumPREV);
			m_pArrayIB[i*3+2] = (WORD)(pIndices[nCount*3+2] - dwVertexStart + dwNumPREV);
		}

		pIB->Unlock();
	}

	//		dwFacesNUMPREV;	// 현재 Attribute 의 FaceStart
	//		dwFaceCount;	// 현재 Attribute 의 FaceCount

	// Note : 리스트 형식으로 넣기
	DxFrameAttribute* pNew = new DxFrameAttribute;
	pNew->SetFrameAttribute( pFrame, dwNumPREV, dwVertexCount, dwFacesNUMPREV, dwFaceCount );
	pNew->m_pNext =	m_pFrameAttribHead;
	m_pFrameAttribHead = pNew;
}

void DxFrameAniVBTEX::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, const char* pName, DxTextureEffMan* pTexEffMan )
{
	// Note : 메쉬의 생성
	CreateMesh( pd3dDevice );

	// Note : 텍스쳐 관련 셋팅
	if( pName )
	{
		TextureManager::ReleaseTexture( m_pTexName, m_pTex );
		SAFE_DELETE_ARRAY( m_pTexName );
		size_t nStrLen = strlen(pName)+1;
		m_pTexName = new char[ nStrLen ];
		StringCchCopy( m_pTexName, nStrLen, pName );
		TextureManager::LoadTexture( m_pTexName, pd3dDevice, m_pTex, 0, 0, TRUE );

		if( m_pTex )	m_emType = TextureManager::GetTexType( m_pTexName );
	}

	m_dwFlag = 0L;

	DxTextureEffMan::MAPTEXEFF_ITER iter = pTexEffMan->GetMapTexEff()->begin();
	for( ; iter!=pTexEffMan->GetMapTexEff()->end(); ++iter )
	{
		DWORD dwTypeID = (*iter).second->GetTypeID();
		switch( dwTypeID )
		{
		case DEF_TEXEFF_DIFFUSE:
			m_dwFlag |= dwTypeID;
			break;
		case DEF_TEXEFF_SPECULAR:
			m_dwFlag |= dwTypeID;
			break;
		};
	}
}

void DxFrameAniVBTEX::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	ANIMESH* pAniMesh = new ANIMESH;
	pAniMesh->CreateMesh( pd3dDevice, m_pArrayVB, m_dwNumVB, m_pArrayIB, m_dwFaces, m_pFrameAttribHead );

	m_dwNumVB = 0L;
	m_dwFaces = 0L;
	SAFE_DELETE_ARRAY( m_pArrayVB );
	SAFE_DELETE_ARRAY( m_pArrayIB );

	//Note : 리스트로 쓴다.
	pAniMesh->m_pNext =	m_pAniMesh;
	m_pAniMesh = pAniMesh;

	// 정리
	m_pFrameAttribHead = NULL;
}

void DxFrameAniVBTEX::Save( CSerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_01 );
	{
		SFile << m_dwFlag;

		UINT nCount = 0;
		ANIMESH* pCur = m_pAniMesh;
		while( pCur )
		{
			++nCount;
			pCur = pCur->m_pNext;
		}

		SFile << nCount;

		pCur = m_pAniMesh;
		while( pCur )
		{
			pCur->Save( SFile );
			pCur = pCur->m_pNext;
		}
	}
	SFile.EndBlock( EMBLOCK_01 );
}

void DxFrameAniVBTEX::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAniManHead, const char* pName )
{
	// Note : 데이터 정리
	CleanUp();

	// Note : 
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		SFile >> m_dwFlag;

		// Note : 텍스쳐 관련 셋팅
		TextureManager::ReleaseTexture( m_pTexName, m_pTex );
		SAFE_DELETE_ARRAY( m_pTexName );
		if( pName )
		{
			size_t nStrLen = strlen(pName)+1;
			m_pTexName = new char[ nStrLen ];
			StringCchCopy( m_pTexName, nStrLen, pName );
		}
		TextureManager::LoadTexture( m_pTexName, pd3dDevice, m_pTex, 0, 0, TRUE );
		if( m_pTex )	m_emType = TextureManager::GetTexType( m_pTexName );

		// Note : 데이터 로딩
		UINT nCount = 0;
		SFile >> nCount;

		for( UINT i=0; i<nCount; ++i )
		{
			ANIMESH* pCur = new ANIMESH;
			pCur->Load( pd3dDevice, SFile, pAniManHead );

			pCur->m_pNext = m_pAniMesh;
			m_pAniMesh = pCur;
		}
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}
}

//-------------------------------------------------------------------------------------------------------
//	Note : 1개의 Animation FrameRoot
//			여러개의 DxFrameAni가 있을 수 있다.
//-------------------------------------------------------------------------------------------------------
const DWORD DxFrameAni::VERSION = 0x0100;

DxFrameAni::DxFrameAni()	
{
}

DxFrameAni::~DxFrameAni()
{
	std::for_each( m_mapFrameAni.begin(), m_mapFrameAni.end(), std_afunc::DeleteMapObject() );
	m_mapFrameAni.clear();
}

void DxFrameAni::Render( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan::MAPTEXEFF* pTexEffList )
{
	MAPTEXFRAMEANI_ITER iter = m_mapFrameAni.begin();
	for( ; iter!=m_mapFrameAni.end(); ++iter )
	{
		(*iter).second->Render( pd3dDevice );
		(*iter).second->RenderTexEff( pd3dDevice, pTexEffList );
	}
}

void DxFrameAni::Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice )
{
	MAPTEXFRAMEANI_ITER iter = m_mapFrameAni.begin();
	for( ; iter!=m_mapFrameAni.end(); ++iter )
	{
		(*iter).second->RenderOnly( pd3dDevice );
	}
}

void DxFrameAni::SetFrameLink( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame )
{
	DxMeshes* pMeshs = pFrame->pmsMeshs;

	if( pMeshs )
	{
		DxOctreeMesh* pMesh = pMeshs->m_pDxOctreeMesh;
		if( pMesh )
		{
			D3DXATTRIBUTERANGE* pAttrib = pMesh->m_pAttribTable;
			DWORD dwAttrib = pMesh->m_dwAttribTableSize;

			UINT nAttribIdCUR;
			for( UINT i=0; i<dwAttrib; ++ i )
			{
				nAttribIdCUR = pAttrib[i].AttribId;

				MAPTEXFRAMEANI_ITER iter = m_mapFrameAni.find( pMeshs->strTextureFiles[ nAttribIdCUR ].GetString() );
				if( iter!=m_mapFrameAni.end() )
				{
					// Note : 동일한 Texture를 쓰는 것이 있다.
					//		데이터 추가를 해야함.
					(*iter).second->AddCloneMesh( pd3dDevice, pFrame, pMesh->m_pVB, pMesh->m_pIB,
										pAttrib[nAttribIdCUR].VertexStart, pAttrib[nAttribIdCUR].VertexCount,
										pAttrib[nAttribIdCUR].FaceStart, pAttrib[nAttribIdCUR].FaceCount);
				}	
				else
				{
					// Note : 새롭게 생성
					DxFrameAniVBTEX* pData = new DxFrameAniVBTEX;
					pData->AddCloneMesh( pd3dDevice, pFrame, pMesh->m_pVB, pMesh->m_pIB,
										pAttrib[nAttribIdCUR].VertexStart, pAttrib[nAttribIdCUR].VertexCount,
										pAttrib[nAttribIdCUR].FaceStart, pAttrib[nAttribIdCUR].FaceCount);

					m_mapFrameAni.insert( std::make_pair( pMeshs->strTextureFiles[ nAttribIdCUR ].GetString(), pData ) );
				}
			}
		}

		pMeshs = pMeshs->pMeshNext;
	}
}

//void DxFrameAni::SetFrameLink( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame )
//{
//	DxMeshes* pMeshs = pFrame->pmsMeshs;
//
//	if( pMeshs )
//	{
//		LPD3DXMESH pMesh = pMeshs->m_pLocalMesh;
//		if( pMesh )
//		{
//			DWORD dwAttrib = 0L;
//			pMesh->GetAttributeTable( NULL, &dwAttrib );
//			D3DXATTRIBUTERANGE* pAttrib = new D3DXATTRIBUTERANGE[dwAttrib];
//			pMesh->GetAttributeTable( pAttrib, &dwAttrib );
//
//			UINT nAttribIdCUR;
//			for( UINT i=0; i<dwAttrib; ++ i )
//			{
//				nAttribIdCUR = pAttrib[i].AttribId;
//
//				MAPTEXFRAMEANI_ITER iter = m_mapFrameAni.find( pMeshs->strTextureFiles[ nAttribIdCUR ].GetString(MAX_PATH) );
//				if( iter!=m_mapFrameAni.end() )
//				{
//					// Note : 동일한 Texture를 쓰는 것이 있다.
//					//		데이터 추가를 해야함.
//					(*iter).second->AddCloneMesh( pd3dDevice, pFrame, pMesh, 
//										pAttrib[nAttribIdCUR].VertexStart, pAttrib[nAttribIdCUR].VertexCount,
//										pAttrib[nAttribIdCUR].FaceStart, pAttrib[nAttribIdCUR].FaceCount);
//				}	
//				else
//				{
//					// Note : 새롭게 생성
//					DxFrameAniVBTEX* pData = new DxFrameAniVBTEX;
//					pData->AddCloneMesh( pd3dDevice, pFrame, pMesh, 
//										pAttrib[nAttribIdCUR].VertexStart, pAttrib[nAttribIdCUR].VertexCount,
//										pAttrib[nAttribIdCUR].FaceStart, pAttrib[nAttribIdCUR].FaceCount);
//
//					m_mapFrameAni.insert( std::make_pair( pMeshs->strTextureFiles[ nAttribIdCUR ].GetString(MAX_PATH), pData ) );
//				}
//			}
//
//			SAFE_DELETE( pAttrib );
//		}
//
//		pMeshs = pMeshs->pMeshNext;
//	}
//}


void DxFrameAni::CreateMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEffMan )
{
	MAPTEXFRAMEANI_ITER iter = m_mapFrameAni.begin();
	for( ; iter!=m_mapFrameAni.end(); ++iter )
	{
		(*iter).second->CreateMesh( pd3dDevice, (*iter).first.c_str(), pTexEffMan );
	}
}

void DxFrameAni::Save( CSerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	{
		SFile << (UINT)m_mapFrameAni.size();

		MAPTEXFRAMEANI_ITER iter = m_mapFrameAni.begin();
		for( ; iter!=m_mapFrameAni.end(); ++iter )
		{
			SFile << (*iter).first;

			(*iter).second->Save( SFile );
		}
	}
	SFile.EndBlock();
}

void DxFrameAni::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxAnimationMan* pAniManHead )
{
	// Note : 초기화
	std::for_each( m_mapFrameAni.begin(), m_mapFrameAni.end(), std_afunc::DeleteMapObject() );
	m_mapFrameAni.clear();

	// Note : 
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		UINT nCount = 0;
		SFile >> nCount;

		std::string strName;
		for( UINT i=0; i<nCount; ++i )
		{
			SFile >> strName;
			DxFrameAniVBTEX* pNew = new DxFrameAniVBTEX;
			pNew->Load( pd3dDevice, SFile, pAniManHead, strName.c_str() );

			m_mapFrameAni.insert( std::make_pair( strName.c_str(), pNew ) );
		}
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}
}
