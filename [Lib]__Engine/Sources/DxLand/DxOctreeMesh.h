#ifndef	__DX_OCTREE_MESH__
#define	__DX_OCTREE_MESH__

#include "./DxCustomTypes.h"

class CSerialFile;

struct DxAABBNode;
struct DxMeshes;

struct DxOctreeMesh
{
	DWORD							m_dwFVF;

	DWORD							m_dwNumVertices;
	LPDIRECT3DVERTEXBUFFERQ			m_pVB;

	DWORD							m_dwNumFaces;
	LPDIRECT3DINDEXBUFFERQ			m_pIB;

	DWORD							m_dwAttribTableSize;
	LPD3DXATTRIBUTERANGE			m_pAttribTable;	

	DxAABBNode						*m_pAABBTreeRoot;

	DWORD GetFVF ()			{ return m_dwFVF; }
	DWORD GetNumVertices ()	{ return m_dwNumVertices; }
	DWORD GetNumFaces ()	{ return m_dwNumFaces; }

public:
	DxOctreeMesh () :
		m_dwNumVertices(0),
		m_pVB(NULL),
		m_dwNumFaces(0),
		m_pIB(NULL),
		m_dwAttribTableSize(0),
		m_pAttribTable(NULL),
		m_pAABBTreeRoot(NULL)
	{
	}
	virtual	~DxOctreeMesh();

	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, D3DXMATRIX *pmatComb );
	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOctreeMesh* pMesh, D3DXMATRIX *pmatComb );

	HRESULT	CreateDxOctreeMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD NumFaces, DWORD NumVertices, DWORD FVF );
	HRESULT	ReleaseDxOctreeMesh	();	

	HRESULT	LockVertexBuffer ( VOID** ppbData );
	HRESULT	LockVertexBuffer ( DWORD dwFlag, VOID** ppbData );

	HRESULT	LockIndexBuffer ( VOID** ppbData );
	HRESULT	LockIndexBuffer ( DWORD dwFlag, VOID** ppbData );

	HRESULT	UnlockVertexBuffer	();
	HRESULT	UnlockIndexBuffer	();

	BOOL					CreateAttribute	( DWORD		AttribTableSize );	
	LPD3DXATTRIBUTERANGE	GetAttributeTbl	();
	BOOL					ReleaseAttribure();	

	HRESULT GetAttributeTable ( LPD3DXATTRIBUTERANGE pAttribTable, DWORD* pdwSize )
	{
		if ( !pAttribTable )
		{
			*pdwSize = m_dwAttribTableSize;
			return S_FALSE;
		}

		memcpy ( pAttribTable, m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize );

		return S_OK;
	}

	HRESULT	DrawSubset			( DWORD	AttribID, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : AABB 面倒 贸府 何盒.
	//
protected:
	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb, WORD *pIndices,
		PBYTE pbPoints, WORD *pwFaces, DWORD dwNumFaces, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	void IsCollision ( DxAABBNode *pAABBCur, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, 
						D3DXVECTOR3 &vNormal, DWORD& dwAttribid, const BOOL bFrontColl );

public:
	HRESULT	MakeAABBTree ( D3DXMATRIX &matComb );
	void IsCollision( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision, BOOL &bCollision, 
					D3DXVECTOR3 &vNormal, DWORD& dwAttribid, const BOOL bFrontColl );

public:
	BOOL		SaveFile ( CSerialFile &SFile );
	BOOL		LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
};

struct DXOCMATERIAL
{
	D3DMATERIALQ		rgMaterial;
	LPDIRECT3DTEXTUREQ	pTexture;
	char				szTexture[MAX_PATH];

	DXOCMATERIAL () :
		pTexture(NULL)
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
	}
};

struct DxOcMeshes
{
	DxOctreeMesh*		pOcMesh;

	DWORD				cMaterials;
	DXOCMATERIAL*		pMaterials;

	DxOcMeshes*			m_pNext;

	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pMesh, D3DXMATRIX *pmatComb );
	HRESULT CloneMesh ( LPDIRECT3DDEVICEQ pd3dDevice, DxOcMeshes *pMesh, D3DXMATRIX *pmatComb );
	void ClearAll ();

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL		SaveFile ( CSerialFile &SFile );
	BOOL		LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

	DxOcMeshes () :
		pOcMesh(0),
		cMaterials(0),
		pMaterials(0),
		m_pNext(0)
	{
	}

	~DxOcMeshes ()
	{
		ClearAll ();

		SAFE_DELETE(m_pNext);
	}
};

#endif	//	__DX_OCTREE_MESH__