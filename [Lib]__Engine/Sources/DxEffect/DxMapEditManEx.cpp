#include "pch.h"
#include "./DxViewPort.h"
#include "./SerialFile.h"

#include "./TextureManager.h"
#include "./StlFunctions.h"

#include "./DxEffectMan.h"

#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxMapEditMan::BASE*	DxMapEditMan::QuadBase::GetBase ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwGrid = (nID_X<<16) + nID_Z;
	
	NSQUADTREE::BASEMAP_ITER iter = NSQUADTREE::m_mapBaseEDIT.find ( dwGrid );

	if ( iter==NSQUADTREE::m_mapBaseEDIT.end() )		// 없다면 넣는다.
	{
		OBJQUADTemp sNew;
		sNew.pObject = this;
		NSQUADTREE::m_mapBaseEDIT.insert ( std::make_pair(dwGrid,sNew) );
	}

	if ( pBase )	return pBase;
	else
	{
		pBase = LoadFile ( pd3dDevice, FALSE );
		return pBase;
	}
}

BOOL DxMapEditMan::QuadBase::IsRenderEnalble ()
{
	if ( !pBase )	return FALSE;

	if ( !pBase->pVB )					return FALSE;
	if ( !pBase->pIB )					return FALSE;
	if ( !pBase->sBlend.IsEnable() )	return FALSE;
	if ( !pBase->sDxColor.IsEnable() )	return FALSE;

	return TRUE;
}

BOOL DxMapEditMan::QuadBase::IsEditEnalble ()
{
	if ( !pBase )	return FALSE;

	if ( !pBase->pVB )					return FALSE;
	if ( !pBase->pIB )					return FALSE;
	if ( !pBase->pVertex )				return FALSE;
	if ( !pBase->pTile )				return FALSE;
	if ( !pBase->sBlend.IsEnable() )	return FALSE;
	if ( !pBase->sDxColor.IsEnable() )	return FALSE;

	return TRUE;
}

// Note : 음. Edit 인지 Game 모드인지 확인 후, 
//			Game 일때는 Thread Loading 하라고 하고
//			Edit 일 경우 강제 Loading을 명령 한다.
HRESULT DxMapEditMan::QuadBase::FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bGame )
{
	if ( !IsRenderEnalble() )
	{
		if ( bGame )
		{
			DWORD dwGrid = (nID_X<<16) + nID_Z;
			
			NSQUADTREE::BASEMAP_ITER iter = NSQUADTREE::m_mapBaseLOADING.find ( dwGrid );

			if ( iter==NSQUADTREE::m_mapBaseLOADING.end() )								// 없다면 넣는다.
			{
				OBJQUADTemp sNew;
				sNew.pObject = this;
				NSQUADTREE::m_mapBaseLOADING.insert ( std::make_pair(dwGrid,sNew) );
			}
		}
		else
		{
			LoadFile ( pd3dDevice, bGame );
		}
	}

	return S_OK;
}

HRESULT DxMapEditMan::QuadBase::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	if ( !IsRenderEnalble() )	return S_OK;

	// Note : 블렌딩 메쉬 뿌림.
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pBase->sBlend.Render ( pd3dDevice );

	m_pSavedSB->Apply();

	// Note : 컬러 메쉬 뿌림.
	DWORD dwZWriteEnable, dwAlphaEnable;
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaEnable );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_DESTCOLOR );	// 강제 셋팅
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_SRCCOLOR );	// 강제 셋팅

	pd3dDevice->SetFVF ( VERTEX::FVF );

	pd3dDevice->SetTexture ( 0, pBase->sDxColor.GetTexture() );
	RenderColor ( pd3dDevice, pBase->pVB, pBase->pIB );

	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaEnable );

	return S_OK;
}

