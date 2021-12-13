#include "pch.h"

#include "./DxLandMan.h"
#include "./TextureManager.h"

#include "DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//	Note	:	보간범위
static	const	float	INTERPOLATOR = 0.1f;
//////////////////////////////////////////////////////////////////////

HRESULT		DxFrame::ConvertMesh ( DxMeshes** ppDstMesh, DxMeshes *pSrcMesh, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note	:	인덱스 버퍼의 복사
	//
	LPD3DXMESH	pMeshData = pSrcMesh->m_pLocalMesh;

	BYTE* pIndices = NULL;
	OCTREEVERTEX* pVertices = NULL;
	int NumOfIndices = pMeshData->GetNumFaces() * 3;
	int	NumOfVertices = pMeshData->GetNumVertices();

	int	NumOfInBoxFaces = 0;
	int NumOfInBoxVertices = 0;
	BOOL* pbInBoxFaces = NULL;	
	BOOL* pbInBoxVertices = NULL;

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//	16비트 메시 인덱스와 32비트 인덱스 구분자
	BOOL b32BitMesh = FALSE;
	if ( pMeshData->GetOptions() & D3DXMESH_32BIT )
		b32BitMesh = TRUE;
	else
		b32BitMesh = FALSE;
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	GetMeshRawData ( &pIndices, b32BitMesh, &pVertices, pMeshData, NumOfIndices, NumOfVertices );
			
	if ( FAILED ( CheckInTheBox ( &NumOfInBoxFaces, &pbInBoxFaces, &NumOfInBoxVertices, &pbInBoxVertices,
			pIndices, b32BitMesh, pVertices, NumOfIndices, NumOfVertices, pvMin, pvMax ) ) )
	{
		return E_FAIL;
	}

	MakeOctreeMeshData ( ppDstMesh, pSrcMesh, NumOfInBoxFaces, pbInBoxFaces,
		NumOfInBoxVertices, pbInBoxVertices, pIndices, b32BitMesh, pVertices, pd3dDevice );	
	DelMeshRawData ( pIndices, b32BitMesh, pVertices, pbInBoxFaces, pbInBoxVertices );
	ExportProgress::CurPos += NumOfInBoxFaces;
	
	return S_OK;
}

HRESULT DxFrame::GetMeshRawData	( BYTE	**ppIndices, BOOL b32BitMesh, OCTREEVERTEX	**ppVertices, LPD3DXMESH pMeshData, int NumOfIndices, int NumOfVertices )
{
	int	DefSize;
	if ( b32BitMesh )	DefSize = sizeof ( DWORD );
	else				DefSize = sizeof ( WORD );

	BYTE	*pLockIndices = NULL;
	BYTE	*pIndices = NULL;	

	pIndices = new BYTE [ NumOfIndices * DefSize ];
	memset ( pIndices, 0, sizeof ( BYTE ) *  NumOfIndices * DefSize );
			
	if (  FAILED ( pMeshData->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pLockIndices ) ) )	
	{
		return E_FAIL;
	}
	memcpy ( pIndices, pLockIndices, sizeof ( BYTE ) * NumOfIndices * DefSize );
	pMeshData->UnlockIndexBuffer();


	//	Note	:	버텍스 버퍼의 복사
	//			
	OCTREEVERTEX	*pLockVertices = NULL;
	OCTREEVERTEX	*pVertices;
	pVertices = new OCTREEVERTEX [ NumOfVertices ];
	memset ( pVertices, 0, sizeof ( OCTREEVERTEX ) * NumOfVertices );

	if ( FAILED ( pMeshData->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pLockVertices ) ) )	
	{
		return E_FAIL;
	}
	//	Note	:	자식으로 내려가면서, 변환을 해 줘야 한다.
	for ( int i = 0; i < NumOfVertices; i++ )
	{				
		D3DXVECTOR3	TransformVertex = D3DXVECTOR3 ( pLockVertices[i].x, pLockVertices[i].y, pLockVertices[i].z );
		D3DXVECTOR3	TransformNormal = D3DXVECTOR3 ( pLockVertices[i].nx, pLockVertices[i].ny, pLockVertices[i].nz );
		D3DXVec3TransformCoord ( &TransformVertex, &TransformVertex, &matCombined );
			
		D3DXVec3TransformNormal ( &TransformNormal, &TransformNormal, &matCombined );
		D3DXVec3Normalize ( &TransformNormal, &TransformNormal );
					
		pVertices[i] = pLockVertices[i];

		pVertices[i].x = TransformVertex.x;
		pVertices[i].y = TransformVertex.y;
		pVertices[i].z = TransformVertex.z;
		pVertices[i].nx = TransformNormal.x;
		pVertices[i].ny = TransformNormal.y;
		pVertices[i].nz = TransformNormal.z;
	}
	pMeshData->UnlockVertexBuffer();

	*ppIndices = pIndices;
	*ppVertices = pVertices;	
	
	return S_OK;
}

