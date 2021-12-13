// NsCollision.h: interface for the CCollision class.
//
//	class DxAABBCollision : FrameMesh 데이타를 넣으면 그 전체에 대한 AABB 트리를 만든다.
//
//	Name : Sung-Hwan Han
//	Begin :2004/10/21
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "DxFrameMesh.h"

struct DxFaceTree
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

	D3DXVECTOR3* pVertex;
	WORD*		pIndex;
	DWORD		dwFace;

	DxFaceTree*	pLeftChild;
	DxFaceTree*	pRightChild;

	float		fDisX;
	float		fDisY;
	float		fDisZ;

	void CleanUp();

	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );

	DxFaceTree() :
		vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		pVertex(NULL),
		pIndex(NULL),
		dwFace(0),
		pLeftChild(NULL),
		pRightChild(NULL)
	{
	};
	~DxFaceTree()
	{
		CleanUp();
		
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	};
};
typedef DxFaceTree* PDXFACETREE;

#include <map>

typedef std::map<std::string,DWORD>				MAPTEXINDEX;
typedef std::map<std::string,DWORD>::iterator	MAPTEXINDEX_ITER;

typedef std::map<DWORD,std::string>				MAPINDEXTEX;
typedef std::map<DWORD,std::string>::iterator	MAPINDEXTEX_ITER;

class DxAABBCollision
{
protected:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

public:
	DxFaceTree*		m_pTree;
	MAPINDEXTEX		m_mapIndexTex;

public:
	void MakeAABBCollision ( DxFrameMesh* pFrameMesh );
	BOOL IsCollisionLine ( const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, WORD& wTexIndex );

protected:
	void CleanUp();

public:
	void Save ( CSerialFile& SFile );
	void Load ( CSerialFile& SFile );

public:
	DxAABBCollision() :
		m_vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX),
		m_vMin(FLT_MAX,FLT_MAX,FLT_MAX),
		m_pTree(NULL)
	{
	};
	~DxAABBCollision()
	{
		CleanUp();
	};
};
typedef DxAABBCollision* PDXAABBCOLLISION;

namespace NSCOLLISIONMAP
{
	struct VERTEX 
	{ 
		D3DXVECTOR3 vPos; 
		D3DXVECTOR3 vNor; 
		D3DXVECTOR2 vTex;
		static const DWORD FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
	};
	typedef VERTEX* PVERTEX;

	void ExportCollision ( DxFrameMesh* pFrameMesh, LPD3DXVECTOR3& pVertex, LPWORD& pIndex, DWORD& dwFace, MAPINDEXTEX& mapIndexTex );	// Vertex 정보를 모음.
	void MakeCollisionMap ( PDXFACETREE& pTree, D3DXVECTOR3* pVertex, WORD* pIndex, DWORD dwFace );										// Tree를 만듬.

	void CollisionLine ( DxFaceTree* pTree, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd, 
						BOOL& bColl, D3DXVECTOR3 &vColl, D3DXVECTOR3 &vNor, WORD& wTexIndex );											// 충돌 감지

	void SaveTree ( CSerialFile& SFile, DxFaceTree* pTree );
	void LoadTree ( CSerialFile& SFile, PDXFACETREE& pTree );
};