BOOL DxMapEditMan::QuadBase::IsCollisionLine ( const D3DXVECTOR3 &vPickRayOrig, const D3DXVECTOR3 &vPickRayDir, float& fLength )
{
	if ( !IsEditEnalble() )	return FALSE;

	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vTemp;
	D3DXVECTOR3 v0, v1, v2;
	FLOAT fBary1, fBary2;
	FLOAT fDist;

	for ( int z=0; z<EM_GRID; ++z )
	{
		for ( int x=0; x<EM_GRID; ++x )
		{
			v0 = pBase->pTile[(z*EM_GRID)+x].sVertex[2].vPos;
			v1 = pBase->pTile[(z*EM_GRID)+x].sVertex[3].vPos;
			v2 = pBase->pTile[(z*EM_GRID)+x].sVertex[0].vPos;
			if( DxEffectMan::GetInstance().IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
							&fDist, &fBary1, &fBary2 ) )
			{
				float fDis0, fDis1, fDis2;
				vPos = v0 + ((v1 - v0)*fBary1) + ((v2 - v0)*fBary2);

				vTemp = vPos - vPickRayOrig;
				fDis0 = D3DXVec3Length ( &vTemp );
				if ( fDis0 < fLength  )
				{
					vTemp = vPos - v0;
					fDis0 = D3DXVec3Length ( &vTemp );

					vTemp = vPos - v1;
					fDis1 = D3DXVec3Length ( &vTemp );

					vTemp = vPos - v2;
					fDis2 = D3DXVec3Length ( &vTemp );

					if ( (fDis0<=fDis1) && (fDis0<=fDis2) )
					{
						m_nPOINT_X = x;
						m_nPOINT_Z = z+1;
					}
					else if ( (fDis1<=fDis0) && (fDis1<=fDis2) )
					{
						m_nPOINT_X = x+1;
						m_nPOINT_Z = z+1;
					}
					else if ( (fDis2<=fDis0) && (fDis2<=fDis1) )
					{
						m_nPOINT_X = x;
						m_nPOINT_Z = z;
					}

					m_vPOINT = vPos;
					m_nTILE_X = x;
					m_nTILE_Z = z;

					ChangeBig2Short ( vPos, nID_X, nID_Z, x, z );

					return TRUE;
				}
			}

			v0 = pBase->pTile[(z*EM_GRID)+x].sVertex[3].vPos;
			v1 = pBase->pTile[(z*EM_GRID)+x].sVertex[1].vPos;
			v2 = pBase->pTile[(z*EM_GRID)+x].sVertex[0].vPos;
			if( DxEffectMan::GetInstance().IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
							&fDist, &fBary1, &fBary2 ) )
			{
				float fDis0, fDis1, fDis2;
				vPos = v0 + ((v1 - v0)*fBary1) + ((v2 - v0)*fBary2);

				vTemp = vPos - vPickRayOrig;
				fDis0 = D3DXVec3Length ( &vTemp );
				if ( fDis0 < fLength  )
				{
					vTemp = vPos - v0;
					fDis0 = D3DXVec3Length ( &vTemp );

					vTemp = vPos - v1;
					fDis1 = D3DXVec3Length ( &vTemp );

					vTemp = vPos - v2;
					fDis2 = D3DXVec3Length ( &vTemp );

					if ( (fDis0<=fDis1) && (fDis0<=fDis2) )
					{
						m_nPOINT_X = x+1;
						m_nPOINT_Z = z+1;
					}
					else if ( (fDis1<=fDis0) && (fDis1<=fDis2) )
					{
						m_nPOINT_X = x+1;
						m_nPOINT_Z = z;
					}
					else if ( (fDis2<=fDis0) && (fDis2<=fDis1) )
					{
						m_nPOINT_X = x;
						m_nPOINT_Z = z;
					}

					m_vPOINT = vPos;
					m_nTILE_X = x;
					m_nTILE_Z = z;

					ChangeBig2Short ( vPos, nID_X, nID_Z, x, z );

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void DxMapEditMan::QuadBase::CreateBaseData ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Position 값 셋팅
	SAFE_DELETE_ARRAY ( pBase->pVertex );
	pBase->pVertex = new VERTEX[EM_VERTEX_PART*EM_VERTEX_PART];
	for ( DWORD z=0; z<EM_VERTEX_PART; ++z )
	{
		for ( DWORD x=0; x<EM_VERTEX_PART; ++x )
		{
			pBase->pVertex[x+(z*EM_VERTEX_PART)].vPos.x = vMin.x + (x*m_dwLength_EDIT);
			pBase->pVertex[x+(z*EM_VERTEX_PART)].vPos.y = 0.f;
			pBase->pVertex[x+(z*EM_VERTEX_PART)].vPos.z = vMin.z + (z*m_dwLength_EDIT);
		}
	}

	Sleep( 1 );

	// Normal 값 수정
	for ( DWORD z=0; z<EM_VERTEX_PART; ++z )
	{
		for ( DWORD x=0; x<EM_VERTEX_PART; ++x )
		{
			ModifyNormal ( &(pBase->pVertex[x+(z*EM_VERTEX_PART)]), x, z );
		}
	}

	Sleep( 1 );

	// Tex 값 수정
	float fUV	= ((float)EM_TEXSIZE-4.f)/((float)EM_GRID);
	for ( DWORD z=0; z<EM_VERTEX_PART; ++z )
	{
		for ( DWORD x=0; x<EM_VERTEX_PART; ++x )
		{
			pBase->pVertex[x+(z*EM_VERTEX_PART)].vTex = D3DXVECTOR2 ( (2.f+(fUV*x))/(float)EM_TEXSIZE, (2.f+(fUV*z))/(float)EM_TEXSIZE );
		}
	}

	Sleep( 1 );

	// Last		각 Tile 마다 정보를 셋팅
	SAFE_DELETE_ARRAY ( pBase->pTile );
	pBase->pTile = new TILE [EM_GRID*EM_GRID];
	for ( DWORD z=0; z<EM_GRID; ++z )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			float fRandX = RANDOM_POS;			// TEST
			float fRandY = RANDOM_POS;			// TEST

			pBase->pTile[(z*EM_GRID)+x].sVertex[0].vPos		= pBase->pVertex[x+(z*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[0].vNor		= pBase->pVertex[x+(z*EM_VERTEX_PART)].vNor;
			pBase->pTile[(z*EM_GRID)+x].sVertex[0].vTex1	= pBase->pVertex[x+(z*EM_VERTEX_PART)].vTex;
			pBase->pTile[(z*EM_GRID)+x].sVertex[0].vTex2	= D3DXVECTOR2 ( 0.f, 0.f );

			pBase->pTile[(z*EM_GRID)+x].sVertex[1].vPos		= pBase->pVertex[x+1+(z*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[1].vNor		= pBase->pVertex[x+1+(z*EM_VERTEX_PART)].vNor;
			pBase->pTile[(z*EM_GRID)+x].sVertex[1].vTex1	= pBase->pVertex[x+1+(z*EM_VERTEX_PART)].vTex;
			pBase->pTile[(z*EM_GRID)+x].sVertex[1].vTex2	= D3DXVECTOR2 ( 1.f, 0.f );

			pBase->pTile[(z*EM_GRID)+x].sVertex[2].vPos		= pBase->pVertex[x+((z+1)*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[2].vNor		= pBase->pVertex[x+((z+1)*EM_VERTEX_PART)].vNor;
			pBase->pTile[(z*EM_GRID)+x].sVertex[2].vTex1	= pBase->pVertex[x+((z+1)*EM_VERTEX_PART)].vTex;
			pBase->pTile[(z*EM_GRID)+x].sVertex[2].vTex2	= D3DXVECTOR2 ( 0.f, 1.f );

			pBase->pTile[(z*EM_GRID)+x].sVertex[3].vPos		= pBase->pVertex[x+1+((z+1)*EM_VERTEX_PART)].vPos;
			pBase->pTile[(z*EM_GRID)+x].sVertex[3].vNor		= pBase->pVertex[x+1+((z+1)*EM_VERTEX_PART)].vNor;
			pBase->pTile[(z*EM_GRID)+x].sVertex[3].vTex1	= pBase->pVertex[x+1+((z+1)*EM_VERTEX_PART)].vTex;
			pBase->pTile[(z*EM_GRID)+x].sVertex[3].vTex2	= D3DXVECTOR2 ( 1.f, 1.f );
		}
	}

	Sleep( 1 );

	SAFE_RELEASE ( pBase->pVB );
	SAFE_RELEASE ( pBase->pIB );

	VERTEX*	pVertices;
	pd3dDevice->CreateVertexBuffer ( 4*EM_GRID*EM_GRID*sizeof(VERTEX), D3DUSAGE_WRITEONLY, VERTEX::FVF, D3DPOOL_MANAGED, &pBase->pVB, NULL );
	pBase->pVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );
	for ( DWORD z=0; z<EM_GRID; ++z )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			for ( DWORD i=0; i<4; ++i )
			{
				pVertices[(z*EM_GRID*4)+(x*4)+i].vPos	= pBase->pTile[(z*EM_GRID)+x].sVertex[i].vPos;
				pVertices[(z*EM_GRID*4)+(x*4)+i].vNor	= pBase->pTile[(z*EM_GRID)+x].sVertex[i].vNor;
				pVertices[(z*EM_GRID*4)+(x*4)+i].vTex	= pBase->pTile[(z*EM_GRID)+x].sVertex[i].vTex1;
			}
		}
	}
	pBase->pVB->Unlock ();

	Sleep( 1 );

	// 인덱스 버퍼 생성
	WORD*	pIndices;
	pd3dDevice->CreateIndexBuffer ( 3*EM_FACE*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pBase->pIB, NULL );
	pBase->pIB->Lock ( 0, 0, (VOID**)&pIndices, 0 );
	DWORD	dwAdd;
	for ( DWORD y=0; y<EM_GRID; ++y )
	{
		for ( DWORD x=0; x<EM_GRID; ++x )
		{
			dwAdd = (x*4) + (y*EM_GRID*4);
			*pIndices++ = (WORD)(2 + dwAdd);
			*pIndices++ = (WORD)(3 + dwAdd);
			*pIndices++ = (WORD)(0 + dwAdd);

			*pIndices++ = (WORD)(3 + dwAdd);
			*pIndices++ = (WORD)(1 + dwAdd);
			*pIndices++ = (WORD)(0 + dwAdd);
		}
	}
	pBase->pIB->Unlock ();

	Sleep( 1 );

	// 맵 생성
	pBase->sDxColor.Create ( pd3dDevice );

	Sleep( 1 );

	// 블렌드 Land 생성
	pBase->sBlend.Create ( pd3dDevice, pBase->pTile );

	Sleep( 1 );
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									s	t	a	t	i	c		 M	e	t	h	o	d
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::RenderColor ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB )
{
	pd3dDevice->SetStreamSource ( 0, pVB, 0, sizeof(VERTEX) );
	pd3dDevice->SetIndices ( pIB );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, EM_VERTEX_ALL, 0, EM_FACE );
}


void	DxMapEditMan::ModifyNormal ( VERTEX* pVertex, DWORD x, DWORD z )
{
	pVertex->vNor = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, 1,-1,	0,-1 );		// 1
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, 1,0,	1,-1 );		// 2
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, 1,1,	1,0 );		// 3
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, 0,1,	1,1 );		// 4
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, -1,1,	0,1 );		// 5
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, -1,0,	-1,1 );		// 6
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, -1,-1, -1,0 );	// 7
	pVertex->vNor += ModifyNormalPart ( pVertex, x,z, 0,-1,	-1,-1 );	// 8

	D3DXVec3Normalize ( &pVertex->vNor, &pVertex->vNor );
}

