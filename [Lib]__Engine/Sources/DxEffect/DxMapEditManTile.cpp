#include "pch.h"
#include "./DxViewPort.h"
#include "./SerialFile.h"

#include "./StlFunctions.h"

#include "./GLDefine.h"

#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// -----------------------------------------------------------------------------------------------------------------------------------------
//									E	N	G	I	N	E	에서 사용되는 것	( M A I N )
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::ModifyTexUV_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_bFINDTILE )		return;			// 타일을 못 찾았다면 변환할 수 없다.

	int			x, z;
	GRID_XZ		sGrid_EDIT;

	GRIDEX_MAP_ITER iter = m_mapSortTILE.begin();
	for ( ; iter!=m_mapSortTILE.end(); ++iter )
	{
		sGrid_EDIT.x = (*iter).first>>16;
		sGrid_EDIT.z = (*iter).first&0xffff;

		GRID_LIST_ITER _iter = (*iter).second.begin();
		for ( ; _iter!=(*iter).second.end(); ++_iter )
		{
			x = (*_iter).nShortX;
			z = (*_iter).nShortZ;

			if ( sGrid_EDIT.x != (*_iter).nBaseX )	x += EM_TEXSIZE_4;
			if ( sGrid_EDIT.z != (*_iter).nBaseZ )	z += EM_TEXSIZE_4;

			// 이상한 값이 나오는 것에 대한 임시 방편
			if ( x<=-1 || z<=-1 || x>=(EM_TEXSIZE_4+EM_TEXSIZE_4-1) || z>=(EM_TEXSIZE_4+EM_TEXSIZE_4-1) )	continue;

			// 컬러 텍스쳐 수정
			if ( m_bCOLOR_DRAW )	ColorChange_MAIN ( pd3dDevice, sGrid_EDIT, x, z );

			// 타일 텍스쳐, 알파 수정
			if ( m_bTILE_DRAW )		TileColorChange_MAIN ( pd3dDevice, sGrid_EDIT, x, z );
		}

		// VB, IB 생성
		if ( m_bTILE_DRAW )			CheckTileUSE_MAIN ( pd3dDevice, sGrid_EDIT );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									E	N	G	I	N	E	에서 사용되는 것	( C O L O R )
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::ColorChange_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid, int x, int z )
{
	QuadBase* pOBJ = NULL;
	D3DXCOLOR cColor;

	// Note : 컬러 얻어 내기
	TILING_EDIT_MAP_ITER _iter = m_mapTiles.find ( m_szTileName_USE );
	if ( _iter != m_mapTiles.end() )
	{
		cColor = GetEditColor ();

		if ( (x<=125) && (z<=125) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sDxColor.ColorChange ( cColor, x+2, z+2  );
			}
		}
		if ( (x>=122) && (z<=125) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sDxColor.ColorChange ( cColor, x+2-EM_TEXSIZE_4, z+2  );
			}
		}

		if ( (x<=125) && (z>=122) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z+1 );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sDxColor.ColorChange ( cColor, x+2, z+2-EM_TEXSIZE_4  );
			}
		}

		if ( (x>=122) && (z>=122) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z+1 );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sDxColor.ColorChange ( cColor, x+2-EM_TEXSIZE_4, z+2-EM_TEXSIZE_4  );
			}
		}
	}
}

D3DXCOLOR DxMapEditMan::GetEditColor ()
{
	D3DXCOLOR	cColor ( 0.f, 0.f, 0.f, 0.f );

	cColor = m_cColor_EDIT[m_nColor_EDIT];

	return cColor;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									E	N	G	I	N	E	에서 사용되는 것	( T E X )
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::TileColorChange_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid, int x, int z )
{
	D3DXCOLOR cColor;
	QuadBase* pOBJ = NULL;

	// Note : 텍스쳐 얻어내기
	const char* strName;
	TILING_EDIT_MAP_ITER _iter = m_mapTiles.find ( m_szTileName_USE );
	if ( _iter != m_mapTiles.end() )
	{
		strName = (*_iter).second.cName.c_str();

		// Note : 원본 데이터에 삽입, 텍스쳐에 그리기
		if ( (x<=125) && (z<=125) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sBlend.ModifyTex ( pd3dDevice, strName, x+2, z+2 );
			}
		}
		if ( (x>=122) && (z<=125) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sBlend.ModifyTex ( pd3dDevice, strName, x+2-EM_TEXSIZE_4, z+2 );
			}
		}

		if ( (x<=125) && (z>=122) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z+1 );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sBlend.ModifyTex ( pd3dDevice, strName, x+2, z+2-EM_TEXSIZE_4 );
			}
		}

		if ( (x>=122) && (z>=122) )
		{
			pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z+1 );
			if ( pOBJ )
			{
				BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
				pBase->sBlend.ModifyTex ( pd3dDevice, strName, x+2-EM_TEXSIZE_4, z+2-EM_TEXSIZE_4 );
			}
		}
	}
}

