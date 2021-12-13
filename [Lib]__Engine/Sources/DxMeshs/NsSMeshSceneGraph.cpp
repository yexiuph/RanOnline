#include "pch.h"

#include <algorithm>
#include "StlFunctions.h"

#include "NsSMeshSceneGraph.h"

//----------------------------------------------------------------------------------------------------------------
//								A	t	t	r	i	b	u	t	e		I	B	
//----------------------------------------------------------------------------------------------------------------
AttributeIB::AttributeIB() :
	m_dwVertexStart(0),
	m_dwFaceStart(0),
	m_dwFaceCount(0),
	m_rIB(NULL)
{
}

AttributeIB::~AttributeIB()
{
}

//----------------------------------------------------------------------------------------------------------------
//							D	x		D	y	n	a	m	i	c		M	e	s	h
//----------------------------------------------------------------------------------------------------------------
IDirect3DDevice9* DxDynamicMesh::g_pd3dDevice = NULL;

DxDynamicMesh::DxDynamicMesh() :
//	m_pVertices(NULL),
//	m_pIndies(NULL),
	m_rTexture(NULL),
	m_pVB(NULL),
	//m_pIB(NULL),
	m_dwVertexCUR(0),
	m_dwFaceCUR(0),
	m_dwVertexMAX(0),
	m_dwFaceMAX(0)
{
}

DxDynamicMesh::~DxDynamicMesh()
{
	CleanUp();
}

void DxDynamicMesh::CleanUp()
{
	SAFE_RELEASE( m_pVB );

	std::for_each( m_vecAttributeIB.begin(), m_vecAttributeIB.end(), std_afunc::DeleteObject() );
	m_vecAttributeIB.clear();
}

void DxDynamicMesh::Reset()
{
	m_dwVertexCUR = 0;
	m_dwFaceCUR = 0;

	std::for_each( m_vecAttributeIB.begin(), m_vecAttributeIB.end(), std_afunc::DeleteObject() );
	m_vecAttributeIB.clear();
}

void DxDynamicMesh::InitVBIB( DWORD dwPointer )
{
	m_dwVertexCUR = 0;
	m_dwFaceCUR = 0;
	m_dwVertexMAX = 3000;
	m_dwFaceMAX = 1000;

	m_rTexture = (LPDIRECT3DTEXTURE9) dwPointer;

	g_pd3dDevice->CreateVertexBuffer( sizeof(VERTEX)*m_dwVertexMAX, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL ); 
	//g_pd3dDevice->CreateIndexBuffer( 3*m_dwFaceMAX*sizeof(WORD), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL );
}

void DxDynamicMesh::InsertVBIB( VERTEX* pVertices, WORD* pIndices, DWORD dwVertexNUM, DWORD dwFaceNUM )
{
	//// Note : 예상 크기를 넘었다. - VB
	//DWORD dwNeedVertexNUM = m_dwVertexCUR+dwVertexNUM;
	//if( dwNeedVertexNUM > m_dwVertexMAX )
	//{
	//	CreateDoubleVB( dwNeedVertexNUM );
	//}

	//// Note : 예상 크기를 넘었다. - IB
	//DWORD dwNeedFaceNUM = m_dwFaceCUR+dwFaceNUM;
	//if( dwNeedFaceNUM > m_dwFaceMAX )
	//{
	//	CreateDoubleIB( dwNeedFaceNUM );
	//}

	//memcpy( &m_pVertices[m_dwVertexCUR], pVertices, sizeof(VERTEX)*dwVertexNUM );
	//m_dwVertexCUR += dwVertexNUM;
	//
	//// Note : 
	//DWORD dwStartIndex = m_dwFaceCUR*3;
	//for( DWORD i=0; i<dwFaceNUM*3; ++i )
	//{
	//	m_pIndies[i+dwStartIndex] = pIndices[i] + ;
	//}
	//m_dwFaceCUR += dwFaceNUM;

	//// Note : 인덱스 쪽에서는 그냥 이렇게 사용하면 안 될꺼 같다.
	////memcpy( &m_pIndies[m_dwFaceCUR*3], pIndices, sizeof(VERTEX)*dwFaceNUM*3 );
	////m_dwFaceCUR += dwFaceNUM;
}