D3DXVECTOR3	DxMapEditMan::ModifyNormalPart ( VERTEX* pVertex, int x, int z, int x1, int y1, int x2, int y2 )
{
	D3DXVECTOR3 vNormal (0.f, 0.f, 0.f);
	D3DXVECTOR3	vLine[2];
	VERTEX* pVertexTEMP;

	int nX, nZ;
	nX = x+x1;
	nZ = z+y1;
	pVertexTEMP = pVertex+x1+(y1*EM_VERTEX_PART);
	if ( (nX<=-1) || (nX>=EM_VERTEX_PART) || (nZ<=-1) || (nZ>=EM_VERTEX_PART) )	return vNormal;
	vLine[0] = pVertexTEMP->vPos - pVertex->vPos;

	nX = x+x2;
	nZ = z+y2;
	pVertexTEMP = pVertex+x2+(y2*EM_VERTEX_PART);
	if ( (nX<=-1) || (nX>=EM_VERTEX_PART) || (nZ<=-1) || (nZ>=EM_VERTEX_PART) )	return vNormal;
	vLine[1] = pVertexTEMP->vPos - pVertex->vPos;

	D3DXVec3Cross ( &vNormal, &vLine[0], &vLine[1] );
	D3DXVec3Normalize ( &vNormal, &vNormal );

	if ( vNormal.y < 0.f )	vNormal *= -1.f;	// 밑을 바라본다면 반대로 방향을 바꾼다.

	return vNormal;
}

