#pragma once

#include <vector>
#include <map>
#include "DxVertexFVF.h"

//----------------------------------------------------------------------------------------------------------------
//								A	t	t	r	i	b	u	t	e		I	B	
//----------------------------------------------------------------------------------------------------------------
struct AttributeIB
{
	DWORD	m_dwBaseVertexIndex;
	DWORD	m_dwVertexStart;
	DWORD	m_dwVertexCount;
	DWORD	m_dwFaceStart;
	DWORD	m_dwFaceCount;
	LPDIRECT3DINDEXBUFFERQ	m_rIB;

	AttributeIB();
	~AttributeIB();
};

//----------------------------------------------------------------------------------------------------------------
//							D	x		D	y	n	a	m	i	c		M	e	s	h
//----------------------------------------------------------------------------------------------------------------
class DxDynamicMesh
{
public:
	static IDirect3DDevice9* g_pd3dDevice;

private:
	typedef std::vector<AttributeIB*>	VEC_ATTRIBUTEIB;
	typedef VEC_ATTRIBUTEIB::iterator	VEC_ATTRIBUTEIB_ITER;

	VEC_ATTRIBUTEIB			m_vecAttributeIB;

	LPDIRECT3DTEXTURE9		m_rTexture;
	LPDIRECT3DVERTEXBUFFERQ m_pVB;
	DWORD	m_dwVertexCUR;
	DWORD	m_dwFaceCUR;

	DWORD	m_dwVertexMAX;
	DWORD	m_dwFaceMAX;

public:
	void Reset();
	void InitVBIB( DWORD dwPointer );
	void InsertVBIB( VERTEX* pVertices, WORD* pIndices, DWORD dwVertexNUM, DWORD dwFaceNUM );
	void InsertIB( LPDIRECT3DINDEXBUFFERQ pIB, const D3DXATTRIBUTERANGE& sAttribTable );
	void Clone( LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB );

	void SetTexture( DWORD dwPointer );
	void CheckVBIB( DWORD dwVertexCount, DWORD dwFaceCount );
	BYTE* GetFreeVB( DWORD dwVertexNUM );
	void UnlockVB();
	void GetVertexFaceMAX( DWORD& dwVertexMAX, DWORD& dwFaceMAX );

	void Render();

private:
	void CreateDoubleVB( DWORD dwMinVertexNUM );
	void CreateDoubleIB( DWORD dwMinFaceNUM );

private:
	void CleanUp();

public:
	DxDynamicMesh();
	~DxDynamicMesh();
};

//----------------------------------------------------------------------------------------------------------------
//			S	M	e	s	h		D	y	n	a	m	i	c		R	e	n	d	e	r		M	a	n
//----------------------------------------------------------------------------------------------------------------
struct IDFACE
{
	DWORD	dwID;
	DWORD	dwFace;
};

typedef std::multimap<DWORD,IDFACE>		MMAP_TEX;		// 버텍스 갯수 / IDFACE
typedef MMAP_TEX::iterator				MMAP_TEX_ITER;
typedef MMAP_TEX::reverse_iterator		MMAP_TEX_RITER;

typedef std::multimap<DWORD,DWORD>		MAP_INDEX;		// 주소 /  배열의 위치.
typedef MAP_INDEX::iterator				MAP_INDEX_ITER;

class SMeshDynamicRenderMan
{
private:
	typedef std::vector<DxDynamicMesh*>		VEC_DNAMICRENDER;
	typedef VEC_DNAMICRENDER::iterator			VEC_DNAMICRENDER_ITER;

	VEC_DNAMICRENDER	m_vecDynamicRENDER;	// ID, 렌더 .~!(?)

	MAP_INDEX			m_mapIndex;
	DWORD				m_dwIndexCUR;	// Unlock 할 경우 Lock을 바로 참조 할 수 있도록.

public:
	void Reset();
	void Init( DWORD dwPointer, DWORD nCount );
	void InsertVBIB( DWORD dwPointer, VERTEX* pVertices, WORD* pIndices, DWORD dwVertexNUM, DWORD dwFaceNUM );
	void InsertIB( DWORD dwPointer, LPDIRECT3DINDEXBUFFERQ pIB, DWORD dwVertexStart, DWORD dwFaceStart, DWORD dwFaceCount );
	void Clone( LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB );

	BYTE* GetFreeVB( DWORD dwPointer, DWORD dwVertexNUM );
	BYTE* GetFreeVBInsertIB( DWORD dwIndex, LPDIRECT3DINDEXBUFFERQ pIB, const D3DXATTRIBUTERANGE& sAttribTable );
	void UnlockVB();
	void GetVertexFaceMAX( DWORD& dwVertexMAX, DWORD& dwFaceMAX );
	BOOL IsGetIndex( DWORD dwPointer, DWORD& nIndex );

	void Render();

public:
	SMeshDynamicRenderMan();
	~SMeshDynamicRenderMan();
};

//----------------------------------------------------------------------------------------------------------------
//					N	S		C	H	A	R		S	C	E	N	E		G	R	A	P	H
//----------------------------------------------------------------------------------------------------------------
namespace NSCHARSG
{
	void OnCreateDevice( IDirect3DDevice9* pd3dDevice );
	void OnDestroyDevice();

	void Reset();				// g_mapIDnCOUNT Reset
	void InsertRenderTexID( DWORD dwID, DWORD dwVertex, DWORD dwFace );
	void InsertVBIB( DWORD dwPointer, VERTEX* pVertices, WORD* pIndices, const D3DXATTRIBUTERANGE& dwAttrib );
	void InsertIB( DWORD dwPointer, WORD* pIndices, const D3DXATTRIBUTERANGE& dwAttrib );
	BYTE* GetFreeVB( DWORD dwPointer, DWORD dwVertexStart, DWORD dwFaceCount );
	BYTE* GetFreeVBInsertIB( DWORD dwIndex, LPDIRECT3DINDEXBUFFERQ pIB, const D3DXATTRIBUTERANGE& sAttribTable );
	void UnlockVB();
	BOOL IsGetIndex( DWORD dwPointer, DWORD& nIndex );
	void Render( IDirect3DDevice9* pd3dDevice );

	void ReleaseTexture( DWORD dwPointer );
};