void DxDynamicMesh::InsertIB( LPDIRECT3DINDEXBUFFERQ pIB, const D3DXATTRIBUTERANGE& sAttribTable )
{
	////// Note : 예상 크기를 넘었다. - IB
	////DWORD dwNeedFaceNUM = m_dwFaceCUR+dwFaceCount;
	////if( dwNeedFaceNUM > m_dwFaceMAX )
	////{
	////	CreateDoubleIB( dwNeedFaceNUM );
	////}

	//// Note : 
	//WORD wData = (WORD)(m_dwVertexCUR - dwVertexStart);
	//DWORD dwStartIndex = m_dwFaceCUR*3;

	//WORD* pIndicesDEST;
	//m_pIB->Lock( dwStartIndex*sizeof(WORD), dwFaceCount*3*sizeof(WORD), (VOID**)&pIndicesDEST, D3DLOCK_DISCARD );
	//{
	//	//memcpy( &pIndicesDEST[m_dwFaceCUR*3], pIndices, sizeof(WORD)*3*dwFaceCount );

	//	DWORD dwIndexCount = dwFaceCount*3;
	//	for( DWORD i=0; i<dwIndexCount; ++i )
	//	{
	//		pIndicesDEST[i] = pIndices[i] + wData;
	//	}
	////	IndexAddData_ASSEM( pIndicesDEST, pIndices, nData, dwFaceCount*3 );

	//	m_dwFaceCUR += dwFaceCount;
	//}
	//m_pIB->Unlock();

	AttributeIB* pNew = new AttributeIB;
	pNew->m_dwBaseVertexIndex = m_dwVertexCUR - sAttribTable.VertexStart;
	pNew->m_dwVertexStart = sAttribTable.VertexStart;//m_dwVertexCUR;
	pNew->m_dwVertexCount = sAttribTable.VertexCount;
	pNew->m_dwFaceStart = sAttribTable.FaceStart;
	pNew->m_dwFaceCount = sAttribTable.FaceCount;
	pNew->m_rIB = pIB;

	m_vecAttributeIB.push_back( pNew );
}

void DxDynamicMesh::SetTexture( DWORD dwPointer )
{
	m_rTexture = (LPDIRECT3DTEXTURE9) dwPointer;
}

void DxDynamicMesh::CreateDoubleVB( DWORD dwMinVertexNUM )
{
	DWORD dwLimitVertexNUM = m_dwVertexMAX*2;
	if( dwLimitVertexNUM<dwMinVertexNUM )	// 요구 점 갯수를 맞추기 못하면
	{
		dwLimitVertexNUM = dwMinVertexNUM;	// Limit 까지 강제로 셋팅한다.
	}
	m_dwVertexMAX = dwLimitVertexNUM;

	SAFE_RELEASE( m_pVB );
	g_pd3dDevice->CreateVertexBuffer( sizeof(VERTEX)*m_dwVertexMAX, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL ); 
}

void DxDynamicMesh::CreateDoubleIB( DWORD dwMinFaceNUM )
{
	//DWORD dwTemp = m_dwFaceMAX*2;
	//if( dwTemp<dwMinFaceNUM )	// 요구 점 갯수를 맞추기 못하면
	//{
	//	dwTemp = dwMinFaceNUM;	// Limit 까지 강제로 셋팅한다.
	//}
	//m_dwFaceMAX = dwTemp;

	//SAFE_RELEASE( m_pIB );
	//g_pd3dDevice->CreateIndexBuffer( 3*m_dwFaceMAX*sizeof(WORD), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &m_pIB, NULL );
}

