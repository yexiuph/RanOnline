#include "pch.h"

#include "SerialFile.h"

#include "DxSMeshDATA.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxSMeshDATA::DxSMeshDATA() :
	m_dwVertices(0L),
	m_dwFaces(0L),
	m_dwFVF(0L),
	m_dwSizeFVF(0L),
	m_dwAttrib(0L),
	m_pVertices(NULL),
	m_pIB(NULL),
	m_pAttribTable(NULL)
{
}

DxSMeshDATA::~DxSMeshDATA()
{
	CleanUp();
}

void DxSMeshDATA::CleanUp()
{
	SAFE_RELEASE( m_pIB );
	SAFE_DELETE_ARRAY( m_pVertices );
	SAFE_DELETE_ARRAY( m_pAttribTable );
}

void DxSMeshDATA::Convert( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh )
{
	CleanUp();

	m_dwFaces = pMesh->GetNumFaces();
	m_dwVertices = pMesh->GetNumVertices();
	m_dwFVF = pMesh->GetFVF();
	m_dwSizeFVF = D3DXGetFVFVertexSize( m_dwFVF );

	pMesh->GetAttributeTable( NULL, &m_dwAttrib );
	m_pAttribTable = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
	pMesh->GetAttributeTable( m_pAttribTable, &m_dwAttrib );

	//BYTE* pVertices;
	//m_pVertices = new BYTE[ m_dwSizeFVF*m_dwVertices ];
	//pMesh->LockVertexBuffer( 0L, (VOID**)&pVertices );
	//memcpy( m_pVertices, pVertices, m_dwSizeFVF*m_dwVertices );
	//pMesh->UnlockVertexBuffer();

	BYTE* pSrc;
	pMesh->LockVertexBuffer( 0L, (VOID**)&pSrc );

	switch( m_dwFVF )
	{
	case 0x1116:
		ConvertSkinnedMesh_0( m_pVertices, pSrc, m_dwVertices );
		break;
	case 0x1118:
		ConvertSkinnedMesh_1( m_pVertices, pSrc, m_dwVertices );
		break;
	case 0x111a:
		ConvertSkinnedMesh_2( m_pVertices, pSrc, m_dwVertices );
		break;
	case 0x111c:
		ConvertSkinnedMesh_3( m_pVertices, pSrc, m_dwVertices );
		break;
	case 0x111e:
		ConvertSkinnedMesh_4( m_pVertices, pSrc, m_dwVertices );
		break;
	}

	pMesh->UnlockVertexBuffer();

	pd3dDevice->CreateIndexBuffer( sizeof(WORD)*m_dwFaces*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	BYTE *pIndicesSRC, *pIndicesDEST;
	m_pIB->Lock( 0, 0, (VOID**)&pIndicesDEST, 0L );
	pMesh->LockIndexBuffer( 0L, (VOID**)&pIndicesSRC );
	memcpy( pIndicesDEST, pIndicesSRC, sizeof(WORD)*m_dwFaces*3 );
	pMesh->UnlockIndexBuffer();
	m_pIB->Unlock();
}

DWORD DxSMeshDATA::GetBoneNUM( float fWeight1, float fWeight2, float fWeight3, float fWeight4 )
{
	float fWeight(0.f);

	fWeight += fWeight1;
	if( fWeight >= 0.9999f )	return 1;

	fWeight += fWeight2;
	if( fWeight >= 0.9999f )	return 2;

	fWeight += fWeight3;
	if( fWeight >= 0.9999f )	return 3;

	fWeight += fWeight4;
	if( fWeight >= 0.9999f )	return 4;

	return 4;
}

void DxSMeshDATA::ConvertSkinnedMesh_0( PBYTE& _pDest, BYTE* _pSrc, DWORD dwVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight;
		D3DXVECTOR2		vTex;
	};

	struct _VERTEX
	{
		D3DXVECTOR3 vPos;
		DWORD		dwIndex;
		D3DXVECTOR3 vNor;
		D3DXVECTOR2 vTex;
	};

	_pDest = new BYTE[dwVertices*sizeof(VERTEX_POSBONEWEIGHT)];

	_VERTEX* pSrc = (_VERTEX*)_pSrc;
	VERTEX_POSBONEWEIGHT* pDest = (VERTEX_POSBONEWEIGHT*)_pDest;

	for( DWORD i=0; i<dwVertices; ++i )
	{
		pDest[i].vPos = pSrc[i].vPos;
		pDest[i].vNor = pSrc[i].vNor;
		pDest[i].vTex = pSrc[i].vTex;

		// Note : 영향 받는 본 갯수를 찾는다.
		pDest[i].dwCount = 1;

		// Note : 본 가중치와 번호를 넣는다.
		pDest[i].sBoneWeight.fWeight = 1.f;
		pDest[i].sBoneWeight.nBoneIdx = pSrc[i].dwIndex&0xff;
	}
}

