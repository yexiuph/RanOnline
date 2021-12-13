#ifndef NAVIGATIONMESH_H
#define NAVIGATIONMESH_H

#include <vector>
#include "./DxCustomTypes.h"
#include "navigationcell.h"
#include "navigationheap.h"

class	NavigationPath;
class	CSerialFile;
struct	DxFrame;
struct	DxAABBNode;

struct	SNaviIntegrityLog
{
	DWORD	CellID[10];
	char	StrLog[256];

	SNaviIntegrityLog()
	{
		memset( StrLog, 0, sizeof(char)*256 );
	};
};

class NavigationMesh
{
public:
	typedef	std::vector<NavigationCell*> CELL_ARRAY;

public:
	NavigationMesh();
	~NavigationMesh();

	void	Clear();
	HRESULT	CreateNaviMesh ( DxFrame *pNaviFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	void	LinkCells();
	HRESULT	MakeAABBTree ();
	BOOL	CheckIntegrity ( char *szDebugFullDir );
	void	GotoErrorPosition ( DWORD	*pIDs, DWORD	nIDs );

protected:
	HRESULT	AddCell ( const DWORD& PointA, const DWORD& PointB, const DWORD& PointC, const DWORD& CellID );
	//	<--	NaviVertex �Լ���, 
	HRESULT	SetNaviVertexIndexData ( D3DXVECTOR3 *pNaviVertex, int nNaviVertex, int nDACell );		
	void	DelNaviVertexIndexData ();
	//	-->	NaviVertex �Լ���	
	void	CheckLink ( DWORD *pUsedCell, DWORD StartCellID, DWORD GroupID );

public:
	//	<--	����Ʈ�� �̿��� ��ã�� �� ���� ����ֱ� �Լ���
	D3DXVECTOR3	SnapPointToCell ( DWORD CellID, const D3DXVECTOR3& Point );
	D3DXVECTOR3	SnapPointToMesh ( DWORD *CellOutID, const D3DXVECTOR3& Point );
	DWORD		FindClosestCell ( const D3DXVECTOR3& Point)	const;
	//	-->	����Ʈ�� �̿��� ��ã�� �� ���� ����ֱ� �Լ���

	bool	LineOfSightTest ( DWORD StartID, const D3DXVECTOR3& StartPos, DWORD EndID, const D3DXVECTOR3& EndPos );
	bool	BuildNavigationPath ( NavigationPath& NavPath, DWORD StartID, const D3DXVECTOR3& StartPos, DWORD EndID, const D3DXVECTOR3& EndPos, float* pfPathDist = NULL);
	void	ResolveMotionOnMesh ( const D3DXVECTOR3& StartPos, DWORD StartID, D3DXVECTOR3& EndPos, DWORD *EndID );
	
	int		TotalCells () const;
	NavigationCell* GetCell ( int index );


protected:
	//	<--	AABB Tree �����	
	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb, DWORD *pCellIndex, DWORD nCellIndex, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );	
	HRESULT GetCenterDistNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	HRESULT GetSizeNode ( D3DXMATRIX &matComb, DWORD *pCellIndex, DWORD nCellIndex,	D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	BOOL	IsWithInTriangle ( D3DXMATRIX &matComb, LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3, float fDivision, DWORD dwAxis );
	void	IsCollision ( DxAABBNode *pAABBCur, D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2, D3DXVECTOR3 &vCollision, DWORD &CollisionID );
	void	GetAllCollisionCell ( DWORD *pCollisionCellID, DWORD& CollisionIDCount, D3DXVECTOR3& vAMax, D3DXVECTOR3& vAMin, DxAABBNode *pAABBCur );	
	//	-->	AABB Tree �����	

public:
	BOOL	IsVailedCollision ()	{ return m_pAABBTreeRoot!=NULL; }
	void	IsCollision ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision, DWORD &CollisionID, BOOL &bCollision );

public:
	BOOL	GetAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	void	SaveFile ( CSerialFile	&SFile );
	void	LoadFile ( CSerialFile	&SFile, LPDIRECT3DDEVICEQ pd3dDevice=NULL );

private:	
	CELL_ARRAY	m_CellArray;		//	�׺���̼� �� ����Ʈ
	DxAABBNode	*m_pAABBTreeRoot;	//	AABB Ʈ��

	D3DXVECTOR3	*m_pNaviVertex;		//	�׺���̼� �� ��ü ���� �迭
	int			m_nNaviVertex;		//	�׺���̼� �� ��ü ���� ����

	NavigationCell	**m_pDACell;	//	Index������ ���� Cell�� ���� ������ �迭

	// path finding data...
	int				m_PathSession;
	NavigationHeap	m_NavHeap;
	
protected:
	//	<--	NavigationMesh Render�� [DEBUG]
//	DWORD							m_dwNumVertices;	<-- m_nNaviVertex�� ���� �����ϴ� -->
	LPDIRECT3DVERTEXBUFFERQ			m_pVB;

//	DWORD							m_dwNumFaces;		<--	TotalCells()�� �������� -->
	LPDIRECT3DINDEXBUFFERQ			m_pIB;

	DWORD	*m_pErrorID;
	DWORD	m_nErrorID;

public:
	HRESULT	CreateVBIB ( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMeshData );
	HRESULT	DeleteVBIB ( );
	void	Update ( float elapsedTime = 1.0f );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );
	//	-->	NavigationMesh Render�� [DEBUG]
};

inline NavigationMesh::NavigationMesh()
:m_PathSession(0),
m_pAABBTreeRoot(NULL),
m_pNaviVertex(NULL),
m_pDACell(NULL)	,
m_pErrorID(NULL),
m_nErrorID(0),
m_pVB(NULL),
m_pIB(NULL)
{
	m_CellArray.clear();
}

inline NavigationMesh::~NavigationMesh()
{
	Clear();
}

inline int NavigationMesh::TotalCells()const
{
	return ((int)m_CellArray.size());
}

inline NavigationCell* NavigationMesh::GetCell ( int index )
{
	if ( m_CellArray.size()<SIZE_T(index) )		return NULL;
	return m_CellArray.at(index);
}

#endif