HRESULT	DxFrame::CheckInTheBox	( int *pNumOfInBoxFaces, BOOL **ppbInBoxFaces,
										int *pNumOfInBoxVertices, BOOL **ppbInBoxVertices,
										BYTE *pIndices, BOOL b32BitMesh, OCTREEVERTEX *pVertices,
										int NumOfIndices, int NumOfVertices, D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax )
{	
	DWORD* p32BitMesh = NULL;
	WORD*  p16BitMesh = NULL;

	if ( b32BitMesh )	p32BitMesh = (DWORD*)pIndices;
	else				p16BitMesh = (WORD* )pIndices;	

	//	Note	:	가져온 데이타의 영역 체크
	//	Note	:	DxFrame 내부의 Meshes...
	//				리스트로 연결
	//				이것은 자식이 없다.
	//				Sol)	범위 체크후 범위內이면, 메쉬를 생성하기 위한 정점배열을 만든다.
	//						이 메쉬의 모든 정점을 고려하였다면 메쉬를 생성하고, 그 포인터를
	//						Octree의 단말노드에 달린 DxFrame 중의 하나의 자식으로 엮어 둔다.			
	float			x, y, z;
	int				NumOfTriangles = NumOfIndices / 3;
	BOOL			*pbInBoxFaces = NULL;
		
	pbInBoxFaces = new BOOL [NumOfTriangles];
	memset ( pbInBoxFaces, 1, sizeof ( BOOL ) * NumOfTriangles );

	//	Note	:	pvMin과 pvMax가 NULL일 경우는, 영역체크를 하지 않는다.
	//				모든 정점들이 들어간다고 간주하는 것이다.
	if ( pvMin && pvMax )
	{				
		//	Note	:	페이스의 어느 한 정점이라도 벗어나면 그것은 제외시킨다.
		//				박스의 각 면은 밖을 바라보고 있다.
		//				즉, 외부에 있는 놈은 FALSE 시킨다는 말이다.
		for( int i = 0; i < NumOfIndices; i++)
		{
			//	Note : 메쉬의 크기에 따라 인덱싱 변경
			//
			DWORD Index = 0xFFFFFFFF;
			if ( b32BitMesh )	Index = (DWORD)p32BitMesh[i];
			else				Index = (DWORD)p16BitMesh[i];

			x = pVertices[Index].x;
			y = pVertices[Index].y;
			z = pVertices[Index].z;

			//	ax + by + cz + d = 0
			//	Note	:	a가 1인 경우
			//
			if ( x > pvMax->x + INTERPOLATOR )
			{
				pbInBoxFaces[i / 3] = FALSE;
				continue;
			}
			//	Note	:	a가 -1인 경우
			//
			else if ( x < pvMin->x - INTERPOLATOR )
			{
				pbInBoxFaces[i / 3] = FALSE;
				continue;
			}

			//	Note	:	b가 1인 경우
			//
			if ( y > pvMax->y + INTERPOLATOR )
			{
				pbInBoxFaces[i / 3] = FALSE;
				continue;
			}
			//	Note	:	b가 -1인 경우
			//
			else if ( y < pvMin->y - INTERPOLATOR )
			{
				pbInBoxFaces[i / 3] = FALSE;
				continue;
			}

			//	Note	:	c가 1인 경우
			//
			if ( z > pvMax->z + INTERPOLATOR )
			{
				pbInBoxFaces[i / 3] = FALSE;
				continue;
			}
			//	Note	:	c가 -1인 경우
			//
			else if ( z < pvMin->z - INTERPOLATOR )
			{
				pbInBoxFaces[i / 3] = FALSE;
				continue;
			}
		}
	}
			
	int		NumOfInBoxFaces = 0;
	BOOL	*pbInBoxVertices = NULL;

	pbInBoxVertices = new BOOL[NumOfVertices];
	memset ( pbInBoxVertices, 0, sizeof ( BOOL ) * NumOfVertices );			

	//	Note	:	제외된 정점을 포함하는 페이스들중, 이 영역내에
	//				걸쳐지는 부분이 많으면 여기에 넣기 위해 값을 설정한다.
	for( int i = 0; i < NumOfTriangles; i++)
	{
		if ( !( pbInBoxFaces[i] ) )
		{

			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			//이 코드의 i*3을 표현하기 위해 인덱스에서 아래와 같이 응용후
			//사용할때는 Index를 바로 적용하도록 함			
			//vCenter.x = ( pVertices[pIndices[i*3]].x + pVertices[pIndices[i*3+1]].x + pVertices[pIndices[i*3+2]].x ) / 3.0f;
			//vCenter.y = ( pVertices[pIndices[i*3]].y + pVertices[pIndices[i*3+1]].y + pVertices[pIndices[i*3+2]].y ) / 3.0f;
			//vCenter.z = ( pVertices[pIndices[i*3]].z + pVertices[pIndices[i*3+1]].z + pVertices[pIndices[i*3+2]].z ) / 3.0f;				
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

			//	Note : 메쉬의 크기에 따라 인덱싱 변경
			//
			DWORD Index0, Index1, Index2;
			if ( b32BitMesh )	Index0 = (DWORD)p32BitMesh[i*3+0];
			else				Index0 = (DWORD)p16BitMesh[i*3+0];

			if ( b32BitMesh )	Index1 = (DWORD)p32BitMesh[i*3+1];
			else				Index1 = (DWORD)p16BitMesh[i*3+1];

			if ( b32BitMesh )	Index2 = (DWORD)p32BitMesh[i*3+2];
			else				Index2 = (DWORD)p16BitMesh[i*3+2];

			//	Note	:	페이스의 세개의 정점중 어느 하나라도 벗어나는 경우
			//				삼각형의 중심(?)을 구해서 그 놈이 박스에 포함되면
			//				끄집어 가져가고 아닌경우 뺀다.
			OCTREEVERTEX	vCenter;

			vCenter.x = ( pVertices[Index0].x + pVertices[Index1].x + pVertices[Index2].x ) / 3.0f;
			vCenter.y = ( pVertices[Index0].y + pVertices[Index1].y + pVertices[Index2].y ) / 3.0f;
			vCenter.z = ( pVertices[Index0].z + pVertices[Index1].z + pVertices[Index2].z ) / 3.0f;				

			//	ax + by + cz + d = 0
			//	Note	:	a가 1인 경우
			//
			if ( vCenter.x > pvMax->x + INTERPOLATOR )
			{
				continue;
			}
			//	Note	:	a가 -1인 경우
			//
			else if ( vCenter.x < pvMin->x - INTERPOLATOR )
			{
				continue;
			}

			//	Note	:	b가 1인 경우
			//
			if ( vCenter.y > pvMax->y + INTERPOLATOR )
			{
				continue;
			}
			//	Note	:	b가 -1인 경우
			//
			else if ( vCenter.y < pvMin->y - INTERPOLATOR )
			{
				continue;
			}

			//	Note	:	c가 1인 경우
			//
			if ( vCenter.z > pvMax->z + INTERPOLATOR )
			{
				continue;
			}
			//	Note	:	c가 -1인 경우
			//
			else if ( vCenter.z < pvMin->z - INTERPOLATOR )
			{
				continue;
			}

			pbInBoxFaces[i] = TRUE;
		}

		if ( pbInBoxFaces[i] )
		{
			NumOfInBoxFaces++;

			//	Note : 메쉬의 크기에 따라 인덱싱 변경
			//
			DWORD Index0, Index1, Index2;
			if ( b32BitMesh )	Index0 = (DWORD)p32BitMesh[i*3+0];
			else				Index0 = (DWORD)p16BitMesh[i*3+0];

			if ( b32BitMesh )	Index1 = (DWORD)p32BitMesh[i*3+1];
			else				Index1 = (DWORD)p16BitMesh[i*3+1];

			if ( b32BitMesh )	Index2 = (DWORD)p32BitMesh[i*3+2];
			else				Index2 = (DWORD)p16BitMesh[i*3+2];

			pbInBoxVertices[Index0] =
			pbInBoxVertices[Index1] =
			pbInBoxVertices[Index2] = TRUE;
		}
	}

	int	NumOfInBoxVertices = 0;
	for ( int i = 0; i < NumOfVertices; i++ )
	{
		if ( pbInBoxVertices[i] )
		{
			NumOfInBoxVertices++;
		}				
	}

	//	Note	:	걸리는 정점이 하나도 없다... ㅡㅡ;;;
	if ( NumOfInBoxVertices < 1 )
	{
		DelMeshRawData ( pIndices, b32BitMesh, pVertices, pbInBoxFaces, pbInBoxVertices );
		return E_FAIL;
	}

	*pNumOfInBoxFaces = NumOfInBoxFaces;
	*ppbInBoxFaces = pbInBoxFaces;
	*pNumOfInBoxVertices = NumOfInBoxVertices;
	*ppbInBoxVertices = pbInBoxVertices;

	return S_OK;
}

