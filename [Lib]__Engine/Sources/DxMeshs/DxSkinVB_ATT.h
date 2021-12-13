#pragma once

class CSerialFile;

//--------------------------------------------------------------------------------------
// Name: struct DxSkinVB_ATT
// Desc: 
//--------------------------------------------------------------------------------------
struct DxSkinVB_ATT
{
	DWORD					m_dwVertices;
	DWORD					m_dwFaces;
	DWORD					m_dwFVF;
	DWORD					m_dwPosFVF;
	DWORD					m_dwSizeFVF;
	DWORD					m_dwScaleFVF;
	DWORD					m_dwScaleSizeFVF;
	DWORD					m_dwAttrib;
	LPDIRECT3DVERTEXBUFFER9 m_pScaleVB;
	D3DXATTRIBUTERANGE*		m_pAttribTable;

	void CloneMeshNONINDEXED( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh );
	void CloneMeshHLSL( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh );
	void CloneMeshNONINDEXED( IDirect3DDevice9 *pd3dDevice, LPD3DXSKININFO pSkinInfo, DWORD *pAdjacency, LPD3DXMESH pMesh );
	void CloneMeshHLSL( IDirect3DDevice9 *pd3dDevice, LPD3DXSKININFO pSkinInfo, DWORD *pAdjacency, LPD3DXMESH pMesh );
	void CloneNormalMESH( IDirect3DDevice9 *pd3dDevice, LPD3DXMESH pMesh );
	void CloneAttribTable( LPD3DXMESH pMesh );
	void Clear();

	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile, LPD3DXMESH pMesh );

	DxSkinVB_ATT();
	~DxSkinVB_ATT();
};
