#pragma once

class CSerialFile;

#include "DxVertexFVF.h"

//--------------------------------------------------------------------------------------
// Name: struct DxSMeshDATA
// Desc: 
//--------------------------------------------------------------------------------------
struct DxSMeshDATA
{
	DWORD					m_dwVertices;
	DWORD					m_dwFaces;
	DWORD					m_dwFVF;	
	DWORD					m_dwSizeFVF;
	DWORD					m_dwAttrib;
	//LPDIRECT3DVERTEXBUFFER9 m_pVB;
	BYTE*					m_pVertices;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	D3DXATTRIBUTERANGE*		m_pAttribTable;

	void Convert( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh );

	DWORD GetBoneNUM( float fWeight1, float fWeight2, float fWeight3, float fWeight4 );
	void ConvertSkinnedMesh_0( PBYTE& pDest, BYTE* pSrc, DWORD dwVertices );
	void ConvertSkinnedMesh_1( PBYTE& pDest, BYTE* pSrc, DWORD dwVertices );
	void ConvertSkinnedMesh_2( PBYTE& pDest, BYTE* pSrc, DWORD dwVertices );
	void ConvertSkinnedMesh_3( PBYTE& pDest, BYTE* pSrc, DWORD dwVertices );
	void ConvertSkinnedMesh_4( PBYTE& pDest, BYTE* pSrc, DWORD dwVertices );

	void CleanUp();
	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	DxSMeshDATA();
	~DxSMeshDATA();
};