//void DxDynamicMesh::CreateDoubleVB( DWORD dwMinVertexNUM )
//{
//	DWORD dwLimitVertexNUM = m_dwVertexMAX*2;
//	if( dwLimitVertexNUM<dwMinVertexNUM )	// 요구 점 갯수를 맞추기 못하면
//	{
//		dwLimitVertexNUM = dwMinVertexNUM;	// Limit 까지 강제로 셋팅한다.
//	}
//	m_dwVertexMAX = dwLimitVertexNUM;
//
//	VERTEX* m_pTemp = new VERTEX[ m_dwVertexMAX ];
//	memcpy( m_pTemp, m_pVertices, sizeof(VERTEX)*m_dwVertexCUR );
//
//	SAFE_DELETE_ARRAY( m_pVertices );
//	m_pVertices = m_pTemp;
//}
//
//void DxDynamicMesh::CreateDoubleIB( DWORD dwMinFaceNUM )
//{
//	DWORD dwTemp = m_dwFaceMAX*2;
//	if( dwTemp<dwMinFaceNUM )	// 요구 점 갯수를 맞추기 못하면
//	{
//		dwTemp = dwMinFaceNUM;	// Limit 까지 강제로 셋팅한다.
//	}
//	m_dwFaceMAX = dwTemp;
//
//	WORD* m_pTemp = new WORD[ m_dwFaceMAX*3 ];
//	memcpy( m_pTemp, m_pIndies, sizeof(WORD)*m_dwFaceCUR*3 );
//
//	SAFE_DELETE_ARRAY( m_pIndies );
//	m_pIndies = m_pTemp;
//}

void DxDynamicMesh::CheckVBIB( DWORD dwVertexCount, DWORD dwFaceCount )
{
	// Note : 예상 크기를 넘었다. - VB
	DWORD dwNeedVertexNUM = m_dwVertexCUR+dwVertexCount;
	if( dwNeedVertexNUM > m_dwVertexMAX )
	{
		// 이전 데이터 모두 렌더함.
		Render();

		// 
		CreateDoubleVB( dwNeedVertexNUM );
	}

	//// Note : 예상 크기를 넘었다. - IB
	//DWORD dwNeedFaceNUM = m_dwFaceCUR+dwFaceCount;
	//if( dwNeedFaceNUM > m_dwFaceMAX )
	//{
	//	// 이전 데이터 모두 렌더함.
	//	Render();

	//	CreateDoubleIB( dwNeedFaceNUM );
	//}
}

void DxDynamicMesh::Clone( LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB )
{
	//DWORD dwSize(0);
	//VERTEX* pVertices(NULL);
	//dwSize = sizeof(VERTEX)*m_dwVertexCUR;
	//pVB->Lock( 0L, dwSize, (VOID**)&pVertices, D3DLOCK_DISCARD );
	//memcpy( pVertices, m_pVertices, dwSize );
	//pVB->Unlock();

	//WORD* pIndices(NULL);
	//dwSize = sizeof(WORD)*m_dwFaceCUR*3;
	//pIB->Lock( 0L, dwSize, (VOID**)&pIndices, D3DLOCK_DISCARD );
	//memcpy( pIndices, m_pIndies, dwSize );
	//pIB->Unlock();
}

BYTE* DxDynamicMesh::GetFreeVB( DWORD dwVertexNUM )
{
	//// Note : 예상 크기를 넘었다. - VB
	//DWORD dwNeedVertexNUM = m_dwVertexCUR+dwVertexNUM;
	//if( dwNeedVertexNUM > m_dwVertexMAX )
	//{
	//	// 이전 데이터 모두 렌더함.
	//	Render();

	//	// 
	//	CreateDoubleVB( dwNeedVertexNUM );
	//}

	BYTE* pReturn(NULL);
	m_pVB->Lock
	(
		sizeof(VERTEX)*m_dwVertexCUR, 
		sizeof(VERTEX)*dwVertexNUM, 
		(VOID**)&pReturn,
		D3DLOCK_DISCARD
	);

	m_dwVertexCUR += dwVertexNUM;

	//BYTE* pReturn = (BYTE*)&m_pVertices[m_dwVertexCUR];
	
	return pReturn;
}