void DxSMeshDATA::ConvertSkinnedMesh_1( PBYTE& _pDest, BYTE* _pSrc, DWORD dwVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[2];
		D3DXVECTOR2		vTex;
	};

	struct _VERTEX
	{
		D3DXVECTOR3 vPos;
		float		fWeight1;
		DWORD		dwIndex;
		D3DXVECTOR3 vNor;
		D3DXVECTOR2 vTex;
	};

	_pDest = new BYTE[dwVertices*sizeof(VERTEX_POSBONEWEIGHT)];

	_VERTEX* pSrc = (_VERTEX*)_pSrc;
	VERTEX_POSBONEWEIGHT* pDest = (VERTEX_POSBONEWEIGHT*)_pDest;

	float fWeightLAST(0.f);
	for( DWORD i=0; i<dwVertices; ++i )
	{
		pDest[i].vPos = pSrc[i].vPos;
		pDest[i].vNor = pSrc[i].vNor;
		pDest[i].vTex = pSrc[i].vTex;

		// Note : 영향 받는 본 갯수를 찾는다.
		fWeightLAST = 1.f - pSrc[i].fWeight1;
		pDest[i].dwCount = GetBoneNUM( pSrc[i].fWeight1, fWeightLAST, 0.f, 0.f );

		// Note : 본 가중치와 번호를 넣는다.
		pDest[i].sBoneWeight[0].fWeight = pSrc[i].fWeight1;
		pDest[i].sBoneWeight[0].nBoneIdx = pSrc[i].dwIndex&0xff;

		if( pDest[i].dwCount >= 2 )
		{
			pDest[i].sBoneWeight[1].fWeight = fWeightLAST;
			pDest[i].sBoneWeight[1].nBoneIdx = (pSrc[i].dwIndex>>8)&0xff;
		}
	}
}

void DxSMeshDATA::ConvertSkinnedMesh_2( PBYTE& _pDest, BYTE* _pSrc, DWORD dwVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[3];
		D3DXVECTOR2		vTex;
	};

	struct _VERTEX
	{
		D3DXVECTOR3 vPos;
		float		fWeight1;
		float		fWeight2;
		DWORD		dwIndex;
		D3DXVECTOR3 vNor;
		D3DXVECTOR2 vTex;
	};

	_pDest = new BYTE[dwVertices*sizeof(VERTEX_POSBONEWEIGHT)];

	_VERTEX* pSrc = (_VERTEX*)_pSrc;
	VERTEX_POSBONEWEIGHT* pDest = (VERTEX_POSBONEWEIGHT*)_pDest;

	float fWeightLAST(0.f);
	for( DWORD i=0; i<dwVertices; ++i )
	{
		pDest[i].vPos = pSrc[i].vPos;
		pDest[i].vNor = pSrc[i].vNor;
		pDest[i].vTex = pSrc[i].vTex;

		// Note : 영향 받는 본 갯수를 찾는다.
		fWeightLAST = 1.f - pSrc[i].fWeight1 - pSrc[i].fWeight2;
		pDest[i].dwCount = GetBoneNUM( pSrc[i].fWeight1, pSrc[i].fWeight2, fWeightLAST, 0.f );

		// Note : 본 가중치와 번호를 넣는다.
		pDest[i].sBoneWeight[0].fWeight = pSrc[i].fWeight1;
		pDest[i].sBoneWeight[0].nBoneIdx = pSrc[i].dwIndex&0xff;

		if( pDest[i].dwCount >= 2 )
		{
			pDest[i].sBoneWeight[1].fWeight = pSrc[i].fWeight2;
			pDest[i].sBoneWeight[1].nBoneIdx = (pSrc[i].dwIndex>>8)&0xff;
		}
		if( pDest[i].dwCount >= 3 )
		{
			pDest[i].sBoneWeight[2].fWeight = fWeightLAST;
			pDest[i].sBoneWeight[2].nBoneIdx = (pSrc[i].dwIndex>>16)&0xff;
		}
	}
}

