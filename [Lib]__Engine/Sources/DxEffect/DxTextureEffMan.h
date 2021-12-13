#pragma once

#include <map>
#include <string>

class CSerialFile;
struct DxMeshes;
class DxTexEffBase;

#define TEXEFF_DEFAULT		0x1000
#define TEXEFF_OPACITY		0x2000
#define TEXEFF_HARDALPHA	0x3000
#define TEXEFF_SOFTALPHA	0x4000
#define TEXEFF_SOFTALPHA_1	0x5000
#define TEXEFF_SOFTALPHA_2	0x6000
#define TEXEFF_DXT			0xf000

struct TEXEFF_PROPERTY
{
	DWORD dwFlag;

	TEXEFF_PROPERTY() :
		dwFlag(0L)
	{
	};
};

class DxTextureEffMan
{
	// Note : Texture List
public:
	typedef	std::map<std::string,DWORD>			MAPTEXLIST;
	typedef	MAPTEXLIST::iterator				MAPTEXLIST_ITER;
	typedef	std::map<std::string,DxTexEffBase*>	MAPTEXEFF;
	typedef	MAPTEXEFF::iterator					MAPTEXEFF_ITER;

protected:
	MAPTEXLIST	m_mapTexList;
	MAPTEXEFF	m_mapTexEff;

public:
	MAPTEXEFF*	GetMapTexEff()		{ return &m_mapTexEff; }

public:
	void InsertTexList( const char* szName );
	void InsertTextureEff( MAPTEXEFF& mapData, const char* szName, DxTexEffBase* pBase );	// Note : 기본 텍스쳐 추가.

public:
	void FrameMove( float fElapsedTime );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs );
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs, const BOOL bShadow );

protected:
	void RenderAlpha( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh, const DWORD dwAttrib, const int nMETR, const BOOL bShadow );

protected:
	void MakeEffectMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes* pMeshs );

	// Note : Tool 에서 사용
public:
	int GetTextureListNUM()		{ return (int)m_mapTexList.size(); }
	BOOL IsGetTextureEFFDXT( DWORD i, LPCSTR& szName, DWORD& dwFlag );
	void DeleteTexEff( const char* pName, DxTexEffBase* pBase );
	DxTexEffBase* GetEffRoot( const char* szName );

public:
	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnLostDevice( LPDIRECT3DDEVICEQ pd3dDevice );
	void OnDestroyDevice();

public:
	void SavePSF( CSerialFile& SFile );
	void LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );

	void CloneMapTexEff( LPDIRECT3DDEVICEQ pd3dDevice, MAPTEXEFF& mapSrc, MAPTEXEFF& mapDest );

public:
	DxTextureEffMan();
	~DxTextureEffMan();
};
