#pragma once

#include "./DxTextureEffMan.h"

struct DxFrame;
class DxFrameMesh;
class DxAnimationMan;
class DxFrameAni;
class DxTexEffBase;

#include "DxAnimationManager.h"

class DxFrameAniMan
{
protected:
	DxFrameAni*		m_pFrameAni;
	DxAnimationMan*	m_pAniManHead;

public:
	DxAnimationMan*	GetAniManHead()				{ return m_pAniManHead; }
	DxFrame* GetAniFrame( const char* szName );
	void MakeAniMan( LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh* pFrameMesh );

protected:
	void MakeFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxTextureEffMan* pTexEffMan );
	void LinkFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrame );
	void AniManMeshDelete();
	void AniManMeshDeleteTree( DxFrame* pFrame );

public:
	void Render( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matWorld, const float fAniTime, DxTextureEffMan::MAPTEXEFF* pTexEffList );
	void Render_EDIT( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxFrameAniMan();
	~DxFrameAniMan();

	void Save( CSerialFile& SFile );
	void Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile );
};

//---------------------------------------------------------------------------------------------------------
//						N	S		O	P	T	I	M	I	Z	E		A	N	I
//---------------------------------------------------------------------------------------------------------
namespace NSOPTIMIZEANI
{
	void OptimizeFrameAni( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame* pFrameRoot, LPDXANIMATIONMAN &pDxAnimationMan, float fUNITTIME );
};