void DxDynamicMesh::UnlockVB()
{
	m_pVB->Unlock();
}

void DxDynamicMesh::GetVertexFaceMAX( DWORD& dwVertexMAX, DWORD& dwFaceMAX )
{
	dwVertexMAX = m_dwVertexCUR;
	dwFaceMAX = m_dwFaceCUR;
}

void DxDynamicMesh::Render()
{
	g_pd3dDevice->SetTexture( 0, m_rTexture );

	g_pd3dDevice->SetFVF( VERTEX::FVF );
	g_pd3dDevice->SetStreamSource( 0, m_pVB, NULL, sizeof(VERTEX) );
	
	for( DWORD i=0; i<m_vecAttributeIB.size(); ++i )
	{
		const AttributeIB* sAttribIB = m_vecAttributeIB[i];

		g_pd3dDevice->SetIndices( sAttribIB->m_rIB );
	
		g_pd3dDevice->DrawIndexedPrimitive
		( 
			D3DPT_TRIANGLELIST, 
			sAttribIB->m_dwBaseVertexIndex,
			sAttribIB->m_dwVertexStart, 
			sAttribIB->m_dwVertexCount, 
			sAttribIB->m_dwFaceStart*3, 
			sAttribIB->m_dwFaceCount
		);
	}

	Reset();
}

//----------------------------------------------------------------------------------------------------------------
//			S	M	e	s	h		D	y	n	a	m	i	c		R	e	n	d	e	r		M	a	n
//----------------------------------------------------------------------------------------------------------------
SMeshDynamicRenderMan::SMeshDynamicRenderMan() :
	m_dwIndexCUR(0)
{
}

SMeshDynamicRenderMan::~SMeshDynamicRenderMan()
{
	std::for_each( m_vecDynamicRENDER.begin(), m_vecDynamicRENDER.end(), std_afunc::DeleteObject() );
	m_vecDynamicRENDER.clear();
}

void SMeshDynamicRenderMan::Reset()
{
	m_mapIndex.clear();

	for( DWORD i=0; i<m_vecDynamicRENDER.size(); ++i )
	{
		m_vecDynamicRENDER[i]->Reset();
	}
}

void SMeshDynamicRenderMan::Init( DWORD dwPointer, DWORD nCount )
{
	m_mapIndex.insert( std::make_pair( dwPointer, nCount ) );
}

void SMeshDynamicRenderMan::InsertVBIB( DWORD dwPointer, VERTEX* pVertices, WORD* pIndices, DWORD dwVertexNUM, DWORD dwFaceNUM )
{
	MAP_INDEX_ITER iter = m_mapIndex.find( dwPointer );
	if( iter==m_mapIndex.end() )	return;

	// Note : 갯수가 적다면 늘려야 한다.
	DWORD dwIndexCUR = (*iter).second;
	if( dwIndexCUR > m_vecDynamicRENDER.size() )
	{
		for( DWORD i=0; i<dwIndexCUR-m_vecDynamicRENDER.size(); ++i )
		{
			DxDynamicMesh* pNew = new DxDynamicMesh;
			m_vecDynamicRENDER.push_back( pNew );
		}
	}

	// 정보 삽입.
	m_vecDynamicRENDER[ dwIndexCUR ]->InsertVBIB( pVertices, pIndices, dwVertexNUM, dwFaceNUM );
}

void SMeshDynamicRenderMan::InsertIB( DWORD dwPointer, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwVertexStart, DWORD dwFaceStart, DWORD dwFaceCount )
{
	//MAP_INDEX_ITER iter = m_mapIndex.find( dwPointer );
	//if( iter==m_mapIndex.end() )	return;

	//m_vecDynamicRENDER[ (*iter).second ]->InsertIB( pIB, dwVertexStart, dwFaceStart, dwFaceCount );
}

void SMeshDynamicRenderMan::Clone( LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB )
{
	for( DWORD i=0; i<m_vecDynamicRENDER.size(); ++i )
	{
		m_vecDynamicRENDER[i]->Clone( pVB, pIB );
	}
}

