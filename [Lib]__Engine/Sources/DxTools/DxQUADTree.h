// DxQUADTree.h: interface for the CCollision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXQUADTREE_H__INCLUDED_)
#define AFX_DXQUADTREE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>

#include "DxCustomTypes.h"

class	CSerialFile;

class OBJQUAD
{
public:
	virtual void GetQUADSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin ) = 0;
	virtual void GetQUADSize ( int& nX, int& nZ ) = 0;
	virtual BOOL IsDetectDivision ( const D3DXVECTOR3 &vDivMax, const D3DXVECTOR3 &vDivMin ) = 0;
	virtual BOOL IsDetectDivision ( const int &nMaxX, const int &nMaxZ, const int &nMinX, const int &nMinZ ) = 0;
	virtual OBJQUAD* GetNext () = 0;
	virtual BOOL IsCollisionLine ( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir, float& fLength ) = 0;

public:
	virtual HRESULT FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bGame ) { return S_OK; }

public:
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )	{ return S_OK; }
};
typedef OBJQUAD* POBJQUAD;

struct OBJQUADTemp
{
	OBJQUAD *pObject;

	OBJQUADTemp () :
		pObject(NULL)
	{
	};
};

struct OBJQUADNode
{
	int nNodeNUM;

	int nMaxX;
	int nMaxZ;
	int nMinX;
	int nMinZ;

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

	OBJQUADNode *pLeftChild;
	OBJQUADNode *pRightChild;

	OBJQUAD *pObject;

	OBJQUADNode () :
		nNodeNUM(0),
		nMaxX(INT_MIN),
		nMaxZ(INT_MIN),
		nMinX(INT_MAX),
		nMinZ(INT_MAX),
		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		vMin(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		pLeftChild(NULL),
		pRightChild(NULL),
		pObject(NULL)
	{
	};

	~OBJQUADNode ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	};
};
typedef OBJQUADNode* POBJQUADNODE;

namespace NSQUADTREE
{
	extern D3DXVECTOR3 vColTestStart;
	extern D3DXVECTOR3 vColTestEnd;

	inline BOOL IsWithInPoint ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vPos )
	{
		if ( vMax.x < vPos.x || vMax.y < vPos.y || vMax.z < vPos.z )	return FALSE;
		if ( vMin.x > vPos.x || vMin.y > vPos.y || vMin.z > vPos.z )	return FALSE;

		return TRUE;
	}

	inline BOOL IsCollisionPoint ( const int &nMaxX, const int &nMaxZ, const int &nMinX, const int &nMinZ, const int &nID_X, const int &nID_Z )
	{
		if ( (nID_X<=nMaxX) && (nID_X>=nMinX) && (nID_Z<=nMaxZ) && (nID_Z>=nMinZ) )	return TRUE;

		return FALSE;
	}

	typedef std::list<POBJQUAD>						COLL_QUAD_LIST;
	typedef COLL_QUAD_LIST::iterator				COLL_QUAD_LIST_ITER;

	typedef std::map<DWORD,OBJQUADTemp>				BASEMAP;
	typedef std::map<DWORD,OBJQUADTemp>::iterator	BASEMAP_ITER;

	extern BASEMAP	 	m_mapBaseLOADING;	// 로딩중 인 곳 체크
	extern BASEMAP	 	m_mapBaseEDIT;		// 에디트 된 곳 체크

	extern DWORD dwLeafCountQUAD;

	D3DXVECTOR2 ReMaxMinQUADTree ( POBJQUADNODE pNode );
	void MakeQUADTree ( POBJQUADNODE &pTreeHead, POBJQUAD pListHead, BOOL bArray=TRUE );
	POBJQUAD FindQUADTree ( POBJQUADNODE pNode, const int &nID_X, const int &nID_Z );
	POBJQUAD FindQUADTree_VECTOR3 ( POBJQUADNODE pNode, const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vDir );
	HRESULT RenderQUADTree ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, POBJQUADNODE &pNode, BOOL bGame );
};

#endif // !defined(AFX_DXQUADTREE_H__INCLUDED_)