void DxMapEditMan::ChangeBig2Short ( D3DXVECTOR3 vPos, int ID_X, int ID_Z, int x, int z )
{
	float fCurPointX = (m_vMin.x+(m_dwLength_EDIT*EM_GRID*ID_X));	//(x*m_dwLength)+
	float fCurPointZ = (m_vMin.z+(m_dwLength_EDIT*EM_GRID*ID_Z));	//(z*m_dwLength)+

	float fLength = ((float)m_dwLength_EDIT/4.f);

	m_nTILE_X_S = (int)( (vPos.x-fCurPointX)/fLength );
	m_nTILE_Z_S = (int)( (vPos.z-fCurPointZ)/fLength );

	if ( m_nTILE_X_S < 0 )				m_nTILE_X_S = 0;
	if ( m_nTILE_Z_S < 0 )				m_nTILE_Z_S = 0;
	if ( m_nTILE_X_S > (EM_TEXSIZE-5) )	m_nTILE_X_S = EM_TEXSIZE-5;
	if ( m_nTILE_Z_S > (EM_TEXSIZE-5) )	m_nTILE_Z_S = EM_TEXSIZE-5;

	m_nPOINT_X_S = m_nTILE_X_S;
	m_nPOINT_Z_S = m_nTILE_Z_S;

	float fCenterX = fCurPointX + (m_nTILE_X_S*fLength) + (fLength*0.5f);
	float fCenterZ = fCurPointZ + (m_nTILE_Z_S*fLength) + (fLength*0.5f);

	if ( vPos.x > fCenterX )	m_nPOINT_X_S += 1;
	if ( vPos.z > fCenterZ )	m_nPOINT_Z_S += 1;
}

