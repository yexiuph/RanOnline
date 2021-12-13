// DxDecal.h: interface for the DxDecal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DXDECAL_H__INCLUDED_)
#define _DXDECAL_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./DxCustomTypes.h"
#include "./Collision.h"

struct DECALVERTEX
{
	float x, y, z;
	DWORD DiffuseColor;
	float tu, tv;

	DECALVERTEX() :
		x(0.0f),
		y(0.0f),
		z(0.0f),
		DiffuseColor(0x000000),
		tu(0.0f),
		tv(0.0f)
	{
	}

	const static DWORD	FVF;
};

#define _MAXDECALVERTICES	(512)

class DxOctree;

class DxDecal
{
protected:
	D3DXVECTOR3		m_vCenter;
	D3DXVECTOR3		m_vNormal;

	union
	{
		struct
		{
			CLIPVOLUME m_ClipVolume;
		};

		struct
		{
			D3DXPLANE	m_PlaneLeft;
			D3DXPLANE	m_PlaneRight;

			D3DXPLANE	m_PlaneTop;
			D3DXPLANE	m_PlaneBottom;
	
			D3DXPLANE	m_PlaneFront;
			D3DXPLANE	m_PlaneBack;
		};
	};

protected:
	DWORD					m_dwVertexCount;
	DWORD					m_dwTriangleCount;
	DECALVERTEX				m_VertexArray[_MAXDECALVERTICES];
	WORD					m_TriangleArray[_MAXDECALVERTICES*3];

	LPDIRECT3DVERTEXBUFFERQ	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFERQ	m_pIndexBuffer;

protected:
	const static float		m_fDecalEpsilon;

protected:
	BOOL AddPolygon ( DWORD dwVertexCount, const D3DXVECTOR3 *pVertex, const D3DXVECTOR3 *pNormal );

	void ClipMeshAABB ( DxAABBNode* pAABBNode );
	DWORD ClipOctree ( DxOctree *pOctree );

	void ClipMesh ( D3DXMATRIX &matCom, PBYTE pbDataVB, PWORD pwIndexB, DWORD dwFVF, DWORD dwFaceNum );
	
	DWORD ClipPolygon ( DWORD dwVertexCount,
		const D3DXVECTOR3 *pVertex, const D3DXVECTOR3 *pNormal, D3DXVECTOR3 *pNewVertex, D3DXVECTOR3 *pNewNormal );

	DWORD ClipPolygonAgainstPlane ( const D3DXPLANE &Plane, DWORD dwVertexCount,
		const D3DXVECTOR3 *pVertex, const D3DXVECTOR3 *pNormal,
		D3DXVECTOR3 *pNewVertex, D3DXVECTOR3 *pNewNormal );

public:
	void CreateDecal ( D3DXVECTOR3 &vCenter, D3DXVECTOR3 &vNormal, D3DXVECTOR3 &vTangent,
		D3DXVECTOR3 &vSize, DxOctree *pOctree );

public:
	DWORD GetVertexCount ()	{ return m_dwVertexCount; }
	DWORD GetFaceCount ()	{ return m_dwTriangleCount; }
	DECALVERTEX* GetVertexArray () { return m_VertexArray; }
	WORD* GetIndexArray () { return m_TriangleArray; }

//public:
//	HRESULT DrawDecal ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	LPDIRECT3DVERTEXBUFFERQ CreateVertexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice );
	LPDIRECT3DINDEXBUFFERQ CreateIndexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice );

	void CreateVertexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ& pVB );
	void CreateIndexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DINDEXBUFFERQ& pIB );

	void CloneVertexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ& pVB );
	void CloneIndexBuffer ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DINDEXBUFFERQ& pIB );

	void CloneVertexArray ( void* pVertexArray);

protected:
	DxDecal();

public:
	virtual ~DxDecal();

public:
	static DxDecal& GetInstance();
};

#endif // !defined(_DXDECAL_H__INCLUDED_)