BYTE* SMeshDynamicRenderMan::GetFreeVB( DWORD dwPointer, DWORD dwVertexNUM )
{
	MAP_INDEX_ITER iter = m_mapIndex.find( dwPointer );
	if( iter==m_mapIndex.end() )	return NULL;

	// Note : 갯수가 적다면 늘려야 한다.
	DWORD dwIndexCUR = (*iter).second;
	if( (dwIndexCUR+1) > m_vecDynamicRENDER.size() )
	{
		DWORD dwEnd = (dwIndexCUR+1)-m_vecDynamicRENDER.size();
		for( DWORD i=0; i<dwEnd; ++i )	
		{
			DxDynamicMesh* pNew = new DxDynamicMesh;
			pNew->InitVBIB( dwPointer );
			m_vecDynamicRENDER.push_back( pNew );
		}
	}

	return m_vecDynamicRENDER[ dwIndexCUR ]->GetFreeVB( dwVertexNUM );
}

BOOL SMeshDynamicRenderMan::IsGetIndex( DWORD dwPointer, DWORD& dwIndex )
{
	MAP_INDEX_ITER iter = m_mapIndex.find( dwPointer );
	if( iter==m_mapIndex.end() )	return FALSE;

	dwIndex = (*iter).second;

	// Note : 갯수가 적다면 늘려야 한다.
	m_dwIndexCUR = dwIndex;
	if( (m_dwIndexCUR+1) > m_vecDynamicRENDER.size() )
	{
		DWORD dwEnd = (m_dwIndexCUR+1)-m_vecDynamicRENDER.size();
		for( DWORD i=0; i<dwEnd; ++i )	
		{
			DxDynamicMesh* pNew = new DxDynamicMesh;
			pNew->InitVBIB( dwPointer );
			m_vecDynamicRENDER.push_back( pNew );
		}
	}

	// Note : 텍스쳐를 넣는다.
	m_vecDynamicRENDER[ m_dwIndexCUR ]->SetTexture( dwPointer );

	return TRUE;
}

BYTE* SMeshDynamicRenderMan::GetFreeVBInsertIB( DWORD dwIndex, LPDIRECT3DINDEXBUFFERQ pIB, const D3DXATTRIBUTERANGE& sAttribTable )
{
	m_dwIndexCUR = dwIndex;

	// Note : VB, IB가 유효한지 체크한다.
	m_vecDynamicRENDER[ m_dwIndexCUR ]->CheckVBIB( sAttribTable.VertexCount, sAttribTable.FaceCount );


	m_vecDynamicRENDER[ m_dwIndexCUR ]->InsertIB( pIB, sAttribTable );

	return m_vecDynamicRENDER[ m_dwIndexCUR ]->GetFreeVB( sAttribTable.VertexCount );
}

void SMeshDynamicRenderMan::UnlockVB()
{
	m_vecDynamicRENDER[ m_dwIndexCUR ]->UnlockVB();
}

void SMeshDynamicRenderMan::GetVertexFaceMAX( DWORD& dwVertexMAX, DWORD& dwFaceMAX )
{
	dwVertexMAX = 0;
	dwFaceMAX = 0;

	DWORD dwCurVertex(0);
	DWORD dwCurFace(0);
	for( DWORD i=0; i<m_vecDynamicRENDER.size(); ++i )
	{
		m_vecDynamicRENDER[i]->GetVertexFaceMAX( dwCurVertex, dwCurFace );

		if( dwVertexMAX < dwCurVertex )	dwVertexMAX = dwCurVertex;
		if( dwFaceMAX < dwCurFace )		dwFaceMAX = dwCurFace;
	}
}

void SMeshDynamicRenderMan::Render()
{
	for( DWORD i=0; i<m_vecDynamicRENDER.size(); ++i )
	{
		m_vecDynamicRENDER[i]->Render();
	}
}