// -----------------------------------------------------------------------------------------------------------------------------------------
//									M	A	P		E	D	I	T		M	A	N		C l a s s
// -----------------------------------------------------------------------------------------------------------------------------------------
void DxMapEditMan::FindTiles ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vPickRayDir )
{
	m_bFINDTILE = FALSE;		// 초기화

	QuadBase* pOBJ = (QuadBase*)NSQUADTREE::FindQUADTree_VECTOR3 ( m_pQuadTree, vPickRayOrig, vPickRayDir );

	if ( pOBJ )
	{
		m_nBASE_X	= pOBJ->nID_X;
		m_nBASE_Z	= pOBJ->nID_Z;
		m_bFINDTILE = TRUE;				// Tile을 찾았는지 알아낸다. 못 찾았으면 에디팅 불가능

		FindPointList();	// 정점들의 리스트를 찾는다.
		FindTileList();		// 타일들의 리스트를 찾는다.

		MakePointMap ();	// 정점을 Map 형식으로 하여 정리한다. 
		MakeTileMap ();		// 타일을 Map 형식으로 하여 정리한다.
	}
}

void DxMapEditMan::FindPointList ()
{
	m_listPOINT_EDIT.clear();

	GRID	sGrid;

	if ( m_nBlushScale_EDIT%2 )
	{
		int nTemp = (int)((m_nBlushScale_EDIT-1)*0.5f);
		int nMinX, nMaxX;
		int nMinZ, nMaxZ;
		nMinX = m_nPOINT_X - nTemp;
		nMaxX = m_nPOINT_X + nTemp;
		nMinZ = m_nPOINT_Z - nTemp;
		nMaxZ = m_nPOINT_Z + nTemp;

		for ( int z=nMinZ; z<=nMaxZ; ++z )
		{
			for ( int x=nMinX; x<=nMaxX; ++x )
			{
				sGrid.nBaseX	= m_nBASE_X;
				sGrid.nBaseZ	= m_nBASE_Z;
				sGrid.nBigX		= x;
				sGrid.nBigZ		= z;
				if ( IsPointEX ( sGrid.nBaseX, sGrid.nBaseZ, sGrid.nBigX, sGrid.nBigZ ) )
				{
					m_listPOINT_EDIT.push_back ( sGrid );
				}		
			}
		}
	}
	else
	{
		int nTemp = (int)(m_nBlushScale_EDIT*0.5f);

		int nMinX, nMaxX;
		int nMinZ, nMaxZ;
		nMinX = m_nTILE_X - nTemp +1;
		nMaxX = m_nTILE_X + nTemp;
		nMinZ = m_nTILE_Z - nTemp +1;
		nMaxZ = m_nTILE_Z + nTemp;

		for ( int z=nMinZ; z<=nMaxZ; ++z )
		{
			for ( int x=nMinX; x<=nMaxX; ++x )
			{
				sGrid.nBaseX	= m_nBASE_X;
				sGrid.nBaseZ	= m_nBASE_Z;
				sGrid.nBigX		= x;
				sGrid.nBigZ		= z;
				if ( IsPointEX ( sGrid.nBaseX, sGrid.nBaseZ, sGrid.nBigX, sGrid.nBigZ ) )
				{
					m_listPOINT_EDIT.push_back ( sGrid );
				}	
			}
		}
	}
}

