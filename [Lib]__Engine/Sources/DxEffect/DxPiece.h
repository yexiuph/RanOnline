#pragma once

#include "DxVertexFVF.h"

class DxPieceEdit;
class DxPieceEff;
class DxStaticAniFrame;
class DxPieceTexEff;

//----------------------------------------------------------------------------------------------------------------------
//											D	x		P	i	e	c	e
//	Note : 
//----------------------------------------------------------------------------------------------------------------------
class DxPiece
{
private:
	float		m_fTime;
	float		m_fAniTime;
	float		m_ElapsedTime;

private:
	DxStaticAniFrame*	m_pStaticAniFrame;	// RefCount�� 1���� ������ Pointer ������� �ߺ������� ����. Container���� ����
	DxPieceEff*			m_pPieceEff;		// RefCount ����. ������ ����. Ȯ���� ������� ��.
	DxPieceTexEff*		m_pAniTexEff;		// Animation�� Texture Effect. ������ ����. Ȯ���� ������� ��.

public:
	D3DXVECTOR3	m_vStaticMax;	// StaticMesh ���� ����. < ĳ���͸� ������츦 üũ�ϱ� ���� >
	D3DXVECTOR3	m_vStaticMin;	// StaticMesh ���� ����. < ĳ���͸� ������츦 üũ�ϱ� ���� >
	D3DXVECTOR3 m_vCenter;		// ��ü�� ����. 						< 1�� üũ�� ���� >
	float		m_fLength;		// �������� ���� �׷��� ����� ������.	< 1�� üũ�� ���� >

public:
	float		m_fAlpha;		// ��ŷ�� �Ǿ��� �� Alpha �� ��ȭ�Ѵ�.		// �Ź� ���ϴ� ��.
	BOOL		m_bAplhaBlend;	// Picking�� ���� AlphaBlending�� �ϴ°�?	// �Ź� ���ϴ� ��.

private:
	VEC_DWORD	m_vecPointIndex;	// �ڽſ��� ������ �ִ� Point Light Index

public:
	D3DXVECTOR3	m_vMax;			// ��ü�� Max
	D3DXVECTOR3	m_vMin;			// ��ü�� Min
	D3DXMATRIX	m_matWorld;

	DxPiece*	m_pNext;		// List
	DxPiece*	m_pTreeNext;	// Tree �ȿ����� List �̴�.
	DxPiece*	m_pSortNext;	// Quick Sort �� ���� List �̴�.

public:
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice );

	// Edit
	void SetAABBBox();
	BOOL IsCollisionLine( const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision );

public:
	void Save( CSerialFile& SFile );	// Tree ������ Save Load
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxPieceEdit* pSrc );

public:
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEdit* pSrc, const D3DXMATRIX& matWorld, const D3DXMATRIX& matFrame );	// DxPieceEdit���� ���� ������ �� ȣ��
	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc );															// DxPiece �༮�� ������ ����
	void CleanUp();

private:
	void CheckPointLight();									// �ڽŰ� �´�� �ִ� ���� üũ�� �Ѵ�.	< �⺻������ ���� �ε��Ǿ� �־�� �� >
	void RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice );	// Point Light�� �����Ų��.

public:
	DxPiece();
	~DxPiece();
};

//----------------------------------------------------------------------------------------------------------------------
//										D	x		P	i	e	c	e		T	r	e	e
//----------------------------------------------------------------------------------------------------------------------
struct DxPieceNode
{
	union
	{
		struct { D3DXVECTOR3 m_vMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 m_vMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	DxPiece*	m_pPiece;

	DxPieceNode*	m_pLeftChild;
	DxPieceNode*	m_pRightChild;

	void	Save( CSerialFile& SFile );
	void	Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxPieceEdit* pSrc );

	// Note : m_pPiece �� �����ش�. 
	//			List�� Tree�� ���� ���� m_pPiece�� ���� �����Ͱ� ������, 
	//			Tree�� Save�� ���� Load�� ������ ���� m_pPiece�� ���� �����Ͱ� �ǹǷ� CleanUp���� m_pPiece�� delete�Ѵ�.
	void	CleanUp();

	DxPieceNode () :
		m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_pPiece(NULL),
		m_pLeftChild(NULL),
		m_pRightChild(NULL)
	{
	};

	~DxPieceNode ()
	{
		m_pPiece = NULL;
		SAFE_DELETE(m_pLeftChild);
		SAFE_DELETE(m_pRightChild);
	};
};
typedef DxPieceNode*	PDXPIECENODE;

//----------------------------------------------------------------------------------------------------------------------
//								D	x		P	i	e	c	e		Q	u	i	c	k		S	o	r	t
//	Note : ���� ��Ʈ�Ѵ��� �Ѹ��� ������� ����. ^^;
//			�Ѱ��� ����ְ� �迭�� ������ �ϴ� �� �ΰ� ?
//----------------------------------------------------------------------------------------------------------------------
class DxPieceQuickSort
{
protected:
	struct PIECEDISTANCE
	{
		float		m_fDistance;
		DxPiece*	m_pPiece;
	};
	PIECEDISTANCE*	m_pArrayPiece;	// �迭
	DWORD			m_dwArraySize;	// �ִ� ������ �迭 ������
	DWORD			m_dwPieceCount;	// ���� �迭 ������
	float			m_fElapsedTime;	// ElapsedTime,

protected:
	static PIECEDISTANCE*	m_pArrayPieceONLY;	// �迭
	static DWORD			m_dwArraySizeONLY;	// �ִ� ������ �迭 ������
	static DWORD			m_dwPieceCountONLY;	// ���� �迭 ������

public:
	void SetArrayMaxSize( DWORD dwCount );																// �ִ� �迭 ������ ����
	void Reset();																						// ���� �迭 ������ �ʱ�ȭ.
	void InsertData( DxPiece* pPiece, DxPiece* pPiecePick, 
					const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength );	// ������ Piece ����Ÿ
	void QuickSortProcess();																			// Quick Sort �۾�
	void FrameMove( const float fTime, const float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

	static void InitPickAlpha();
	static void FrameMoveONLY( const float fTime, const float fElapsedTime );
	static void RenderPickAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxStaticMesh* pStaticMesh, DxFrameMesh* pFrameMesh );

protected:
	void DxQuickSort( PIECEDISTANCE pArray[], int m, int n );
	void Swap( PIECEDISTANCE& pData1, PIECEDISTANCE& pData2 );

public:
	DxPieceQuickSort();
	~DxPieceQuickSort();
};

// -----------------------------------------------------------------------------------------------------------------------------------------
//											N	S		P	I	E	C	E		T	R	E	E
// -----------------------------------------------------------------------------------------------------------------------------------------
namespace NSPIECETREE
{
	void MakeTree( PDXPIECENODE& pTree, DxPiece* pPiece );	// Tree �� �����. pPiece �� Tree�� Leaf ������ ��� �ȴ�. List ���Ұ�.
	void InsertQuickSort( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, DxPieceQuickSort* pSort, DxPiece* pPiecePick, 
						const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength );
	void CollisionLine( DxPieceNode* pTree, const CLIPVOLUME &sCV, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, BOOL& bColl );
	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const CLIPVOLUME &sCV );

	void Save( DxPieceNode* pNode, CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, CSerialFile& SFile, DxPieceEdit* pSrc );
};