void DxSMeshDATA::ConvertSkinnedMesh_3( PBYTE& _pDest, BYTE* _pSrc, DWORD dwVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[4];
		D3DXVECTOR2		vTex;
	};

	struct _VERTEX
	{
		D3DXVECTOR3 vPos;
		float		fWeight1;
		float		fWeight2;
		float		fWeight3;
		DWORD		dwIndex;
		D3DXVECTOR3 vNor;
		D3DXVECTOR2 vTex;
	};

	_pDest = new BYTE[dwVertices*sizeof(VERTEX_POSBONEWEIGHT)];

	_VERTEX* pSrc = (_VERTEX*)_pSrc;
	VERTEX_POSBONEWEIGHT* pDest = (VERTEX_POSBONEWEIGHT*)_pDest;

	float fWeightLAST(0.f);
	for( DWORD i=0; i<dwVertices; ++i )
	{
		pDest[i].vPos = pSrc[i].vPos;
		pDest[i].vNor = pSrc[i].vNor;
		pDest[i].vTex = pSrc[i].vTex;

		// Note : 영향 받는 본 갯수를 찾는다.
		fWeightLAST = 1.f - pSrc[i].fWeight1 - pSrc[i].fWeight2 - pSrc[i].fWeight3;
		pDest[i].dwCount = GetBoneNUM( pSrc[i].fWeight1, pSrc[i].fWeight2, pSrc[i].fWeight3, fWeightLAST );

		// Note : 본 가중치와 번호를 넣는다.
		pDest[i].sBoneWeight[0].fWeight = pSrc[i].fWeight1;
		pDest[i].sBoneWeight[0].nBoneIdx = pSrc[i].dwIndex&0xff;

		if( pDest[i].dwCount >= 2 )
		{
			pDest[i].sBoneWeight[1].fWeight = pSrc[i].fWeight2;
			pDest[i].sBoneWeight[1].nBoneIdx = (pSrc[i].dwIndex>>8)&0xff;
		}
		if( pDest[i].dwCount >= 3 )
		{
			pDest[i].sBoneWeight[2].fWeight = pSrc[i].fWeight3;
			pDest[i].sBoneWeight[2].nBoneIdx = (pSrc[i].dwIndex>>16)&0xff;
		}
		if( pDest[i].dwCount >= 4 )
		{
			pDest[i].sBoneWeight[3].fWeight = fWeightLAST;
			pDest[i].sBoneWeight[3].nBoneIdx = (pSrc[i].dwIndex>>24)&0xff;
		}
	}
}

