// Collision.h: interface for the CCollision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISION_H__064700F3_5173_436C_8A96_CB946BB77C7C__INCLUDED_)
#define AFX_COLLISION_H__064700F3_5173_436C_8A96_CB946BB77C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxCustomTypes.h"

class DxLandMan;
class DxFrameMesh;

#define AABB_NONINDEX	(0xFFFFFFFF)
class	CSerialFile;
struct DxAABBNode
{
	union
	{
		struct { float fMaxX, fMaxY, fMaxZ; };
		struct { D3DXVECTOR3 vMax; };
	};

	union
	{
		struct { float fMinX, fMinY, fMinZ; };
		struct { D3DXVECTOR3 vMin; };
	};
	
	DWORD dwFace;

	DxAABBNode *pLeftChild;
	DxAABBNode *pRightChild;

	DxAABBNode () :
		fMaxX(0.0f),
		fMaxY(0.0f),
		fMaxZ(0.0f),
		fMinX(0.0f),
		fMinY(0.0f),
		fMinZ(0.0f),
		dwFace(AABB_NONINDEX),
		pLeftChild(NULL),
		pRightChild(NULL)
	{
	}

	~DxAABBNode ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	}

	BOOL IsCollision ( D3DXVECTOR3 &vP1, D3DXVECTOR3 &vP2 );
	BOOL IsCollisionVolume ( CLIPVOLUME & cv );

public:
	BOOL		SaveFile ( CSerialFile &SFile );
	BOOL		LoadFile ( CSerialFile &SFile );
};
typedef DxAABBNode* PDXAABBNODE;

struct	DxFrame;
class	DxOctree;
class OBJAABB
{
public:
	typedef OBJAABB* POBJAABB;

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin ) = 0;
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin ) = 0;
	virtual OBJAABB* GetNext () = 0;
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor )					{ return FALSE; }
	virtual BOOL IsCollisionLine( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, POBJAABB& pObj )	{ return FALSE; }

public:
	virtual void	FrameMove( const float fElapsedTime )																	{ return; }
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )												{ return S_OK; }
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )	{ return S_OK; }

	virtual HRESULT FirstLoad( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, CSerialFile* const SFile )	{ return S_OK; }
	virtual HRESULT RenderTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, 
									LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading )											{ return S_OK; }
	virtual HRESULT RenderDYNAMIC( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &pCV, CSerialFile* const SFile )						{ return S_OK; }
	virtual HRESULT Render( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &pCV, const char* szName )										{ return S_OK; }

public:
	virtual void InsertColorList( LOADINGDATALIST &listColorData )		{ return; }
};
typedef OBJAABB* POBJAABB;

struct OBJAABBNode
{
	union
	{
		struct { D3DXVECTOR3 vMax; };
		struct { float m_fMaxX, m_fMaxY, m_fMaxZ; };
	};
	union
	{
		struct { D3DXVECTOR3 vMin; };
		struct { float m_fMinX, m_fMinY, m_fMinZ; };
	};

	OBJAABBNode *pLeftChild;
	OBJAABBNode *pRightChild;

	OBJAABB *pObject;

	OBJAABBNode () :
		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		vMin(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		pLeftChild(NULL),
		pRightChild(NULL),
		pObject(NULL)
	{
	}

	~OBJAABBNode ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	}
};
typedef OBJAABBNode* POBJAABBNODE;

namespace COLLISION
{
	extern D3DXVECTOR3 vColTestStart;
	extern D3DXVECTOR3 vColTestEnd;