void DxMapEditMan::FindTileList ()
{
	m_listTILE_EDIT.clear();

	GRID	sGrid;

	if ( m_nBlushScale_EDIT%2 )
	{
		int nTemp = (int)((m_nBlushScale_EDIT-1)*0.5f);
		int nMinX, nMaxX;
		int nMinZ, nMaxZ;
		nMinX = m_nTILE_X_S - nTemp;
		nMaxX = m_nTILE_X_S + nTemp;
		nMinZ = m_nTILE_Z_S - nTemp;
		nMaxZ = m_nTILE_Z_S + nTemp;

		for ( int z=nMinZ; z<=nMaxZ; ++z )
		{
			for ( int x=nMinX; x<=nMaxX; ++x )
			{
				sGrid.nBaseX	= m_nBASE_X;
				sGrid.nBaseZ	= m_nBASE_Z;
				sGrid.nShortX = x;
				sGrid.nShortZ = z;
				if ( IsTileEX ( sGrid.nBaseX, sGrid.nBaseZ, sGrid.nShortX, sGrid.nShortZ ) )
				{
					sGrid.nBigX = (sGrid.nShortX/4);
					sGrid.nBigZ = (sGrid.nShortZ/4);

					m_listTILE_EDIT.push_back ( sGrid );
				}
			}
		}
	}
	else
	{
		int nTemp = (int)(m_nBlushScale_EDIT*0.5f);

		m_nTILE_X;

		int nMinX, nMaxX;
		int nMinZ, nMaxZ;
		nMinX = m_nPOINT_X_S - nTemp;
		nMaxX = m_nPOINT_X_S + nTemp -1;
		nMinZ = m_nPOINT_Z_S - nTemp;
		nMaxZ = m_nPOINT_Z_S + nTemp -1;

		for ( int z=nMinZ; z<=nMaxZ; ++z )
		{
			for ( int x=nMinX; x<=nMaxX; ++x )
			{
				sGrid.nBaseX	= m_nBASE_X;
				sGrid.nBaseZ	= m_nBASE_Z;
				sGrid.nShortX = x;
				sGrid.nShortZ = z;
				if ( IsTileEX ( sGrid.nBaseX, sGrid.nBaseZ, sGrid.nShortX, sGrid.nShortZ ) )
				{
					sGrid.nBigX = (sGrid.nShortX/4);
					sGrid.nBigZ = (sGrid.nShortZ/4);
					m_listTILE_EDIT.push_back ( sGrid );
				}
			}
		}
	}
}

BOOL DxMapEditMan::IsCircle ( float x1, float z1, float x2, float z2, float _fLength )
{
	float x = x1-x2;
	float z = z1-z2;

	x = fabsf(x);
	z = fabsf(z);

	float fLength = (x*x) + (z+z);
	if ( fLength < (_fLength*_fLength) )	return FALSE;
	else									return TRUE;
}