//----------------------------------------------------------------------------------------------------------------
//					N	S		C	H	A	R		S	C	E	N	E		G	R	A	P	H
//----------------------------------------------------------------------------------------------------------------
namespace NSCHARSG
{
	struct COUNTVERTEXFACE
	{
		DWORD	dwCount;
		DWORD	dwVertex;
		DWORD	dwFace;
	};

	// Note : 텍스쳐 ID, 렌더용 텍스쳐 ID
	typedef std::map<DWORD,COUNTVERTEXFACE>	MAP_TEX;
	typedef MAP_TEX::iterator				MAP_TEX_ITER;
	typedef MAP_TEX::reverse_iterator		MAP_TEX_RITER;

	

	MAP_TEX		g_mapIDnCOUNT;	// Texture ID, Count	// 갯수를 센 후 다음 프레임에 적용한다.
	MMAP_TEX	g_mapSortID;	// Texture Count, ID	// 갯수가 같은 것은 여러개 있을 수 있다.

	SMeshDynamicRenderMan	g_sMeshDynamicRenderMAN;

	// Note : 단 한개.~!
	DWORD	g_dwVERTEX_SIZE = 5000;	// 점 갯수.~!
	DWORD	g_dwFACE_SIZE = 5000;	// 면 갯수.~!
	LPDIRECT3DVERTEXBUFFERQ g_pVB = NULL;
	LPDIRECT3DINDEXBUFFERQ g_pIB = NULL;

	void OnCreateDevice( IDirect3DDevice9* pd3dDevice )
	{
		DxDynamicMesh::g_pd3dDevice = pd3dDevice;

		SAFE_RELEASE( g_pVB );
		SAFE_RELEASE( g_pIB );

		pd3dDevice->CreateVertexBuffer( sizeof(VERTEX)*g_dwVERTEX_SIZE, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &g_pVB, NULL ); 
		pd3dDevice->CreateIndexBuffer( 3*g_dwFACE_SIZE*sizeof(WORD), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &g_pIB, NULL );
	}

	void OnDestroyDevice()
	{
		SAFE_RELEASE( g_pVB );
		SAFE_RELEASE( g_pIB );
	}

	void Reset()
	{
		g_mapIDnCOUNT.clear();

		g_sMeshDynamicRenderMAN.Reset();

		int nCount(0);
		MMAP_TEX_RITER riter = g_mapSortID.rbegin();
		for( ; riter!=g_mapSortID.rend(); ++riter, ++nCount )
		{
			g_sMeshDynamicRenderMAN.Init( (*riter).second.dwID, nCount );
		}
	}

	void InsertRenderTexID( DWORD dwID, DWORD dwVertex, DWORD dwFace )
	{
		MAP_TEX_ITER iter = g_mapIDnCOUNT.find( dwID );
		if( iter!= g_mapIDnCOUNT.end() )	// 같은 값이 있다면.. !!
		{
			(*iter).second.dwCount += 1;
			(*iter).second.dwVertex += dwVertex;
			(*iter).second.dwFace += dwFace;
		}
		else
		{
			COUNTVERTEXFACE sData;
			sData.dwCount = 1;
			sData.dwVertex = dwVertex;
			sData.dwFace = dwFace;
			g_mapIDnCOUNT.insert( std::make_pair( dwID, sData ) );
		}
	}

	void InsertVBIB( DWORD dwPointer, VERTEX* pVertices, WORD* pIndices, const D3DXATTRIBUTERANGE& dwAttrib )
	{
		g_sMeshDynamicRenderMAN.InsertVBIB
		(
			dwPointer, 
			pVertices, 
			pIndices, 
			dwAttrib.VertexCount, 
			dwAttrib.FaceCount 
		);
	}

	void InsertIB( DWORD dwPointer, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwVertexStart, DWORD dwFaceStart, DWORD dwFaceCount )
	{
		//g_sMeshDynamicRenderMAN.InsertIB
		//(
		//	dwPointer, 
		//	pIB, 
		//	dwVertexStart, 
		//	dwFaceCount 
		//);
	}

