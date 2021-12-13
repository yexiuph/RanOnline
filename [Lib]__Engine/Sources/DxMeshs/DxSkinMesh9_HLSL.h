#pragma once

class CSerialFile;
#include "DxSkinMesh9.h"
#include "DxMaterialHLSL.h"

//--------------------------------------------------------------------------------------
// Name: class DxSkinMesh9_HLSL
// Desc: 
//--------------------------------------------------------------------------------------
class DxSkinMesh9_HLSL : public DxSkinMesh9
{
public:
	static DWORD	VERSION;

protected:
	TCHAR			m_szFileName[32];			// 파일 이름.
	TCHAR			m_szSkinName[32];		// Skin xFile 이름.
	TCHAR			m_szSkeleton[32];		// Skeleton xFile 이름.

public:
	static DWORD			g_NumBoneMatricesMax;
	static D3DXMATRIXA16*	g_pBoneMatrices;

public:
	static ID3DXEffect*	m_pFX;

protected:
	static ID3DXEffect*	m_pEffect;	// D3DX effect interface
	static LPDIRECT3DVERTEXDECLARATION9	m_pNormalDCRT;

public:
	static HRESULT	StaticCreate (IDirect3DDevice9* pd3dDevice );
	static HRESULT StaticResetDevice( IDirect3DDevice9* pd3dDevice );
	static void StaticLost();
	static void StaticDestroy();

public:
	ID3DXEffect* GetEffect ()				{	return m_pEffect; }

public:
	void DrawMeshContainer( IDirect3DDevice9 *pd3dDevice, const CHARSETTING& sCharSetting );
	void DrawMeshSpecular( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPieceNULL, 
							DXMATERIAL_CHAR_EFF* pmtrlSpecular=NULL, BOOL bWorldIdentity=FALSE );
	void DrawMeshCartoon( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, RENDER_OPT emCTOP );
	void DrawMeshNORMALMAP( IDirect3DDevice9 *pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, SMATERIAL_PIECE *pmtrlPiece, 
							DxMaterialHLSL* pMaterialHLSL, BOOL bWorldIdentity=FALSE );

private:
	void RenderCartoon( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
							SMATERIAL_PIECE *pmtrlPiece );

	void RenderEdge( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr );

	void RenderNORMALMAP( LPDIRECT3DDEVICEQ pd3dDevice, SMeshContainer *pmcMesh, int ipattr, DWORD dwThisAttribId, 
						SMATERIAL_PIECE *pmtrlPiece, DxMaterialHLSL* pMaterialHLSL );

public:
	void Load( TCHAR* pName, IDirect3DDevice9 *pd3dDevice );

private:
	void Load( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice );
	void LoadNormalMap( CSerialFile& SFile, IDirect3DDevice9 *pd3dDevice );

	void CleanUp();

public:
	DxSkinMesh9_HLSL();
	~DxSkinMesh9_HLSL();

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
};