BOOL DxMapEditMan::IsPointEX ( int& EX_X, int& EX_Z, int& x, int& z )
{
	if ( x < 0 )
	{
		EX_X -= 1;
		if ( EX_X < 0 )	return FALSE;
		x = x - 1 + EM_VERTEX_PART;
	}
	if ( z < 0 )
	{
		EX_Z -= 1;
		if ( EX_Z < 0 )	return FALSE;
		z = z - 1 + EM_VERTEX_PART;
	}
	if ( x > (int)(EM_VERTEX_PART-1) )
	{
		EX_X += 1;
		if ( EX_X > (int)(m_dwGrid-1) )	return FALSE;
		x = x + 1 - EM_VERTEX_PART;
	}
	if ( z > (int)(EM_VERTEX_PART-1) )
	{
		EX_Z += 1;
		if ( EX_Z > (int)(m_dwGrid-1) )	return FALSE;
		z = z + 1 - EM_VERTEX_PART;
	}

    return TRUE;
}

BOOL DxMapEditMan::IsTileEX ( int& EX_X, int& EX_Z, int& x, int& z )
{
	if ( x < 0 )
	{
		EX_X -= 1;
		if ( EX_X < 0 )	return FALSE;
		x = x + EM_TEXSIZE_4;
	}
	if ( z < 0 )
	{
		EX_Z -= 1;
		if ( EX_Z < 0 )	return FALSE;
		z = z + EM_TEXSIZE_4;
	}
	if ( x > (int)(EM_TEXSIZE_4-1) )
	{
		EX_X += 1;
		if ( EX_X > (int)(m_dwGrid-1) )	return FALSE;
		x = x - EM_TEXSIZE_4;
	}
	if ( z > (int)(EM_TEXSIZE_4-1) )
	{
		EX_Z += 1;
		if ( EX_Z > (int)(m_dwGrid-1) )	return FALSE;
		z = z - EM_TEXSIZE_4;
	}

    return TRUE;
}

void DxMapEditMan::MakePointMap ()
{
	m_mapSortPOINT.clear();

	GRID_XZ		sGrid_EDIT;
	DWORD		dwTileNUM;

	GRID_LIST_ITER iter = m_listPOINT_EDIT.begin();
	for ( ; iter!=m_listPOINT_EDIT.end(); ++iter )
	{
		sGrid_EDIT	= GetCloneRange ( (*iter) );
		dwTileNUM	= (sGrid_EDIT.x<<16) + sGrid_EDIT.z;

		GRIDEX_MAP_ITER _iter = m_mapSortPOINT.find ( dwTileNUM );
		if ( _iter != m_mapSortPOINT.end() )		// 값이 있을 경우
		{	
			(*_iter).second.push_back ( (*iter) );
		}
		else									// 값이 없을 경우
		{
			GRID_LIST sGridList;
			sGridList.push_back ( (*iter) );
			m_mapSortPOINT.insert ( std::make_pair ( dwTileNUM, sGridList ) );
		}
	}
}

void DxMapEditMan::MakeTileMap ()
{
	m_mapSortTILE.clear();

	GRID_XZ		sGrid_EDIT;
	DWORD		dwTileNUM;

	GRID_LIST_ITER iter = m_listTILE_EDIT.begin();
	for ( ; iter!=m_listTILE_EDIT.end(); ++iter )
	{
		sGrid_EDIT	= GetCloneRange ( (*iter) );
		dwTileNUM	= (sGrid_EDIT.x<<16) + sGrid_EDIT.z;

		GRIDEX_MAP_ITER _iter = m_mapSortTILE.find ( dwTileNUM );
		if ( _iter != m_mapSortTILE.end() )		// 값이 있을 경우
		{	
			(*_iter).second.push_back ( (*iter) );
		}
		else									// 값이 없을 경우
		{
			GRID_LIST sGridList;
			sGridList.push_back ( (*iter) );
			m_mapSortTILE.insert ( std::make_pair ( dwTileNUM, sGridList ) );
		}
	}
}

void DxMapEditMan::EditMode ()
{
	while(1)
	{
		int nSize = (int)NSQUADTREE::m_mapBaseLOADING.size();
		if ( nSize == 0 )	break;							// 쓰레드 완료시 까지 기다려 준다.
	}
}