	BOOL IsLineTriangleCollision ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
								D3DXVECTOR3 *pPoint1, D3DXVECTOR3 *pPoint2,
								D3DXVECTOR3 *pCollision, D3DXVECTOR3 *pNormal=NULL, const BOOL bFrontColl=FALSE );

	BOOL IsSpherePointCollision ( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCenter, const float fRadius );

	BOOL IsSpherePlolygonCollision ( D3DXVECTOR3 *pTri0, D3DXVECTOR3 *pTri1, D3DXVECTOR3 *pTri2,
								const D3DXVECTOR3 &vCenter, const float fRadius );

	BOOL IsCollisionVolume ( const CLIPVOLUME& cv, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );
	BOOL IsCollisionAABBToAABB( const D3DXVECTOR3 &vAMax, const D3DXVECTOR3 &vAMin, const D3DXVECTOR3 &vBMax, const D3DXVECTOR3 &vBMin );
	BOOL IsCollisionLineToAABB( const D3DXVECTOR3 vStart, const D3DXVECTOR3 vEnd, const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );
	BOOL IsCollisionLineToSphere( const D3DXVECTOR3 vStart, const D3DXVECTOR3 vEnd, const D3DXVECTOR3 &vCenter, const float fLength );

	inline BOOL IsfEqual ( float a, float b )
	{
		float dx = a - b;
		if ( dx < 0.001f && dx > -0.001f )	return TRUE;
		return FALSE;
	}

	inline BOOL IsWithInPoint ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin,
							const D3DXVECTOR3 &vPos )
	{
		if ( vMax.x < vPos.x || vMax.y < vPos.y || vMax.z < vPos.z )	return FALSE;
		if ( vMin.x > vPos.x || vMin.y > vPos.y || vMin.z > vPos.z )	return FALSE;

		return TRUE;
	}

	HRESULT CalculateBoundingBox ( const D3DXMATRIX *const pmatComb, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin,
								PBYTE pbPoints, UINT cVertices, DWORD dwFVF );

	void TransformAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const D3DXMATRIX &matTrans );

	extern DWORD dwLeafCount;
	void MakeAABBTree ( POBJAABBNODE &pTreeHead, POBJAABB pListHead, BOOL bQUAD=FALSE );

	POBJAABB IsCollisonPointToOBJAABBTree ( POBJAABBNODE &pTreeNode, const D3DXVECTOR3 &vPos );

	extern DWORD dwRendAABB;
	HRESULT RenderAABBTree ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB );

	HRESULT RenderAABBTreeFrame ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, POBJAABBNODE &pNode, BOOL bRendAABB, DxLandMan* pLandMan );


	HRESULT GetSizeNode ( D3DXMATRIX &matComb,
									WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
									WORD *pwFaces, DWORD dwNumFaces,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	BOOL IsWithInTriangle ( D3DXMATRIX &matComb,
							LPD3DXVECTOR3 pvT1, LPD3DXVECTOR3 pvT2, LPD3DXVECTOR3 pvT3,
							float fDivision, DWORD dwAxis );

	HRESULT GetCenterDistNode ( D3DXMATRIX &matComb,
									WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
									WORD *pwFaces, DWORD dwNumFaces,
									D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	void CalcAxisTable ( float SizeX, float SizeY, float SizeZ, PDWORD pdwAxis );

	HRESULT MakeAABBNode ( DxAABBNode *pNode, D3DXMATRIX &matComb,
										WORD *pIndices, PBYTE pbPoints, DWORD dwFVF,
										WORD *pwFaces, DWORD dwNumFaces,
										D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

	void MINDETECTAABB ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, const float fRate, const float fMinLeng );

	void CollisionLineToTreePARENT ( OBJAABBNode* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									POBJAABB& pOBJ, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor );

	void CollisionLineToTree ( OBJAABBNode* pTree, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, 
									POBJAABB& pOBJ, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor );

	bool CalculatePlane2Line ( const D3DXPLANE &plane, const D3DXVECTOR3 &vLine1, const D3DXVECTOR3 &vLine2, D3DXVECTOR3 &vCol );

	void	FrameMove( const float fElapsedTime, OBJAABBNode* pNode );
	HRESULT RenderAABBTree ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, const char* szName );
	HRESULT RenderAABBTreeTHREAD( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, 
									LOADINGDATALIST &listLoadingData, CRITICAL_SECTION &CSLockLoading );
	HRESULT RenderAABBTreeDYNAMIC ( const LPDIRECT3DDEVICEQ pd3dDevice, const CLIPVOLUME &sCV, OBJAABBNode* pNode, CSerialFile* const SFile );
	HRESULT LoadAABBTreeBOX ( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vMax, const D3DXVECTOR3& vMin, OBJAABBNode* pNode, CSerialFile* const SFile );
	void InsertColorList( OBJAABBNode* pNode, LOADINGDATALIST& listColorData );
};

#endif // !defined(AFX_COLLISION_H__064700F3_5173_436C_8A96_CB946BB77C7C__INCLUDED_)