void DxSMeshDATA::ConvertSkinnedMesh_4( PBYTE& _pDest, BYTE* _pSrc, DWORD dwVertices )
{
	struct	 VERTEX_WEIGHT
	{
		int		nBoneIdx;
		float	fWeight;
	};

	struct	 VERTEX_POSBONEWEIGHT
	{
		D3DXVECTOR3		vPos;
		D3DXVECTOR3		vNor;
		DWORD			dwCount;
		VERTEX_WEIGHT	sBoneWeight[4];
		D3DXVECTOR2		vTex;
	};

	struct _VERTEX
	{
		D3DXVECTOR3 vPos;
		float		fWeight1;
		float		fWeight2;
		float		fWeight3;
		float		fWeight4;
		DWORD		dwIndex;
		D3DXVECTOR3 vNor;
		D3DXVECTOR2 vTex;
	};

	_pDest = new BYTE[dwVertices*sizeof(VERTEX_POSBONEWEIGHT)];

	_VERTEX* pSrc = (_VERTEX*)_pSrc;
	VERTEX_POSBONEWEIGHT* pDest = (VERTEX_POSBONEWEIGHT*)_pDest;

	float fWeightLAST(0.f);
	for( DWORD i=0; i<dwVertices; ++i )
	{
		pDest[i].vPos = pSrc[i].vPos;
		pDest[i].vNor = pSrc[i].vNor;
		pDest[i].vTex = pSrc[i].vTex;

		// Note : 영향 받는 본 갯수를 찾는다.
		pDest[i].dwCount = GetBoneNUM( pSrc[i].fWeight1, pSrc[i].fWeight2, pSrc[i].fWeight3, pSrc[i].fWeight4 );

		// Note : 본 가중치와 번호를 넣는다.
		pDest[i].sBoneWeight[0].fWeight = pSrc[i].fWeight1;
		pDest[i].sBoneWeight[0].nBoneIdx = pSrc[i].dwIndex&0xff;

		if( pDest[i].dwCount >= 2 )
		{
			pDest[i].sBoneWeight[1].fWeight = pSrc[i].fWeight2;
			pDest[i].sBoneWeight[1].nBoneIdx = (pSrc[i].dwIndex>>8)&0xff;
		}
		if( pDest[i].dwCount >= 3 )
		{
			pDest[i].sBoneWeight[2].fWeight = pSrc[i].fWeight3;
			pDest[i].sBoneWeight[2].nBoneIdx = (pSrc[i].dwIndex>>16)&0xff;
		}
		if( pDest[i].dwCount >= 4 )
		{
			pDest[i].sBoneWeight[3].fWeight = pSrc[i].fWeight4;
			pDest[i].sBoneWeight[3].nBoneIdx = (pSrc[i].dwIndex>>24)&0xff;
		}
	}
}

void DxSMeshDATA::Save( CSerialFile& SFile )
{
	SFile << m_dwVertices;
	SFile << m_dwFaces;
	SFile << m_dwFVF;
	SFile << m_dwSizeFVF;
	SFile << m_dwAttrib;

	if( m_dwVertices && m_dwSizeFVF && m_pVertices )
	{
		SFile << (BOOL)TRUE;
		SFile.WriteBuffer( m_pVertices, sizeof(m_dwSizeFVF)*m_dwVertices );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	if( m_dwFaces && m_pIB )
	{
		SFile << (BOOL)TRUE;

		BYTE  *pIndicesDEST;
		m_pIB->Lock( 0, 0, (VOID**)&pIndicesDEST, 0L );
		SFile.WriteBuffer( pIndicesDEST, sizeof(WORD)*m_dwFaces*3 );
		m_pIB->Unlock();
	}
	else
	{
		SFile << (BOOL)FALSE;
	}

	if( m_dwAttrib && m_pAttribTable )
	{
		SFile << (BOOL)TRUE;
		SFile.WriteBuffer( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttrib );
	}
	else
	{
		SFile << (BOOL)FALSE;
	}
}

void DxSMeshDATA::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	CleanUp();

	SFile >> m_dwVertices;
	SFile >> m_dwFaces;
	SFile >> m_dwFVF;
	SFile >> m_dwSizeFVF;
	SFile >> m_dwAttrib;

	BOOL bExist(FALSE);
	SFile >> bExist;
	if( bExist )
	{
		m_pVertices = new BYTE[sizeof(m_dwSizeFVF)*m_dwVertices];
		SFile.ReadBuffer( m_pVertices, sizeof(m_dwSizeFVF)*m_dwVertices );
	}

	SFile >> bExist;
	if( bExist )
	{
		pd3dDevice->CreateIndexBuffer( sizeof(WORD)*m_dwFaces*3, 0L, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
		BYTE *pIndicesDEST;
		m_pIB->Lock( 0, 0, (VOID**)&pIndicesDEST, 0L );
		SFile.ReadBuffer( pIndicesDEST, sizeof(WORD)*m_dwFaces*3 );
		m_pIB->Unlock();
	}

	SFile >> bExist;
	if( bExist )
	{
		m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttrib];
		SFile.ReadBuffer( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttrib );
	}
}