//BOOL DxMapEditMan::IsCollsion ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vTargetPOS, D3DXVECTOR3& vMin, D3DXVECTOR3& vMax )
//{
//	D3DXVECTOR3 v0, v1, v2;
//
//	v0 = D3DXVECTOR3 ( vMax.x, vMax.y, vMax.z );	// 위
//	v1 = D3DXVECTOR3 ( vMin.x, vMax.y, vMax.z );
//	v2 = D3DXVECTOR3 ( vMax.x, vMax.y, vMin.z );
//	if ( IsCollsion ( vPickRayOrig, vTargetPOS, vMin, vMax, v0, v1, v2 ) )	return TRUE;
//
//	v0 = D3DXVECTOR3 ( vMin.x, vMax.y, vMin.z );	// 좌
//	v1 = D3DXVECTOR3 ( vMin.x, vMax.y, vMax.z );
//	v2 = D3DXVECTOR3 ( vMin.x, vMin.y, vMax.z );
//	if ( IsCollsion ( vPickRayOrig, vTargetPOS, vMin, vMax, v0, v1, v2 ) )	return TRUE;
//
//	v0 = D3DXVECTOR3 ( vMax.x, vMax.y, vMax.z );	// 뒤
//	v1 = D3DXVECTOR3 ( vMin.x, vMax.y, vMax.z );
//	v2 = D3DXVECTOR3 ( vMin.x, vMin.y, vMax.z );
//	if ( IsCollsion ( vPickRayOrig, vTargetPOS, vMin, vMax, v0, v1, v2 ) )	return TRUE;
//
//	v0 = D3DXVECTOR3 ( vMax.x, vMax.y, vMax.z );	// 우
//	v1 = D3DXVECTOR3 ( vMax.x, vMin.y, vMax.z );
//	v2 = D3DXVECTOR3 ( vMax.x, vMax.y, vMin.z );
//	if ( IsCollsion ( vPickRayOrig, vTargetPOS, vMin, vMax, v0, v1, v2 ) )	return TRUE;
//
//	v0 = D3DXVECTOR3 ( vMax.x, vMin.y, vMin.z );	// 앞
//	v1 = D3DXVECTOR3 ( vMin.x, vMax.y, vMin.z );
//	v2 = D3DXVECTOR3 ( vMin.x, vMin.y, vMin.z );
//	if ( IsCollsion ( vPickRayOrig, vTargetPOS, vMin, vMax, v0, v1, v2 ) )	return TRUE;
//
//	v0 = D3DXVECTOR3 ( vMax.x, vMin.y, vMin.z );	// 밑
//	v1 = D3DXVECTOR3 ( vMin.x, vMin.y, vMax.z );
//	v2 = D3DXVECTOR3 ( vMin.x, vMin.y, vMin.z );
//	if ( IsCollsion ( vPickRayOrig, vTargetPOS, vMin, vMax, v0, v1, v2 ) )	return TRUE;
//
//	return FALSE;
//}
//
//BOOL DxMapEditMan::IsCollsion ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vTargetPOS, D3DXVECTOR3& vMin, D3DXVECTOR3& vMax, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2 )
//{
//	D3DXVECTOR3	vPos;
//	D3DXPLANE	sPlane;
//	D3DXPlaneFromPoints ( &sPlane, &v0, &v1, &v2 );
//
//	if ( (v0.x==v1.x) && (v0.y==v1.y) && (v0.z==v1.z) )	return FALSE;
//	if ( (v0.x==v2.x) && (v0.y==v2.y) && (v0.z==v2.z) )	return FALSE;
//	if ( (v1.x==v2.x) && (v1.y==v2.y) && (v1.z==v2.z) )	return FALSE;
//
//	D3DXPlaneIntersectLine ( &vPos, &sPlane, &vPickRayOrig, &vTargetPOS );
//
//	if ( (*vPos) == NULL )
//	{
//		return FALSE;
//	}
//	else
//	{
//		if ( (vPos.x<(vMin.x-0.001f)) || (vPos.y<(vMin.y-0.001f)) || (vPos.z<(vMin.z-0.001f)) ||
//			(vPos.x>(vMax.x+0.001f)) || (vPos.y>(vMax.y+0.001f)) || (vPos.z>(vMax.z+0.001f)) )
//		{
//			return FALSE;
//		}
//	}
//
//	return TRUE;
//}