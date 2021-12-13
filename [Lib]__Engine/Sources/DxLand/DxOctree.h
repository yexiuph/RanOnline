#ifndef	__DX_OCTREE__
#define	__DX_OCTREE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//	COMMENT : ���� ���� Ŭ���� Octree
//
//	���� �ۼ��� : ���⿱
//	���� �ۼ��� : 02.03.11
//
//	����[BY/00.00.00] : 
//

#include <vector>

#include "./DxCustomTypes.h"
#include "./DxObject.h"
#include "./DxEffectFrame.h"
#include "./DxFrameMesh.h"

using namespace std;

namespace	OctreeDebugInfo
{
	extern	int			TotalNodesDrawn;
	extern	int			EndNodeCount;
	extern	int			AmountDxFrame;
	extern	int			TodalDxFrameDrawn;
	extern	int			FaceViewCount;
	extern	int			VertexViewCount;
};

//class	CCollisionMapMaker;
class	CCollisionMap;
struct	SCell;

class	DxFrameMesh;
struct	DxFrame;
class	CSerialFile;
class	DxOctree : public	DxObject
{
protected:
	//	Note	:	��� �ڽ��ΰ�?
	enum//	eOctreeNodes
	{
		TOP_LEFT_FRONT,			// 0
		TOP_LEFT_BACK,			// 1
		TOP_RIGHT_BACK,			// etc...
		TOP_RIGHT_FRONT,
		BOTTOM_LEFT_FRONT,
		BOTTOM_LEFT_BACK,
		BOTTOM_RIGHT_BACK,
		BOTTOM_RIGHT_FRONT
	};
protected:
	//	Note : �ø� ����.
	//
	D3DXVECTOR3		m_vMax;	//	TOP_RIGHT_BACK
	D3DXVECTOR3		m_vMin;	//	BOTTOM_LEFT_FRONT

	BOOL			m_bLoaded;
	DWORD			m_DataAddress;
	DWORD			m_DataSize;
	
	//	Note : ���� ����Ʈ.
	//
	DxFrame			*m_pDxFrameHead;
	
	//	Note : Alpha Map ������ ����Ʈ.
	//
	DxFrame			*m_pDxAlphaMapFrameHead;

	//	Note : Ʈ�� ����.
	//
	BOOL			m_bSubDivided;

	//	Note : �ӽ� ����.
	//
	CSerialFile*	m_SFile;
	PDXEFFECTBASE*	m_pEffGlobalHead;

	union
	{
		struct
		{
			DxOctree*	m_pOctreeNodes[8];
		};
		struct
		{
			DxOctree*	m_pOctreeNodes_0;
			DxOctree*	m_pOctreeNodes_1;
			DxOctree*	m_pOctreeNodes_2;
			DxOctree*	m_pOctreeNodes_3;
			DxOctree*	m_pOctreeNodes_4;
			DxOctree*	m_pOctreeNodes_5;
			DxOctree*	m_pOctreeNodes_6;
			DxOctree*	m_pOctreeNodes_7;
		};
	};

public:
	DxOctree ();
	~DxOctree();

public:	
	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice);	
	virtual HRESULT DeleteDeviceObjects();	

protected:
	void	DrawFrame ( DxFrame *pFrame, DxLandMan* pLandMan );
	void	DrawFrame_NOEFF ( DxFrame *pFrame );

public:	
	void	Render( CLIPVOLUME &cv, CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, DxLandMan* pLandMan );
	void	Render_NOEFF( CLIPVOLUME &cv, CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead );
	void	RenderAlpha( CLIPVOLUME &cv, BOOL bShadow=FALSE );

public:
	BOOL	SaveFile ( CSerialFile &SFile );

public:
	void	SetLoadState ( BOOL bLoaded );
	BOOL	LoadFile ( CSerialFile &SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, BOOL bUseDynamicLoad );

	BOOL	DynamicLoad ( CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead );
	BOOL	LoadBasicPos ( CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );

public:
	//DxFrame*		FindFrame ( char *szFrame );	
	void	InsertFrameMesh ( DxFrame *pDxFrame );
	void	ResizeAllBox ( D3DXVECTOR3 *pvMin, D3DXVECTOR3 *pvMax );
	
	//	Note	:	�浹 ó����.
	//
public:
	HRESULT	MakeAABBTree ();
	BOOL	IsCollision ( const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision, D3DXVECTOR3 &vNormal, 
						BOOL &bCollision, LPDXFRAME &pDxFrame, LPCSTR& szName, const BOOL bFrontColl );

public:
	BOOL			IsSubDivision ()					{ return m_bSubDivided; }
	DxFrame*		GetDxFrameHead ()					{ return m_pDxFrameHead; }
	DxOctree*		GetOctreeNode ( int nIndex )		{ return m_pOctreeNodes[nIndex]; }
	BOOL			BoxIsInTheFrustum(CLIPVOLUME &cv)	{ return COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ); }
	D3DXVECTOR3&	GetMax () {	return m_vMax; }
	D3DXVECTOR3&	GetMin () {	return m_vMin; }

private:
	void CreateNode ( D3DXVECTOR3 vCenter, float Width, SCell* pCellInfo, CCollisionMap* pCollisionMap );

	//	Note	:	�ڽ��� �� ���� ���ϴ� �Լ� ( �ӽ����� ���� ), ���߿� ������ �����Ͽ� ������	
	void GetNewNodeCenter ( D3DXVECTOR3 vCenter, float width, D3DXVECTOR3 vSmallCenter[8] );
	
	//	Note : �ﰢ�� ������ üũ�ؼ� ���� �� �ɰ����ϴ��� �Ǵ��� �ɰ��ų� ����
	//
	void CreateNewNode ( SCell* pCell, float fWidth, D3DXVECTOR3 vCenter,
							  int NodeID, CCollisionMap* pCollisionMap );

	//	Note : ������ ������(���̽���)�� ���� �ִ´�.
	//
	HRESULT AssignVerticesToNode ( SCell* pCell, CCollisionMap* pCollisionMap );	
	void ModifyFrameMeshBoundary( SCell* pCell, CCollisionMap* pCollisionMap );

	//	Note : �ﰢ��(���̽�)�� ��Ÿ�� ���� --> �� �� ȿ������ ���������� ���� �Լ�
	//
	D3DXVECTOR3 GetTriCenter ( D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3 );

	//	Note : �ڽ��� ��Ÿ(�������� ����)�� ����
	//
	D3DXVECTOR3 GetNewNodeCenter(D3DXVECTOR3 vCenter, float width, int nodeID);

	void	SetMinMaxCorners ( D3DXVECTOR3 vCenter, float width );

public:
	HRESULT	Import( int MaxTriangles, int MaxSubdivisions, CCollisionMap* pCollisionMap );	

protected:
	void AddAlphaMapFrame ( DxFrame *pDxFrame );
	void DelAlphaMapFrame ( DxFrame *pDxFrame );
	void ModiflyAlphaMapFrame ( DxFrame *pDxFrame );

public:
	static	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	static	DxFrameMesh			*m_pFrameMesh;

	public:
	static	DWORD	m_MaxTriangles;
	static	DWORD	m_MaxSubdivisions;
	static	DWORD	m_CurrentSubdivision;

protected:
	BOOL			RenderDebugBox(BOOL	bIsAnimationBox = FALSE);
};

#endif	//	__DX_OCTREE__