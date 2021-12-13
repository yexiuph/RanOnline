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
	DxStaticAniFrame*	m_pStaticAniFrame;	// RefCount로 1개만 생성후 Pointer 사용으로 중복생성을 안함. Container에서 관리
	DxPieceEff*			m_pPieceEff;		// RefCount 없음. 여러개 생성. 확실히 지워줘야 함.
	DxPieceTexEff*		m_pAniTexEff;		// Animation용 Texture Effect. 여러개 생성. 확실히 지워줘야 함.

public:
	D3DXVECTOR3	m_vStaticMax;	// StaticMesh 만의 한정. < 캐릭터를 가릴경우를 체크하기 위함 >
	D3DXVECTOR3	m_vStaticMin;	// StaticMesh 만의 한정. < 캐릭터를 가릴경우를 체크하기 위함 >
	D3DXVECTOR3 m_vCenter;		// 물체의 중점. 						< 1차 체크를 위함 >
	float		m_fLength;		// 중점에서 원을 그렸을 경우의 반지름.	< 1차 체크를 위함 >

public:
	float		m_fAlpha;		// 픽킹이 되었을 때 Alpha 가 변화한다.		// 매번 변하는 값.
	BOOL		m_bAplhaBlend;	// Picking에 의한 AlphaBlending을 하는가?	// 매번 변하는 값.

private:
	VEC_DWORD	m_vecPointIndex;	// 자신에게 영향을 주는 Point Light Index

public:
	D3DXVECTOR3	m_vMax;			// 전체의 Max
	D3DXVECTOR3	m_vMin;			// 전체의 Min
	D3DXMATRIX	m_matWorld;

	DxPiece*	m_pNext;		// List
	DxPiece*	m_pTreeNext;	// Tree 안에서의 List 이다.
	DxPiece*	m_pSortNext;	// Quick Sort 할 때의 List 이다.

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
	void Save( CSerialFile& SFile );	// Tree 에서의 Save Load
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, DxPieceEdit* pSrc );

public:
	void Import( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceEdit* pSrc, const D3DXMATRIX& matWorld, const D3DXMATRIX& matFrame );	// DxPieceEdit에서 값을 가져올 때 호출
	void CloneData( LPDIRECT3DDEVICEQ pd3dDevice, DxPiece* pSrc );															// DxPiece 녀석들 끼리의 복제
	void CleanUp();

private:
	void CheckPointLight();									// 자신과 맞닿아 있는 빛을 체크를 한다.	< 기본적으로 빛은 로딩되어 있어야 함 >
	void RenderPointLight( LPDIRECT3DDEVICEQ pd3dDevice );	// Point Light를 적용시킨다.

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

	// Note : m_pPiece 을 지워준다. 
	//			List로 Tree를 만든 경우는 m_pPiece가 참조 데이터가 되지만, 
	//			Tree로 Save에 이은 Load로 생성된 경우는 m_pPiece가 순수 데이터가 되므로 CleanUp으로 m_pPiece를 delete한다.
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
//	Note : 괜히 소트한다음 뿌리면 느릴까봐 걱정. ^^;
//			한곳에 집어넣고 배열로 정렬을 하는 것 인가 ?
//----------------------------------------------------------------------------------------------------------------------
class DxPieceQuickSort
{
protected:
	struct PIECEDISTANCE
	{
		float		m_fDistance;
		DxPiece*	m_pPiece;
	};
	PIECEDISTANCE*	m_pArrayPiece;	// 배열
	DWORD			m_dwArraySize;	// 최대 가능한 배열 사이즈
	DWORD			m_dwPieceCount;	// 현재 배열 사이즈
	float			m_fElapsedTime;	// ElapsedTime,

protected:
	static PIECEDISTANCE*	m_pArrayPieceONLY;	// 배열
	static DWORD			m_dwArraySizeONLY;	// 최대 가능한 배열 사이즈
	static DWORD			m_dwPieceCountONLY;	// 현재 배열 사이즈

public:
	void SetArrayMaxSize( DWORD dwCount );																// 최대 배열 사이즈 셋팅
	void Reset();																						// 현재 배열 사이즈 초기화.
	void InsertData( DxPiece* pPiece, DxPiece* pPiecePick, 
					const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength );	// 무작위 Piece 데이타
	void QuickSortProcess();																			// Quick Sort 작업
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
	void MakeTree( PDXPIECENODE& pTree, DxPiece* pPiece );	// Tree 를 만든다. pPiece 는 Tree의 Leaf 속으로 흡수 된다. List 사용불가.
	void InsertQuickSort( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, DxPieceQuickSort* pSort, DxPiece* pPiecePick, 
						const CLIPVOLUME &sCV, const D3DXVECTOR3& vFromPt, const D3DXVECTOR3& vLookatPt, const float& fLength );
	void CollisionLine( DxPieceNode* pTree, const CLIPVOLUME &sCV, const D3DXVECTOR3& vStart, D3DXVECTOR3& vEnd, D3DXVECTOR3& vCollision, BOOL& bColl );
	void Render_Reflect( LPDIRECT3DDEVICEQ pd3dDevice, DxPieceNode* pTree, const CLIPVOLUME &sCV );

	void Save( DxPieceNode* pNode, CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, PDXPIECENODE& pNode, CSerialFile& SFile, DxPieceEdit* pSrc );
};