	BYTE* GetFreeVB( DWORD dwPointer, const D3DXATTRIBUTERANGE& dwAttrib )
	{
		return g_sMeshDynamicRenderMAN.GetFreeVB( dwPointer, dwAttrib.VertexCount );
	}

	BYTE* GetFreeVBInsertIB( DWORD dwIndex, LPDIRECT3DINDEXBUFFERQ pIB, const D3DXATTRIBUTERANGE& sAttribTable )
	{
		return g_sMeshDynamicRenderMAN.GetFreeVBInsertIB( dwIndex, pIB, sAttribTable );
	}

	void UnlockVB()
	{
		g_sMeshDynamicRenderMAN.UnlockVB();
	}

	BOOL IsGetIndex( DWORD dwPointer, DWORD& dwIndex )
	{
		return g_sMeshDynamicRenderMAN.IsGetIndex( dwPointer, dwIndex );
	}

	void SortTextureID()
	{
		g_mapSortID.clear();
		MAP_TEX_ITER iter = g_mapIDnCOUNT.begin();
		for( ; iter!=g_mapIDnCOUNT.end(); ++iter )
		{
			if( (*iter).second.dwCount > 1 )	// 갯수가 1개 이상인 애들만 작업한다.
			{
				IDFACE sData;
				sData.dwID = (*iter).first;
				sData.dwFace = (*iter).second.dwFace;
				g_mapSortID.insert( std::make_pair( (*iter).second.dwVertex, sData ) );
			}
		}
	}

	void RenderSortTEST()
	{
		int nCount(14);
		MMAP_TEX_RITER riter = g_mapSortID.rbegin();
		for( ; riter!=g_mapSortID.rend(); ++riter, ++nCount )
		{
			CDebugSet::ToView( nCount, "Vertex Count : %d, Polygon : %d", (*riter).first, (*riter).second.dwFace );

			if( nCount==21 )	goto _RETURN;
		}

_RETURN:
		CDebugSet::ToView( 22, "g_mapSortID Size : %d", g_mapSortID.size() );	// 1개짜리는 빠진다.
	}

	// Note : 버텍스 버퍼랑 인덱스 버퍼를 체크한 후 작다면 크게 늘린다.
	void CheckVBIB( IDirect3DDevice9* pd3dDevice )
	{
		DWORD dwNeedVertexNUM(0);
		DWORD dwNeedFaceNUM(0);

		DWORD dwVertexMAX(0);
		DWORD dwFaceMAX(0);
		g_sMeshDynamicRenderMAN.GetVertexFaceMAX( dwVertexMAX, dwFaceMAX );

		if( g_dwVERTEX_SIZE  < dwVertexMAX )
		{
			g_dwVERTEX_SIZE = dwVertexMAX;
			SAFE_RELEASE( g_pVB );
			pd3dDevice->CreateVertexBuffer( sizeof(VERTEX)*g_dwVERTEX_SIZE, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &g_pVB, NULL ); 
		}

		if( g_dwFACE_SIZE  < dwFaceMAX )
		{
			g_dwFACE_SIZE = dwFaceMAX;
			SAFE_RELEASE( g_pIB );
			pd3dDevice->CreateIndexBuffer( 3*g_dwFACE_SIZE*sizeof(WORD), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &g_pIB, NULL );
		}
	}

	void Render( IDirect3DDevice9* pd3dDevice )
	{
//		CheckVBIB( pd3dDevice );
//		g_sMeshDynamicRenderMAN.Clone( g_pVB, g_pIB );

		g_sMeshDynamicRenderMAN.Render();

		// Note : 이번 프레임에 모은 정보를 소팅한다.
		SortTextureID();
		RenderSortTEST();
	}

	void ReleaseTexture( DWORD dwPointer )
	{
		if( !dwPointer )	return;

		MAP_TEX_ITER iter = g_mapIDnCOUNT.find( dwPointer );
		if( iter!= g_mapIDnCOUNT.end() )	// 같은 값이 있다면.. !!
		{
			g_mapIDnCOUNT.erase( iter );
			return;
		}
	}
};