void DxMapEditMan::CheckTileUSE_MAIN ( LPDIRECT3DDEVICEQ pd3dDevice, GRID_XZ& sGrid )
{
	QuadBase* pOBJ = NULL;
	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z );
	if ( pOBJ )
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		pBase->sBlend.Modify ( pd3dDevice, pBase->pTile );
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z );
	if ( pOBJ )
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		pBase->sBlend.Modify ( pd3dDevice, pBase->pTile );
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x, sGrid.z+1 );
	if ( pOBJ )
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		pBase->sBlend.Modify ( pd3dDevice, pBase->pTile );
	}

	pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, sGrid.x+1, sGrid.z+1 );
	if ( pOBJ )
	{
		BASE*	pBase = pOBJ->GetBase ( pd3dDevice );
		pBase->sBlend.Modify ( pd3dDevice, pBase->pTile );
	}
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									E	N	G	I	N	E	에서 사용되는 것	( R E N D E R )
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::ModifyTexUV_RENDER ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;
	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;
	sMaterial.Specular.r = 0.f;
	sMaterial.Specular.g = 0.f;
	sMaterial.Specular.b = 0.f;
	sMaterial.Specular.a = 0.f;
	pd3dDevice->SetMaterial ( &sMaterial );

	DWORD	dwLighting;
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &dwLighting );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	DWORD	dwColorOP;
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP, &dwColorOP );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );

	GRID_LIST_ITER iter = m_listTILE_EDIT.begin();
	for ( ; iter!=m_listTILE_EDIT.end(); ++iter )
	{
		QuadBase* pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree ( m_pQuadTree, (*iter).nBaseX, (*iter).nBaseZ );
		if ( pOBJ )
		{
			BASE*	pBase = pOBJ->GetBase ( pd3dDevice );

			D3DXVECTOR3	vDirect[2];
			D3DXVECTOR3	sVertex[3];
			sVertex[0] = pBase->pVertex[ (*iter).nBigX + ((*iter).nBigZ*EM_VERTEX_PART) ].vPos;
			sVertex[1] = pBase->pVertex[ ((*iter).nBigX+1) + ((*iter).nBigZ*EM_VERTEX_PART) ].vPos;
			sVertex[2] = pBase->pVertex[ (*iter).nBigX + (((*iter).nBigZ+1)*EM_VERTEX_PART) ].vPos;

			int nX = (*iter).nShortX%4;
			int nZ = (*iter).nShortZ%4;

			vDirect[0] = sVertex[1] - sVertex[0];		// x축
			vDirect[1] = sVertex[2] - sVertex[0];		// z축

			ModifyTexUV_RENDER_PART ( pd3dDevice, &(sVertex[0]), &(vDirect[0]), nX+1, nZ, nX, nZ );
			ModifyTexUV_RENDER_PART ( pd3dDevice, &(sVertex[0]), &(vDirect[0]), nX+1, nZ+1, nX+1, nZ );
			ModifyTexUV_RENDER_PART ( pd3dDevice, &(sVertex[0]), &(vDirect[0]), nX+1, nZ+1, nX, nZ+1 );
			ModifyTexUV_RENDER_PART ( pd3dDevice, &(sVertex[0]), &(vDirect[0]), nX, nZ+1, nX, nZ );
		}
	}

	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, dwLighting );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, dwColorOP );
}

void DxMapEditMan::ModifyTexUV_RENDER_PART ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3* vPos, D3DXVECTOR3* vDir, int x1, int z1, int x2, int z2 )
{
	D3DXVECTOR3	sRender[2];

	sRender[0] = vPos[0] + D3DXVECTOR3 ( 0.f, 1.f, 0.f );
	sRender[0] += (vDir[0]/4.f)*(float)x1;
	sRender[0] += (vDir[1]/4.f)*(float)z1;

	sRender[1] = vPos[0] + D3DXVECTOR3 ( 0.f, 1.f, 0.f );
	sRender[1] += (vDir[0]/4.f)*(float)x2;
	sRender[1] += (vDir[1]/4.f)*(float)z2;
	pd3dDevice->DrawPrimitiveUP ( D3DPT_LINELIST, 1, sRender, sizeof(D3DXVECTOR3) );
}

// ----------------------------------------------------------------------------------------------------------------------------------
//											T	O	O	L	에서 사용되는 것
// ----------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::SetTileData ( const char* pName )
{
	TILING_EDIT	sTile;

	m_mapTiles.insert ( std::make_pair ( pName, sTile ) );
}

void DxMapEditMan::DelTileData ( const char* pName )
{
	TILING_EDIT_MAP_ITER iter = m_mapTiles.begin();
	m_mapTiles.erase ( iter );
}