#ifndef	__C_COLLISION_MAP__
#define	__C_COLLISION_MAP__

#include <string>
#include "CollisionContainer.h"

//	Ŭ���� ����
struct	DxAABBNode;
struct	DxFrame;
class	DxFrameMesh;
class	CCollisionMapCell;
class	DxFrameMesh;
class	CSerialFile;

class	CCollisionMap
{
public:
			CCollisionMap ();
	virtual	~CCollisionMap();

public:
	// Note : Import()	
	//			bEdit : Animation ���� Collision ���� ������ΰ��� üũ.!!
	BOOL	Import( const BOOL bEdit );	
	BOOL	CleanUp ();

public:
	BOOL	SaveFile ( CSerialFile& SFile );
	BOOL	LoadFile ( CSerialFile& SFile );

public:
	DxFrame* const GetDxFrame ( DWORD CellID );
	DWORD	GetUsedMemory ();

	void	SetDxFrameMesh ( DxFrameMesh* pFrameMesh );

public:
	//	�浹 üũ
	BOOL	IsVailedCollision ()	{ return (m_pAABBTreeRoot != NULL); }
	BOOL	IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL bEditor, const DxFrame* pFrame, char* pName, const BOOL bFrontColl );
	DxFrame* const		GetCollisionDetectedFrame ();
	void				SetCollisionDetectedFrame ( DxFrame* pDxFrame );
	const D3DXVECTOR3	GetCollisionPos ();

public:
	DWORD	GetCellCount ();
	DWORD	GetVertexCount();

public:
	D3DXVECTOR3	GetTreeCenter ();
	void	GetTreeWidth ( D3DXVECTOR3* pvWidth );
	float	GetTreeXWidth ();
	float	GetTreeYWidth ();
	float	GetTreeZWidth ();

public:
	BOOL	GetVertex ( D3DXVECTOR3* pvPoint, DWORD ID );
	BOOL	GetCellVertex ( D3DXVECTOR3* pvPoint, DWORD ID );

public:
	static unsigned int WINAPI ImportThread( LPVOID pParam );

private:
	BOOL	SetDataToMap ( DWORD nIndices, DWORD* pIndices, DWORD nVertices, D3DXVECTOR3* pVertices,
				DxFrame** pFrame, D3DXVECTOR3 vCenter, D3DXVECTOR3 vWidth );
	BOOL	DelDataToMap ();

private:
	//	�� ����Ÿ ��Ʈ��	
	BOOL	CreateCellArray ( DWORD nMapCells );
	BOOL	DeleteCellArray ();

	BOOL	AddCell ( DWORD VertA, DWORD VertB, DWORD VertC, DWORD CellID );
	CCollisionMapCell*	GetCell( DWORD CellID );
	DWORD	GetMapCellCount ();

private:
	//	DxFrame ����Ÿ ��Ʈ��
	BOOL	CreateDxFrameArray ();
	BOOL	DeleteDxFrameArray ();

	BOOL	SetDxFrameArray ( DxFrame** pDxFrame );
	BOOL	GetDxFrameArray ( DxFrame** pDxFrame );
	DWORD	GetDxFrameCount ();

private:
	//	���ؽ� ����Ÿ ��Ʈ��
	BOOL	CreateVertexArray ( DWORD nVertices );
	BOOL	DeleteVertexArray ();

	BOOL	SetVertices ( D3DXVECTOR3* pVertices );
	BOOL	GetVertices ( D3DXVECTOR3* pVertices );
	DWORD	GetVerticesCount ();

private:
	//	AABB Tree �����	
	HRESULT	MakeAABBTree ();
	HRESULT LoseAABBTree ();

private:
	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb, DWORD *pCellIndex, DWORD nCellIndex, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );	
	HRESULT GetCenterDistNode ( D3DXMATRIX &matComb, DWORD	*pCellIndex, DWORD nCellIndex, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	HRESULT GetSizeNode ( D3DXMATRIX &matComb, DWORD *pCellIndex, DWORD nCellIndex,	D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	BOOL	IsWithInTriangle ( D3DXMATRIX &matComb, LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3, float fDivision, DWORD dwAxis );
	void	IsCollision ( DxAABBNode *pAABBCur, const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2, BOOL bEditor, const BOOL bFrontColl );

private:
	D3DXVECTOR3			m_vTreeCenter;

	union
	{
		struct
		{
			float				m_XWidth;
			float				m_YWidth;
			float				m_ZWidth;
		};
		struct
		{
			D3DXVECTOR3			m_Width;
		};
	};

private:
	DWORD				m_nMapCells;	
//	CCollisionMapCell**	m_pMapCells;	//	�ﰢ�� �迭
	CCollisionMapCell*	m_pMapCells;	//	�ﰢ�� �迭
	
//	DWORD				m_nDxFrame;	
	DxFrame**			m_ppDxFrame;	// Octree ���۽� ���� ������ ���� ���. DirectAccess

	DWORD				m_nVertices;
	D3DXVECTOR3*		m_pVertices;	//	Vertex �迭

	DxAABBNode*			m_pAABBTreeRoot;//	AABB Ʈ��

private:	
	CCollisionContainer	m_CollisionContainer;	//	�浹 �����̳�

private:
    SCollisionCell*		m_pCollisionCell;

private:
	DxFrameMesh*		m_pFrameMesh;
};

#endif	//	__C_COLLISION_MAP__