HRESULT	DxFrame::MakeOctreeMeshData ( DxMeshes **ppDstMesh, DxMeshes *pSrcMesh,
										 int NumOfInBoxFaces, BOOL *pbInBoxFaces,
										 int NumOfInBoxVertices, BOOL *pbInBoxVertices,
										 BYTE *pIndices, BOOL b32BitMesh, OCTREEVERTEX	*pVertices,
										 LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD* p32BitMesh = NULL;
	WORD*  p16BitMesh = NULL;

	if ( b32BitMesh )	p32BitMesh = (DWORD*)pIndices;
	else				p16BitMesh = (WORD* )pIndices;

	HRESULT hr;

	LPD3DXMESH	pMeshData = pSrcMesh->m_pLocalMesh;

	int				NumOfIndices = pMeshData->GetNumFaces() * 3;
	int				NumOfVertices = pMeshData->GetNumVertices();	
	
	//	Note	:	걸러진 데이타들을 이용하기 위한 새로운 메시의 생성
	//				기본값은 같이 복사해 넣는다.
	DxMeshes *pNewMesh = new DxMeshes();
			
	pNewMesh->m_pDxOctreeMesh = new DxOctreeMesh ();			
	pNewMesh->m_pDxOctreeMesh->CreateDxOctreeMesh( pd3dDevice, NumOfInBoxFaces, NumOfInBoxVertices, OCTREEVERTEX::FVF );
			
	//	Note	:	생성된 메쉬에 걸러진 데이타를 집어넣는다.			
	DWORD		*pRelationGap = NULL;
	pRelationGap = new DWORD [NumOfVertices];			
	memset ( pRelationGap, 0, sizeof ( DWORD ) * NumOfVertices );			
			
	//	Note	:	버텍스 버퍼의 복사
	//
	OCTREEVERTEX *pLockVertices = NULL;
	if ( FAILED ( pNewMesh->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pLockVertices ) ) )	
	{
		return E_FAIL;
	}

	DWORD					AttribTableSize = 0;
	LPD3DXATTRIBUTERANGE	pSrcAttribTable = NULL;

	pMeshData->GetAttributeTable( NULL, &AttribTableSize );
	pSrcAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];			
	pMeshData->GetAttributeTable( pSrcAttribTable, &AttribTableSize );
			
	LPD3DXATTRIBUTERANGE	pDstAttribTable = NULL;
	pDstAttribTable = new D3DXATTRIBUTERANGE [ AttribTableSize ];

	DWORD					AttribTableCurPos = 0;
	int						NumOfInBoxVerticesData = 0;
	pDstAttribTable[AttribTableCurPos].VertexStart = 0;
	pDstAttribTable[AttribTableCurPos].AttribId = pSrcAttribTable[AttribTableCurPos].AttribId;
	AttribTableCurPos++;
	for ( int i = 0; i < NumOfVertices; i++ )
	{
		//	Note	:	가지고 있는 Material을 다 쓰지 않는 경우도 있더라...
		//
		//				예를들면, pSrcMesh->cMaterials의 개수는 100개인데, AttribTableSize는 20개일때
		//				오류가 난다는 말이다.
		if ( AttribTableCurPos < AttribTableSize && pSrcAttribTable[AttribTableCurPos].VertexStart == i )
		{
			pDstAttribTable[AttribTableCurPos].AttribId = pSrcAttribTable[AttribTableCurPos].AttribId;
			pDstAttribTable[AttribTableCurPos].VertexStart = NumOfInBoxVerticesData;
			pDstAttribTable[AttribTableCurPos - 1].VertexCount =
				pDstAttribTable[AttribTableCurPos].VertexStart - pDstAttribTable[AttribTableCurPos - 1].VertexStart;
			AttribTableCurPos++;			
		}

		if ( pbInBoxVertices[i] )
		{
			pLockVertices[NumOfInBoxVerticesData] = pVertices[i];					
			pRelationGap[i] = i - NumOfInBoxVerticesData;
			NumOfInBoxVerticesData++;
		}				
	}
	pDstAttribTable[AttribTableCurPos - 1].VertexCount =
		( NumOfInBoxVerticesData ) - pDstAttribTable[AttribTableCurPos - 1].VertexStart;
			
	pNewMesh->m_pDxOctreeMesh->UnlockVertexBuffer();



	//	Note	:	Material의 개수만큼만 돌도록...
	//
	int		AttibCountDebug = AttribTableCurPos;

	//	Note	:	인덱스 버퍼의 복사
	//			
	AttribTableCurPos = 0;
	int		AmountIndices = 0;
	BYTE *pLockIndices = NULL;
	if ( FAILED ( pNewMesh->m_pDxOctreeMesh->LockIndexBuffer( (VOID**)&pLockIndices ) ) )	
	{
		return E_FAIL;
	}

	pDstAttribTable[AttribTableCurPos].FaceStart = 0;
	pDstAttribTable[AttribTableCurPos].AttribId = pSrcAttribTable[AttribTableCurPos].AttribId;
	AttribTableCurPos++;

	for ( int i = 0; i < NumOfIndices; i++ )
	{
		if ( (int) AttribTableCurPos < AttibCountDebug && pSrcAttribTable[AttribTableCurPos].FaceStart * 3 == i )
		{					
			if ( pDstAttribTable[AttribTableCurPos].AttribId != pSrcAttribTable[AttribTableCurPos].AttribId )
			{
				return E_FAIL;
			}
			pDstAttribTable[AttribTableCurPos].FaceStart = AmountIndices / 3;			 
			pDstAttribTable[AttribTableCurPos - 1].FaceCount  =
				pDstAttribTable[AttribTableCurPos].FaceStart - pDstAttribTable[AttribTableCurPos - 1].FaceStart;			

			AttribTableCurPos++;
		}

		if ( pbInBoxFaces[i / 3] )
		{
			//	Note : 메쉬의 크기에 따라 인덱싱 변경
			//
			if ( b32BitMesh )
			{
				const  DWORD Index = p32BitMesh[i];
				DWORD* const p32BitLockIndex = (DWORD*)pLockIndices;
				p32BitLockIndex[AmountIndices] = Index - pRelationGap[Index];
			}
			else
			{
				const WORD  Index = p16BitMesh[i];
				WORD* const p16BitLockIndex = (WORD*)pLockIndices;
				p16BitLockIndex[AmountIndices] = Index - (WORD)pRelationGap[Index];
			}		

			AmountIndices++;
		}
	}
	pDstAttribTable[AttribTableCurPos - 1].FaceCount   =
		( ( AmountIndices ) / 3 ) - pDstAttribTable[AttribTableCurPos - 1].FaceStart;
	

	if ( FAILED ( pNewMesh->m_pDxOctreeMesh->UnlockIndexBuffer() ) )
	{
		return E_FAIL;
	}

	//	이부분부터....
	//	에트리뷰트 재설정해야한다.
    DWORD	UseMaterialCount = 0;
	for ( int i = 0; i < (int)AttribTableCurPos; i++ )
	{
        if ( pDstAttribTable[i].FaceCount || pDstAttribTable[i].VertexCount )
		{
			UseMaterialCount++;
		}
	}

	DWORD	AssignMaterialCount = 0;
	//	<--	에트리뷰트 테이블도 쓰는 만큼 정렬된다.
	//		이때, 그 놈들이 가리키는 부분을 재조정하기 위해서
	//		릴레이션이 쓰이게 된다.
	//	-->
	DWORD	*pRelationalAttributeTable = new DWORD[UseMaterialCount];
	LPD3DXATTRIBUTERANGE	pArrangeAttribTable = NULL;
	pArrangeAttribTable = new D3DXATTRIBUTERANGE [ UseMaterialCount ];
	for ( int i = 0; i < (int)AttribTableCurPos; i++ )
	{
		if ( pDstAttribTable[i].FaceCount || pDstAttribTable[i].VertexCount )
		{
			pArrangeAttribTable[AssignMaterialCount] = pDstAttribTable[i];
			pArrangeAttribTable[AssignMaterialCount].AttribId = AssignMaterialCount;
			pRelationalAttributeTable[AssignMaterialCount] = 
				pDstAttribTable[i].AttribId - AssignMaterialCount;

			AssignMaterialCount++;
		}
	}

	if ( UseMaterialCount != AssignMaterialCount )
	{
		SAFE_DELETE_ARRAY ( pRelationGap );
		SAFE_DELETE_ARRAY ( pSrcAttribTable );
		SAFE_DELETE_ARRAY ( pDstAttribTable );
		SAFE_DELETE_ARRAY ( pArrangeAttribTable );
		SAFE_DELETE_ARRAY ( pRelationalAttributeTable );

		return E_FAIL;
	}
    

	pNewMesh->m_pDxOctreeMesh->CreateAttribute ( UseMaterialCount );

	LPD3DXATTRIBUTERANGE	pMemberAttribTbl = NULL;
	pMemberAttribTbl = pNewMesh->m_pDxOctreeMesh->GetAttributeTbl();			
	memcpy ( pMemberAttribTbl, pArrangeAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * UseMaterialCount );

	pNewMesh->cMaterials = UseMaterialCount;
	pNewMesh->rgMaterials = new D3DMATERIALQ[UseMaterialCount];
	pNewMesh->exMaterials = new D3DEXMATERIAL[UseMaterialCount];
	pNewMesh->pTextures = new LPDIRECT3DTEXTUREQ[UseMaterialCount];
	pNewMesh->strTextureFiles = new CString[UseMaterialCount];
	for ( int i = 0; i < (int)UseMaterialCount; i++ )
	{
		DWORD AttribPos = pNewMesh->m_pDxOctreeMesh->m_pAttribTable[i].AttribId + pRelationalAttributeTable[i];
	
		pNewMesh->rgMaterials[i] = pSrcMesh->rgMaterials[AttribPos];
		pNewMesh->exMaterials[i] = pSrcMesh->exMaterials[AttribPos];	//	ADDON - 추가 설정 파라메타 복사.

		hr = TextureManager::LoadTexture ( (char*) pSrcMesh->strTextureFiles[AttribPos].GetString(),
			pd3dDevice, pNewMesh->pTextures[i], 0, 0 );
		if (FAILED(hr))
		{
			hr = S_OK;
			pNewMesh->pTextures[i] = NULL;
		}

		if ( pSrcMesh->strTextureFiles[AttribPos].GetLength() )
			pNewMesh->strTextureFiles[i] = pSrcMesh->strTextureFiles[AttribPos];
	}			
	//	이부분까지....

	if ( pSrcMesh->szName )
	{
		int nStrLen = strlen(pSrcMesh->szName) + 1;
		pNewMesh->szName = new char[nStrLen];
		StringCchCopy( pNewMesh->szName, nStrLen, pSrcMesh->szName );
//		memset ( pNewMesh->szName, 0, sizeof ( char ) * strlen ( pSrcMesh->szName ) );
//		memcpy ( pNewMesh->szName, pSrcMesh->szName, sizeof ( char ) * strlen ( pSrcMesh->szName ) );
	}

	*ppDstMesh = pNewMesh;

	SAFE_DELETE_ARRAY ( pRelationGap );
	SAFE_DELETE_ARRAY ( pSrcAttribTable );
	SAFE_DELETE_ARRAY ( pDstAttribTable );
	SAFE_DELETE_ARRAY ( pArrangeAttribTable );	
	SAFE_DELETE_ARRAY ( pRelationalAttributeTable );
	
	return S_OK;
}

void DxFrame::DelMeshRawData ( BYTE *pIndices, BOOL b32BitMesh, OCTREEVERTEX *pVertices, BOOL *pbInBoxFaces, BOOL *pbInBoxVertices )
{
	SAFE_DELETE_ARRAY ( pIndices );
	SAFE_DELETE_ARRAY ( pVertices );
	SAFE_DELETE_ARRAY ( pbInBoxFaces );
	SAFE_DELETE_ARRAY ( pbInBoxVertices );
}